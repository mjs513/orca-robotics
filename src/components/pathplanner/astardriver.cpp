/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcapathplan/orcapathplan.h>
#include <orcamisc/orcamisc.h>

#include "astardriver.h"

using namespace std;

namespace pathplanner {

AStarDriver::AStarDriver( orcaogmap::OgMap &ogMap,
                          double robotDiameterMetres,
                          double traversabilityThreshhold,
                          bool doPathOptimization )
    : ogMap_(ogMap)
{
    cout<<"TRACE(astardriver.cpp): AStarDriver()" << endl;

    pathPlanner_ = new orcapathplan::AStarPathPlanner( ogMap_,
                                                       robotDiameterMetres,
                                                       traversabilityThreshhold,
                                                       doPathOptimization );
}


void AStarDriver::computePath( const orca::PathPlanner2dTask& task,
                               orca::PathPlanner2dData& pathData )
{   
    // for each waypoint in the coarse path:
    const orca::Path2d &coarsePath = task.coarsePath;
    const orca::Waypoint2d *startWp = &(task.coarsePath[0]);
    
    for (unsigned int i=1; i<coarsePath.size(); i++)
    {
        const orca::Waypoint2d *goalWp = &(coarsePath[i]);
        orcapathplan::Cell2DVector pathSegment;

        orcamisc::CpuStopwatch watch(true);
        assert(pathPlanner_!=0);
        try {
            int startX, startY, endX, endY;
            ogMap_.getCellIndices( startWp->target.p.x, startWp->target.p.y, startX, startY );
            ogMap_.getCellIndices( goalWp->target.p.x,  goalWp->target.p.y,  endX,   endY );
            pathPlanner_->computePath( startX,
                                       startY,
                                       endX,
                                       endY,
                                       pathSegment );
        }
        catch ( orcapathplan::Exception &e )
        {
            std::stringstream ss;
            ss << "Error planning path segment from (" 
                    << startWp->target.p.x <<","<<startWp->target.p.y << ") to ("
                    << goalWp->target.p.x << ","<<goalWp->target.p.y<<"): "
                    << e.what()
                    << endl;
            
            throw orcapathplan::Exception( ss.str() );
        }
        cout<<"TRACE(astardriver.cpp): computing path segment took " << watch.elapsedSeconds() << "s" << endl;

//         // ====== Add waypoint parameters ================================
//         // ====== Different options could be implemented and chosen and runtime (via .cfg file)
//         vector<WaypointParameter> wpParaVector;
//         addWaypointParameters( wpParaVector, startWp, goalWp, pathSegment.size() );
//         // ===============================================================
        
        // ===== Append to the pathData which contains the entire path  ========
        orcapathplan::Result result = orcapathplan::PathOk;
//         orcapathplan::convert( ogMap_, pathSegment, wpParaVector, result, pathData );
        orcapathplan::convert( ogMap_, pathSegment, result, pathData );
        // ========================================================================
        
        // set last goal cell as new start cell
        startWp = goalWp;
    }
}

}


