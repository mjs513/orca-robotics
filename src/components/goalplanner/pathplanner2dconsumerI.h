/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PATHPLANNER_CONSUMER_I_H
#define ORCA2_PATHPLANNER_CONSUMER_I_H

#include <orca/pathplanner2d.h>
#include <orcaice/orcaice.h>
#include <orcaice/ptrproxy.h>


namespace goalplanner
{

class PathPlanner2dConsumerI : public orca::PathPlanner2dConsumer
{
public:
    PathPlanner2dConsumerI(orcaice::Proxy<orca::PathPlanner2dData> &pathBuffer);

    // remote call
    virtual void setData(const ::orca::PathPlanner2dData&, const ::Ice::Current& = ::Ice::Current());

private:
    orcaice::Proxy<orca::PathPlanner2dData> &pathBuffer_;
};

} // namespace

#endif
