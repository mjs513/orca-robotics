#ifndef ORCAQGUI2DFACTORY_BUTTONELEMENT_H
#define ORCAQGUI2DFACTORY_BUTTONELEMENT_H

#include <hydroqguielementutil/guielement.h>
#include <orca/button.h>
#include <string>
#include <orcaice/context.h>

namespace hydroqguielementutil {
    class IHumanManager;
}

namespace orcaqgui2d {

//!
//! @author Alex Brooks
//!
class ButtonElement : public hydroqguielementutil::GuiElement
{

public: 

    ButtonElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                   const orcaice::Context                     &context,
                   const std::string                          &proxyString,
                   hydroqguielementutil::IHumanManager        &humanManager );

    virtual QStringList contextMenu();
    virtual void execute( int action );

    // This function doesn't need an implementation coz we don't paint anything.
    bool isInGlobalCS() { return false; }  

    void press();

private: 

    void connect();

    orcaice::Context context_;
    std::string      proxyString_;
    orca::ButtonPrx  buttonPrx_;

    hydroqguielementutil::IHumanManager &humanManager_;

    bool isConnected_;
};

}

#endif
