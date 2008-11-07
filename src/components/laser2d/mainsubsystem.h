/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef MAIN_SUBSYSTEM_H
#define MAIN_SUBSYSTEM_H

#include <memory>
#include <orcaice/subsystem.h>
#include <hydrodll/dynamicload.h>
// remote interface
#include <orcaifaceimpl/laserscanner2dImpl.h>
// hardware interface
#include <hydrointerfaces/laserscanner2d.h>

namespace laser2d {

class MainSubsystem : public orcaice::Subsystem
{
public:
    MainSubsystem( const orcaice::Context &context );

private:
    // from Subsystem
    virtual void initialise();
    virtual void work();
    virtual void finalise();

    void initSettings();

    // Tries repeatedly to instantiate the driver
    void initHardwareDriver();

    // Loops until established
    void initNetworkInterface();

    void readData();

    // The laser object
    orcaifaceimpl::LaserScanner2dImplPtr laserInterface_;

    hydrointerfaces::LaserScanner2d::Config config_;
    // an extra config to allow sensor mounted upside-down
    bool compensateRoll_;

    // space for data
    orca::LaserScanner2dDataPtr           orcaLaserData_;
    hydrointerfaces::LaserScanner2d::Data hydroLaserData_;

    // The library that contains the driver factory (must be declared first so it's destructed last!!!)
    std::auto_ptr<hydrodll::DynamicallyLoadedLibrary> driverLib_;
    // Generic driver for the hardware
    std::auto_ptr<hydrointerfaces::LaserScanner2d> driver_;
};

} // namespace

#endif