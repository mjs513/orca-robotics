#include "viewhandler.h"
#include <iostream>
#include <GL/gl.h>
#include <gbxutilacfr/mathdefs.h>

using namespace std;

namespace orcaview3d {

namespace {
    const int INIT_YAW   = 0;
    const int INIT_PITCH = -60;
}

ViewHandler::ViewHandler()
    : xOffset_(-7),
      yOffset_(0),
      zOffset_(2),
      yaw_(90),
      pitch_(INIT_PITCH)
{
}

void 
ViewHandler::applyViewingTransformation() const
{
//     cout<<"TRACE(viewhandler.cpp): "<<__func__<<": x,y,z:     "<<xOffset_<<","<<yOffset_<<","<<zOffset_ << endl;
//     cout<<"TRACE(viewhandler.cpp): "<<__func__<<": pitch,yaw: "<<pitch_<<","<<yaw_ << endl;

    // Put the camera in position
    // It starts looking down the negative-z axis, with y being 'up'.
    // OpenGL uses a left-handed coordinate-system?

    glRotatef(pitch_,1,0,0);
    glRotatef(yaw_,0,0,1);
    glTranslatef(-xOffset_, -yOffset_, -zOffset_);    
}

void
ViewHandler::mousePressEvent( QMouseEvent* e )
{
    prevDragPos_ = e->pos();
}

bool
ViewHandler::mouseMoveEvent( QMouseEvent* e )
{
    const float sensitivity = 0.02;

    float dx=0, dy=0;
    if ( e->buttons() & Qt::LeftButton ||
         e->buttons() & Qt::RightButton ||
         e->buttons() & Qt::MidButton )
    {
        dx = e->pos().x() - prevDragPos_.x();
        dy = e->pos().y() - prevDragPos_.y();        
        prevDragPos_ = e->pos();        
    }

    // cout<<"TRACE(worldview3d.cpp): dx, dy: " << dx << "," << dy << endl;

    if ( e->buttons() & Qt::LeftButton )
    {
        // Rotate camera
        //cout<<"TRACE(worldview3d.cpp): rotate" << endl;
        yaw_   -= sensitivity*dx*10;
        pitch_ -= sensitivity*dy*10;
        if (pitch_ > 0)   pitch_ = 0;
        if (pitch_ < -90) pitch_ = -90;
        return true;
    }
    else if ( e->buttons() & Qt::RightButton )
    {
        // Pan camera (relative to the view-point)
        float x = sensitivity*dx*0.1;
        float y = sensitivity*dy*0.1;
        // cout<<"TRACE(viewhandler.cpp): x,y: " << x << ", " << y << endl;

        float yawRad = yaw_*M_PI/180.0;

        xOffset_ += x*cos(yawRad) - y*sin(yawRad);
        yOffset_ -= x*sin(yawRad) + y*cos(yawRad);
        //cout<<"TRACE(worldview3d.cpp): pan" << endl;
        return true;
    }
    return false;
}

bool 
ViewHandler::keyPressEvent(QKeyEvent *e)
{
    const double amt = 0.2;

    bool needUpdate=true;
    switch ( e->key() )
    {
    case Qt::Key_O:
    {
        double yawRad = yaw_*M_PI/180.0;
        xOffset_ += amt*cos(yawRad);
        yOffset_ -= amt*sin(yawRad);
        break;
    }
    case Qt::Key_U:
    {
        double yawRad = yaw_*M_PI/180.0;
        xOffset_ -= amt*cos(yawRad);
        yOffset_ += amt*sin(yawRad);
        break;
    }
    case Qt::Key_K:
    {
        double yawRad = yaw_*M_PI/180.0;
        xOffset_ += -amt*sin(yawRad);
        yOffset_ -=  amt*cos(yawRad);
        break;
    }
    case Qt::Key_I:
    {
        double yawRad = yaw_*M_PI/180.0;
        xOffset_ -= -amt*sin(yawRad);
        yOffset_ +=  amt*cos(yawRad);
        break;        
    }
    case Qt::Key_J:
    {
        yaw_ -= 5;
        break;
    }
    case Qt::Key_L:
    {
        yaw_ += 5;
        break;        
    }
    case Qt::Key_Up:
    {
        zOffset_ += 0.1;
        break;
    }
    case Qt::Key_Down:
    {
        zOffset_ -= 0.1;
        break;        
    }
    default:
    {
        needUpdate=false;
        break;
    }
    }
    return needUpdate;
}

}
