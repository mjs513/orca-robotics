/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "probefactory.h"

// interface handlers
#include "homeprobe.h"
#include "powerprobe.h"

using namespace probe;

bool 
ProbeFactory::isSupported( const std::string & id )
{
    if ( id == "::orca::Home" || id == "::orca::Power" ) {
        return true;
    }
    else {
        return false;
    }
}

InterfaceProbe* 
ProbeFactory::create( const std::string & id, const orca::FQInterfaceName & name, 
                      DisplayDriver & display, const orcaice::Context & context )
{
    if ( !isSupported( id ) ) {
        return 0;
    }

    if ( id == "::orca::Home" ) {
        return new HomeProbe( name, display, context );
    }
    else if ( id == "::orca::Power" ) {
        return new PowerProbe( name, display, context );
    }

    // shouldn't get here
    return 0;
}
