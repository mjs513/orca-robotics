/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef __featuremap2dI_h__
#define __featuremap2dI_h__

#include <orca/featuremap2d.h>
#include <orcaice/orcaice.h>

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/featuremap2d.h>

namespace featuremaploader {

//
// Implements this interface: Handles all our remote calls.
//
class FeatureMap2dI : public orca::FeatureMap2d
{
public:

    FeatureMap2dI( orca::FeatureMap2dDataPtr  theMap,
                   const std::string         &tag,
                   orcaice::Context           context );

    // Remote calls:

    // Hand out data to people
    orca::FeatureMap2dDataPtr getData(const Ice::Current&) const;

    // We ignore this request
    virtual void subscribe(const ::orca::FeatureMap2dConsumerPrx&,
                           const Ice::Current&);

    // We ignore this request
    virtual void unsubscribe(const ::orca::FeatureMap2dConsumerPrx&,
                             const Ice::Current&);

private:

    // The topic to which we'll publish
    IceStorm::TopicPrx             topicPrx_;
    // The interface to which we'll publish
    orca::FeatureMap2dConsumerPrx  consumerPrx_;

    orca::FeatureMap2dDataPtr theMap_;

    orcaice::Context context_;
};

}

#endif
