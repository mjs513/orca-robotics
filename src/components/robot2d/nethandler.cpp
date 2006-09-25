/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>

#include "nethandler.h"
#include <IceUtil/Time.h>
#include <IceStorm/IceStorm.h>

// implementations of Ice objects
#include "platform2dI.h"

#include <orcaice/orcaice.h>

using namespace std;
using namespace orca;
using namespace robot2d;

NetHandler::NetHandler(
                 orcaice::PtrProxy<orca::Position2dDataPtr>    & position2dPipe,
                 orcaice::PtrNotify<orca::Velocity2dCommandPtr>& commandPipe,
                 orcaice::PtrProxy<orca::Platform2dConfigPtr>  & setConfigPipe,
                 orcaice::PtrProxy<orca::Platform2dConfigPtr>  & currentConfigPipe,
                 const orcaice::Context                        & context )
      : position2dPipe_(position2dPipe),
        commandPipe_(commandPipe),
        setConfigPipe_(setConfigPipe),
        currentConfigPipe_(currentConfigPipe),
        position2dData_(new Position2dData),
        commandData_(new Velocity2dCommand),
        context_(context)
{
    init();
}

NetHandler::~NetHandler()
{
}

void
NetHandler::init()
{
    //
    // Read settings
    //
    std::string prefix = context_.tag() + ".Config.";
    
    position2dPublishInterval_ = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            prefix+"Position2dPublishInterval", -1 );
    statusPublishInterval_ = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            prefix+"StatusPublishInterval", 60.0 );

    // PROVIDED: Platform2d
    // Find IceStorm Topic to which we'll publish.
    // NetworkException will kill it, that's what we want.
    IceStorm::TopicPrx platfTopicPrx = orcaice::connectToTopicWithTag<Position2dConsumerPrx>
                ( context_, position2dPublisher_, "Platform2d" );

    // create servant for direct connections and tell adapter about it
    // don't need to store it as a member variable, adapter will keep it alive
    Ice::ObjectPtr platform2dObj = new Platform2dI( position2dPipe_, commandPipe_,
                                      setConfigPipe_, currentConfigPipe_, platfTopicPrx );
    // two possible exceptions will kill it here, that's what we want
    orcaice::createInterfaceWithTag( context_, platform2dObj, "Platform2d" );
    
    // all cool, assume we can send and receive
    context_.tracer()->debug("network enabled",5);
}

void
NetHandler::run()
{
    try // this is once per run try/catch: waiting for the communicator to be destroyed
    {

    int position2dReadTimeout = 1000; // [ms]
    orcaice::Timer pushTimer;

    while ( isActive() )
    {
        try {
            cout<<"activating..."<<endl;
            context_.activate();
            cout<<"activated."<<endl;
            break;
        }
        catch ( orcaice::NetworkException & e )
        {
            std::stringstream ss;
            ss << "nethandler::run: Caught NetworkException: " << e.what() << endl << "Will try again...";
            context_.tracer()->warning( ss.str() );
        }
        catch ( Ice::Exception & e )
        {
            std::stringstream ss;
            ss << "nethandler::run: Caught Ice::Exception while activating: " << e << endl << "Will try again...";
            context_.tracer()->warning( ss.str() );
        }
        catch ( ... )
        {
            cout << "Caught some other exception while activating." << endl;
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
    
    while( isActive() )
    {
        // block on the most frequent data source: position
        int ret = position2dPipe_.getNext( position2dData_, position2dReadTimeout );
        
        // it's time to publish if we publish every data point or enough time has elapsed
        bool isTimeToPublishPosition2d = position2dPublishInterval_ < 0
                || position2dPublishTimer_.elapsed().toSecondsDouble()>position2dPublishInterval_;
        if ( ret == 0 && isTimeToPublishPosition2d )
        {
//debug
//cout<<"push: " << pushTimer.elapsed().toMilliSecondsDouble()<<endl;
//pushTimer.restart();

            // managed to read new data and it's time to publish
            try
            {
                position2dPublisher_->setData( position2dData_ );
                position2dPublishTimer_.restart();
            }
                catch ( const Ice::ConnectionRefusedException & e )
            {
                context_.tracer()->warning("lost connection to IceStorm");
                // now what?
            }
        }

        // now send less frequent updates
        try
        {
            // todo: the logic of this needs revisiting
            if ( statusPublishInterval_<0 ||
                        statusPublishTimer_.elapsed().toSecondsDouble()>statusPublishInterval_ ) {
                //cout<<"sending heartbeat"<<endl;
                context_.tracer()->heartbeat( context_.tracer()->status() );
                statusPublishTimer_.restart();
            }
        }
        catch ( const Ice::ConnectionRefusedException & e )
        {
            context_.tracer()->warning("lost connection to IceStorm");
            // now what?
        }
            
    } // while
    
    }
    catch ( const Ice::CommunicatorDestroyedException & e )
    {
        // it's ok, we must be quitting.
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    context_.tracer()->debug( "exiting NetHandler thread...",5);
}

void
NetHandler::send()
{
    // push data to IceStorm
    try
    {
        if ( position2dPublishTimer_.elapsed().toSecondsDouble()>position2dPublishInterval_ ) {
            // check that there's new data
            position2dPipe_.get( position2dData_ );
            position2dPublisher_->setData( position2dData_ );
            position2dPublishTimer_.restart();
        }
        if ( statusPublishTimer_.elapsed().toSecondsDouble()>statusPublishInterval_ ) {
            //cout<<"sending heartbeat"<<endl;
            context_.tracer()->heartbeat("status OK");
            statusPublishTimer_.restart();
        }
    }
    catch ( const Ice::ConnectionRefusedException & e )
    {
        context_.tracer()->warning("lost connection to IceStorm");
    }
    catch ( const Ice::CommunicatorDestroyedException & e )
    {
        // it's ok, the communicator may already be destroyed
    }
}
