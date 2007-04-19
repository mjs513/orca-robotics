/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_IMU_SERVICE_H
#define ORCA2_IMU_SERVICE_H

#include <orcaice/service.h>

namespace imu {

//
// Class to build the component into an icebox service
//
class Service : public orcaice::Service
{

public:
    Service();

};

} //namespace

#endif
