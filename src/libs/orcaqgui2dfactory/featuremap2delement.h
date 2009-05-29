#ifndef FEATUREMAP2DELEMENT_H
#define FEATUREMAP2DELEMENT_H

#include <orcaqguielementutil/icestormguielement2d.h>
#include <orcaqgui2dfactory/featuremap2dpainter.h>

namespace hydroqguielementutil {
    class IHumanManager;
};

namespace orcaqgui2d
{

//!
//! @author Alex Brooks
//!
class FeatureMap2dElement
    : public orcaqguielementutil::IceStormGuiElement2d<FeatureMap2dPainter,
                             orca::FeatureMap2dData,
                             orca::FeatureMap2dPrx,
                             orca::FeatureMap2dConsumer,
                             orca::FeatureMap2dConsumerPrx>
{

public: 

    FeatureMap2dElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                         const orcaice::Context  &context,
                         const std::string       &proxyString,
                         hydroqguielementutil::IHumanManager *humanManager );

    virtual bool isInGlobalCS() { return true; }
    virtual QStringList contextMenu();
    virtual void execute( int action );
    virtual void setUseTransparency( bool useTransparency ) { painter_.setUseTransparency( useTransparency ); };

 public:
    void saveFeatureMapAs();
    void saveFeatureMap();

private:
    FeatureMap2dPainter painter_;
    hydroqguielementutil::IHumanManager *humanManager_;
    QString featureMapFileName_;
    bool featureMapFileNameSet_;
};

}

#endif
