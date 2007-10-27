/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_DETAIL_LOCAL_STATUS_H
#define ORCAICE_DETAIL_LOCAL_STATUS_H

#include <hydroutil/status.h>
#include <hydroutil/uncopyable.h>
#include <hydroutil/timer.h>

#include <IceUtil/Mutex.h>
#include <map>

namespace orcaice
{
namespace detail
{

class StatusI;

class LocalStatus : public hydroutil::Status,
                    public hydroutil::Uncopyable
{
public:

    //////////////////////////////////////////////////////////////////////
    struct SubsystemStatus
    {
        SubsystemStatusType type;
        std::string message;
        hydroutil::Timer heartbeatTimer;
        double maxHeartbeatInterval;
    };
    //////////////////////////////////////////////////////////////////////

    // If statusI is NULL, an orca::Status interface will not be established
    LocalStatus( const orcaice::Context& context,
                 StatusI *statusI );
    virtual ~LocalStatus();
    
    // from hydroutil::Status

    // Must be called before notifcations of the subsytem's status.
    // Status will flag an error if the subsystem's status is not updated
    // in more than maxHeartbeatInterval seconds.
    virtual void setMaxHeartbeatInterval( const std::string& subsystem,
                                          double maxHeartbeatIntervalSec );

    virtual void heartbeat( const std::string& subsystem );
    
    virtual void initialising( const std::string& subsystem, const std::string& message="" );

    virtual void ok( const std::string& subsystem, const std::string& message="" );

    virtual void warning( const std::string& subsystem, const std::string& message );

    virtual void fault( const std::string& subsystem, const std::string& message );

    // inherited from Status
    // alexm: it's public status will probably change.
    virtual void process();

private:
    // utility
    void setSubsystemStatus( const std::string& subsystem,
                             SubsystemStatusType type,
                             const std::string& message );


    orcaice::Context context_;

    // We only have one communicator but may have multiple threads, have
    // to protect ourself from simultaneous access
    IceUtil::Mutex mutex_;

    std::map<std::string,SubsystemStatus> subsystems_;

    StatusI *statusI_;

    bool statusTouched_;
    hydroutil::Timer publishTimer_;
//     IceUtil::Time lastPublishTime_;
    double publishPeriodSec_;
};

} // namespace
} // namespace

#endif
