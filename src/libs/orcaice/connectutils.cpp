/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include "connectutils.h"
#include "configutils.h"
#include "exceptions.h"

namespace orcaice {

void
createInterfaceWithString( const Context      & context,
                            Ice::ObjectPtr    & object,
                            const std::string & proxyString )
{
    try
    {
        context.adapter()->add( object, context.communicator()->stringToIdentity(proxyString) );
    }
    catch ( const Ice::AlreadyRegisteredException &e )
    {
        std::stringstream ss;
        ss<<"Failed to create interface "<<proxyString<<": "<<e;
        throw orcaice::Exception( ERROR_INFO, ss.str() );
    }

    // @todo Record which servant we created as a provided interface so that Home can report this.
}

void
createInterfaceWithTag( const Context      & context,
                        Ice::ObjectPtr     & object,
                        const std::string  & ifaceTag )
{
    context.adapter()->add( object, context.communicator()->stringToIdentity( getProvidedInterface(context,ifaceTag).iface ) );
}

IceStorm::TopicPrx 
connectToIceStormTopicPrx( const Context & context,
                                        const std::string   & topicName,
                                        bool                createIfMissing )
{
    return connectToIceStormTopicPrx( context.communicator(), topicName, createIfMissing );
}


IceStorm::TopicPrx
connectToIceStormTopicPrx( const Ice::CommunicatorPtr & communicator,
                           const std::string          & topicName,
                           bool                       createIfMissing )
{
    // find IceStorm, there's a default value for this prop set in setDefaultOrcaProperties()
    std::string topicManagerString =
            communicator->getProperties()->getProperty("IceStorm.TopicManager.Proxy");
    
    return connectToIceStormTopicPrxWithManager( communicator, topicName, topicManagerString, createIfMissing );

}

// tracer already created
IceStorm::TopicPrx
connectToIceStormTopicPrxWithManager( const Ice::CommunicatorPtr & communicator,
                                      const std::string          & topicName,
                                      const std::string          & topicManagerString,
                                      bool                       createIfMissing )
{
    Ice::ObjectPrx base = communicator->stringToProxy( topicManagerString );
    IceStorm::TopicManagerPrx stormManager = IceStorm::TopicManagerPrx::checkedCast(base);
    if ( !stormManager ) 
    {
        std::string errorString = "invalid proxy to IceStorm";
        communicator->getLogger()->error( errorString );
        throw( orcaice::Exception( ERROR_INFO, errorString ) );
        return IceStorm::TopicPrx();
    }
    
    // Retrieve the topic by name, if does not exist, return empty one, do not create
    IceStorm::TopicPrx topic;
    try
    {
        topic = stormManager->retrieve( topicName );
    }
    catch( const IceStorm::NoSuchTopic& e )
    {
        if ( createIfMissing ) {
            communicator->getLogger()->warning( "no such topic, name=" + e.name + ". Creating a new one." );
            // if the topic doesn't exist, create a new one.
            topic = stormManager->create( topicName );
        }
        else {
            communicator->getLogger()->warning( "no such topic, name=" + e.name );
            // do not create a new topic
            // @todo what do we do? rethrow?
            //throw orcaice::Exception(ERROR_INFO,"Couldn't create topic");
        }
    }
    
    // debug
/*    
    sleep(1);
    try {
        topic->ice_ping();
    }
    catch( const Ice::Exception & e)
    {
        std::string errorString = "failed to ping topic "+topicName+" on server "+topicManagerString;
        communicator->getLogger()->error( errorString );
        std::cout<<e<<std::endl;
        exit(0);
    }
*/
    return topic;
}

// this version of the function is called before the standard tracer is created,
// so we use tracing functions.
Ice::ObjectPrx 
connectToIceStormTopicPublisherPrx( const Ice::CommunicatorPtr & communicator,
                                                   const std::string          & topicName )
{
    // Retrieve the topic by name, if does not exist: create
    IceStorm::TopicPrx topic = connectToIceStormTopicPrx( communicator, topicName, true );

    // now that we have the topic itself, get its publisher object
    return connectToIceStormTopicPublisherPrx( topic );
}

Ice::ObjectPrx 
connectToIceStormTopicPublisherPrx( const IceStorm::TopicPrx & topic )
{
    // Get the topic's publisher object
    // @todo should we verify that the publisher is an object of the right type? need template.
    Ice::ObjectPrx obj = topic->getPublisher();
    
    // create a oneway proxy (for efficiency reasons).
    if(!obj->ice_isDatagram()) {
        obj = obj->ice_oneway();
    }

    return obj;
}

void
getNetworkState( const Context  & context,
                 bool           & isNetworkUp, 
                 bool           & isRegistryUp )
{
    assert( context.communicator() );

    try 
    {
        Ice::LocatorPrx locator = context.communicator()->getDefaultLocator();

        // debug
        context.tracer()->debug( "pinging "+context.communicator()->proxyToString( locator ),5 );
        
        // ping the registry
        locator->ice_ping();

        isNetworkUp = true;
        isRegistryUp = true;
        context.tracer()->debug( "all good",5 );
    }
    catch ( Ice::DNSException & )
    {
        isNetworkUp = false;
        isRegistryUp = false;
        context.tracer()->debug( "no DNS",5 );
    }
    catch ( Ice::ConnectionRefusedException & )
    {
        isNetworkUp = true;
        isRegistryUp = false;
        context.tracer()->debug( "refused",5 );
    }
    catch( const Ice::Exception & e )
    {
        isNetworkUp = false;
        isRegistryUp = false;
        std::cout<<e<<std::endl;
        context.tracer()->debug( "who knows",5 );
//         context.tracer()->warning( "Unexpected exception while checking network." );
    }
}

// THESE FUNCTIONS WERE OBSOLETED TO REDUCE THE SIZE OF THE API

/*
void
subscribeToIceStormTopicWithTag( const Context & context,
                        Ice::ObjectPtr               & consumer,
                        const std::string            & interfaceTag,
                        bool                         createIfMissing )
{
    subscribeToIceStormTopicWithName( context, consumer,
                                      toString(getRequiredTopic( context,interfaceTag )),
                                      createIfMissing );
}

void
subscribeToIceStormTopicWithName( const Context & context,
                                   Ice::ObjectPtr            & consumer,
                                   const std::string         & topicName,
                                   bool                      createIfMissing )
{
    Ice::ObjectPrx prx = context.adapter()->addWithUUID( consumer );

    // make a direct proxy
    Ice::ObjectPrx callbackPrx = context.adapter()->createDirectProxy( prx->ice_getIdentity() );

    IceStorm::TopicPrx topicPrx;
    try
    {
        topicPrx = orcaice::connectToIceStormTopicPrx
                        ( context.communicator(), topicName, createIfMissing );
    }
    catch ( const orcaice::Exception & e )
    {
        return;
    }

    IceStorm::QoS qos;
    topicPrx->subscribe( qos, callbackPrx );
}
*/
} // namespace
