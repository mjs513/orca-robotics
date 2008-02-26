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
#include "hwthread.h"

using namespace std;
using namespace insgps;

HwThread::HwThread( const orcaice::Context &context) :
    hydroiceutil::SubsystemThread( context.tracer(), context.status(), "HwThread" ),
    dataPipe_(new hydroiceutil::EventQueue),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 20.0 );

    // Read settings
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    if ( !config_.isValid() ) {
        context_.tracer().error( "Failed to validate insgps configuration. "+config_.toString() );
        // this will kill this component
        throw hydroutil::Exception( ERROR_INFO, "Failed to validate insgps configuration" );
    }
}


void
HwThread::initHardwareDriver()
{
    subStatus().setMaxHeartbeatInterval( 20.0 );

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    // Dynamically load the library and find the factory
    std::string driverLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libOrcaInsGpsFake.so" );
    context_.tracer().debug( "HwThread: Loading driver library "+driverLibName, 4 );
    // The factory which creates the driver
    std::auto_ptr<hydrointerfaces::InsGpsFactory> driverFactory;
    try {
        driverLib_.reset( new hydrodll::DynamicallyLoadedLibrary(driverLibName) );
        driverFactory.reset( 
            hydrodll::dynamicallyLoadClass<hydrointerfaces::InsGpsFactory,DriverFactoryMakerFunc>
            ( *driverLib_, "createDriverFactory" ) );
    }
    catch(hydrodll::DynamicLoadException &e){
        // unrecoverable error
        context_.tracer().error( e.what() );
        context_.shutdown();
        throw;
    }

    // create the driver
    while(!isStopping()){
        std::stringstream exceptionSS;
        try {
            context_.tracer().info( "HwThread: Creating driver..." );
            driver_.reset( 0 ); // if this is a reset, we have to destroy the old driver first
            driver_.reset( driverFactory->createDriver( config_, context_.toHydroContext() ) );
            break;
        }
        catch ( IceUtil::Exception &e ) {
            exceptionSS << "HwThread: Caught exception while creating driver: " << e;
        }
        catch ( std::exception &e ) {
            exceptionSS << "HwThread: Caught exception while initialising driver: " << e.what();
        }
        catch ( char *e ) {
            exceptionSS << "HwThread: Caught exception while initialising driver: " << e;
        }
        catch ( std::string &e ) {
            exceptionSS << "HwThread: Caught exception while initialising driver: " << e;
        }
        catch ( ... ) {
            exceptionSS << "HwThread: Caught unknown exception while initialising driver";
        }

        // we get here only after an exception was caught
        context_.tracer().error( exceptionSS.str() );
        subStatus().fault( exceptionSS.str() );

        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    }

    subStatus().setMaxHeartbeatInterval( 1.0 );
}

