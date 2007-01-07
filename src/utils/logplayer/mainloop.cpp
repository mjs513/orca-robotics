/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <assert.h>
#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

#include "mainloop.h"
#include "replayclock.h"

using namespace std;
using namespace logplayer;


MainLoop::MainLoop( orcalog::ReplayMaster* master,
              std::vector<orcalog::Replayer*> & replayers,
              std::vector<orcalog::ReplayFactory*> & factories,
              const orcaice::Context & context )
    :  master_(master),
       replayers_(replayers),
       factories_(factories),
       context_(context)
{
}

MainLoop::~MainLoop()
{
}

void
MainLoop::run()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {

    // config file parameters
    Ice::PropertiesPtr props = context_.properties();
    string prefix = context_.tag() + ".Config.";

    // BeginTime: Replay data starting from BeginTime (seconds) from the start of the log
    orca::Time tempTime;
    orcaice::setInit( tempTime );
    tempTime  = orcaice::getPropertyAsTimeDurationWithDefault( props, prefix+"BeginTime", tempTime );
    IceUtil::Time beginTime = orcaice::toIceTime( tempTime );
    if ( beginTime<IceUtil::Time() ) {
        context_.tracer()->warning( "Reset negative BeginTime to 0.0" );
        beginTime = IceUtil::Time();
    }

    // EndTime: Replay data up to EndTime (seconds) from the start of the log
    // by convention, negative time means play to the ened
    orcaice::setInit( tempTime );
    tempTime.seconds = -1;
    tempTime = orcaice::getPropertyAsTimeDurationWithDefault( props, prefix+"EndTime", tempTime );
    IceUtil::Time endTime = orcaice::toIceTime( tempTime );
    if ( endTime<IceUtil::Time() ) {
        context_.tracer()->info( "Negative EndTime means the log file will play to the end" );
    }

    // ReplayRate: Adjusts the playback speed
    double replayRate = orcaice::getPropertyAsDoubleWithDefault( props, prefix+"ReplayRate", 1.0 );
    ReplayClock clock;        
    bool waitForUserInput;
    // special case: wait for human input for every object
    if ( replayRate == 0.0 ) {
        waitForUserInput = true;
        context_.tracer()->info( "Replay is triggered by user input." );
    }
    else {
        waitForUserInput = false;
        clock.setReplayRate( replayRate );
        stringstream ss;
        ss<<"Replay is triggered by the clock (real/replay="<<replayRate<<").";
        context_.tracer()->info( ss.str() );
    }

    stringstream ss;
    ss<<"Initializing "<<replayers_.size()<<" replayers.";
    context_.tracer()->debug( ss.str() );
    for ( unsigned int i=0; i<replayers_.size(); ++i ) {
        replayers_[i]->init();
    }

    bool autoStart  = orcaice::getPropertyAsIntWithDefault( props, prefix+"AutoStart", 0 );
    if ( !autoStart ) {
        cout<<"Type ENTER to start replay:" << endl;
        cout<<"==>"<<endl;
        getchar();
    }

    int seconds = 0;
    int useconds = 0;
    int id = 0;
    int index = 0;

    // Read the first data line
    if ( master_->getData( seconds, useconds, id, index ) ) {
        context_.tracer()->error( "Failed to read the 1st line of master file" );
        exit(1);
    }

    // fast forward to the desired start time
    // have to do it after we read the first line because begin time
    // is specified since the start of the log
    if ( beginTime>IceUtil::Time() ) {
        // absolute time of the time from which to start replay
        IceUtil::Time beginLogTime = beginTime +
                    IceUtil::Time::seconds(seconds) + IceUtil::Time::microSeconds(useconds);
        cout<<"DEBUG: Begin log absolute time :"<<beginLogTime.toSeconds()<<endl;

        // use special Fast-Forward version of getData()
        if ( master_->getData( seconds, useconds, id, index, 
                beginLogTime.toSeconds(), beginLogTime.toMicroSeconds()%1000000 ) ) {
            context_.tracer()->error( "Failed to fast forward to BeginTime "+beginTime.toDuration() );
            exit(1);
        }

        ostringstream ss;
        ss<<"Fast-forwarded to "<<seconds<<"s, "<<useconds<<"us";
        context_.tracer()->info( ss.str() );
    }

    if ( id > (int)replayers_.size() ) {
        ostringstream ss;
        ss << "Reference to subfile number " << id << ", but only " << replayers_.size() << " exist.";
        context_.tracer()->error( ss.str() );
        exit(1);
    }

    try {
        replayers_[id]->replayData( index );
    }
    catch ( const std::exception & e) {
        ostringstream ss;
        ss<<"Caught from replayer of type "<<replayers_[id]->interfaceType()<<": "<<e.what();;
        context_.tracer()->warning( ss.str() );
    }

    // Read the start time: this is the offset for the rest of the playback
    clock.setReplayStartTime( IceUtil::Time::now() );
    clock.setLogStartTime( IceUtil::Time::seconds(seconds) + IceUtil::Time::microSeconds(useconds) );
    IceUtil::Time nextLogTime;
    IceUtil::Time untilNextLogTime;
    // should this be a configurable parameter? can't think why it would be useful.
    IceUtil::Time replayTimeTolerance = IceUtil::Time::microSeconds(10);

    ///////////////////////////////////////////////////
    //
    // The main loop
    //
    ///////////////////////////////////////////////////
    while( isActive() )
    {
        // read a line and act appropriately
        if ( master_->getData( seconds, useconds, id, index ) ) {
            // end of file
            break;
        }   
        nextLogTime = IceUtil::Time::seconds(seconds)+IceUtil::Time::microSeconds(useconds);
        
        // check end time (negative end time means play to the end)
        if ( endTime>IceUtil::Time() && nextLogTime>=endTime ) {
            // reached specified end time
            break;
        }

        if ( id > (int)replayers_.size() ) {
            ostringstream ss;
            ss << "Reference to subfile number " << id << ", when only " << replayers_.size() << " exist.";
            context_.tracer()->error( ss.str() );
            exit(1);
        }

        // replay is driven by user input
        if ( waitForUserInput ) {
            cout<<"Type ENTER to replay the next object:" << endl;
            cout<<"==>"<<endl;
            getchar();
        }
        // replay is driven by the clock
        else {
            // alexm: probably better to do a loop, but for some reason we *always* sleep twice.
            do
            {
                // Work out how long till we should send
                untilNextLogTime = clock.untilNextLogTime( nextLogTime );
                    
                if ( untilNextLogTime > replayTimeTolerance )
                {
                    IceUtil::ThreadControl::sleep(untilNextLogTime);
                }
                
            } while ( isActive() && untilNextLogTime > replayTimeTolerance  );
        }
    
        //
        // Now send it out
        //
        try {
            replayers_[id]->replayData( index );
        } 
        catch ( const std::exception & e) {
            ostringstream ss;
            ss<<"Caught from replayer of type "<<replayers_[id]->interfaceType()<<": "<<e.what();;
            context_.tracer()->warning( ss.str() );
        }

    } // end of main loop
    
    // we can only get here if it's the end of the logfile
    context_.tracer()->info( "Reached the end of log file." );
    
    
    // we are done here. So to let the application know that we are finished,
    // we destroy the communicator, triggering clean-up and shutdown.
    context_.communicator()->destroy();

    } // try
    // we'll get this exception on Ctrl-C
    catch ( const Ice::CommunicatorDestroyedException & e )
    {
        // it's ok, we must be quitting.
    }
    // all we can do when catching a replayer exception is to quit
    catch (const orcalog::Exception & e)
    {
        stringstream ss;
        ss << e.what() << ". We'll have to quit!"; 
        context_.tracer()->error( ss.str() ); 
        context_.communicator()->destroy(); 
    }
    //
    // unexpected exceptions
    //
    catch ( const orca::OrcaException & e )
    {
        stringstream ss;
        ss << "unexpected (remote?) orca exception: " << e << ": " << e.what;
        context_.tracer()->error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const orcaice::Exception & e )
    {
        stringstream ss;
        ss << "unexpected (local?) orcaice exception: " << e.what();
        context_.tracer()->error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const Ice::Exception & e )
    {
        stringstream ss;
        ss << "unexpected Ice exception: " << e;
        context_.tracer()->error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const std::exception & e )
    {
        // once caught this beast in here, don't know who threw it 'St9bad_alloc'
        stringstream ss;
        ss << "unexpected std exception: " << e.what();
        context_.tracer()->error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( ... )
    {
        context_.tracer()->error( "unexpected exception from somewhere.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    
    context_.tracer()->debug( "Exiting MainLoop thread", 5 );
}
