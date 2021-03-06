/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHPLANNER2DI_H
#define PATHPLANNER2DI_H

#include <IceStorm/IceStorm.h>

#include <orca/pathplanner2d.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <gbxsickacfr/gbxiceutilacfr/buffer.h>

namespace pathplanner
{

class PathPlanner2dI : public orca::PathPlanner2d
{
public:
    PathPlanner2dI( gbxiceutilacfr::Buffer<orca::PathPlanner2dTask> &pathPlannerTaskStore,
                    const orcaice::Context                     &context );

    // remote calls
    virtual ::Ice::Int setTask(const ::orca::PathPlanner2dTask&, const ::Ice::Current& = ::Ice::Current());

    virtual IceStorm::TopicPrx subscribe(const ::orca::PathPlanner2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual ::orca::PathPlanner2dData getData(const ::Ice::Current&);

    // local calls
    void localSetData( const ::orca::PathPlanner2dData& data );

private:

    gbxiceutilacfr::Buffer<orca::PathPlanner2dTask>& pathPlannerTaskBuffer_;

    // the driver puts the latest computed path into here using localSetData
    gbxiceutilacfr::Store<orca::PathPlanner2dData> pathPlannerDataStore_;

    // The topic to which we'll publish
    IceStorm::TopicPrx topicPrx_;
    
    // The interface to which we'll publish
    orca::PathPlanner2dConsumerPrx  consumerPrx_;

    orcaice::Context context_;

};

} // namespace

#endif
