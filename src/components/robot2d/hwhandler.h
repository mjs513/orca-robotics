/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ROBOT2D_HARDWARE_HANDLER_H
#define ORCA2_ROBOT2D_HARDWARE_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/context.h>
#include <orcaice/notify.h>
#include <orcaice/proxy.h>
#include <orcaice/timer.h>

#include "hwdriver.h"

#include <orca/velocitycontrol2d.h>
#include <orca/vehicledescription.h>

#include "types.h"

namespace robot2d
{

class HwHandler : public orcaice::Thread, public orcaice::NotifyHandler<orca::VelocityControl2dData>
{
public:

    HwHandler( orcaice::Buffer<Data>& dataPipe,
               orcaice::Notify<orca::VelocityControl2dData>& commandPipe,
               const orca::VehicleDescription &descr,
               const orcaice::Context& context );
    virtual ~HwHandler();

    // from Thread
    virtual void run();

    // from PtrNotifyHandler
    virtual void handleData( const orca::VelocityControl2dData & obj );

private:

    // network/hardware interface
    orcaice::Buffer<Data>& dataPipe_;

    // generic interface to the hardware
    HwDriver* driver_;

    struct Config
    {
        bool isMotionEnabled;
        double maxSpeed;
        double maxSideSpeed;
        double maxTurnrate;
    };
    Config config_;

    // component current context
    orcaice::Context context_;

    // write status has to be protected to be accessed from both read and write threads
    orcaice::Proxy<bool> writeStatusPipe_;

    // debug
    orcaice::Timer readTimer_;
    orcaice::Timer writeTimer_;

    // utilities
    static void convert( const orca::VelocityControl2dData& network, robot2d::Command& internal );

};

} // namespace

#endif
