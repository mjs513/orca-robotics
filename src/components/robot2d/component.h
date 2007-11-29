/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ROBOT2D_COMPONENT_H
#define ORCA2_ROBOT2D_COMPONENT_H

#include <orcaice/component.h>

namespace robot2d
{

class Component : public orcaice::Component
{
public:

    Component();

    // from Component
    virtual void start();
    virtual void stop();

private:
    // loop responsible for interaction with the network
    hydroutil::ThreadPtr NetThread_;
    // loop responsible for interaction with local hardware
    hydroutil::ThreadPtr HwThread_;
};

} // namespace

#endif
