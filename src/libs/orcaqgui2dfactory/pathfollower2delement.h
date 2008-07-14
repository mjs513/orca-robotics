/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHFOLLOWER2DELEMENT_H
#define PATHFOLLOWER2DELEMENT_H

#include <orca/pathfollower2d.h>

#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>

#include <hydroqguielementutil/guielement2d.h>
#include <orcaqgui2dfactory/pathpainter.h>
#include <orcaqgui2dfactory/pathinput.h>

namespace orcaqgui2d {

////////////////////////////////////////////////////////////////////////////////
// The consumer object. We need this here because PathFollower2dElement cannot inherit from IceStormElement2d.
// Reason is that PathFollower2dConsumer has several non-standard purely virtual member functions.
class PathUpdateConsumer : public orca::PathFollower2dConsumer
{
public:
    
    void setData( const orca::PathFollower2dData& newPath, const ::Ice::Current& );
    void setWaypointIndex( int index, const ::Ice::Current& );
    void setActivationTime( const orca::Time& absoluteTime, double relativeTime, const ::Ice::Current& );
    void setEnabledState( bool enabledState, const ::Ice::Current& );

    gbxiceutilacfr::Store<orca::PathFollower2dData> pathPipe_;
    gbxiceutilacfr::Store<int> indexPipe_;
};
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This class sets up all the buttons and actions for user interaction.
// It can be instantiated on the heap and deleted, Qt cleans up for us
class PathfollowerButtons : public QObject
{
    Q_OBJECT
            
public:
    PathfollowerButtons( QObject                       *parent,
                         hydroqguielementutil::IHumanManager      &humanManager,
                         hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager,
                         std::string                    proxyString);
    ~PathfollowerButtons();
    
    void setWpButton( bool onOff );

private:
    QAction *hiWaypoints_;
    hydroqguielementutil::IHumanManager &humanManager_;
    hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager_;
};
//////////////////////////////////////////////////////////////////////////////

class PathFollower2dElement;
//
// Handles the human-interface stuff for the PathFollower2dElement
//
class PathFollowerHI  : public QObject
{
    Q_OBJECT

public:
    PathFollowerHI( PathFollower2dElement *pfElement,
                    std::string proxyString,
                    hydroqguielementutil::IHumanManager &humanManager,
                    hydroqguielementutil::MouseEventManager &mouseEventManager,
                    hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager,
                    const hydroqgui::GuiElementSet &guiElementSet,
                    PathPainter &painter,
                    WaypointSettings wpSettings,
                    bool activateImmediately,
                    QString dumpPath );

    void noLongerMouseEventReceiver();
    void paint( QPainter *p );

    void mousePressEvent(QMouseEvent *e) 
        {pathInput_->processPressEvent(e);}
    void mouseMoveEvent(QMouseEvent *e) {pathInput_->processMoveEvent(e);}
    void mouseReleaseEvent(QMouseEvent *e) {pathInput_->processReleaseEvent(e);}
    void mouseDoubleClickEvent(QMouseEvent *e) {pathInput_->processDoubleClickEvent(e);}
    void setFocus( bool inFocus );
    void setUseTransparency( bool useTransparency ); 
    
    // to dump the user (green) path to /tmp
    void savePath( const QString &fileName ) const
    {
        pathInput_->savePath( fileName );
    }

public slots:
    void savePathAs();
    void savePath();
    void waypointSettingsDialog();
    void waypointModeSelected();
    void send();
    void cancel();
    void allGo();
    void allStop();
    void go();
    void stop();

private:

    PathFollower2dElement *pfElement_;
    std::string proxyString_;
    hydroqguielementutil::IHumanManager &humanManager_;
    hydroqguielementutil::MouseEventManager &mouseEventManager_;
    hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager_;
    const hydroqgui::GuiElementSet &guiElementSet_;
    PathPainter   &painter_;

    QString pathFileName_;
    bool pathFileSet_;

    WaypointSettings wpSettings_;
    bool             activateImmediately_;

