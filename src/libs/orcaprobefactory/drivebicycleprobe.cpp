/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaprobe/orcaprobe.h>

#include "drivebicycleprobe.h"

using namespace std;
using namespace orcaprobefactory;

DriveBicycleProbe::DriveBicycleProbe( const orca::FQInterfaceName& name, 
                                      orcaprobe::DisplayDriver& display,
                                      const orcaice::Context& context ) :
    InterfaceProbe(name,display,context)
{
    id_ = "::orca::DriveBicycle";
    
    addOperation( "getDescription" );
    addOperation( "getData" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );

    Ice::ObjectPtr consumer = this;
    callbackPrx_ = orcaice::createConsumerInterface<orca::DriveBicycleConsumerPrx>( context_, consumer );
}
    
int 
DriveBicycleProbe::loadOperationEvent( const int index, orcacm::OperationData& data )
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
DriveBicycleProbe::loadGetData( orcacm::OperationData& data )
{
    orca::DriveBicycleData result;    
    try
    {
        orca::DriveBicyclePrx derivedPrx = orca::DriveBicyclePrx::checkedCast(prx_);
        result = derivedPrx->getData();
        orcaprobe::reportResult( data, "data", orcaice::toString(result) );
    }
    catch( const orca::DataNotExistException& e )
    {
        orcaprobe::reportException( data, "data is not ready on the remote interface" );
    }
    catch( const orca::HardwareFailedException& e )
    {
        orcaprobe::reportException( data, "remote hardware failure" );
    }
    catch( const Ice::Exception& e )
    {
        stringstream ss;
        ss<<e<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}

int 
DriveBicycleProbe::loadGetDescription( orcacm::OperationData& data )
{
    orca::VehicleDescription result;
    try
    {
        orca::DriveBicyclePrx derivedPrx = orca::DriveBicyclePrx::checkedCast(prx_);
        result = derivedPrx->getDescription();
        orcaprobe::reportResult( data, "data", orcaice::toString(result) );
    }
    catch( const Ice::Exception& e )
    {
        stringstream ss;
        ss<<e<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}

int 
DriveBicycleProbe::loadSubscribe( orcacm::OperationData& data )
{
    cout<<"subscribing "<<callbackPrx_->ice_toString()<<endl;

    try
    {
        orca::DriveBicyclePrx derivedPrx = orca::DriveBicyclePrx::checkedCast(prx_);
        derivedPrx->subscribe( callbackPrx_ );
        orcaprobe::reportSubscribed( data );

        // save the op data structure so we can use it when the data arrives
        subscribeOperationData_ = data;
    }
    catch( const Ice::Exception& e )
    {
        stringstream ss;
        ss<<e<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}

int 
DriveBicycleProbe::loadUnsubscribe( orcacm::OperationData& data )
{
    cout<<"unsubscribing "<<callbackPrx_->ice_toString()<<endl;

    try
    {
        orca::DriveBicyclePrx derivedPrx = orca::DriveBicyclePrx::checkedCast(prx_);
        derivedPrx->unsubscribe( callbackPrx_ );
        orcaprobe::reportUnsubscribed( data );
    }
    catch( const Ice::Exception& e )
    {
        stringstream ss;
        ss<<e<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}

void 
DriveBicycleProbe::setData(const orca::DriveBicycleData& result, const Ice::Current&)
{
//     std::cout << orcaice::toString(result) << std::endl;
    subscribeOperationData_.results.clear();
    orcaprobe::reportResult( subscribeOperationData_, "data", orcaice::toString(result) );
    display_.setOperationData( subscribeOperationData_ );
};