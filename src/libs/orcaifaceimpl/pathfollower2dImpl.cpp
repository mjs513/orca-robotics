#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaice/convertutils.h>
 
#include "pathfollower2dImpl.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class PathFollower2dI : virtual public ::orca::PathFollower2d
{
public:

    PathFollower2dI( PathFollower2dImpl   &impl,
                     AbstractPathFollowerCallback &callback )
        : impl_(impl),
          callback_(callback) {}
    virtual ::orca::PathFollower2dData getData(const Ice::Current&)
        { return impl_.internalGetData(); }
    virtual IceStorm::TopicPrx subscribe(const orca::PathFollower2dConsumerPrx& subscriber, const ::Ice::Current& = ::Ice::Current())
        { return impl_.internalSubscribe(subscriber); }

    virtual void setData( const orca::PathFollower2dData &path, bool activateImmediately, const Ice::Current& )
        { callback_.setData(path,activateImmediately); }
    virtual void activateNow(const Ice::Current&)
        { callback_.activateNow(); }
    virtual void setEnabled( bool enabled, const Ice::Current& )
        { callback_.setEnabled(enabled); }
    virtual orca::PathFollower2dState getState(const Ice::Current&)
        { return callback_.getState(); }

private:
    PathFollower2dImpl   &impl_;
    AbstractPathFollowerCallback &callback_;
};

//////////////////////////////////////////////////////////////////////

PathFollower2dImpl::PathFollower2dImpl( AbstractPathFollowerCallback    &callback,
                                        const std::string       &interfaceTag, 
                                        const orcaice::Context  &context  ) :
    callback_(callback),
    interfaceName_(orcaice::getProvidedInterface(context,interfaceTag).iface),
    context_(context)
{
    init();
}

PathFollower2dImpl::PathFollower2dImpl( AbstractPathFollowerCallback    &callback,
                                        const orcaice::Context  &context,
                                        const std::string       &interfaceName ) :
    callback_(callback),
    interfaceName_(interfaceName),
    context_(context)
{
    init();
}

PathFollower2dImpl::~PathFollower2dImpl()
{
    orcaice::tryRemoveInterface( context_, interfaceName_ );
}

void
PathFollower2dImpl::init()
{
    topicHandler_.reset( new PathFollower2dTopicHandler( orcaice::toTopicAsString(context_.name(),interfaceName_), context_ ) );
    ptr_ = new PathFollower2dI( *this, callback_ );
}

void
PathFollower2dImpl::initInterface()
{
    topicHandler_->connectToTopic();
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
PathFollower2dImpl::initInterface( gbxutilacfr::Stoppable* activity, const std::string& subsysName, int retryInterval )
{
    topicHandler_->connectToTopic( activity, subsysName, retryInterval );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, activity, subsysName, retryInterval );
}

::orca::PathFollower2dData
PathFollower2dImpl::internalGetData()
{
    context_.tracer().debug( "PathFollower2dImpl::internalGetData()", 5 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::PathFollower2dData data;
    dataStore_.get( data );
    return data;
}


IceStorm::TopicPrx 
PathFollower2dImpl::internalSubscribe(const orca::PathFollower2dConsumerPrx& subscriber)
{
    if ( !topicHandler_.get() ) 
    {
        throw orca::SubscriptionFailedException("topicHandler_ does not exist.");
    }
    
    // if we have data, send all the information we have to the new subscriber (and to no one else)
    if ( dataStore_.isEmpty() )
    {
        return topicHandler_->subscribe( subscriber );
    }
    else
    {
        orca::PathFollower2dData data;
        dataStore_.get( data );
    
        return topicHandler_->subscribe( subscriber, data );
    }
}

void
PathFollower2dImpl::localSend( const orca::PathFollower2dData& data )
{
    topicHandler_->publish( data );
}

void
PathFollower2dImpl::localSetAndSend( const orca::PathFollower2dData& data )
{
    dataStore_.set( data );
    
    topicHandler_->publish( data );
}

void 
PathFollower2dImpl::localSetState( const orca::PathFollower2dState &state )
{
    // check that communicator still exists
    if ( !context_.communicator() ) {
        return;
    }

    try {
        topicHandler_->publisherPrx()->setState(state);
    }
    catch ( Ice::CommunicatorDestroyedException & )
    {
        // If we see this, we're obviously shutting down.  Don't bitch about anything.
    }
    catch ( Ice::Exception &e )
    {
        std::stringstream ss; ss << interfaceName_ << "::" << __func__ << ": Failed push to IceStorm: " << e;
        context_.tracer().warning( ss.str() );

        bool reconnected = topicHandler_->connectToTopic();
        if ( reconnected )
        {
            try {
                // try again to push that bit of info
                topicHandler_->publisherPrx()->setState(state);
            }
            catch ( Ice::Exception &e )
            {
                std::stringstream ss;
                ss << interfaceName_ << ": Re-push of state failed: " << e;
                context_.tracer().info( ss.str() );
            }
        }
    }
}

}

