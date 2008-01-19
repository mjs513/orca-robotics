/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <orcaice/component.h>
#include <orcaqgui/guielementfactory.h>
#include <hydrodll/dynamicload.h>

using namespace std;

namespace orcaview3d {
    
class Component : public orcaice::Component
{
public:
    Component( std::string compName );
    ~Component();

    // component interface
    virtual void start();
    virtual void stop();

private:

    void loadPluginLibraries( const std::string &factoryLibNames );

    std::vector<orcaqgui::GuiElementFactory*>                factories_;
    std::vector<hydrodll::DynamicallyLoadedLibrary*> libraries_;
          
};

}

#endif

