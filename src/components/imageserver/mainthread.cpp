/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include "mainthread.h"

using namespace std;
using namespace imageserver;

MainThread::MainThread( const orcaice::Context &context ) :
    orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 20.0 );

    //
    // Read settings
    //
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    config_.width = orcaice::getPropertyAsIntWithDefault( prop, prefix+"ImageWidth", 0 );
    config_.height = orcaice::getPropertyAsIntWithDefault( prop, prefix+"ImageHeight", 0 );
    config_.size = config_.width*config_.height*3; //TODO look at image type

    if ( !config_.validate() ) {
        context_.tracer().error( "Failed to validate image configuration. "+config_.toString() );
        // this will kill this component
        throw gbxutilacfr::Exception( ERROR_INFO, "Failed to validate image configuration" );
    }

}

void
MainThread::initNetworkInterface()
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    //
    // SENSOR DESCRIPTION
    //
    orca::ImageDescriptionPtr descr(new orca::ImageDescription);
    // transfer internal sensor configs
    descr->width       = config_.width;
    descr->height      = config_.height;

    //
    // EXTERNAL PROVIDED INTERFACE
    //

    imageInterface_ = new orcaifaceimpl::ImageImpl( descr,
                                                    "Image",
                                                    context_ );
    // init
    imageInterface_->initInterface( this, subsysName() );
}

void
MainThread::initHardwareDriver()
{
    subStatus().setMaxHeartbeatInterval( 20.0 );

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    // Dynamically load the library and find the factory
    std::string driverLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libHydroImageFake.so" );
    context_.tracer().debug( "MainThread: Loading driver library "+driverLibName, 4 );
    // The factory which creates the driver
    std::auto_ptr<hydrointerfaces::ImageFactory> driverFactory;
    try {
        driverLib_.reset( new hydrodll::DynamicallyLoadedLibrary(driverLibName) );
        driverFactory.reset( 
            hydrodll::dynamicallyLoadClass<hydrointerfaces::ImageFactory,DriverFactoryMakerFunc>
            ( *driverLib_, "createDriverFactory" ) );
    }
    catch (hydrodll::DynamicLoadException &e)
    {
        // unrecoverable error
        context_.shutdown(); 
        throw;
    }

    // create the driver
    while ( !isStopping() )
    {
        std::stringstream exceptionSS;
        try {
            context_.tracer().info( "HwThread: Creating driver..." );
            driver_.reset(0);
            driver_.reset( driverFactory->createDriver( config_, context_.toHydroContext() ) );
            break;
        }
        catch ( ... ) {
            orcaice::catchExceptionsWithStatusAndSleep( "initialising hardware driver", subStatus() );
        }   
    }

    subStatus().setMaxHeartbeatInterval( 1.0 );
}

void
MainThread::readData()
{
    //
    // Read from the laser driver
    //
    hydroImageData_.haveWarnings = false;
    driver_->read( hydroImageData_ );

    orcaImageData_->timeStamp.seconds  = hydroImageData_.timeStampSec;
    orcaImageData_->timeStamp.useconds = hydroImageData_.timeStampUsec;
}

void
MainThread::walk()
{
    // Set up the laser-scan objects
    orcaImageData_ = new orca::ImageData;
    orcaImageData_->data.resize( config_.width*config_.height*3 );

    // Point the pointers in hydroImageData_ at orcaImageData_
    hydroImageData_.data      = &(orcaImageData_->data[0]);

    // These functions catch their exceptions.
    activate( context_, this, subsysName() );

    initNetworkInterface();
    initHardwareDriver();

    //
    // IMPORTANT: Have to keep this loop rolling, because the '!isStopping()' call checks for requests to shut down.
    //            So we have to avoid getting stuck anywhere within this main loop.
    //
    while ( !isStopping() )
    {
        try 
        {
            // this blocks until new data arrives
            readData();
            
            imageInterface_->localSetAndSend( orcaImageData_ );
            if ( hydroImageData_.haveWarnings )
            {
                subStatus().warning( hydroImageData_.warnings );
            }
            else
            {
                subStatus().ok();
            }

            stringstream ss;
            ss << "MainThread: Read laser data: " << orcaobj::toString(orcaImageData_);
            context_.tracer().debug( ss.str(), 5 );

            continue;

        } // end of try
        catch ( ... ) 
        {
            orcaice::catchMainLoopExceptions( subStatus() );

            // Re-initialise the driver, unless we are stopping
            if ( !isStopping() ) {
                initHardwareDriver();
            }
        }

    } // end of while

    // Image hardware will be shut down in the driver's destructor.
}
