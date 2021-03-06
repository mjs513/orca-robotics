/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "buttonImpl.h"
 

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class ButtonI : public orca::Button
{
public:
    ButtonI( ButtonImpl &impl )
        : impl_(impl) {}

    virtual void press( const ::Ice::Current& current )
        {  impl_.internalPress(); }

private:
    ButtonImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

ButtonImpl::ButtonImpl( 
            const std::string &interfaceTag,
            const orcaice::Context &context )
    : interfaceName_(orcaice::getProvidedInterface(context,interfaceTag).iface),
      context_(context)
{
}

ButtonImpl::ButtonImpl( 
            const orcaice::Context &context,
            const std::string &interfaceName )
    : interfaceName_(interfaceName),
      context_(context)
{
}

ButtonImpl::~ButtonImpl()
{
    orcaice::tryRemoveInterface( context_, interfaceName_ );
}

void
ButtonImpl::initInterface()
{
    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new ButtonI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
ButtonImpl::initInterface( gbxutilacfr::Stoppable* activity, const std::string& subsysName, int retryInterval )
{
    ptr_ = new ButtonI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, activity, subsysName, retryInterval );
}

void
ButtonImpl::internalPress()
{
    if ( this->hasNotifyHandler() ) {
        this->set( true );
    }
}

} // namespace
