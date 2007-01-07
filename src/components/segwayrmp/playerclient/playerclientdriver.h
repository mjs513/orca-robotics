/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ROBOT_PLAYER_CLIENT_DRIVER_H
#define ORCA2_ROBOT_PLAYER_CLIENT_DRIVER_H

#include "../hwdriver.h"
#include "playerclientdriverconfig.h"

// Player proxies
namespace PlayerCc
{
class PlayerClient;
class Position2dProxy;
class Position3dProxy;
class PowerProxy;
}

namespace segwayrmp
{

class PlayerClientDriver : public HwDriver
{
public:

    PlayerClientDriver( const orcaice::Context & context );
    //PlayerClientDriver( const std::map<std::string,std::string> & props );
    virtual ~PlayerClientDriver();

    // returns: 0 = success, non-zero = failure
    virtual int enable();

    virtual int read( orca::Position2dData& position2d, orca::Position3dData& position3d, 
                      orca::PowerData &power, std::string & status );

    virtual int write( const orca::Velocity2dCommand& command );

private:

    int disable();

    bool enabled_;
    PlayerCc::PlayerClient *robot_;
    PlayerCc::Position2dProxy *positionProxy_;
    PlayerCc::Position3dProxy *position3dProxy_;
    PlayerCc::PowerProxy *powerProxy_;

    // configuration
    orcaice::Context context_;
    PlayerClientDriverConfig config_;
};

} // namespace

#endif
