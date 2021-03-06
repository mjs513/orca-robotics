/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef TERM_DISPLAY_EVENTS_H
#define TERM_DISPLAY_EVENTS_H

#include <hydroiceutil/event.h>
#include <orcacm/types.h>
#include <orcaprobe/abstractdisplay.h> // for focus types

// alexm: we put these event definitions on the top level (not with term-iostream driver)
// in case we write another terminal display driver (e.g. ncurses) which is likely to 
// use the same events.

namespace probe
{

enum EventType  
{      
    NetworkActivityChanged=0,
    FocusChanged,
    RegistryDataChanged,
    PlatformDataChanged,
    ComponentDataChanged,
    InterfaceDataChanged,
    OperationDataChanged
};

class NetworkActivityChangedEvent : public hydroiceutil::Event
{
public:
    NetworkActivityChangedEvent( bool isActive )
        : Event( NetworkActivityChanged ),
          isActive_(isActive) {};

    bool isActive_;
};
typedef IceUtil::Handle<NetworkActivityChangedEvent> NetworkActivityChangedEventPtr;

class FocusChangedEvent : public hydroiceutil::Event
{
public:
    FocusChangedEvent( orcaprobe::AbstractDisplay::FocusType focus )
        : Event( FocusChanged ),
          focus_(focus)  {};
    orcaprobe::AbstractDisplay::FocusType focus_;
};
typedef IceUtil::Handle<FocusChangedEvent> FocusChangedEventPtr;

class RegistryDataChangedEvent : public hydroiceutil::Event
{
public:
    RegistryDataChangedEvent( const orcacm::RegistryHierarchicalData1 & data )
        : Event( RegistryDataChanged ),
          data_(data) {};
    orcacm::RegistryHierarchicalData1 data_;
};
typedef IceUtil::Handle<RegistryDataChangedEvent> RegistryDataChangedEventPtr;

class PlatformDataChangedEvent : public hydroiceutil::Event
{
public:
    PlatformDataChangedEvent( const orcacm::RegistryHierarchicalData2 & data )
        : Event( PlatformDataChanged ),
          data_(data) {};
    orcacm::RegistryHierarchicalData2 data_;
};
typedef IceUtil::Handle<PlatformDataChangedEvent> PlatformDataChangedEventPtr;

class ComponentDataChangedEvent : public hydroiceutil::Event
{
public:
    ComponentDataChangedEvent( const orcacm::ComponentData & data )
        : Event( ComponentDataChanged ),
          data_(data) {};
    orcacm::ComponentData data_;
};
typedef IceUtil::Handle<ComponentDataChangedEvent> ComponentDataChangedEventPtr;

class InterfaceDataChangedEvent : public hydroiceutil::Event
{
public:
    InterfaceDataChangedEvent( const orcacm::InterfaceData & data )
        : Event( InterfaceDataChanged ),
          data_(data) {};
    orcacm::InterfaceData data_;
};
typedef IceUtil::Handle<InterfaceDataChangedEvent> InterfaceDataChangedEventPtr;


class OperationDataChangedEvent : public hydroiceutil::Event
{
public:
    OperationDataChangedEvent( const orcacm::OperationData & data )
        : Event( OperationDataChanged ),
          data_(data) {};
    orcacm::OperationData data_;
};
typedef IceUtil::Handle<OperationDataChangedEvent> OperationDataChangedEventPtr;


} // namespace

#endif
