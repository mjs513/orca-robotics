#include "platformcsfinder.h"
#include <iostream>
#include "iknowsplatformposition2d.h"

using namespace std;

namespace orcaqgui {

bool 
PlatformCSFinder::findPlatformCS( const QList<GuiElementModel::InterfaceNode*> elements,
                                  const QString                                &platform,
                                  float                                        &x,
                                  float                                        &y,
                                  float                                        &theta )
{
    //
    // Find the most reliable element that reckons it knows where we are.
    //
    int mostReliable = -1;
    IKnowsPlatformPosition2d *mostReliablePosElement = NULL;

    for ( int i=0; i < elements.size(); ++i )
    {
        //cout<<endl<<elements_[i]->id.toStdString()<<" on "<<elements_[i]->platformatrix.toStdString();

        orcaqgui::IKnowsPlatformPosition2d *posElem = 
            dynamic_cast<orcaqgui::IKnowsPlatformPosition2d*>(elements[i]->element);
        if ( posElem != NULL )
        {
            // Ignore if it's the wrong platform.
            if ( elements[i]->platform != platform ) continue;

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
    theta = mostReliablePosElement->theta();
    return true;
}


}
