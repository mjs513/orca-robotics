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

#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

#include "gpslogger.h"

using namespace std;
using namespace orcalogfactory;


GpsLogger::GpsLogger( orcalog::LogMaster *master, 
                    const std::string & typeSuffix,
                    const std::string & format,
                    const std::string & filenamePrefix,
                    const orcaice::Context & context )
    : orcalog::Logger( master, 
             "Gps",
             "Gps"+typeSuffix,
             format,
             filenamePrefix+"gps"+typeSuffix+".log",
             context )
{
    // check that we support this format
    if ( format_!="ice" ) //&& format_!="ascii" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}

GpsLogger::~GpsLogger()
{
}

void 
GpsLogger::init()
{ 
    // throws orcaice::NetworkException but it's ok to quit here
    orca::GpsPrx objectPrx;
    orcaice::connectToInterfaceWithTag<orca::GpsPrx>( context_, objectPrx, interfaceTag_ );

    // Config and geometry
    orca::GpsDescription descr = objectPrx->getDescription();
    writeDescriptionToFile( descr );
    
    // data consumer
    consumerGps_ = new GpsConsumerI(this);
    orca::GpsConsumerPrx callbackGpsPrx = 
        orcaice::createConsumerInterface<orca::GpsConsumerPrx>( context_, consumerGps_ );
    objectPrx->subscribe( callbackGpsPrx );

    consumerTime_ = new GpsTimeConsumerI(this);
    orca::GpsTimeConsumerPrx callbackTimePrx = 
        orcaice::createConsumerInterface<orca::GpsTimeConsumerPrx>( context_, consumerTime_ );
    objectPrx->subscribeForTime( callbackTimePrx );
    
    consumerMap_ = new GpsMapGridConsumerI(this);
    orca::GpsMapGridConsumerPrx callbackMapPrx = 
        orcaice::createConsumerInterface<orca::GpsMapGridConsumerPrx>( context_, consumerMap_ );
    objectPrx->subscribeForMapGrid( callbackMapPrx );
}

void 
GpsLogger::writeDescriptionToFile( const orca::GpsDescription& obj )
{
    orcalog::IceWriteHelper helper( context_.communicator() );
    ice_writeGpsDescription( helper.stream_, obj );
    helper.write( file_ );  
}

void 
GpsLogger::localSetData( const orca::GpsData& data )
{
    // Write reference to master file
    appendMasterFile();

    orcalog::IceWriteHelper helper( context_.communicator() );
    ice_writeGpsData( helper.stream_, data );
    helper.write( file_, 0 );
}


void 
GpsLogger::localSetData( const orca::GpsTimeData& data )
{
    // Write reference to master file
    appendMasterFile();
    
    orcalog::IceWriteHelper helper( context_.communicator() );
    ice_writeGpsTimeData( helper.stream_, data );
    helper.write( file_, 1 );
}

void 
GpsLogger::localSetData( const orca::GpsMapGridData& data )
{
    // Write reference to master file
    appendMasterFile();
    
    orcalog::IceWriteHelper helper( context_.communicator() );
    ice_writeGpsMapGridData( helper.stream_, data );
    helper.write( file_, 2 );
}
