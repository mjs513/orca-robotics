/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef NETWORKTRACESENDER_H
#define NETWORKTRACESENDER_H

#include <IceStorm/IceStorm.h>
#include <IceUtil/Mutex.h>
#include <orca/tracer.h>
#include <orcaice/context.h>

namespace orcaice {
namespace detail {

//
// Knows how to send trace messages to an IceStorm topic.
//
// Note that this does _not_ inherit from gbxsickacfr::gbxutilacfr::Tracer, it's simply
// a convenience class for use by gbxsickacfr::gbxutilacfr::Tracer classes.
//
// @author Alex Brooks
//
class NetworkTraceSender
{

public: 

    NetworkTraceSender( const std::string &topicName,
                        IceUtil::Mutex &mutex,
                        const orcaice::Context &context );

    // sub/unsub subscribers to the topic we're publishing to
    void subscribe( const ::orca::TracerConsumerPrx& subscriber );
    void unsubscribe( const ::orca::TracerConsumerPrx& subscriber );

    // Returns: true = success.
    bool connectToIceStorm();

    void sendToNetwork( const orca::TracerData &tracerData );

private: 

    IceStorm::TopicPrx topicPrx_;
    orca::TracerConsumerPrx publisher_;

    std::string topicName_;

    // We only have one communicator but may have multiple threads.
    IceUtil::Mutex &mutex_;

    orcaice::Context context_;
};

}
}

#endif
