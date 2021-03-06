/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAPROBEFACTORY_PATHFOLLOWER2D_INTERFACE_PROBE_H
#define ORCA_ORCAPROBEFACTORY_PATHFOLLOWER2D_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orca/pathfollower2d.h>

namespace orcaprobefactory
{

class PathFollower2dProbe : public orca::PathFollower2dConsumer, public orcaprobe::InterfaceProbe
{

public:

    PathFollower2dProbe( const orca::FQInterfaceName& name, 
                         const Ice::ObjectPrx&        adminPrx, 
                         orcaprobe::AbstractDisplay&  display,
                         const orcaice::Context&      context );

    virtual int loadOperationEvent( const int index, orcacm::OperationData & data );
    
    virtual void setData(const orca::PathFollower2dData & data, const Ice::Current&);
    virtual void setState( const orca::PathFollower2dState &state, const Ice::Current&);

private:

    int loadGetData( orcacm::OperationData & data );
    int loadSubscribe( orcacm::OperationData & data );
    int loadUnsubscribe( orcacm::OperationData & data );

};

} // namespace

#endif
