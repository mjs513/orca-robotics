/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCAMISC_REALTIMESTOPWATCH_H
#define ORCAMISC_REALTIMESTOPWATCH_H

#include <orcamisc/stopwatch.h>

namespace orcamisc {

//!
//! @brief times real time
//!
//! This fucker times wallclock time.
//!
//! The 'start' and 'stop' buttons do the obvious thing.
//! 'startWithoutReset' starts the clock ticking _without_ resetting first.
//! 'reset' resets and stops the clock.
//!
//! Doesn't work under Windows.
//!
//! @sa CpuStopwatch
//!
//! @author Alex Brooks
//!
class RealTimeStopwatch : public Stopwatch
{

public: 

    RealTimeStopwatch( bool startOnInitialisation=true );

private: 

    void getTimeNow( struct timeval &now ) const;
};

}

#endif
