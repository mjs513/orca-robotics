/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_INSGPSI_H
#define ORCA_INSGPSI_H

#include <orcaice/context.h>

namespace insgps{
    
// All the interface implementations in insgps should inherit from this
// The handler will call the publish function

class InsGpsI
{
public:
    InsGpsI( const orcaice::Context & context ) : context_(context) {};

    virtual ~InsGpsI() {};
    
    // Read  from the drivers buffer and publish to the outside world
    // This function is called by the handler
    virtual void publish() = 0;
    
private:
    orcaice::Context context_;
    
};

}// end of namespace

#endif
