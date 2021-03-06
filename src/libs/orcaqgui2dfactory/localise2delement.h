/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_LOCALISE2D_GUI_ELEMENT_H
#define ORCAGUI_LOCALISE2D_GUI_ELEMENT_H

#include <QPointF>

#include <orcaice/context.h>

#include <hydroqguielementutil/iknowsplatformposition2d.h>
#include <orcaqguielementutil/icestormguielement2d.h>
#include <hydroqguielementutil/ihumanmanager.h>
#include <orcaqgui2dfactory/localise2dpainter.h>

namespace orcaqgui2d
{

class Localise2dElement
    : public orcaqguielementutil::IceStormGuiElement2d<Localise2dPainter,
                             orca::Localise2dData,
                             orca::Localise2dPrx,
                             orca::Localise2dConsumer,
                             orca::Localise2dConsumerPrx>,
      public hydroqguielementutil::IKnowsPlatformPosition2d
{
public:
    Localise2dElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                       const orcaice::Context  &context,
                       bool                     beginDisplayHistory=false,
                       int                      timeoutMs=30000 )
        : orcaqguielementutil::IceStormGuiElement2d<Localise2dPainter,
                            orca::Localise2dData,
                            orca::Localise2dPrx,
                            orca::Localise2dConsumer,
                            orca::Localise2dConsumerPrx>(guiElementInfo, context, painter_, timeoutMs ),
          painter_( beginDisplayHistory ),
          x_(0),
          y_(0),
          theta_(0),
          haveGeometry_(false)
        {
        };

    virtual bool isInGlobalCS() { return true; }
    virtual void iceStormConnectedEvent();
    virtual QStringList contextMenu();
    virtual void execute( int action );
    
    virtual void setColor( QColor color ) { painter_.setColor(color); }
    virtual void setFocus( bool inFocus ) { painter_.setFocus( inFocus ); };
    virtual void setUseTransparency( bool useTransparency ) { painter_.setUseTransparency( useTransparency ); };

    // Need a special update function to set (x,y,theta)
    // The Localise2dElement needs this because it's special kind of GuiElement:
    //   A platform has a position in the world and various other things are
    //   drawn with respect to that position.
    virtual void update();

    // Access to ML estimate.
    virtual double x() const { return x_; }
    virtual double y() const { return y_; }
    virtual double theta() const { return theta_; }
    virtual int platformKnowledgeReliability() const { return 7; }
    virtual QPointF pos() const { return QPointF(x_,y_); };

private:
    Localise2dPainter painter_;

    double x_;
    double y_;
    double theta_;
    
    void tryToGetGeometry();
    bool haveGeometry_;
};

}

#endif
