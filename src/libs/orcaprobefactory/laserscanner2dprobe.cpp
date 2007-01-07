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

#include "laserscanner2dprobe.h"

using namespace std;
using namespace orcaprobefactory;

LaserScanner2dProbe::LaserScanner2dProbe( const orca::FQInterfaceName & name, orcaprobe::DisplayDriver & display,
                                const orcaice::Context & context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::LaserScanner2d";

    addOperation( "getData",        "nonmutating RangeScanner2dData getData()" );
    addOperation( "getDescription", "nonmutating RangeScanner2dDescription getDescription()" );
    addOperation( "subscribe",      "void subscribe( RangeScanner2dConsumer *subscriber )" );
    addOperation( "unsubscribe",    "idempotent void unsubscribe( RangeScanner2dConsumer *subscriber )" );
}
    
int 
LaserScanner2dProbe::loadOperationEvent( const int index, orcacm::OperationData & data )
{    
    switch ( index )
    {
    case orcaprobe::UserIndex :
        return loadGetData( data );
    case orcaprobe::UserIndex+1 :
        return loadGetDescription( data );
    case orcaprobe::UserIndex+2 :
        return loadSubscribe( data );
    case orcaprobe::UserIndex+3 :
        return loadUnsubscribe( data );
    }
    return 1;
}

int 
LaserScanner2dProbe::loadGetData( orcacm::OperationData & data )
{
    orca::RangeScanner2dDataPtr result;
    orcacm::ResultHeader res;
    
    try
    {
        orca::LaserScanner2dPrx derivedPrx = orca::LaserScanner2dPrx::checkedCast(prx_);
        result = derivedPrx->getData();
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

//     cout<<orcaice::toString(result)<<endl;
    res.name = "data";
    res.text = orcaice::toString(result);
    data.results.push_back( res );
    return 0;
}

int 
LaserScanner2dProbe::loadGetDescription( orcacm::OperationData & data )
{
    orca::RangeScanner2dDescription result;
    orcacm::ResultHeader res;
    
    try
    {
        orca::LaserScanner2dPrx derivedPrx = orca::LaserScanner2dPrx::checkedCast(prx_);
        result = derivedPrx->getDescription();
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

//     cout<<orcaice::toString(result)<<endl;
    res.name = "data";
    res.text = orcaice::toString(result);
    data.results.push_back( res );
    return 0;
}

int 
LaserScanner2dProbe::loadSubscribe( orcacm::OperationData & data )
{
    Ice::ObjectPtr consumer = this;
    orca::RangeScanner2dConsumerPrx callbackPrx = 
            orcaice::createConsumerInterface<orca::RangeScanner2dConsumerPrx>( context_, consumer );

    orcacm::ResultHeader res;

    try
    {
        orca::LaserScanner2dPrx derivedPrx = orca::LaserScanner2dPrx::checkedCast(prx_);
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
LaserScanner2dProbe::loadUnsubscribe( orcacm::OperationData & data )
{
//     try
//     {
//         orca::PowerPrx derivedPrx = orca::PowerPrx::checkedCast(prx_);
// //         cout<<"unsub  "<<Ice::identityToString( consumerPrx_->ice_getIdentity() )<<endl;
//         
//         orca::PowerConsumerPrx powerConsumerPrx = orca::PowerConsumerPrx::uncheckedCast(consumerPrx_);
// //         cout<<"unsub  "<<Ice::identityToString( powerConsumerPrx->ice_getIdentity() )<<endl;
//         derivedPrx->unsubscribe( powerConsumerPrx );
//     }
//     catch( const Ice::Exception & e )
//     {
//         cout<<"caught "<<e<<endl;
//         return 1;
//     }
    
    return 0;
}

void 
LaserScanner2dProbe::setData(const orca::RangeScanner2dDataPtr& result, const Ice::Current&)
{
    std::cout << orcaice::toString(result) << std::endl;

    orcacm::OperationData data;
    // this is the result for operation "subscribe" which has user index=2;
    fillOperationData( orcaprobe::UserIndex+2, data );

    orcacm::ResultHeader res;
    res.name = "data";
    res.text = orcaice::toString(result);
    data.results.push_back( res );

    display_.setOperationData( data );
};
