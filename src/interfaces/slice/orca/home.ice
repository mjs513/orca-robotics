/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_HOME_INTERFACE_ICE
#define ORCA2_HOME_INTERFACE_ICE

// for Ice::StringSeq
#include <Ice/BuiltinSequences.ice>

#include <orca/orca.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_home Home
    @brief Component meta information.
    @{
*/

/*!
    @brief Contains data on the component's interfaces.
*/
struct HomeData
{    
    //! Time when data was measured.
    Time timeStamp;

    //! The Home interface itself is also included in this list of provided interfaces.
    ComponentData comp;

    //! Number of seconds since the start of the component.
    int timeUp;
};

//! Our properties are defined the same way as Ice::PropertyDict.
dictionary<string,string> ComponentProperties;

/*!
    @brief Access to information about components interfaces.
*/
interface Home
{
    //! Returns a list the component's interfaces.
    nonmutating HomeData getInterfaces();

    //! Returns the number of seconds since the start of the component.
    nonmutating int getTimeUp();
    
    //! Returns all of this component's properties.
    nonmutating ComponentProperties getProperties();
};

/*!
    @}
*/
}; // module

#endif
