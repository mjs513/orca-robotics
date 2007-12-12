#include <iostream>
#include <orcaice/orcaice.h>
#include "util.h"
#include "featuremap2dImpl.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class FeatureMap2dI : virtual public ::orca::FeatureMap2d
{
public:

    FeatureMap2dI( FeatureMap2dImpl &impl )
        : impl_(impl) {}

    // remote functions

    virtual ::orca::FeatureMap2dData getData(const Ice::Current&) const
        { return impl_.internalGetData(); }

    virtual void subscribe(const ::orca::FeatureMap2dConsumerPrx& consumer,
                           const Ice::Current&)
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::FeatureMap2dConsumerPrx& consumer,
                 const Ice::Current&)
        { impl_.internalUnsubscribe( consumer ); }

private:
    FeatureMap2dImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

FeatureMap2dImpl::FeatureMap2dImpl( const std::string       &interfaceTag, 
                                    const orcaice::Context  &context  )
    : interfaceName_(getInterfaceNameFromTag(context,interfaceTag)),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName_)),
      context_(context)
{
}

FeatureMap2dImpl::FeatureMap2dImpl( const orcaice::Context  &context,
                                    const std::string       &interfaceName )
    : interfaceName_(interfaceName),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName)),
      context_(context)
{
}

FeatureMap2dImpl::~FeatureMap2dImpl()
{
    tryRemoveInterface( context_, interfaceName_ );
}

void
FeatureMap2dImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString<orca::FeatureMap2dConsumerPrx>
        ( context_, consumerPrx_, topicName_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new FeatureMap2dI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
FeatureMap2dImpl::initInterface( hydroutil::Thread* thread, const std::string& subsysName, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithString( context_, consumerPrx_, topicName_, thread, subsysName, retryInterval );

    ptr_ = new FeatureMap2dI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );
}

::orca::FeatureMap2dData
FeatureMap2dImpl::internalGetData() const
{
    context_.tracer().debug( "FeatureMap2dImpl::internalGetData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::FeatureMap2dData data;
    dataProxy_.get( data );
    return data;
}

void
FeatureMap2dImpl::internalSubscribe(const ::orca::FeatureMap2dConsumerPrx& subscriber)
{   
    context_.tracer().debug( "FeatureMap2dImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    try {
        topicPrx_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway() );
    }
    catch ( const IceStorm::AlreadySubscribed & e ) {
        std::stringstream ss;
        ss <<"Request for subscribe but this proxy has already been subscribed, so I do nothing: "<< e;
        context_.tracer().debug( ss.str(), 2 );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"FeatureMap2dImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void
FeatureMap2dImpl::internalUnsubscribe(const ::orca::FeatureMap2dConsumerPrx& subscriber)
{
    context_.tracer().debug( "FeatureMap2dImpl::internalUnsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
FeatureMap2dImpl::localSetAndSend( const orca::FeatureMap2dData& data )
{
    dataProxy_.set( data );
    
    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::FeatureMap2dConsumerPrx,orca::FeatureMap2dData>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          interfaceName_,
          topicName_ );
}

}

