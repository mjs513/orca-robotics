/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_I_REPLAYER_FACTORY_H
#define ORCA2_I_REPLAYER_FACTORY_H

#include <orcaice/context.h>

namespace orcalog
{

class Replayer;
    
class IReplayerFactory
{
public:
    virtual ~IReplayerFactory() {};
    
    virtual Replayer* create( const std::string &interfaceType, 
                              const std::string &format,
                              orcaice::Context   context) = 0;
};

} // namespace

#endif
