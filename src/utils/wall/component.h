/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */


#ifndef ORCA2_WALL_COMPONENT_H
#define ORCA2_WALL_COMPONENT_H

#include <vector>
#include <string>
#include <orcaice/component.h>
#include <orcaice/thread.h>
#include <orcadynamicload/dynamicload.h>

namespace orcawall
{
    class Factory;
}

namespace wall
{

class Component : public orcaice::Component
{
public:
    Component();
    virtual ~Component();

    // from orcaice::Component
    virtual void start();
    virtual void stop();

private:
    std::vector<orcawall::Factory*> factories_;
    std::vector<orcadynamicload::DynamicallyLoadedLibrary*> libraries_;

    // a set of interface simulators, each has its own thread
    // cannot use ThreadPtr's here, because when they will try to re-destruct themselves.
    std::vector<orcaice::Thread*> sims_;

    // loads all factory libs and returns a listing of unique supported interfaces
    std::vector<std::string> loadPluginLibraries( const std::string & factoryLibNames );
    // utility to load a plugin from one of the factories
    void createPlugin( const std::string& interfaceType, const std::string& tag );
};

} // namespace

#endif