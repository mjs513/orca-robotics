/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_CPU_ICE
#define ORCA2_CPU_ICE

#include <orca/common.ice>
#include <IceStorm/IceStorm.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_cpu Cpu
    @brief Computer status

    @{
*/

/*!
    @brief General CPU information. 

    To be more useful in cross-platform environments this object should probably have a bunch of fields.
    For example, IceGrid returns the following node description:
@verbatim
node `Alpha'
{
    operating system = `Linux'
    host name = `alpha'
    release = `2.6.15-1-686'
    version = `#2 Mon Mar 6 15:27:08 UTC 2006'
    machine type = `i686'
    number of processors = `1'
}
@endverbatim
*/
struct CpuDescription
{
    //! Time when info was compiled.
    Time timeStamp;
    //! Unparsed string with CPU information, e.g. output of uname in Linux.
    string info;
};

/*!
    @brief Current CPU information.
*/
struct CpuData
{
    //! Time when data was measured.
    Time timeStamp;
    //! CPU load in user space [%]
    int userLoad;
    //! CPU load in system space [%]
    int systemLoad;
    //! CPU idle load [%]
    int idleLoad;
    //! Temperature of processor or motherboard [C]
    float temperature;
};

/*!
    @brief Subscriber to CPU information.
*/
interface CpuConsumer
{
    //! Set data
    void setData( CpuData obj );
};

//! Interface to CPU information.
interface Cpu
{
    //! Get general CPU information.
    idempotent CpuDescription getDescription();

    //! Get current CPU state. Raises DataNotExistException if data is not available.
    idempotent CpuData getData()
        throws DataNotExistException;

    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( CpuConsumer* subscriber )
        throws SubscriptionFailedException, SubscriptionPushFailedException;
};

/*! @} */
}; // module

#endif
