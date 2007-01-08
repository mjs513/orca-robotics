/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaprobe/orcaprobe.h>

#include "position3dprobe.h"

using namespace std;
using namespace orcaprobefactory;

Position3dProbe::Position3dProbe( const orca::FQInterfaceName & name, orcaprobe::DisplayDriver & display,
                                const orcaice::Context & context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::Position3d";
    
    addOperation( "getDescription" );
    addOperation( "getData" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );
}
    
int 
Position3dProbe::loadOperationEvent( const int index, orcacm::OperationData & data )
{
    switch ( index )
    {
    case orcaprobe::UserIndex :
        return loadGetDescription( data );
    case orcaprobe::UserIndex+1 :
        return loadGetData( data );
    case orcaprobe::UserIndex+2 :
        return loadSubscribe( data );
    case orcaprobe::UserIndex+3 :
        return loadUnsubscribe( data );
    }
    return 1;
}

int 
Position3dProbe::loadGetData( orcacm::OperationData & data )
{
    orca::Position3dData result;
    orcacm::ResultHeader res;
    
    try
    {
        orca::Position3dPrx derivedPrx = orca::Position3dPrx::checkedCast(prx_);
        result = derivedPrx->getData();
    }
    catch( const orca::DataNotExistException & e )
    {
        cout<<"data is not ready on the remote interface"<<endl;
        return 1;
    }
    catch( const orca::HardwareFailedException & e )
    {
        cout<<"remote hardware failure"<<endl;
        return 1;
    }
    catch( const Ice::Exception & e )
    {
        cout<<"ice exception: "<<e<<endl;
        return 1;
    }

    res.name = "data";
    res.text = orcaice::toString(result);
    data.results.push_back( res );
    return 0;
}

int 
Position3dProbe::loadGetDescription( orcacm::OperationData & data )
{
    orcacm::ResultHeader res;
    res.name = "outcome";
    res.text = "operation not implemented";
    data.results.push_back( res );
    return 0;
}

int 
Position3dProbe::loadSubscribe( orcacm::OperationData & data )
{
    Ice::ObjectPtr consumer = this;
    orca::Position3dConsumerPrx callbackPrx =
            orcaice::createConsumerInterface<orca::Position3dConsumerPrx>( context_, consumer );

    orcacm::ResultHeader res;

    try
    {
        orca::Position3dPrx derivedPrx = orca::Position3dPrx::checkedCast(prx_);
        derivedPrx->subscribe( callbackPrx );
    }
    catch( const Ice::Exception & e )
    {
        stringstream ss;
        ss << e;
        res.name = "exception";
        res.text = ss.str();
        data.results.push_back( res );
        return 1;
    }
    
    res.name = "outcome";
    res.text = "Subscribed successfully";
    data.results.push_back( res );
    return 0;
}

int 
Position3dProbe::loadUnsubscribe( orcacm::OperationData & data )
{
//     try
//     {
//         orca::Position3dPrx derivedPrx = orca::Position3dPrx::checkedCast(prx_);
// //         cout<<"unsub  "<<Ice::identityToString( consumerPrx_->ice_getIdentity() )<<endl;
//         
//         orca::Position3dConsumerPrx powerConsumerPrx = orca::Position3dConsumerPrx::uncheckedCast(consumerPrx_);
// //         cout<<"unsub  "<<Ice::identityToString( powerConsumerPrx->ice_getIdentity() )<<endl;
//         derivedPrx->unsubscribe( powerConsumerPrx );
//     }
//     catch( const Ice::Exception & e )
//     {
//         cout<<"caught "<<e<<endl;
//         return 1;
//     }
    
    orcacm::ResultHeader res;
    res.name = "outcome";
    res.text = "operation not implemented";
    data.results.push_back( res );
    return 0;
}

void 
Position3dProbe::setData(const orca::Position3dData & data, const Ice::Current&)
{
    std::cout << orcaice::toString(data) << std::endl;
};
