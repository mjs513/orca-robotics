/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHPLANNER_BUTTONS_H
#define PATHPLANNER_BUTTONS_H

#include <QObject>

class QAction;

namespace hydroqguielementutil {
    class ShortcutKeyManager;
    class IHumanManager;   
}

namespace hydroqguipath {

//
// This class sets up all the buttons and actions user interaction with the pathplanner.
//
// Author: Tobias Kaupp
//
class PathplannerButtons : public QObject
{
    Q_OBJECT
            
public:
    PathplannerButtons( QObject                                  *parent, 
                        hydroqguielementutil::IHumanManager      &humanManager, 
                        hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager,
                        const std::string                        &proxyString);
    ~PathplannerButtons();
    void setWpButton( bool onOff );

private:
    QAction *hiWaypoints_;
    hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager_;
};

}

#endif
