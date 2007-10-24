/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOG_REPLAYER_H
#define ORCA2_ORCALOG_REPLAYER_H

#include <string>

namespace orcalog
{

//! Handles a file which contains actual data (ie not the master file).
//! All replayers inherit from this. 
class Replayer
{
public:

    virtual ~Replayer() {}

    virtual void init() {}

    //! Replays the index'th item in the log
    virtual void replayData( int index )=0;

    //! Human-readable string describing the replayer.
    virtual std::string toString() const=0;
    
};

} // namespace

#endif
