/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <memory>
#include <hydroiceutil/hydroiceutil.h>
#include <orcaice/context.h>
// remote interfaces
#include <orcaifaceimpl/insImpl.h>
#include <orcaifaceimpl/gpsImpl.h>
#include <orcaifaceimpl/imuImpl.h>

namespace insgps {

//
// @brief the main executing loop of this insgps component.
//
class MainThread : public gbxsickacfr::gbxiceutilacfr::SubsystemThread
{

public:

    MainThread( const orcaice::Context &context );
    virtual ~MainThread();

    // from SubsystemThread
    virtual void walk();

private:
    // Loops until established
    void initNetworkInterface();

    // provided interfaces
    orcaifaceimpl::InsImplPtr insInterface_;
    orcaifaceimpl::GpsImplPtr gpsInterface_;
    orcaifaceimpl::ImuImplPtr imuInterface_;

    // Thread talking to hardware
    IceUtil::Handle<class HwThread> hwThread_;

    orcaice::Context context_;
};

} // namespace

#endif
