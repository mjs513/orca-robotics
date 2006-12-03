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
#include <fstream>
#include <sstream>
#include <IceUtil/Time.h>
#include <orcaice/orcaice.h>
#include <orcalog/exceptions.h>
#include "logmaster.h"
#include "utils.h"

using namespace std;
using namespace orcalog;


LogMaster::LogMaster( const std::string &filename, const orcaice::Context & context )
    : logCounter_(-1),
      dataCounter_(0),
      isStarted_(false),
      context_(context)
{
    // create master file
    file_ = new ofstream( filename.c_str() );
    if ( !file_->is_open() ) {
        context_.tracer()->warning( "Could not create master file " + filename );
        throw orcalog::FileException( ERROR_INFO, "Could not create master file " + filename );
    }
    
    // write some global information as comments to the header
    (*file_) << "# == Component ==" << endl;
    (*file_) << "# Logger host    : " << orcaice::getHostname() << endl;
    (*file_) << "# Log start time : " << IceUtil::Time::now().toString() << endl;
    (*file_) << "# Ice version    : " << ICE_STRING_VERSION << endl;
    (*file_) << "# Orca version   : " << orcaice::orcaVersion() << endl;
    (*file_) << "# " << endl;
    (*file_) << "# == Interfaces / Logs Legend ==" << endl;
    (*file_) << "# interface_name" << endl;
    (*file_) << "# log_filename interface_type log_format" << endl;
    (*file_) << "# " << endl;
    (*file_) << "# == Interfaces / Logs ==" << endl;
}

LogMaster::~LogMaster()
{
    std::stringstream ss;
    ss << "shutting down after writing "<<dataCounter_<<" objects to "<<(logCounter_+1)<<" logs.";
    context_.tracer()->info( ss.str() );

    if ( file_ ) {
        file_->close();
        delete file_;
    }
}

int
LogMaster::addLog( const std::string & filename, 
                    const std::string & interfaceType,
                    const std::string & format,
                    const std::string & proxyString )
{
    context_.tracer()->debug( 
        "adding log: file="+filename+" id="+interfaceType+" fmt="+format+" prx="+proxyString, 5 );

    IceUtil::Mutex::Lock lock(mutex_);

    if ( isStarted_ ) {
        context_.tracer()->warning("Can't register after started appending!");
        throw orcalog::Exception( ERROR_INFO, "Can't register after started appending!" );
    }

    // this is just a comment 
    (*file_) << "# " << proxyString << endl;

    // this will be used replay this log back
    (*file_) << orcalog::headerLine( filename, interfaceType, format ) << endl;

    // increment unique logger id
    ++logCounter_;

    return logCounter_;
}

void 
LogMaster::addData( int seconds, int useconds, int id, int index )
{
    IceUtil::Mutex::Lock lock(mutex_);

    if ( !isStarted_ ) {
        // Write the beginning of data field (end of the header)
        (*file_) << "# " << endl;
        (*file_) << "# == Data Legend ==" << endl;
        (*file_) << "# seconds useconds file_id object_id" << endl;
        (*file_) << "# " << endl;
        (*file_) << orcalog::endOfHeader() << endl;
        isStarted_ = true;
    }

    //
    // write to master file
    //
    (*file_) << orcalog::dataLine( seconds, useconds, id, index ) << endl;

    // count data objects
    ++dataCounter_;
}
