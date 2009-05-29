/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_ICESTORM_GUIELEMENT_2D_H
#define ORCAGUI_ICESTORM_GUIELEMENT_2D_H

#include <orcaqguielementutil/icestormelement.h>
#include <hydroqguielementutil/guielement2d.h>

namespace orcaqguielementutil {

template<class PainterType, class DataType, class ProxyType, class ConsumerType, class ConsumerPrxType>
class
IceStormGuiElement2d : public hydroqguielementutil::GuiElement2d,
                    public IceStormElement<DataType,ProxyType,ConsumerType,ConsumerPrxType>
{
public:

    //! timeoutMs is how long we wait before assuming a problem and trying to reconnect.
    //! (timoutMs = -1 means we never timeout)
    IceStormGuiElement2d( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                    const orcaice::Context                     &context,
                    const std::string                          &proxyString,
                    PainterType                                &painter,
                    const double                                timeoutMs = 5000.0 )
        : GuiElement2d(guiElementInfo),
        IceStormElement<DataType,ProxyType,ConsumerType,ConsumerPrxType>(context,proxyString,timeoutMs),
        painter_(painter)
    {}

    //! Derived classes may override this if they have to paint other stuff than just interface data (e.g. user input)
    virtual void paint( QPainter *p, int z )
        { painter_.paint( p, z ); }

    bool paintThisLayer( int z ) const
        { return painter_.paintThisLayer( z ); }

    // From GuiElement2d
    void update();
        
protected:
    
    //! Object to handle painting to our data to screen
    PainterType                     &painter_;
};
    
template<class PainterType, class DataType, class ProxyType, class ConsumerType, class ConsumerPrxType>
void IceStormGuiElement2d<PainterType,DataType,ProxyType,ConsumerType,ConsumerPrxType>::update()
{ 
    if (IceStormElement<DataType,ProxyType,ConsumerType,ConsumerPrxType>::shouldClearPainter_)
    {
        painter_.clear();
        IceStormElement<DataType,ProxyType,ConsumerType,ConsumerPrxType>::shouldClearPainter_ = false;
    }
    
    if (IceStormElement<DataType,
                        ProxyType,
                        ConsumerType,
                        ConsumerPrxType>::updateFromStore() )
    {               
        // transfer data into painter
        painter_.setData( IceStormElement<DataType,ProxyType,ConsumerType,ConsumerPrxType>::data_ );
    }
}
    

}

#endif
