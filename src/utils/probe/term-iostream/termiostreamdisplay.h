/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef IOSTREAM_DISPLAY_DRIVER_H
#define IOSTREAM_DISPLAY_DRIVER_H

#include <vector>
#include <gbxsickacfr/gbxiceutilacfr/buffer.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <hydroiceutil/eventqueue.h>
#include <orcaprobe/factory.h>

#include <orcaprobe/abstractdisplay.h>
#include <orcaprobe/ibrowser.h>

namespace probe
{

class TermIostreamDisplay : public orcaprobe::AbstractDisplay
{

public:
    // probe factories here are simply queried for support
    TermIostreamDisplay( const std::vector<std::string> & supportedInterfaces );
    virtual ~TermIostreamDisplay();

    // from orcaprobe::AbstractDisplay
    // for consistency with the Qt driver, this function will hug the thread
    virtual void enable( orcaprobe::IBrowser* browser );
    virtual void showNetworkActivity( bool isActive );
    virtual void setRegistryData( const orcacm::RegistryHierarchicalData1 & data );
    virtual void setPlatformData( const orcacm::RegistryHierarchicalData2 & data );
    virtual void setComponentData( const orcacm::ComponentData & data );
    virtual void setInterfaceData( const orcacm::InterfaceData & data );
    virtual void setOperationData( const orcacm::OperationData & data );
    virtual void setFocus( orcaprobe::AbstractDisplay::FocusType focus );

private:

    std::vector<std::string>    supportedInterfaces_;
    orcaprobe::IBrowser    *browser_;

    hydroiceutil::EventQueuePtr events_;

    // local data model
    orcacm::RegistryHierarchicalData1 registryData_;
    orcacm::RegistryHierarchicalData2 platformData_;
    orcacm::ComponentData componentData_;
    orcacm::InterfaceData interfaceData_;
    orcacm::OperationData operationData_;
    std::string filter_;

    // display functions
    void printNetworkActivity( bool isActive );
    void printRegistryData( const orcacm::RegistryHierarchicalData1& );
    void printPlatformData( const orcacm::RegistryHierarchicalData2& );
    void printComponentData( const orcacm::ComponentData& );
    void printInterfaceData( const orcacm::InterfaceData& );
    void printOperationData( const orcacm::OperationData& );

    // utilities
    void getUserInput( unsigned int optionCount );

};

} // namespace

#endif
