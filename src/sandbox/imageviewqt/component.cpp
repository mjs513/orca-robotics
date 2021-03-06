/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2008 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "component.h"
#include "mainthread.h"
#include "viewwidget.h"
#include "imagequeue.h"

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

using namespace imageviewqt;

Component::Component()
: orcaice::Component( "ImageViewQt" )
, imageQueue_(new ImageQueue(1))
{
}

Component::~Component()
{
    delete imageQueue_;
}

void
Component::start()
{

    //create the event loop
    int argc = 0;
    char **argv = 0;
    
    QApplication app(argc, argv);
    QWidget* window = new QWidget();
    window->setWindowTitle("Orca Qt Image Viewer");

    // create and show viewer and some information in labels
    ViewWidget* viewer = new ViewWidget(imageQueue_);

    QLabel * fpsLabel = new QLabel("FPS: ");
    QLabel * fpsDisplay = new QLabel;
    QHBoxLayout *hbox1 = new QHBoxLayout;
    hbox1->addWidget(fpsLabel);
    hbox1->addWidget(fpsDisplay);
    QWidget* labels = new QWidget;
    labels->setLayout(hbox1);
    labels->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(viewer);
    vbox->addWidget(labels);

    window->setLayout(vbox);
    window->show();

    // connect signal from queue to slot of widget to update
    QObject::connect(imageQueue_, SIGNAL(imagePushed()), viewer, SLOT(update()));
    QObject::connect(viewer, SIGNAL(fpsChanged(double)), fpsDisplay, SLOT(setNum(double)));

    // start network thread
    mainThread_ = new MainThread(imageQueue_, context() );
    mainThread_->start();


    // start QApplication event loop
    app.exec();
    
    //app loop is done, delete gui objects
    delete viewer;
  
    // normally ctrl-c handler does this, now we have to because UserThread keeps the thread
    context().communicator()->shutdown();
}

void 
Component::stop()
{
    gbxiceutilacfr::stopAndJoin( mainThread_ );
}
