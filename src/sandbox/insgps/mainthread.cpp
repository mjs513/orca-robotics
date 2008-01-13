/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include "mainthread.h"
#include "hwhandler.h"

using namespace std;
using namespace insgps;


MainThread::MainThread( const orcaice::Context &context ) :
    hydroiceutil::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    dataPipe_(new hydroiceutil::EventQueue),
    hwHandler_(new HwHandler(context, dataPipe_)),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 20.0 );

    // Read settings
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    if ( !config_.validate() ) {
        context_.tracer().error( "Failed to validate insgps configuration. "+config_.toString() );
        // this will kill this component
        throw hydroutil::Exception( ERROR_INFO, "Failed to validate insgps configuration" );
    }
}

/*
MainThread::~MainThread()
{
    context_.tracer().debug( "stopping mainThread", 5 );
    hydroiceutil::stopAndJoin( hwHandler_ );
    context_.tracer().debug( "stopped mainThread", 5 );
    return;
}
*/

void
MainThread::initNetworkInterface()
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    // SENSOR DESCRIPTION
    orca::InsDescription insDescr;
    orca::GpsDescription gpsDescr;
    orca::ImuDescription imuDescr;

    insDescr.timeStamp = orcaice::getNow();
    gpsDescr.timeStamp = orcaice::getNow();
    imuDescr.timeStamp = orcaice::getNow();

    // transfer internal sensor configs
    orcaice::setInit(insDescr.offset);
    orcaice::setInit(gpsDescr.antennaOffset);
    orcaice::setInit(imuDescr.offset);
    orcaice::setInit(imuDescr.size);

    insDescr.offset = orcaice::getPropertyAsFrame3dWithDefault( prop, prefix+"Ins.Offset", insDescr.offset );
    gpsDescr.antennaOffset = orcaice::getPropertyAsFrame3dWithDefault( prop, prefix+"Gps.AntennaOffset", gpsDescr.antennaOffset );
    imuDescr.offset = orcaice::getPropertyAsFrame3dWithDefault( prop, prefix+"Imu.Offset", imuDescr.offset );
    imuDescr.size = orcaice::getPropertyAsSize3dWithDefault( prop, prefix+"Imu.Size", imuDescr.size );

    // EXTERNAL PROVIDED INTERFACES
    insInterface_ = new orcaifaceimpl::InsImpl( insDescr, "Ins", context_ );
    gpsInterface_ = new orcaifaceimpl::GpsImpl( gpsDescr, "Gps", context_ );
    imuInterface_ = new orcaifaceimpl::ImuImpl( imuDescr, "Imu", context_ );

    insInterface_->initInterface( this, subsysName() );
    gpsInterface_->initInterface( this, subsysName() );
    imuInterface_->initInterface( this, subsysName() );
}

void
MainThread::walk()
{
    // These functions catch their exceptions.
    activate( context_, this, subsysName() );
    initNetworkInterface();
    hwHandler_->start();

    //
    // IMPORTANT: Have to keep this loop rolling, because the '!isStopping()' call checks for requests to shut down.
    //            So we have to avoid getting stuck anywhere within this main loop.
    //
    while(!isStopping()){
        stringstream exceptionSS;
        try{
            hydroiceutil::EventPtr e;

            // this blocks until new data arrives
            if(true == dataPipe_->timedGet(e,1000)){
                // determine what we've got and shove it of to IceStorm
                switch(e->type()){
                    case OrcaIns:
                        insInterface_->localSetAndSend((OrcaInsEventPtr::dynamicCast(e))->data);
                        break;
                    case OrcaGps:
                        gpsInterface_->localSetAndSend((OrcaGpsEventPtr::dynamicCast(e))->data);
                        break;
                    case OrcaImu:
                        imuInterface_->localSetAndSend((OrcaImuEventPtr::dynamicCast(e))->data);
                        break;
                    default:
                        break;
                }
            }else{
                subStatus().warning( "timed out read on hwHandler" );
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
    } // end of while
    hydroiceutil::stopAndJoin( hwHandler_ );
}
