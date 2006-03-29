/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
 
#include <iostream>
#include <orcaice/orcaice.h>

#include "mainloop.h"
#include "staticdriver.h"

#ifdef HAVE_STAGE_DRIVER
    #include "stage/stagedriver.h"
#endif

using namespace std;
using namespace orca;
using namespace simlocaliser;
using orcaice::operator<<;

MainLoop::MainLoop( const Localise2dConsumerPrx                    localise2dConsumer,
                    orcaice::PtrBuffer<Localise2dDataPtr>         &locBuffer,
                    orcaice::Context                               context )
    : localise2dConsumer_(localise2dConsumer),
      locBuffer_(locBuffer),
      context_(context)
{
    assert(localise2dConsumer_ != 0);

    //
    // Read settings
    //
    std::string prefix = context_.tag() + ".Config.";

    // based on the config parameter, create the right driver
    string driverName = orcaice::getPropertyWithDefault( context_.properties(),
            prefix+"Driver", "static" );
            
    if ( driverName == "static" )
    {      
        std::string driverPrefix = prefix + "Static.";
        orca::Frame2d pose;
        orcaice::setInit(pose);
        pose = orcaice::getPropertyAsFrame2dWithDefault( context_.properties(),
                driverPrefix+"Pose", pose );
                
        driver_ = new StaticDriver( pose );
    }
    else if ( driverName == "stage" )
    {
#ifdef HAVE_STAGE_DRIVER
        context_.tracer()->debug( "loading Player-Client driver",3);
        
        std::string driverPrefix = prefix + "Stage.";
        std::string playerHost = orcaice::getPropertyWithDefault( context_.properties(),
                driverPrefix+"Host", "localhost" );
        int playerPort = orcaice::getPropertyAsIntWithDefault( context_.properties(),
                driverPrefix+"Port", 6665 );
        std::string playerId = orcaice::getPropertyWithDefault( context_.properties(),
                driverPrefix+"Id", "model1" );
    
        driver_ = new StageDriver( playerHost.c_str(), playerPort, playerId.c_str() );
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver 'stage' because it was not built!" );
#endif
    }
    
    context_.tracer()->debug("driver instantiated",5);
}

MainLoop::~MainLoop()
{
}

void
MainLoop::run()
{
    Localise2dDataPtr localiseData = new Localise2dData;

    // we are in a different thread now, catch all stray exceptions
    try
    {

    //
    // Enable driver
    //
    while ( isActive() && driver_->enable() ) {
        context_.tracer()->warning("failed to enable the driver; will try again in 2 seconds.");
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
        context_.tracer()->info("driver enabled");

    // This is the main loop
    try 
    {
        int readStatus;
         
        while ( isActive() )
        {

            //
            // Read data
            //
            // readTimer_.restart();
            readStatus = driver_->read( localiseData );
            // cout<<"read: " << readTimer_.elapsed().toMilliSecondsDouble()<<endl;
        
            if ( readStatus==0 ) {
//                 cout<<"TRACE(mainloop.cpp): new localise data: " << localiseData << endl;
                
                // Stick the new data in the buffer
                locBuffer_.push( localiseData );
    
                try {
                    // push to IceStorm
                    localise2dConsumer_->setData( localiseData );
                }
                catch ( Ice::ConnectionRefusedException &e )
                {
                    // This could happen if IceStorm dies.
                    // If we're running in an IceBox and the IceBox is shutting down, 
                    // this is expected (our co-located IceStorm is obviously going down).
                    context_.tracer()->warning( "Failed push to IceStorm." );
                }
            } else {
                context_.tracer()->error("failed to read data from Segway hardware. Repairing....");
                driver_->repair();
            }

        }
    }
    catch ( Ice::CommunicatorDestroyedException &e )
    {
        // This is OK: it means that the communicator shut down (eg via Ctrl-C)
        // somewhere in mainLoop.
        //
        // Could probably handle it better for an Application by stopping the component on Ctrl-C
        // before shutting down communicator.
    }

    // reset the hardware
    if ( driver_->disable() ) {
        context_.tracer()->warning("failed to disable driver");
    }
    else {
        context_.tracer()->debug("driver disabled",5);
    }

    //
    // unexpected exceptions
    //
    } // try
    catch ( const orca::OrcaException & e )
    {
        context_.tracer()->print( e.what );
        context_.tracer()->error( "unexpected (remote?) orca exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const orcaice::Exception & e )
    {
        context_.tracer()->print( e.what() );
        context_.tracer()->error( "unexpected (local?) orcaice exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const Ice::Exception & e )
    {
        cout<<e<<endl;
        context_.tracer()->error( "unexpected Ice exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const std::exception & e )
    {
        // once caught this beast in here, don't know who threw it 'St9bad_alloc'
        cout<<e.what()<<endl;
        context_.tracer()->error( "unexpected std exception.");
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
}
