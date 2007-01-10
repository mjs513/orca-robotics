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

#include "odometry2dlogger.h"

using namespace std;
using namespace orcalogfactory;

Odometry2dLogger::Odometry2dLogger(orcalog::LogMaster *master, 
                        const std::string & typeSuffix,
                        const std::string & format,
                        const std::string & filenamePrefix,
                        const orcaice::Context & context )
    : orcalog::Logger( master, 
             "Odometry2d",
             "Odometry2d"+typeSuffix,
             format,
             filenamePrefix+"odometry2d"+typeSuffix+".log",
             context )
{
    // check that we support this format
    if ( format_!="ice" && format_!="ascii" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}

Odometry2dLogger::~Odometry2dLogger()
{
}

void 
Odometry2dLogger::init()
{   
    // throws orcaice::NetworkException but it's ok to quit here
    orca::Odometry2dPrx objectPrx;
    orcaice::connectToInterfaceWithTag<orca::Odometry2dPrx>( context_, objectPrx, interfaceTag_ );
    
    // Config and geometry
    orca::Odometry2dDescription descr = objectPrx->getDescription();
    writeDescriptionToFile( descr );

    // consumer
    Ice::ObjectPtr consumer = this;
    orca::Odometry2dConsumerPrx callbackPrx = 
        orcaice::createConsumerInterface<orca::Odometry2dConsumerPrx>( context_, consumer );

    context_.tracer()->debug("Subscribing to IceStorm now.",5);
    objectPrx->subscribe( callbackPrx );
}

void 
Odometry2dLogger::setData(const orca::Odometry2dData& data, const Ice::Current&)
{
    // Write reference to master file
    appendMasterFile( data.timeStamp.seconds, data.timeStamp.useconds );
        
    if ( format_ == "ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writeOdometry2dData( helper.stream_, data );
        helper.write( file_ );
    }
    else if ( format_ == "ascii" )
    {
        (*file_) << orcalog::toLogString(data) << endl;
    }
    else
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}

void 
Odometry2dLogger::writeDescriptionToFile( const orca::Odometry2dDescription& obj )
{
    context_.tracer()->print( "Writing description to file" );
    
    if ( format_ == "ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writeOdometry2dDescription( helper.stream_, obj );
        helper.write( file_ ); 
    }
    else if ( format_ == "ascii" )
    {
        (*file_) << "Implement streaming Odometry2dDescription" << endl;
//         (*file_) << orcaice::toString(obj) << endl;
    }
    else
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
} 
