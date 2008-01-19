/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCA_COMPONENT_MODEL_ICE
#define ORCA2_ORCA_COMPONENT_MODEL_ICE

module orca
{

/*!
    @brief Fully qualified executable name.
*/
struct FQExecutableName
{
    //! Name of the executable file
    string executable;
    //! Host name
    string host;
};

/*!
    @brief Fully qualified component name.
    @see FQInterfaceName, FQTopicName
*/
struct FQComponentName
{
    //! Platform name
    string platform;
    //! Component name
    string component;
};

/*!
    @brief Fully qualified interface name.
    Usually represented in a single string as 'iface\@platform/component'

    @note Cannot use member variable 'interface', it's reserved by Ice.
    @see FQComponentName, FQTopicName
*/
struct FQInterfaceName
{
    //! Platform name
    string platform;
    //! Component name
    string component;
    //! Interface name
    string iface;
};

/*!
    @brief Fully qualified Ice Storm topic name.
    The idea is that the same interface can publish to multiple topics corresponding
    different data streams.
    When written as a string, this is formatted as:
      - interface/topicName@platform/component
    @see FQInterfaceName, FQComponentName
*/
struct FQTopicName
{
    //! Platform name
    string platform;
    //! Component name
    string component;
    //! Interface name
    string iface;
    //! Topic name
    string topic;
};

//! Brief version of provided interface data
struct ProvidedInterface
{
    //! Interface name
    string name;
    //! Object ID of the interface
    string id;
};

//! Brief version of required interface data
struct RequiredInterface
{
    //! Interface name
    FQInterfaceName name;
    //! Object ID of the interface
    string id;
};

//! All provided interfaces
sequence<ProvidedInterface> ProvidesInterfaces;

//! All required interfaces
sequence<RequiredInterface> RequiresInterfaces;

//! Component data
struct ComponentData
{
    //! Component name
    orca::FQComponentName name;
    //! A listing of provided interfaces
    ProvidesInterfaces provides;
    //! A listing of required interfaces
    RequiresInterfaces requires;
};

}; // module

#endif
