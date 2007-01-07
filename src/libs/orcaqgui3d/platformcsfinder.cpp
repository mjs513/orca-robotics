/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include "iknowsplatformposition.h"
#include "platformcsfinder.h"

using namespace std;

namespace orcaqgui3d {

bool 
PlatformCSFinder::findPlatformCS( const QList<orcaqgui::GuiElement*> elements,
                                    const QString                                &platform,
                                    float                                        &x,
                                    float                                        &y,
                                    float                                        &z,
                                    float                                        &roll,
                                    float                                        &pitch,
                                    float                                        &yaw )
{
    //
    // Find the most reliable element that reckons it knows where we are.
    //
    int mostReliable = -1;
    IKnowsPlatformPosition *mostReliablePosElement = NULL;

    for ( int i=0; i < elements.size(); ++i )
    {
        //cout<<endl<<elements_[i]->id.toStdString()<<" on "<<elements_[i]->platformatrix.toStdString();

        IKnowsPlatformPosition *posElem = 
            dynamic_cast<IKnowsPlatformPosition*>(elements[i]);
        if ( posElem != NULL )
        {
            // Ignore if it's the wrong platform.
            if ( elements[i]->platform() != platform ) continue;

            if ( posElem->platformKnowledgeReliability() > mostReliable )
            {
                mostReliablePosElement = posElem;
                mostReliable = posElem->platformKnowledgeReliability();
            }
        }
    }

    if ( mostReliablePosElement == NULL )
    {
        // No-one knows where we are.
        return false;
    }

    x     = mostReliablePosElement->x();
    y     = mostReliablePosElement->y();
    z     = mostReliablePosElement->z();
    roll  = mostReliablePosElement->roll();
    pitch = mostReliablePosElement->pitch();
    yaw   = mostReliablePosElement->yaw();
    return true;
}


}
