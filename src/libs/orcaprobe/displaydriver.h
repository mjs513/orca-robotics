/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAPROBE_DISPLAY_DRIVER_H
#define ORCA2_ORCAPROBE_DISPLAY_DRIVER_H

#include <orcacm/types.h>

namespace orcaprobe
{

class BrowserDriver;

class DisplayDriver
{

public:

    enum FocusType
    {
        RegistryFocus=0,
        PlatformFocus,
        ComponentFocus,
        InterfaceFocus,
        OperationFocus,
        ResultFocus
    };

    virtual ~DisplayDriver() {};

    // need a separate enable function because the Qt driver does not return
    virtual void enable( BrowserDriver* browser )=0;

    virtual void showNetworkActivity( bool isActive )=0;

    virtual void setRegistryData( const orcacm::RegistryHierarchicalData1 & data )=0;

    virtual void setPlatformData( const orcacm::RegistryHierarchicalData2 & data )=0;
    
    virtual void setComponentData( const orcacm::ComponentData & data )=0;
    
    virtual void setInterfaceData( const orcacm::InterfaceData & data )=0;
    
    virtual void setOperationData( const orcacm::OperationData & data )=0;

    virtual void setFocus( FocusType focus )=0;
};

} // namespace

#endif
