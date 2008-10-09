/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_POINT_CLOUD_IMPL_H
#define ORCA_POINT_CLOUD_IMPL_H

#include <orca/pointcloud.h>
#include <IceStorm/IceStorm.h>

// utilities
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <orcaice/context.h>

namespace gbxiceutilacfr { class Thread; }

namespace orcaifaceimpl {

//!
//! Implements the orca::PointCloud interface: Handles remote calls.
//!
class PointCloudImpl : public IceUtil::Shared
{
friend class PointCloudI;

public:
    //! constructor using interfaceTag (may throw ConfigFileException)
    PointCloudImpl( //const orca::PointCloudDescription &descr,
                    const std::string              &interfaceTag, 
                    const orcaice::Context         &context );
    //! constructor using interfaceName
    PointCloudImpl( //const orca::PointCloudDescription &descr,
                    const orcaice::Context         &context,
                    const std::string              &interfaceName );
    ~PointCloudImpl();

    // local interface:
    //! May throw gbxutilacfr::Exceptions.
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

    //! A local call which sets the data reported by the interface
    void localSet( const orca::PointCloudData& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::PointCloudData& data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::PointCloudData internalGetData() const;
    //::orca::PointCloudDescription internalGetDescription() const;
    void internalSubscribe(const ::orca::PointCloudConsumerPrx&);
    void internalUnsubscribe(const ::orca::PointCloudConsumerPrx&);

    //orca::PointCloudDescription     descr_;
    gbxiceutilacfr::Store<orca::PointCloudData> dataStore_;

    orca::PointCloudConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx             topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string       interfaceName_;
    const std::string       topicName_;
    orcaice::Context        context_;
};
typedef IceUtil::Handle<PointCloudImpl> PointCloudImplPtr;

}

#endif