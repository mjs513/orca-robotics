/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alexei Makarenko, George Mathews, Matthew Ridley
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "raytrace.h"
#include <orca/bros1.h>

#include <hydropathplan/hydropathplan.h>
#include <hydroogmap/hydroogmap.h>
#include <gbxutilacfr/mathdefs.h>

using namespace orca;
using namespace hydroogmap;
using namespace hydropathplan;
using namespace std;

namespace {

hydropathplan::Cell2D 
point2cell( const hydroogmap::WorldCoords & pt, const double & originX, const double & originY,
            const double & res )
{
    return hydropathplan::Cell2D( (int)floor( ( pt.x-originX ) / res + 0.5 ) , (int)floor( ( pt.y-originY ) / res + 0.5 ) );
}

}

//
// Uses Bresenham's line algorithm
//

namespace details
{

    void rayTraceOct0( const hydropathplan::Cell2D & c0,
		      const hydropathplan::Cell2D & c1,
		      std::vector<Cell2D> & retCellVect )
    {
        int y = c0.y(); 
        int dx = 2*(c1.x()-c0.x());
        int dy = 2*(c1.y()-c0.y()); 
        int dydx = dy-dx; 
        int F = (dy-dx)/2; 

        for (int x=c0.x(); x<=c1.x(); x++) 
        { 
            retCellVect.push_back( Cell2D(x,y) );
            if ( F<0 ) 
            {
                F += dy; 
            }
            else 
            {
                y++; 
                F += dydx;
            } 
        }
    }

    void rayTraceOct1( const hydropathplan::Cell2D & c0,
		      const hydropathplan::Cell2D & c1,
		      std::vector<Cell2D> & retCellVect  )
    {
        int x = c0.x(); 
        int dx = 2*(c1.x()-c0.x());
        int dy = 2*(c1.y()-c0.y()); 
        int dxdy = dx-dy; 
        int F = (dx-dy)/2; 

        for (int y=c0.y(); y<=c1.y(); y++) 
        { 
            retCellVect.push_back( Cell2D(x,y) );
            if ( F<0 ) 
            {
                F += dx; 
            }
            else 
            {
                x++; 
                F += dxdy;
            } 
        }
    }

    void rayTraceOct2( const hydropathplan::Cell2D & c0,
		      const hydropathplan::Cell2D & c1,
		      std::vector<Cell2D> & retCellVect  )
    {
        int x = c0.x(); 
        int dx = -2*(c1.x()-c0.x());
        int dy = 2*(c1.y()-c0.y()); 
        int dxdy = dx-dy; 
        int F = (dx-dy)/2; 

        for (int y=c0.y(); y<=c1.y(); y++) 
        { 
            retCellVect.push_back( Cell2D(x,y) );
            if ( F<0 ) 
            {
                F += dx; 
            }
            else 
            {
                x--; 
                F += dxdy;
            } 
        }
    }

    void rayTraceOct7( const hydropathplan::Cell2D & c0,
		      const hydropathplan::Cell2D & c1,
		      std::vector<Cell2D> & retCellVect  )
    {
        int y = c0.y(); 
        int dx = 2*(c1.x()-c0.x());
        int dy = -2*(c1.y()-c0.y());
        int dydx = dy-dx; 
        int F = (dy-dx)/2; 

        for (int x=c0.x(); x<=c1.x(); x++) 
        { 
            retCellVect.push_back( Cell2D(x,y) );
            if ( F<0 ) 
            {
                F += dy; 
            }
            else 
            {
                y--; 
                F += dydx;
            } 
        }
    }
    
}

std::vector<Cell2D>
laser2og::rayTrace(const hydropathplan::Cell2D & c0,
		   const hydropathplan::Cell2D & c1 )
{
    /*
    *  Octants are defined as follows:
    *
    *   \2|1/ 
    *   3\|/0
    *   --x--
    *   4/|\7
    *   /5|6\ 
    *
    */
    
    
    std::vector<Cell2D> retCellVect;
    
    if( c0 == c1 )
    {
        retCellVect.push_back(c0);
        return retCellVect;
    }
    
    float slope;
    if ( c0.x() == c1.x() )
        slope = 999999;
    else
        slope = (float)(c1.y() - c0.y()) / (float)(c1.x() - c0.x());
    
    if ( slope >= 0.0 && slope <= 1.0 )
    {
        if ( c1.x() > c0.x() ) //(x1 > x0 )
        {
            // octant 0
            details::rayTraceOct0( c0, c1, retCellVect );
        }
        else
        {
            // octant 4
            details::rayTraceOct0( c1, c0, retCellVect );
        }
    }
    else if ( slope > 1.0 ) //(& = -inf)
    {
        if ( c1.y() > c0.y() ) //( x1 > x0 )
        {
            // octant 1
            details::rayTraceOct1(  c0, c1, retCellVect  );
        }
        else
        {
            // octant 5
            details::rayTraceOct1( c1, c0, retCellVect );
        }
    }
    else if ( slope < -1.0 )
    {
        if ( c1.y() > c0.y() ) //( y1 > y0 )
        {
            // octant 2
            details::rayTraceOct2( c0, c1, retCellVect  );
        }
        else
        {
            // octant 6
            details::rayTraceOct2( c1, c0, retCellVect );
        }
    }
    else // ( slope < 0.0 && slope >= -1.0 )
    {
        if ( c1.x() > c0.x() )//( x1 > x0 )
        {
            // octant 7
            details::rayTraceOct7(  c0, c1, retCellVect  );
        }
        else
        {
            // octant 3
            details::rayTraceOct7(  c1, c0, retCellVect  );
        }
    }
    
    // check ordering or cell
    if( retCellVect.size() > 1 )
        if( c0 != retCellVect.front() )
            reverse( retCellVect.begin(), retCellVect.end() );
    
//     if( retCellVect.size() > 1 )
//         if( c0 != retCellVect.front() )
//             cout<<"error"<<endl;
    
    return retCellVect;
}


std::vector<Cell2D>
laser2og::rayTrace( const hydroogmap::WorldCoords & p1, const hydroogmap::WorldCoords & p2,
                              const double originX, const double originY,
                              const double mapRes ) 
{
    Cell2D c1 = point2cell(p1, originX, originY, mapRes);
    Cell2D c2 = point2cell(p2, originX, originY, mapRes);
    
    return rayTrace(c1,c2);
}
