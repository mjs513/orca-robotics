#include "util.h"
#include <iostream>

using namespace std;

namespace orcaifaceimpl {

void
tryRemovePtr( orcaice::Context &context,
              Ice::ObjectPtr &ptr )
{
    if ( ptr == 0 ) return;

    try {
        context.adapter()->remove( context.communicator()->stringToIdentity( ptr->ice_id() ) );
    }
    catch ( Ice::ObjectAdapterDeactivatedException & )
    {
        // This is OK, we're shutting down.
    }
    catch ( Ice::Exception &e )
    {
        cout << "orcaifaceimpl: Exception when removing Ice::ObjectPtr from adapter: " << e << endl;
    }
}

std::string getInterfaceNameFromTag( const orcaice::Context &context,
                                     const std::string      &interfaceTag )
{
    orca::FQInterfaceName fqIName = orcaice::getProvidedInterface( context, interfaceTag );
    return fqIName.iface;
}
    
std::string getTopicNameFromInterfaceName( const orcaice::Context &context,
                                           const std::string      &interfaceName )
{
    return orcaice::toString( orcaice::getProvidedTopicWithString( context, interfaceName ) );
}

}
