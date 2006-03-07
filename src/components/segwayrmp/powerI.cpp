/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "powerI.h"

using namespace std;
using namespace orca;
using namespace segwayrmp;

PowerI::PowerI( orcaice::PtrProxy<orca::PowerDataPtr> &power, const IceStorm::TopicPrx &topic )
      : powerPipe_(power),
        topic_(topic)
{
}

orca::PowerDataPtr PowerI::getData(const ::Ice::Current& ) const
{
    //std::cout << "Sending data back" << std::endl;

    // create null smart pointer. data will be cloned into it.
    orca::PowerDataPtr data;

    try
    {
        powerPipe_.get( data );
    }
    catch ( const orcaice::Exception & e )
    {
        throw orca::DataNotExistException( "power proxy is not populated yet" );
    }
    return data;
}

void PowerI::subscribe(const ::orca::PowerConsumerPrx& subscriber, const ::Ice::Current&)
{
    //cout<<"subscription request"<<endl;
    IceStorm::QoS qos;
    topic_->subscribe( qos, subscriber );
}

void PowerI::unsubscribe(const ::orca::PowerConsumerPrx& subscriber, const ::Ice::Current&)
{
    //cout<<"unsubscription request"<<endl;
    topic_->unsubscribe( subscriber );
}

