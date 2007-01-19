/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef LOCALNAVDRIVER_H
#define LOCALNAVDRIVER_H

#include <orca/velocitycontrol2d.h>
#include <orca/rangescanner2d.h>
#include <orcaice/context.h>
#include <orca/vehicledescription.h>

namespace localnav {

// fwd decl
class Goal;

//
// @author Alex Brooks
//
// Base class for local navigation drivers.
// The manager sets the goal location (in the robot's coordinate system)
// by modifying the goal.
//
class IDriver
{

public: 

    IDriver() {};
    virtual ~IDriver() {};

    //
    // Sets cmd.
    //
    virtual void getCommand( bool                               stalled,
                             const orca::Twist2d               &currentVelocity,
                             const orca::RangeScanner2dDataPtr  obs,
                             const Goal                        &goal,
                             orca::VelocityControl2dData       &cmd ) = 0;

protected: 

};

// Helper class to instantiate drivers
class DriverFactory {
public:
    virtual ~DriverFactory() {};
    virtual IDriver *createDriver( const orcaice::Context &context,
                                   const orca::VehicleDescription &descr ) const=0;
};

} // namespace

// Function for dynamically instantiating drivers.
// A driver must have a function like so:
// extern "C" {
//     localnav::DriverFactory *createDriverFactory();
// }
typedef localnav::DriverFactory *DriverFactoryMakerFunc();

#endif