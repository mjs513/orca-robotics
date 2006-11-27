/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_INSGPS_GPSI_H
#define ORCA2_INSGPS_GPSI_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/gps.h>

// for context
#include <orcaice/orcaice.h>

// utilities
#include <orcaice/ptrbuffer.h>

// hardware driver      
#include "driver.h"

// base class
#include "insgpsi.h"

//
// Implements the Gps interface:
//     - Reads the gps messages provided by the driver and publishes them
//     - Handles all our remote calls.
//
// The component interacts with hardware and the outside
// world through the handler and (thread-safe) buffers.
//
class GpsI : public orca::Gps, public insgps::InsGpsI
{
public:
    GpsI(orca::GpsDescriptionPtr descr,
         insgps::Driver*  hwDriver,
         orcaice::Context       context);

    //
    // gps message handler functions
    //
     
    // the handler calls this function which reads from the hwDriver_'s  buffers
    // and then publishes to the outside world   
    virtual void publish();
    
    //
    // remote calls:
    //
    
    // Get GPS Data
    virtual ::orca::GpsDataPtr     getData(const ::Ice::Current& ) const;
    virtual ::orca::GpsTimeDataPtr getTimeData(const ::Ice::Current& ) const;
    virtual ::orca::GpsMapGridDataPtr getMapGridData(const ::Ice::Current& ) const;
    
    virtual ::orca::GpsDescriptionPtr getDescription(const ::Ice::Current& ) const;

    // Subscribe and unsubcribe people
    virtual void subscribe(const ::orca::GpsConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void unsubscribe(const ::orca::GpsConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void subscribeForTime(const ::orca::GpsTimeConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void unsubscribeForTime(const ::orca::GpsTimeConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void subscribeForMapGrid(const ::orca::GpsMapGridConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void unsubscribeForMapGrid(const ::orca::GpsMapGridConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Set GPS Data
    void localSetData( ::orca::GpsDataPtr data );
    void localSetTimeData( ::orca::GpsTimeDataPtr data );
    void localSetMapGridData( ::orca::GpsMapGridDataPtr data );

    // Get Gps Description
    orca::GpsDescriptionPtr localGetDescription() const;

private:

    //
    // remote call stuff
    //      

    // the handler (this class) will put the latest data into this buffer
    orcaice::PtrBuffer<orca::GpsDataPtr> gpsDataBuffer_;
    orcaice::PtrBuffer<orca::GpsMapGridDataPtr> gpsMapGridDataBuffer_;
    orcaice::PtrBuffer<orca::GpsTimeDataPtr> gpsTimeDataBuffer_;

    //publishers
    orca::GpsConsumerPrx gpsPublisher_;
    orca::GpsMapGridConsumerPrx gpsMapGridPublisher_;
    orca::GpsTimeConsumerPrx gpsTimePublisher_;

    //topics
    IceStorm::TopicPrx topicPrx_;
    IceStorm::TopicPrx topicMapGridPrx_;
    IceStorm::TopicPrx topicTimePrx_;

    orca::GpsDescriptionPtr descr_;

    //
    // handler stuff
    //

    // hardware driver
    insgps::Driver* hwDriver_;

    orca::GpsDataPtr gpsData_;
    
    // read from the hwDriver_'s buffer
    void read( ::orca::GpsDataPtr& );
    
    orcaice::Context context_;
    
};


#endif
