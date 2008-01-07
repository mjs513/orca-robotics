/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <hydroutil/safethread.h>
#include <orcaice/context.h>

namespace registrylist
{

class MainThread: public hydroutil::SafeThread
{    	
public:
    MainThread( const orcaice::Context & context );
    ~MainThread();

    virtual void walk();

private:

    orcaice::Context context_;

};

} // namespace

#endif