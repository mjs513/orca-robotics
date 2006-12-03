#ifndef IHUMANMANAGER_H
#define IHUMANMANAGER_H

#include <QObject>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <orcaqgui/guielement.h>
#include <orcaqgui/guielementmodel.h>

namespace orcaqgui {

enum MessageType
{
    Information,
    Warning,
    Error    
};

//!
//! @brief For managing the humans
//!
//! @author Alex Brooks
//!
class IHumanManager
{

public: 

    virtual ~IHumanManager() {}

    virtual void showBoxMsg( MessageType type, QString msg ) = 0;
    virtual void showStatusMsg( MessageType type, QString msg ) = 0;

    // Others can add things to the menus/toolbars.
    // Play nicely with each other!
    virtual QMenu    *fileMenu()=0;
    virtual QMenu    *optionsMenu()=0;
    virtual QMenu    *displayMenu()=0;
    virtual QToolBar *toolBar()=0;

    //! There's a global 'mode'.
    //! Zero or one of the GuiElements 'owns' this mode.
    virtual GuiElement *modeOwner() const=0;
    virtual bool requestMode( GuiElement *requester )=0;
    virtual bool modeIsOwned()=0;
    virtual void relinquishMode( GuiElement *relinquisher )=0;

    // Allows access to _all_ GuiElements.  Use with caution!
    virtual GuiElementModel &guiElementModel()=0;

private: 

};

}

#endif
