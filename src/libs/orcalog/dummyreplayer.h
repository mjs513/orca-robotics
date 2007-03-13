/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOG_DUMMY_REPLAYER_H
#define ORCA2_ORCALOG_DUMMY_REPLAYER_H

#include <orcalog/replayer.h>

namespace orcalog
{

//! This one can be used for unsupported or disabled interfaces.
class DummyReplayer : public Replayer
{
public:
    DummyReplayer( const orcaice::Context& context ) :
        Replayer( "dummy", "dummy", "dummy", context ) {};

    virtual void replayData( int index, bool isTest=false ) {};

protected:
    virtual void initInterfaces() {};

    virtual void initDescription() {};
};

} // namespace

#endif