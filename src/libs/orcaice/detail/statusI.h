/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_DETAIL_STATUS_I_H
#define ORCAICE_DETAIL_STATUS_I_H

#include <orca/status.h>
#include <hydroiceutil/localstatus.h>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>

#include <IceStorm/IceStorm.h>
#include <IceUtil/Mutex.h>

namespace orcaice
{
namespace detail
{

// An implementation of the (remote) Status interface.
class StatusI : public virtual orca::Status, public hydroiceutil::LocalStatus
{
public:
    
    StatusI( const orcaice::Context & context );
    virtual ~StatusI();

    // from orca::Status interface: remote calls
    virtual ::orca::StatusData getData(const ::Ice::Current& ) const;
    virtual void subscribe(const ::orca::StatusConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void unsubscribe(const ::orca::StatusConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // from LocalStatus
    virtual void publish();

private:

    bool connectToIceStorm();
    void icestormConnectFailed( const std::string &topicName,
                                orca::StatusConsumerPrx &publisher,
                                bool isStatusTopicRequired );

    void setStatusData( const hydroiceutil::NameStatusMap &subsystemStatus );
    void sendToIceStorm( const orca::StatusData &statusData );

    IceStorm::TopicPrx topic_;
    orca::StatusConsumerPrx publisher_;
    std::string topicName_;
    bool isStatusTopicRequired_;
    mutable orca::StatusData statusData_;

    // Protect from simultaneous get/set of statusData_
    IceUtil::Mutex mutex_;
    gbxsickacfr::gbxiceutilacfr::Timer upTimer_;
    orcaice::Context context_;
};

} // namespace
} // namespace

#endif
