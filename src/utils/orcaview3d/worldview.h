/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAGUI3D_WORLDVIEW3D_H
#define ORCA_ORCAGUI3D_WORLDVIEW3D_H

#include <QMouseEvent>
#include <QFileDialog>
#include <QString>
#include <QtOpenGL>
#include <hydroqgui/hydroqgui.h>
#include <orcaqgui3d/platformcsfinder.h>
#include <orcaqgui3d/view.h>
#include "viewhandler.h"

namespace orcaview3d 
{

class WorldView : public QGLWidget, 
                  public orcaqgui3d::View
{
   Q_OBJECT
public:
    WorldView( orcaqgui3d::PlatformCSFinder              &platformCSFinder,
               ::hydroqguielementutil::MouseEventManager &mouseEventManager,
               hydroqgui::GuiElementSet                  &guiElementSet,
               hydroqgui::CoordinateFrameManager         &coordinateFrameManager,
               ::hydroqguielementutil::IHumanManager     &humanManager,
               hydroqgui::PlatformFocusManager           &platformFocusManager,
               int                                        displayRefreshTime,
               QWidget                                   *parent = NULL );

    QSize sizeHint() const { return QSize( 400, 400 ); }

    // All in S.I. units and Orca-style coordinate system
    virtual double cameraX() const { return viewHandler_.x(); }
    virtual double cameraY() const { return viewHandler_.y(); }
    virtual double cameraZ() const { return viewHandler_.z(); }
    virtual double cameraRoll() const { return viewHandler_.roll(); }
    virtual double cameraPitch() const { return viewHandler_.pitch(); }
    virtual double cameraYaw() const { return viewHandler_.yaw(); }

    virtual bool isAntialiasingEnabled() const { return isAntialiasingEnabled_; }

public slots:

    void setAntiAliasing( bool antiAliasing );

private slots:

    void reDisplay();

private:

    void updateAllGuiElements();
    void paintAllGuiElements( bool isCoordinateFramePlatformLocalised );

    // finds coord system of a platform
    const orcaqgui3d::PlatformCSFinder &platformCSFinder_;
    hydroqgui::CoordinateFrameManager &coordinateFrameManager_;

    // handles distribution of mouse events to GuiElements
    ::hydroqguielementutil::MouseEventManager &mouseEventManager_;

    hydroqgui::GuiElementSet     &guiElementSet_;

    ::hydroqguielementutil::IHumanManager &humanManager_;
    hydroqgui::PlatformFocusManager &platformFocusManager_;

    // Returns true if the platform which owns the coordinate system is localised
    // or if "global" owns the coordinate system 
    bool transformToPlatformOwningCS();

    // from QGLWidget
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    // event handlers
    void mousePressEvent( QMouseEvent* );
    void mouseMoveEvent( QMouseEvent* );
    void mouseReleaseEvent( QMouseEvent* );
    void mouseDoubleClickEvent( QMouseEvent* );
    void keyPressEvent(QKeyEvent *e);
    
    QTimer *displayTimer_;

    bool isAntialiasingEnabled_;

    // Current camera viewpoint
    ViewHandler viewHandler_;
};

} // namespace

#endif