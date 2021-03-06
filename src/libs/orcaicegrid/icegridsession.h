/*
 * Orca-Robotics Project: Components for robotics
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alex Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICEGRID_ICEGRID_SESSION_H
#define ORCAICEGRID_ICEGRID_SESSION_H

#include <IceGrid/Registry.h>
#include <gbxsickacfr/gbxiceutilacfr/safethread.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <orcaice/context.h>

namespace orcaicegrid
{

//! This exception is raised when the session is not connected.
class SessionNotConnectedException : public gbxutilacfr::Exception
{
public:
    SessionNotConnectedException(const char *file, const char *line, const char *message)
            : Exception( file, line, message ) {};
    SessionNotConnectedException(const char *file, const char *line, const std::string &message)
            : Exception( file, line, message ) {};
};

//!
//! @brief Creates and maintains an IceGrid admin session.
//!
//! @author Alex Brooks
//!
class IceGridSession : public gbxiceutilacfr::SafeThread
{
public:
    //! Constructor.
    //!
    //! The session will automatically register these observers the next time
    //! (and every time) session is created. Pass an null proxy in place
    //! of an observer which you don't want to register.
    IceGridSession( const orcaice::Context& context,
        const IceGrid::RegistryObserverPrx&     reg =IceGrid::RegistryObserverPrx(),
        const IceGrid::NodeObserverPrx&         node=IceGrid::NodeObserverPrx(),
        const IceGrid::ApplicationObserverPrx&  app=IceGrid::ApplicationObserverPrx(),
        const IceGrid::AdapterObserverPrx&      adpt=IceGrid::AdapterObserverPrx(),
        const IceGrid::ObjectObserverPrx&       obj=IceGrid::ObjectObserverPrx() );

    ~IceGridSession() {};

    //! State of the Session
    enum SessionState
    {
        //! Disconnected
        Disconnected,
        //! Connecting
        Connecting,
        //! Connected
        Connected,
        //! Disconnecting
        Disconnecting
    };

    //! Returns current state of the session. Thread-safe.
    SessionState getState();

    //! Set the observer proxies that receive notifications when the state of the registry or nodes changes.
    //! Calls the corresponding method of the Admin session. Catches the ObserverAlreadyRegisteredException
    //! exception.
    //! If the session has already been created, the observers are registered with the session. If the
    //! session does not exist, local variables are set and registration will happen the next time
    //! the session is created.
    void setObservers(
        const IceGrid::RegistryObserverPrx&     reg =IceGrid::RegistryObserverPrx(),
        const IceGrid::NodeObserverPrx&         node=IceGrid::NodeObserverPrx(),
        const IceGrid::ApplicationObserverPrx&  app=IceGrid::ApplicationObserverPrx(),
        const IceGrid::AdapterObserverPrx&      adpt=IceGrid::AdapterObserverPrx(),
        const IceGrid::ObjectObserverPrx&       obj=IceGrid::ObjectObserverPrx() );

    //! Reimplement this to perform custom action right after a new session is created.
    //! Returns TRUE if successful. On FALSE the session is destroyed and another one is created.
    virtual bool connectedEvent() { return true; };

    //! Reimplement this to perform custom action right after a existing session was lost (intentionally or not).
    //! (No value is returned because nothing can be done if something does go wrong).
    virtual void disconnectedEvent() {};

    // from SafeThread
    virtual void walk();

    static std::string toString( SessionState state );

protected:
    IceGrid::AdminSessionPrx session_;

    bool tryCreateSession();

private:

    gbxiceutilacfr::Store<SessionState> stateStore_;
    int timeoutSec_;

    IceGrid::RegistryObserverPrx     registryObserverPrx_;
    IceGrid::NodeObserverPrx         nodeObserverPrx_;
    IceGrid::ApplicationObserverPrx  applicationObserverPrx_;
    IceGrid::AdapterObserverPrx      adapterObserverPrx_;
    IceGrid::ObjectObserverPrx       objectObserverPrx_;

    // this actually tries to set observers as defined by member variables
    // if the session is not Connected, does nothing
    void trySetObservers();
    IceUtil::Mutex observerMutex_;

    orcaice::Context context_;
};

} // namespace

#endif