void
HwThread::walk()
{
    initHardwareDriver();

    //
    // IMPORTANT: Have to keep this loop rolling, because the '!isStopping()' call checks for requests to shut down.
    //            So we have to avoid getting stuck anywhere within this main loop.
    //
    while(!isStopping()){
        stringstream exceptionSS;
        try{
            namespace hif = hydrointerfaces;

            std::auto_ptr<hif::InsGps::GenericData> generic;
            hydroiceutil::EventPtr e;

            generic = driver_->read();
            // figure out what we got; convert the data to orcainterfaces
            // and the container to Event; then shove it over to the network side
            switch(generic->type()){
                case hif::InsGps::Ins:
                {
                    hif::InsGps::InsData *insData = 
                        dynamic_cast<hif::InsGps::InsData *>( generic.get() );
                    assert( insData != NULL );
                    e = new OrcaInsEvent(*insData);
                    dataPipe_->add(e);
                    break;
                }
                case hif::InsGps::Gps:
                {
                    hif::InsGps::GpsData *gpsData = 
                        dynamic_cast<hif::InsGps::GpsData *>( generic.get() );
                    assert( gpsData != NULL );
                    e = new OrcaGpsEvent(*gpsData);
                    dataPipe_->add(e);
                    break;
                }
                case hif::InsGps::Imu:
                {
                    hif::InsGps::ImuData *imuData = 
                        dynamic_cast<hif::InsGps::ImuData *>( generic.get() );
                    assert( imuData != NULL );
                    e = new OrcaImuEvent(*imuData);
                    dataPipe_->add(e);
                    break;
                }
                default:
                    break;
            }
            // report status info
            switch(generic->statusMessageType){
                case hif::InsGps::Ok:
                    subStatus().ok( generic.get()->statusMessage );
                    break;
                case hif::InsGps::Warning:
                    subStatus().warning( generic.get()->statusMessage );
                    break;
                case hif::InsGps::Fault:
                    subStatus().fault( generic.get()->statusMessage );
                    break;
                case hif::InsGps::NoMsg:
                    subStatus().heartbeat();
                    break;
                default:
                    //shouldn't happen
                    //do nothing; should we whinge about some unhandled status??
                    break;
            }

            continue;

        } // end of try
        catch ( Ice::CommunicatorDestroyedException & ) {
            // This is OK: it means that the communicator shut down (eg via Ctrl-C)
            // somewhere in mainLoop. Eventually, component will tell us to stop.
        }
        catch ( const Ice::Exception &e ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected exception: " << e;
        }
        catch ( const std::exception &e ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected exception: " << e.what();
        }
        catch ( const std::string &e ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected string: " << e;
        }
        catch ( const char *e ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected char *: " << e;
        }
        catch ( ... ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected unknown exception.";
        }

        if ( !exceptionSS.str().empty() ) {
            context_.tracer().error( exceptionSS.str() );
            subStatus().fault( exceptionSS.str() );     
            // Slow things down in case of persistent error
            sleep(1);
        }

        // If we got to here there's a problem.
        // Re-initialise the driver.
        initHardwareDriver();

    } // end of while

    // insgps hardware will be shut down in the driver_ 's destructor.
}

OrcaInsEvent::OrcaInsEvent(const hydrointerfaces::InsGps::InsData &hydroIns)
    : hydroiceutil::Event( OrcaIns )
{
    data.latitude = hydroIns.lat;
    data.longitude = hydroIns.lon;
    data.altitude = hydroIns.alt;
    data.heightAMSL = hydroIns.altAMSL;
    data.vENU.x = hydroIns.vENU[0];
    data.vENU.y = hydroIns.vENU[1];
    data.vENU.z = hydroIns.vENU[2];
    data.o.r = hydroIns.oRPY[0];
    data.o.p = hydroIns.oRPY[1];
    data.o.y = hydroIns.oRPY[2];
    data.timeStamp.seconds = hydroIns.time.tv_sec;
    data.timeStamp.useconds = hydroIns.time.tv_usec;
    return;
}

OrcaGpsEvent::OrcaGpsEvent(const hydrointerfaces::InsGps::GpsData &hydroGps)
    : hydroiceutil::Event( OrcaGps )
{
    data.latitude = hydroGps.lat;
    data.longitude = hydroGps.lon;
    data.altitude = hydroGps.alt;
    data.horizontalPositionError = hydroGps.hDop;
    data.verticalPositionError = hydroGps.vDop;
    data.heading = hydroGps.heading;
    data.speed = hydroGps.speed;
    data.climbRate = hydroGps.climbRate;
    data.satellites = hydroGps.sat;
    data.observationCountOnL1 = hydroGps.obsL1;
    data.observationCountOnL2 = hydroGps.obsL2;
    data.geoidalSeparation = data.altitude - hydroGps.altAMSL;
    data.timeStamp.seconds = hydroGps.time.tv_sec;
    data.timeStamp.useconds = hydroGps.time.tv_usec;
    return;
}

OrcaImuEvent::OrcaImuEvent(const hydrointerfaces::InsGps::ImuData &hydroImu)
    : hydroiceutil::Event( OrcaImu )
{
    data.accel.x = hydroImu.acc[0];
    data.accel.y = hydroImu.acc[1];
    data.accel.z = hydroImu.acc[2];
    data.gyro.x = hydroImu.turnRate[0];
    data.gyro.y = hydroImu.turnRate[1];
    data.gyro.z = hydroImu.turnRate[2];
    //data.?? = hydroImu.tempr;
    data.timeStamp.seconds = hydroImu.time.tv_sec;
    data.timeStamp.useconds = hydroImu.time.tv_usec;
    return;
}