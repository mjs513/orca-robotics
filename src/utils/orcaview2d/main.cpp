/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */


#include <orcaice/application.h>
#include "component.h"

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    orcaview2d::Component component("OrcaView2d");
    // do not install ctrl-c handler
    bool installCtrlCHandler = false;
    orcaice::Application app( component, installCtrlCHandler );
    return app.main(argc, argv);
}
