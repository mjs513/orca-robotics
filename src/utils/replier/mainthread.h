/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef MAINTHREAD_H
#define MAINTHREAD_H

#include <gbxsickacfr/gbxiceutilacfr/safethread.h>
#include <orcaice/context.h>

namespace replier
{

class MainThread: public gbxsickacfr::gbxiceutilacfr::SafeThread
{    	
public:
    MainThread( const orcaice::Context& context );

    virtual void walk();

private:
    orcaice::Context context_;

};

} // namespace
#endif
