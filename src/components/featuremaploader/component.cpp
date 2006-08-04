/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "component.h"
#include "fakemaploader.h"

// implementations of Ice objects
#include "featuremap2dI.h"

#include <orcaice/orcaice.h>

using namespace std;
using namespace orca;

namespace featuremaploader {

Component::Component()
    : orcaice::Component( "FeatureMapLoader" )
{
}

Component::~Component()
{
}

int 
Component::loadMap( const std::string &fileName, orca::FeatureMap2dDataPtr &theMap )
{
    try
    {
        FILE *f = fopen( fileName.c_str(), "r" );
        if (!f)
        {
            cout << "error: cannot open file '"<<fileName<<"'" <<endl;
            return -1;
        } 
        else 
        {
            orcaice::loadFromFile( theMap, f );
            fclose( f );
        }
    }
    catch ( const std::string &e )
    {
        cout<<"error: "<<e<<endl;
        return -1;
    }
    return 0;
}

void
Component::start()
{
    //
    // INITIAL CONFIGURATION
    //

    Ice::PropertiesPtr prop = properties();
    std::string prefix = tag();
    prefix += ".Config.";

    //
    // LOAD THE MAP
    //
    orca::FeatureMap2dDataPtr theMap = new FeatureMap2dData;

    std::string driverName = orcaice::getPropertyWithDefault( prop, prefix+"Driver", "fake" );
    if ( driverName == "fake" )
    {
        cout<<"TRACE(component.cpp): Instantiating fake driver" << endl;
        fakeLoadMap( theMap );
    }
    else if ( driverName == "real" )
    {
        std::string mapFileName = orcaice::getPropertyWithDefault( prop, prefix+"MapFileName", "mapfilename" );
        if ( loadMap( mapFileName, theMap ) != 0 )
        {
            throw orcaice::Exception( ERROR_INFO, "Failed to load map" );
        }
    }
    else
    {
        std::string errString = "Unknown driver type: "+driverName;
        throw orcaice::Exception( ERROR_INFO, errString );
    }

    //
    // EXTERNAL PROVIDED INTERFACES
    //
    // create servant for direct connections
    featureMap2dObj_ = new FeatureMap2dI( theMap, "FeatureMap2d", context() );
    orcaice::createInterfaceWithTag( context(), featureMap2dObj_, "FeatureMap2d" );

    ////////////////////////////////////////////////////////////////////////////////

    //
    // ENABLE NETWORK CONNECTIONS
    //
    activate();

    //
    // MAIN DRIVER LOOP: No need to do anything here, since we don't need our own thread.
    //
}

void Component::stop()
{
    // Nothing to do, since we don't have our own thread.
}


}
