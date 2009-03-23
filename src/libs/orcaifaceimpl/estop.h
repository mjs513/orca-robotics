/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_ESTOP_H
#define ORCAIFACEIMPL_ESTOP_H

#include <orcaifaceimpl/providerWithDescriptionImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/estop.h>

namespace orcaifaceimpl {

typedef ProviderWithDescriptionImpl<orca::EStop, orca::EStopPrx,
                    orca::EStopConsumer, orca::EStopConsumerPrx,
                    orca::EStopData, orca::EStopDescription> EStopImpl;
typedef IceUtil::Handle<EStopImpl> EStopImplPtr;

typedef StoringConsumerImpl<orca::EStop, orca::EStopPrx,
                    orca::EStopConsumer, orca::EStopConsumerPrx,
                    orca::EStopData> StoringEStopConsumerImpl;
typedef IceUtil::Handle<StoringEStopConsumerImpl> StoringEStopConsumerImplPtr;

typedef BufferedConsumerImpl<orca::EStop, orca::EStopPrx,
                        orca::EStopConsumer, orca::EStopConsumerPrx,
                        orca::EStopData> BufferedEStopConsumerImpl;
typedef IceUtil::Handle<BufferedEStopConsumerImpl> BufferedEStopConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::EStop, orca::EStopPrx,
                        orca::EStopConsumer,
                        orca::EStopConsumerPrx,
                        orca::EStopData> NotifyingEStopConsumerImpl;
typedef IceUtil::Handle<NotifyingEStopConsumerImpl> NotifyingEStopConsumerImplPtr;

typedef PrintingConsumerImpl<orca::EStop, orca::EStopPrx,
                        orca::EStopConsumer, orca::EStopConsumerPrx,
                        orca::EStopData> PrintingEStopConsumerImpl;
typedef IceUtil::Handle<PrintingEStopConsumerImpl> PrintingEStopConsumerImplPtr;

}

#endif
