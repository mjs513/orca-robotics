/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHPLANNER2DELEMENT_H
#define PATHPLANNER2DELEMENT_H

#include <iostream>
#include <orca/pathplanner2d.h>

#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <orcaqguielementutil/icestormguielement2d.h>

#include <orcaqgui2dfactory/pathplannerpainter.h>
#include <orcaqgui2dfactory/pathplanneruserinteraction.h>

namespace orcaqgui2d {

// There's two consumer objects: the first one for icestorm (pathplanner pushes out whatever it computed) which is part
// of the base class IceStormGuiElement2d.
// The second one here is for answers to tasks which were set by the GUI.

////////////////////////////////////////////////////////////////////////////////
class PathPlannerTaskAnswerConsumer : public orca::PathPlanner2dConsumer
{
    public:

        virtual void setData(const ::orca::PathPlanner2dData& data, const ::Ice::Current& = ::Ice::Current());
        gbxiceutilacfr::Store<QString> msgStore_;
};
////////////////////////////////////////////////////////////////////////////////


//
// @author Tobias Kaupp
//
class PathPlanner2dElement : public orcaqguielementutil::IceStormGuiElement2d<PathPlannerPainter,
                             orca::PathPlanner2dData,
                             orca::PathPlanner2dPrx,
                             orca::PathPlanner2dConsumer,
                             orca::PathPlanner2dConsumerPrx>
{

public: 

    PathPlanner2dElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                          const orcaice::Context                     &context );
    ~PathPlanner2dElement();

    void update();
    
    // overriding paint since we have to paint human input
    void paint( QPainter *p, int z );
    
    virtual void iceStormConnectedEvent();
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

    PathPlannerPainter painter_;
    
    // Task answer
    PathPlannerTaskAnswerConsumer *pathTaskAnswerConsumer_;
    orca::PathPlanner2dConsumerPrx taskCallbackPrx_;
    Ice::ObjectPtr pathPlanner2dConsumerObj_;
    
    orca::PathPlanner2dPrx pathPlanner2dPrx_;
    
    orcaice::Context context_;
    std::string proxyString_;
        
    // Handles user interaction
    PathPlannerUserInteraction pathHI_;
};

}

#endif
