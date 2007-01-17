/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>

#include <orcaice/orcaice.h>
#include "nethandler.h"

// implementations of Ice objects
#include <orcaifaceimpl/odometry2dI.h>
#include "velocitycontrol2dI.h"

using namespace std;
using namespace orca;
using namespace robot2d;

NetHandler::NetHandler(
                 orcaice::Buffer<orca::Odometry2dData>& odometryPipe,
                 orcaice::Notify<orca::VelocityControl2dData>& commandPipe,
                 const orca::VehicleDescription &descr,
                 const orcaice::Context& context ) :
    odometryPipe_(odometryPipe),
    commandPipe_(commandPipe),
    descr_(descr),
    context_(context)
{
}

NetHandler::~NetHandler()
{
}

void
NetHandler::run()
{
    try // this is once per run try/catch: waiting for the communicator to be destroyed
    {

    while ( isActive() )
    {
        try {
            context_.activate();
            break;
        }
        catch ( orcaice::NetworkException & e )
        {
            std::stringstream ss;
            ss << "nethandler::run: Caught NetworkException: " << e.what() << endl << "Will try again...";
            context_.tracer()->warning( ss.str() );
        }
        catch ( Ice::Exception & e )
        {
            std::stringstream ss;
            ss << "nethandler::run: Caught Ice::Exception while activating: " << e << endl << "Will try again...";
            context_.tracer()->warning( ss.str() );
        }
        catch ( ... )
        {
            cout << "Caught some other exception while activating." << endl;
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
    
    std::string prefix = context_.tag() + ".Config.";
    Ice::PropertiesPtr prop = context_.properties();
    
    //
    // PROVIDED: VelocityControl2d
    //

    // create servant for direct connections and tell adapter about it
    // don't need to store it as a member variable, adapter will keep it alive
    Ice::ObjectPtr velocityControl2dI = new VelocityControl2dI( descr_, commandPipe_ );

    // two possible exceptions will kill it here, that's what we want
    orcaice::createInterfaceWithTag( context_, velocityControl2dI, "VelocityControl2d" );


    //
    // PROVIDED: Odometry2d
    //
    orcaifaceimpl::Odometry2dIPtr odometry2dI = 
            new orcaifaceimpl::Odometry2dI( descr_, "Odometry2d", context_ );


    while ( isActive() ) {
        try {
            odometry2dI->initInterface();
            context_.tracer()->debug( "odometry interface initialized",2);
            break;
        }
        catch ( const orcaice::NetworkException& e ) {
            context_.tracer()->warning( "Failed to setup interface. Check Registry and IceStorm. Will try again in 2 secs...");
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
        catch ( const Ice::Exception& e ) {
            context_.tracer()->warning( "Failed to setup interface. Check Registry and IceStorm. Will try again in 2 secs...");
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
    }

    orca::Odometry2dData odometry2dData;
    const int odometryReadTimeout = 500; // [ms]
    orcaice::Timer publishTimer;
    double publishInterval = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            prefix+"Odometry2dPublishInterval", 0 );

    //
    // Main loop
    //
    while( isActive() )
    {
//         context_.tracer()->debug( "net handler loop spinning ",1);

        // block on the most frequent data source: odometry
        if ( odometryPipe_.getAndPopNext( odometry2dData, odometryReadTimeout ) ) {
//             context_.tracer()->debug( "Net loop timed out", 1);
            continue;
        }

        // check that we were not told to terminate while we were sleeping
        // otherwise, we'll get segfault (there's probably a way to prevent this inside the library)
        if ( isActive() && publishTimer.elapsed().toSecondsDouble()>=publishInterval ) {
            odometry2dI->localSetAndSend( odometry2dData );
            publishTimer.restart();
        } 
        else {
            odometry2dI->localSet( odometry2dData );
        }
    } // main loop
    
    }
    catch ( const Ice::CommunicatorDestroyedException & e )
    {
        context_.tracer()->debug( "net handler cought CommunicatorDestroyedException",1);        
        // it's ok, we must be quitting.
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    context_.tracer()->debug( "exiting NetHandler thread...",1);
}
