/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_COMPONENT_H
#define ORCA2_SEGWAY_RMP_COMPONENT_H

#include <orcaice/component.h>

namespace segwayrmp
{

class Component : public orcaice::Component
{
public:

    Component();

    // from Component
    virtual void start();
    virtual void stop();

private:

    // thread responsible for interaction with the network
    hydroutil::ThreadPtr netThread_;
    // thread responsible for interaction with local hardware
    hydroutil::ThreadPtr hwThread_;
};

} // namespace

#endif