    std::auto_ptr<PathFollowerInput> pathInput_;
    
    // sets up and destroys buttons and associated actions
    std::auto_ptr<PathfollowerButtons> buttons_;

    // Do we own the global mode?
    bool gotMode_;
    
    bool useTransparency_;
    
    QString dumpPath_;
    int numPathDumps_;
    QString lastSavedPathFile_;
};

// We need to inherit from GuiElement2d, not from IceStormElement2d. 
// Reason is that PathFollower2dConsumer has a non-standard purely virtual 
// member function setWaypointIndex. 
// Disadvantage is that we have to subscribe ourselves.
class PathFollower2dElement : public hydroqguielementutil::GuiElement2d
{

public: 

    PathFollower2dElement( const orcaice::Context & context,
                           const std::string &proxyString,
                           hydroqguielementutil::IHumanManager &humanManager,
                           hydroqguielementutil::MouseEventManager &mouseEventManager,
                           hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager,
                           const hydroqgui::GuiElementSet &guiElementSet );

    // inherited from guielement
    void update();
    void paint( QPainter *p, int z );
    bool paintThisLayer( int z ) const
        { return painter_.paintThisLayer(z); }
    
    virtual bool isInGlobalCS() { return true; }
    virtual QStringList contextMenu();
    virtual void execute( int action );
    virtual void setColor( QColor color ) { painter_.setColor( color ); };
    virtual void setFocus( bool inFocus );
    virtual void setUseTransparency( bool useTransparency );

    virtual void noLongerMouseEventReceiver() { if ( pathHI_.get() ) pathHI_->noLongerMouseEventReceiver(); }
    virtual void mousePressEvent(QMouseEvent *e) { if ( pathHI_.get() ) pathHI_->mousePressEvent(e); }
    virtual void mouseMoveEvent(QMouseEvent *e) { if ( pathHI_.get() ) pathHI_->mouseMoveEvent(e); }
    virtual void mouseReleaseEvent(QMouseEvent *e) { if ( pathHI_.get() ) pathHI_->mouseReleaseEvent(e); }
    virtual void mouseDoubleClickEvent(QMouseEvent *e) { if ( pathHI_.get() ) pathHI_->mouseDoubleClickEvent(e); }
    
    // Tries to enable or disable the remote interface. Returns true if successful
    bool tryEnableRemoteInterface( bool enable );

    void go();
    void stop();
    void sendPath( const PathFollowerInput &pathInput, bool activateImmediately );

    void enableHI();
    void disableHI();
    bool isHIEnabled() { return pathHI_.get() != 0; }

    PathPainter &pathPainter() { return painter_; }

private: 
    void doInitialSetup();

    PathPainter painter_;

    int connectToInterface();
    bool isConnected_;
    void getInitialData();

    PathUpdateConsumer *pathUpdateConsumer_;
    orca::PathFollower2dConsumerPrx callbackPrx_;
    orca::PathFollower2dPrx pathFollower2dPrx_;
    std::string proxyString_;

    orcaice::Context context_;
    
    hydroqguielementutil::IHumanManager       &humanManager_;
    hydroqguielementutil::MouseEventManager   &mouseEventManager_;
    hydroqguielementutil::ShortcutKeyManager  &shortcutKeyManager_;
    const hydroqgui::GuiElementSet &guiElementSet_;
    
    bool firstTime_;
    gbxiceutilacfr::Timer *timer_;
    gbxiceutilacfr::Timer *activationTimer_;
    
    // toggle states
    bool displayWaypoints_;
    bool displayPastWaypoints_;
    bool displayFutureWaypoints_;
    bool displayOlympicMarker_;
    bool currentTransparency_;
    bool isInFocus_;

    bool isRemoteInterfaceSick_;
    
    // Handles human interface
    std::auto_ptr<PathFollowerHI> pathHI_;
    
    // returns 0 if remote call works otherwise -1
    int isFollowerEnabled( bool &isEnabled );
    
};

}

#endif
