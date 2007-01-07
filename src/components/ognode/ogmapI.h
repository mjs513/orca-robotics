/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef _OG_MAP_I_H_
#define _OG_MAP_I_H_

#include <orca/ogmap.h>

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/ogmap.h>

// utilities
#include <orcaice/ptrbuffer.h>
#include <orcaice/context.h>

namespace ognode
{

//
// Implements the Laser interface: Handles all our remote calls.
//
class OgMapI : public orca::OgMap
{
public:

    OgMapI( const orca::OgMapData& theMap,
            const std::string  &tag,
            const orcaice::Context & context );

    // Remote calls:

    // Hand out data to people
    orca::OgMapData getData(const Ice::Current&) const;

    void localSetData( const orca::OgMapData& data);

    virtual void subscribe(const ::orca::OgMapConsumerPrx&,
                           const Ice::Current&);

    virtual void unsubscribe(const ::orca::OgMapConsumerPrx&,
                             const Ice::Current&);

private:

    // The topic to which we'll publish
    IceStorm::TopicPrx             topicPrx_;
    // The interface to which we'll publish
    orca::OgMapConsumerPrx         consumerPrx_;

    // The latest data goes into this buffer
    orcaice::Buffer<orca::OgMapData> ogMapDataBuffer_;

    orcaice::Context context_;
};

} // namespace

#endif
