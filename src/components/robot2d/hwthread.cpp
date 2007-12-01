/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "hwthread.h"
#include <iostream>

// we need these to throw orca exceptions from the functions executed in the network thread
#include <orca/exceptions.h>
#include <orcaice/orcaice.h>

using namespace std;

namespace {
    const char *SUBSYSTEM = "HwThread";
}

namespace robot2d {

HwThread::HwThread( const orcaice::Context &context ) :
    driver_(0),
    driverFactory_(0),
    driverLib_(0),
    context_(context)
{
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 10.0 );
    context_.status()->initialising( SUBSYSTEM );

    //
    // Read settings
    //
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    isMotionEnabled_ = (bool)orcaice::getPropertyAsIntWithDefault( prop, prefix+"EnableMotion", 1 );

    // Dynamically load the library and find the factory
    std::string driverLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libHydroRobot2dPlayerClient.so" );
    context_.tracer()->debug( "HwThread: Loading driver library "+driverLibName, 4 );
    try {
        driverLib_ = new hydrodll::DynamicallyLoadedLibrary(driverLibName);
        driverFactory_ = 
            hydrodll::dynamicallyLoadClass<hydrointerfaces::Robot2dFactory,DriverFactoryMakerFunc>
            ( *driverLib_, "createDriverFactory" );
    }
    catch (hydrodll::DynamicLoadException &e)
    {
        context_.tracer()->error( e.what() );
        throw;
    }

    // create the driver
    hydroutil::Properties props( prop->getPropertiesForPrefix(prefix), prefix );
    hydrointerfaces::Context driverContext( props, context_.tracer(), context_.status() );
    try {
        context_.tracer()->info( "HwThread: Initialising driver..." );
        driver_ = driverFactory_->createDriver( driverContext );
    }
    catch ( ... )
    {
        stringstream ss;
        ss << "HwThread: Caught unknown exception while initialising driver";
        context_.tracer()->error( ss.str() );
        throw;
    }  
}

void
HwThread::enableDriver()
{
    while ( !isStopping() ) 
    {
        try {
            context_.tracer()->info("HwThread: (Re-)Enabling driver...");
            driver_->enable();
            context_.tracer()->info( "HwThread: Enable succeeded." );
            return;
        }
        catch ( std::exception &e )
        {
            std::stringstream ss;
            ss << "HwThread::enableDriver(): enable failed: " << e.what();
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
            stateMachine_.setFault( ss.str() );
        }
        catch ( ... )
        {
            std::stringstream ss;
            ss << "HwThread::enableDriver(): enable failed due to unknown exception.";
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
            stateMachine_.setFault( ss.str() );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
}

void
HwThread::walk()
{
    std::string reason;

    //
    // Main loop
    //
    while( !isStopping() )
    {
        //
        // Make sure we're OK
        //
        if ( stateMachine_.isFault( reason ) )
        {
            // Try to (re-)enable
            context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 5.0 );    
            enableDriver();

            // we enabled, so presume we're OK.
            stateMachine_.setOK();
            context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 2.0 );

            // but make sure we're not shutting down.
            if ( isStopping() )
                break;
        }

        //
        // Read data from the hardware
        //
        try {
            hydrointerfaces::Robot2d::Data data;
            bool stateChanged = driver_->read( data );

            // stick it in the store, so that NetThread can distribute it                
            dataStore_.set( data );

            // Update status
            if ( stateChanged ) 
            {
                std::string status;
                bool isWarn, isFault;
                driver_->getStatus( status, isWarn, isFault );
                std::stringstream ss;
                ss << "Saw state change: " << status;
                if ( isFault )
                {
                    stateMachine_.setFault( ss.str() );
                    context_.tracer()->error( ss.str() );
                }
                else if ( isWarn )
                {
                    stateMachine_.setWarning( ss.str() );
                    context_.tracer()->warning( ss.str() );
                }
                else
                {
                    stateMachine_.setOK();
                    context_.tracer()->info( ss.str() );
                }
            }
        }
        catch ( std::exception &e )
        {
            std::stringstream ss;
            ss << "HwThread: Failed to read: " << e.what();
            context_.tracer()->error( ss.str() );

            stateMachine_.setFault( ss.str() );
        }
        catch ( ... )
        {
            std::stringstream ss;
            ss << "HwThread: Failed to read due to unknown exception.";
            context_.tracer()->error( ss.str() );

            stateMachine_.setFault( ss.str() );            
        }

        //
        // Write pending commands to the hardware
        //
        if ( !commandStore_.isEmpty() && !stateMachine_.isFault() )
        {
            hydrointerfaces::Robot2d::Command command;
            commandStore_.get( command );

            try {
                driver_->write( command );

                stringstream ss;
                ss << "HwThread: wrote command: " << command.toString();
                context_.tracer()->debug( ss.str() );
            }
            catch ( std::exception &e )
            {
                std::stringstream ss;
                ss << "HwThread: Failed to write command to hardware: " << e.what();
                context_.tracer()->error( ss.str() );

                // set local state to failure
                stateMachine_.setFault( ss.str() );
            }
            catch ( ... )
            {
                std::stringstream ss;
                ss << "HwThread: Failed to write command to hardware due to unknown exception.";
                context_.tracer()->error( ss.str() );

                // set local state to failure
                stateMachine_.setFault( ss.str() );                
            }
        }

        // Tell the 'status' engine what our local state machine knows.
        if ( stateMachine_.isFault(reason) )
        {
            context_.status()->fault( SUBSYSTEM, reason );
        }
        else if ( stateMachine_.isWarning(reason) )
        {
            context_.status()->warning( SUBSYSTEM, reason );
        }
        else
        {
            context_.status()->ok( SUBSYSTEM );
        }

    } // while
}

void
HwThread::setCommand( const hydrointerfaces::Robot2d::Command &command )
{
    // if we know we can't write, don't try: inform remote component of problem
    std::string reason;
    if ( stateMachine_.isFault(reason) )
    {
        throw orca::HardwareFailedException( reason );
    }

    if ( !isMotionEnabled_ )
    {
        throw orca::HardwareFailedException( "Motion is disabled" );
    }

    commandStore_.set( command );

    stringstream ss;
    ss << "HwThread::setCommand( "<<command.toString()<<" )";
    context_.tracer()->debug( ss.str() );
}

}
