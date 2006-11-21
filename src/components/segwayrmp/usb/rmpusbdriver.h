/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_USB_DRIVER_H
#define ORCA2_SEGWAY_RMP_USB_DRIVER_H

#include "../hwdriver.h"
#include "rmpusbdriverconfig.h"

namespace segwayrmp
{

// forward declarations
class RmpUsbIo;
class RmpUsbDataFrame;
class CanPacket;

class RmpUsbDriver : public HwDriver
{
public:

    RmpUsbDriver( const orcaice::Context & context );
    virtual ~RmpUsbDriver();

    virtual int enable();
    virtual int repair();
    virtual int disable();

    virtual int read( orca::Position2dDataPtr &position2d, orca::Position3dDataPtr &position3d, 
                      orca::PowerDataPtr &power, std::string & status );

    virtual int write( const orca::Velocity2dCommandPtr & command );

    virtual std::string toString();

private:

    enum OperationalMode
    {
        OperationalModeTractor=1,
        OperationalModeBalance=2,
        OperationalModePowerdown=3
    };

    enum BalanceLockout
    {
        BalanceAllowed=0,
        BalanceNotAllowed=1
    };

    struct Status
    {
        int buildId;
        int cuState;
        int opMode;
        int gainSchedule;
        //OperationalMode opMode;
    };
    
    void setMaxVelocityScaleFactor( double scale );
    void setMaxTurnrateScaleFactor( double scale );
    void setMaxAccelerationScaleFactor( double scale );
    void setMaxCurrentLimitScaleFactor( double scale );
    void resetAllIntegrators();

    void setOperationalMode( OperationalMode mode );
    void setGainSchedule( int sched );
    void enableBalanceMode( bool enable );

    // driver/hardware interface
    RmpUsbIo         *rmpusbio_;
    RmpUsbDataFrame  *frame_;
    CanPacket        *pkt_;

    // configuration
    orcaice::Context context_;
    RmpUsbDriverConfig config_;

    // last motion commands [segway counts]
    // used to load into status command
    int16_t lastTrans_;
    int16_t lastRot_;

    // for integrating odometry
    uint32_t lastRawYaw_;
    uint32_t lastRawForeaft_;

    // for odometry calculation
    double odomX_;
    double odomY_;
    double odomYaw_;
    
    // for detecting internal state change
    int lastStatusWord1_;
    int lastStatusWord2_;

    void readFrame();
    void integrateMotion();
    void updateData( orca::Position2dDataPtr &position2d, orca::Position3dDataPtr &position3d,
                     orca::PowerDataPtr &power, Status & status );

    // helper to take a player command and turn it into a CAN command packet
    void makeMotionCommandPacket( CanPacket* pkt, const orca::Velocity2dCommandPtr & command );
    void makeStatusCommandPacket( CanPacket* pkt, uint16_t commandId, uint16_t value );
    void makeShutdownCommandPacket( CanPacket* pkt );

    // Calculate the difference between two raw counter values, taking care of rollover.
    int diff(uint32_t from, uint32_t to, bool first);
    // bullshit
    bool firstread_;
    bool repairCounter_;

    // chip's utilities
    void watchPacket( CanPacket* pkt, short int pktID );
    void watchDataStream( CanPacket* pkt );

};

} // namespace

#endif
