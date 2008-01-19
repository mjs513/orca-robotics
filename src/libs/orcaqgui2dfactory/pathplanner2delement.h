/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHPLANNER2DELEMENT_H
#define PATHPLANNER2DELEMENT_H

#include <iostream>
#include <orca/pathplanner2d.h>
#include <hydroiceutil/store.h>
#include <orcaqgui2d/icestormelement.h>
#include <orcaqgui2d/guielement2d.h>
#include <orcaqgui2dfactory/pathpainter.h>
#include <orcaqgui2dfactory/pathinput.h>

namespace orcaqgui2d {

class hydroqgui::IHumanManager;

// There's two consumer objects: the first one for icestorm (pathplanner pushes out whatever it computed) which is part
// of the base class IceStormElement.
// The second one here is for answers to tasks which were set by the GUI.

////////////////////////////////////////////////////////////////////////////////
class PathPlannerTaskAnswerConsumer : public orca::PathPlanner2dConsumer
{
    public:

        virtual void setData(const ::orca::PathPlanner2dData& data, const ::Ice::Current& = ::Ice::Current());
        hydroiceutil::Store<QString> msgStore_;
};
////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////

// This class sets up all the buttons and actions for user interaction.
// It can be instantiated on the heap and deleted, Qt cleans up for us
class PathplannerButtons : public QObject
{
    Q_OBJECT
            
public:
    PathplannerButtons( QObject *parent, hydroqgui::IHumanManager &humanManager, std::string proxyString);
    ~PathplannerButtons() 
    { 
        // Qt cleans up for us 
    };
    void setWpButton( bool onOff );

private:
    QAction *hiWaypoints_;
};
/////////////////////////////////////////

class PathPlanner2dElement;
class PathPlannerHI  : public QObject
{
    Q_OBJECT

public:
    PathPlannerHI( PathPlanner2dElement *ppElement,
                   std::string proxyString,
                   hydroqgui::IHumanManager &humanManager,
                   hydroqgui::MouseEventManager &mouseEventManager,
                   PathPainter &painter,
                   WaypointSettings wpSettings );
    ~PathPlannerHI();

    void noLongerMouseEventReceiver();
    void paint( QPainter *p );

    void mousePressEvent(QMouseEvent *e) 
        {pathInput_->processPressEvent(e);}
    void mouseMoveEvent(QMouseEvent *e) {pathInput_->processMoveEvent(e);}
    void mouseReleaseEvent(QMouseEvent *e) {pathInput_->processReleaseEvent(e);}
    void mouseDoubleClickEvent(QMouseEvent *e) {pathInput_->processDoubleClickEvent(e);}
    void setFocus( bool inFocus );
    void setUseTransparency( bool useTransparency ); 

public slots:
    void savePathAs();
    void savePath();
    void waypointSettingsDialog();
    void waypointModeSelected();
    void send();
    void cancel();

private:

    PathPlanner2dElement *ppElement_;
    std::string proxyString_;
    hydroqgui::IHumanManager &humanManager_;
    hydroqgui::MouseEventManager &mouseEventManager_;
    PathPainter   &painter_;

    QString pathFileName_;
    bool pathFileSet_;

    WaypointSettings wpSettings_;

    PathPlannerInput *pathInput_;
    
    // sets up and destroys buttons and associated actions
    PathplannerButtons *buttons_;
    
    // Do we own the global mode?
    bool gotMode_;
    
    bool useTransparency_;
};

//
// @author Tobias Kaupp
//
class PathPlanner2dElement : public orcaqgui2d::IceStormElement<PathPainter,
                             orca::PathPlanner2dData,
                             orca::PathPlanner2dPrx,
                             orca::PathPlanner2dConsumer,
                             orca::PathPlanner2dConsumerPrx>
{

public: 

    PathPlanner2dElement( const orcaice::Context       &context,
                          const std::string            &proxyString,
                          hydroqgui::IHumanManager     &humanManager,
                          hydroqgui::MouseEventManager &mouseEventManager );
    ~PathPlanner2dElement();

    void update();
    
    // overriding paint since we have to paint human input
    void paint( QPainter *p, int z );
    
    virtual void actionOnConnection();
    virtual bool isInGlobalCS() { return true; }
    virtual QStringList contextMenu();
    virtual void execute( int action );
    virtual void setColor( QColor color ) { painter_.setColor( color ); };
    virtual void setFocus( bool inFocus );
    virtual void setUseTransparency( bool useTransparency );

    virtual void noLongerMouseEventReceiver() { pathHI_.noLongerMouseEventReceiver(); }
    virtual void mousePressEvent(QMouseEvent *e) { pathHI_.mousePressEvent(e); }
    virtual void mouseMoveEvent(QMouseEvent *e) { pathHI_.mouseMoveEvent(e); }
    virtual void mouseReleaseEvent(QMouseEvent *e) { pathHI_.mouseReleaseEvent(e); }
    virtual void mouseDoubleClickEvent(QMouseEvent *e) { pathHI_.mouseDoubleClickEvent(e); }
    void sendPath( const PathPlannerInput &pathInput );

private: 

    PathPainter painter_;
    
    // Task answer
    PathPlannerTaskAnswerConsumer *pathTaskAnswerConsumer_;
    orca::PathPlanner2dConsumerPrx taskCallbackPrx_;
    Ice::ObjectPtr pathPlanner2dConsumerObj_;
    
    orca::PathPlanner2dPrx pathPlanner2dPrx_;
    
    orcaice::Context context_;
    std::string proxyString_;
    hydroqgui::IHumanManager &humanManager_;

    bool displayWaypoints_;
    bool currentTransparency_;
        
    // Handles human interface
    PathPlannerHI pathHI_;

};

}

#endif
