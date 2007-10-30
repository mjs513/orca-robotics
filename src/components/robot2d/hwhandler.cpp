#include "hwhandler.h"
#include <iostream>
#include <orca/exceptions.h>

using namespace std;

namespace {
    const char *SUBSYSTEM = "HwHandler";
}

namespace robot2d {

HwHandler::HwHandler( HwDriver               &hwDriver,
                      bool                    isMotionEnabled,
                      const orcaice::Context &context )
    : driver_(hwDriver),
      isMotionEnabled_(isMotionEnabled),
      context_(context)
{
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 10.0 );
    context_.status()->initialising( SUBSYSTEM );
}

void
HwHandler::enableDriver()
{
    while ( !isStopping() ) 
    {
        try {
            context_.tracer()->info("HwHandler: (Re-)Enabling driver...");
            driver_.enable();
            context_.tracer()->info( "HwHandler: Enable succeeded." );
            return;
        }
        catch ( std::exception &e )
        {
            std::stringstream ss;
            ss << "HwHandler::enableDriver(): enable failed: " << e.what();
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
            stateMachine_.setFault( ss.str() );
        }
        catch ( ... )
        {
            std::stringstream ss;
            ss << "HwHandler::enableDriver(): enable failed due to unknown exception.";
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
            stateMachine_.setFault( ss.str() );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
}

void
HwHandler::walk()
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
            Data data;
            bool stateChanged = driver_.read( data );

            // stick it in the store, so that NetHandler can distribute it                
            dataStore_.set( data );

            // Update status
            if ( stateChanged ) 
            {
                std::string status;
                bool isWarn, isFault;
                driver_.getStatus( status, isWarn, isFault );
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
            ss << "HwHandler: Failed to read: " << e.what();
            context_.tracer()->error( ss.str() );

            stateMachine_.setFault( ss.str() );
        }
        catch ( ... )
        {
            std::stringstream ss;
            ss << "HwHandler: Failed to read due to unknown exception.";
            context_.tracer()->error( ss.str() );

            stateMachine_.setFault( ss.str() );            
        }

        //
        // Write pending commands to the hardware
        //
        if ( !commandStore_.isEmpty() && !stateMachine_.isFault() )
        {
            Command command;
            commandStore_.get( command );

            try {
                driver_.write( command );

                stringstream ss;
                ss << "HwHandler: wrote command: " << command.toString();
                context_.tracer()->debug( ss.str() );
            }
            catch ( std::exception &e )
            {
                std::stringstream ss;
                ss << "HwHandler: Failed to write command to hardware: " << e.what();
                context_.tracer()->error( ss.str() );

                // set local state to failure
                stateMachine_.setFault( ss.str() );
            }
            catch ( ... )
            {
                std::stringstream ss;
                ss << "HwHandler: Failed to write command to hardware due to unknown exception.";
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
HwHandler::setCommand( const Command &command )
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
    ss << "HwHandler::setCommand( "<<command.toString()<<" )";
    context_.tracer()->debug( ss.str() );
}

}

