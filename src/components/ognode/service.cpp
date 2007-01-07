/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "service.h"
#include "component.h"

using namespace std;
using namespace ognode;

extern "C"
{
    // Factory function

    IceBox::Service* create( Ice::CommunicatorPtr communicator )
    {
        return new ognode::Service;
    }
}

Service::Service()
{
    component_ = new ognode::Component;
}
