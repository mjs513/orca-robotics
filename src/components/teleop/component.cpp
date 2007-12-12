/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include "component.h"
#include "networkthread.h"
#include "termdisplaythread.h"
#include "inputthread.h"

using namespace std;
using namespace teleop;

Component::Component() :
    orcaice::Component( "Teleop", orcaice::HomeInterface ),
    networkThread_(0),
    displayThread_(0),
    inputThread_(0)
{
}

Component::~Component()
{
    // do not delete networkThread_ and displayThread_!!! They derive from Ice::Thread and deletes itself.
}

// warning: this function returns after it's done, all variable that need to be permanet must
//          be declared as member variables.
void
Component::start()
{
    tracer().debug("starting component",2);

    //
    // USER DISPLAY
    //
    // the constructor may throw, we'll let the application shut us down
    displayThread_ = new TermDisplayThread( context() );
    displayThread_->start();
    
    //
    // NETWORK
    //
    // the constructor may throw, we'll let the application shut us down
    networkThread_ = new NetworkThread( (Display*)displayThread_, context() );
    networkThread_->start();

    //
    // USER INPUT
    //
    // the constructor may throw, we'll let the application shut us down
    inputThread_ = new InputThread( (Network*)networkThread_, context() );
    inputThread_->start();

    // the rest is handled by the application/service
}

void
Component::stop()
{
    // inputThread_ is blocked on user input
    // the only way for it to realize that we want to stop is to give it some keyboard input.
    tracer().info( "Component is quitting but the InputThread is blocked waiting for user input.");
    tracer().print( "************************************************" );
    tracer().print( "Press any key or shake the joystick to continue." );
    tracer().print( "************************************************" );
    
    hydroutil::stopAndJoin( inputThread_ );
    hydroutil::stopAndJoin( networkThread_ );
    hydroutil::stopAndJoin( displayThread_ );
}
