/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_PRINT_UTILITIES_H
#define ORCAICE_PRINT_UTILITIES_H

#include <orcaice/context.h>

#include <orca/orca.h>

// These functions should've been in privateutils.h but they are needed for templated functions
// in connectutils.h.
// Do not doxygen, these are not part of public API.

namespace orcaice
{
    // A uniform way to print messages before the standard tracer object is created.
    void initTracerInfo( const std::string & message );
    void initTracerWarning( const std::string & message );
    void initTracerError( const std::string & message );

    // Will try to use the Tracer, will print to screen if fails
    void initTracerInfo( const orcaice::Context & context, const std::string & message, const int level=1 );
    void initTracerWarning( const orcaice::Context & context, const std::string & message, const int level=1 );
    void initTracerError( const orcaice::Context & context, const std::string & message, const int level=1 );

} // namespace

#endif
