/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef NETWORK_THREAD_H
#define NETWORK_THREAD_H

#include <orca/tracer.h>
#include <gbxsickacfr/gbxiceutilacfr/safethread.h>
#include <orcaice/context.h>
#include <hydroiceutil/eventqueue.h>
#include "user.h"
#include "network.h"

namespace tracermon
{

class NetworkThread : public gbxiceutilacfr::SafeThread, public Network
{
public:

    NetworkThread( User* user, const orcaice::Context & context );

    // from Network
    virtual void setVerbosityLevel( int error, int warn, int info, int debug );

private:
    // from SafeThread
    virtual void walk();

    User* user_;
    hydroiceutil::EventQueuePtr events_;

    orcaice::Context context_;
    
    orca::TracerPrx tracerPrx_;

    void setRemoteVerbosity( int error, int warn, int info, int debug );

};

} // namespace

#endif
