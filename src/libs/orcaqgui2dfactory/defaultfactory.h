/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_DEFAULT_FACTORY_H
#define ORCAGUI_DEFAULT_FACTORY_H

#include <orcaqgui2d/iguielementfactory.h>
#include <orcaice/context.h>
#include <hydroutil/hydroutil.h>

namespace orcaqgui2d
{

    //!
    //! Generates 2D Gui Elements based on its type.
    //!
    class DefaultFactory : public IGuiElementFactory    
    {
    public:

        DefaultFactory();

        void setContext( const orcaice::Context &context )
            { context_ = context; isContextSet_ = true; }

        virtual hydroqgui::IGuiElement* create( const QString                &elementType,
                                                const QStringList            &elementDetails,
                                                QColor                        suggestedColor,
                                                hydroqgui::IHumanManager     &humanManager,
                                                hydroqgui::MouseEventManager &mouseEventManager,
                                                hydroqgui::ShortcutKeyManager &shortcutKeyManager,
                                                const hydroqgui::GuiElementSet &guiElementSet ) const;
        
        bool lookupElementType( const QStringList &ids, QString &elementType ) const;

    private:
        
        bool                      isContextSet_;
        orcaice::Context          context_;

    };

}


//
// Hook for dynamic loading
//
extern "C" {
    hydroqgui::IGuiElementFactory *createFactory();
}

#endif
