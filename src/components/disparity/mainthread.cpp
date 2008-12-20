/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2008 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 * 
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "mainthread.h"

#include <iostream>
#include <memory>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h> // for getPropertyAs...()

using namespace std;
using namespace disparity;

MainThread::MainThread( const orcaice::Context &context ) :
    orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
}

void
MainThread::initialise()
{
    readSettings();

    initPluginInterface();

    initNetworkInterface();
}

void
MainThread::work() 
{
    subStatus().setHeartbeatInterval( 1.0 );

    while( !isStopping() )
    {
        try
        {
            //read data in
            
            //process it

            //push processed data out
        }
        catch( ... )
        {
        }
    }
}

void
MainThread::finalise() 
{
}

////////////////////

void
MainThread::readSettings()
{
    // read in settings from a config file
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";
   
    //connect with a proxy and fetch the incoming description object
    orcaice::connectToInterfaceWithTag<orca::MultiCameraPrx>( context_
            , incomingPrx_
            , "MultiCamera"
            , this
            , subsysName()
            );

    incomingDescr_ = incomingPrx_->getDescription();

    context_.tracer().info( orcaobj::toString( incomingDescr_ ) );

    pluginConfig_.width  = incomingDescr_->width;
    pluginConfig_.height = incomingDescr_->height;
    pluginConfig_.size   = incomingDescr_->width * incomingDescr_->height;
    pluginConfig_.format = "GRAY8"
    pluginConfig_.shifts = orcaice::getPropertyAsIntWithDefault( prop, prefix+"Shifts", 16 ); 
    pluginConfig_.offset = orcaice::getPropertyAsIntWithDefault( prop, prefix+"Offset", 0 );
    
}

void 
MainThread::initPluginInterface()
{
    subStatus().setMaxHeartbeatInterval( 20.0 );

    //copy from description to config

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    // Dynamically load the library and find the factory
    std::string driverLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libHydroDisparitySimple.so" );
    context_.tracer().info( "MainThread: Loading driver library "+driverLibName  );

    // The factory which creates the driver
    std::auto_ptr<hydrointerfaces::DisparityFactory> driverFactory;
    try {
        driverLib_.reset( new hydrodll::DynamicallyLoadedLibrary(driverLibName) );
        driverFactory.reset( 
            hydrodll::dynamicallyLoadClass<hydrointerfaces::DisparityFactory,DriverFactoryMakerFunc>
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
        catch ( IceUtil::Exception &e ) {
            exceptionSS << "MainThread: Caught exception while creating driver: " << e;
        }
        catch ( std::exception &e ) {
            exceptionSS << "MainThread: Caught exception while initialising driver: " << e.what();
        }
        catch ( char *e ) {
            exceptionSS << "MainThread: Caught exception while initialising driver: " << e;
        }
        catch ( std::string &e ) {
            exceptionSS << "MainThread: Caught exception while initialising driver: " << e;
        }
        catch ( ... ) {
            exceptionSS << "MainThread: Caught unknown exception while initialising driver";
        }

        // we get here only after an exception was caught
        context_.tracer().error( exceptionSS.str() );
        subStatus().fault( exceptionSS.str() );          

        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));        
    }

    // copy the config structure to the outgoing description structure
    outgoingDescr_->width = pluginConfig_.width;
    outgoingDescr_->height = pluginConfig_.height;
    outgoingDescr_->format = pluginConfig_.format;
    outgoingDescr_->size = pluginConfig_.size;

    subStatus().setMaxHeartbeatInterval( 1.0 );

}

void
MainThread::initNetworkInterface()
{
    active( context_, this, subsysName() );

    // incoming network interface
    incomingInterface_ = new orcaifaceimpl::BufferedMultiCameraConsumerImpl( 1
            , gbxiceutilacfr::BufferTypeCircular
            , context_ 
            );

    incomingInterface_->subscribeWithTag( "MultiCamera"
            , this
            , subsysName() 
            );

    // outgoing network interface
    outgoingInterface_ = new orcaifaceimpl::ImageImpl( outgoingDescr_
        , "Disparity"
        , context_ 
        );

    outgoingInterface_->initInterface( this, subsysName() );

}


