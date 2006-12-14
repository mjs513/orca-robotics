/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <orcaobj/mathdefs.h>

#include "pathplanutils.h"
#include "ipathplanner2d.h"
#include <orcamisc/orcamisc.h>

const float AC = 1.0;
const float DC = 1.4142;

using namespace std;
using namespace orcaogmap;


namespace orcapathplan {



// ================ NON-MEMBER FUNCTIONS =============================

bool areAllNans( const FloatMap &floatMap )
{
    for (unsigned int x=0; x<floatMap.sizeX(); x++ )
    {
        for (unsigned int y=0; y<floatMap.sizeY(); y++ )
        {
            // if we find a single element that is not nan we return false
            if ( !isnan(floatMap.element(x,y)) ) return false;
        }
    }
    // if we get here all elements are nan
    return true;
}
    
int sub2ind( const int          &indX,
             const int          &indY,
             const unsigned int &sizeX,
             const unsigned int &sizeY )
{
    assert( indX >= 0         &&
            indX < (int)sizeX && 
            indY >= 0         &&
            indY < (int)sizeY );

    return indY * sizeX + indX;
}

int sub2ind( const Cell2D       &cell,
             const unsigned int &sizeX,
             const unsigned int &sizeY )
{
    return sub2ind( cell.x(), cell.y(), sizeX, sizeY );
}
    
Cell2D ind2sub( const int          &index,
                const unsigned int &sizeX,
                const unsigned int &sizeY )
{
    int x = index % sizeX;
    int y = index / sizeX;

    assert ( x >= 0 && x < (int)sizeX );
    assert ( y >= 0 && y < (int)sizeY );
    return Cell2D( x, y );
}

// Returns a character corresponding to the value for console display (limited use)
std::string displayMapCell(float value)
{
    if ( isnan(value) ) {
        return "#";
    } else if (value==0.0) {
        return "0";
    } else {
        return " ";
    }
}

// Converts float map to a string for output on the screen
std::string toText( FloatMap &map )
{
    std::stringstream ss;
    int szX=map.sizeX();
    int szY=map.sizeY();
    int x, y;
    int step = 1 + map.sizeX() / 200;

    ss<<endl;
    ss<<"      ";
    for( x=0; x<szX; x+=step )
    {
        ss<<"-";
    }
    ss<<"-"<<endl;

    for( y=szY-1; y>=0; y-=step)
    {
        ss<<"     | ";
        for( x=0; x<szX; x+=step)
        {
            ss<<displayMapCell( map.element(x,y) );
        }
        ss<<"|"<<endl;
    }

    ss<<"  y   ";
    for( x=0; x<szX; x+=step )
    {
        ss<<"-";
    }
    ss<<"-"<<endl;

    ss<<"  |"<<endl;
    ss<<"  |____ x"<<endl;
    return ss.str();
}

// Returns the cell with the lowest value for all 8 cells surrounding c
Cell2D min8Cells( const FloatMap & floatMap, const Cell2D & c )
{
    double qval;
    Cell2D q;

    Cell2D cmin = c;
    double cval = floatMap.element( c );

    for ( int k=0; k<8; k++ )
    {
        q = surroundCell(c,k);
        if( floatMap.isInGrid( q ) )
        {
            qval = floatMap.element( q );
            if( qval < cval && !isnan(qval) )
            {
                cmin = q;
                cval = qval;
            }
        }
    }
    return cmin;
}

 // Returns the cell with the highes value for the 4 adjacent cells around c
Cell2D max4Cells( const FloatMap & floatMap, const Cell2D & c )
{
    Cell2D q,cmax;
    float qval,cval;
    
    //start with one cell
    cmax = adjacentCell(c,0);
    cval = floatMap.element( cmax );
    
    // and check the rest
    for ( int i=1; i<4; i++ )
    {
        q = adjacentCell(c,i);
        qval = floatMap.element( q );
        
        if( qval > cval )
        {
            cmax = q;
            cval = qval;
        }
    }
    return cmax;
}

// Returns the cell with the highest value for all 8 cells surrounding c
Cell2D max8Cells( const FloatMap & floatMap, const Cell2D & c )
{
    Cell2D q;
    float qval;
    
    //start with one cell
    Cell2D cmax = surroundCell(c,0);
    float cval = floatMap.element( cmax );

    // and check the rest
    for ( int i=1; i<8; i++ )
    {
        q = surroundCell(c,i);
        qval = floatMap.element( q );
        if( qval > cval )
        {
            cmax = q;
            cval = qval;
        }
    }
    return cmax;
}

bool isTraversable( const OgMap &ogMap,
                    const int indX,
                    const int indY,
                    const float traversabilityThreshhold)
{
    unsigned char val;
    if( ogMap.tryGridCell( indX, indY, val ) == true )
    {
        return ( val < (unsigned char) ((traversabilityThreshhold)*orcaogmap::CELL_OCCUPIED) );
    }
    return false;   
}

bool isTraversableNeighbors( const OgMap &ogMap,
                             const int indX,
                             const int indY,
                             const float traversabilityThreshhold )
{
    return ( isTraversable(ogMap,indX+1,indY,traversabilityThreshhold) || 
             isTraversable(ogMap,indX-1,indY,traversabilityThreshhold) ||
             isTraversable(ogMap,indX,indY+1,traversabilityThreshhold) || 
             isTraversable(ogMap,indX,indY-1,traversabilityThreshhold) );
}

// Convenience function, see above
inline bool isTraversableNeighbors( const orcaogmap::OgMap & ogMap, const Cell2D cell, const float traversabilityThreshhold )
{ return ( isTraversableNeighbors(ogMap,cell.x(),cell.y(),traversabilityThreshhold) ); } 

// Checks whether cell is in a free space region. If not, attempts to move it into one of the numCells surrounding cells. 
// If successful return TRUE, otherwise FALSE.
bool validateCell( const FloatMap & navMap, Cell2D & c, int numCells )
{

    if( !containsNan( navMap, c ) ) return true;

    //cout<< "validateCell: ("<<c.x()<<","<<c.y()<<") is inside or too close to obstacle, trying too move it out..." << endl;

    Cell2DVector Lcurr,Lnext;
    Cell2D q, q1;

    Lcurr.push_back(c);

    for( int i=0; i<numCells; i++ )
    {
        while ( !Lcurr.empty() )
        {
            q = Lcurr.back();
            Lcurr.pop_back();
            for ( int k=0; k<4; k++ )
            {
                q1 = adjacentCell(q, k);
                if ( containsNan( navMap,q1 ) )
                {
                    Lnext.push_back( q1 );
                }
                else
                {
                    c=q1;
                    //cout<< "cell moved to: ("<<c.x()<<","<<c.y()<<")"<<endl;
                    return true;
                }
            }
        }
        Lcurr.swap(Lnext); // Lcurr was empty
    }
    return false;
}

// Returns true if the cell is at the border of the ogMap otherwise false
bool isBorderCell( const orcaogmap::OgMap & ogMap, int i, int j )
{
    int sizeX = ogMap.numCellsX();
    int sizeY = ogMap.numCellsY();
        
    if ( (i==0) || (i==sizeX-1) || (j==0) || (j==sizeY-1) ) return true;
     
    return false;
}

bool losExists( const orcaogmap::OgMap &ogMap, double traversabilityThreshhold, const Cell2D &c1, const Cell2D &c2 )
{
    assert( ogMap.cellWithinMap( c1.x(), c1.y() ) );
    assert( ogMap.cellWithinMap( c2.x(), c2.y() ) );
    orcaogmap::OgLosTracer rayTracer( ogMap, (unsigned char)(traversabilityThreshhold*orcaogmap::CELL_OCCUPIED) );
    return rayTracer.isClearCells( c1.x(), c1.y(), c2.x(), c2.y() );
}

bool isIncluded( const Cell2DVector & C, const Cell2D & c )
{
    // better to start from the back where recent points were added ???
    if( find( C.rbegin(), C.rend(), c ) != C.rend() )
    {
        return true;
    }
    return false;
}

bool isIncluded( const Cell2DList & C, const Cell2D & c )
{
    // better to start from the back where recent points were added ???
    if( find( C.rbegin(), C.rend(), c ) != C.rend() )
    {
        return true;
    }
    return false;
}

int robotDiameterInCells( const OgMap & ogMap, const double robotDiameterMetres )
{
    assert( robotDiameterMetres >= 0.0 && 
            robotDiameterMetres < ogMap.worldSizeX() && 
            robotDiameterMetres < ogMap.worldSizeY() );

    double a = ogMap.metresPerCellX();
    double b = ogMap.metresPerCellY();
    double minRes = a < b ? a : b;
    return (int) ceil( robotDiameterMetres / minRes );
}

bool
calcSimpleNavigation( const OgMap & ogMap, FloatMap & navMap, Cell2D & startCell, const double traversabilityThreshhold, const double robotDiameterMetres )
{

    if ( ogMap.numCellsX()==0 || ogMap.numCellsY()==0 ) {
        cout << "ERROR(pathplanutils.cpp): calcSimpleNavigation: OgMap is empty along one dimension at least. Returning..." << endl;
        return false;
    }

    // initialize navMap
    navMap.resize( ogMap.numCellsX(), ogMap.numCellsY() );
    navMap.fill( NAN );

    //M represents a large potential which must be at least the size of n-neighbours of the goal cell where n is the max number of goal cell neighbours in the world. We set it to twice the worldsize.
    const double M = 2.0 * ogMap.numCellsX() * ogMap.numCellsY();
    
    int counter = 0;
    // for every q in GCfree do U(q)=M (large number)
    for ( unsigned int i=0; i<navMap.sizeX(); i++ )
    {
        for ( unsigned int j=0; j<navMap.sizeY(); j++ )
        {
            if ( isTraversable( ogMap, i,j, traversabilityThreshhold ) ) {
                navMap.setElement(i,j, M );
                counter ++;
            }
        }
    }


    // current and next list of cells
    Cell2DVector Lcurr,Lnext;

    // current and next cells
    Cell2D q, q1;

    
    // grow obstacles to robot diameter (extends NaN of utility grid)
    orcamisc::CpuStopwatch watch;
    watch.start();
    growObstaclesNavMap( ogMap, navMap, traversabilityThreshhold, robotDiameterMetres );
    watch.stop();
    cout << "growObstaclesNavMap took " << watch.elapsedSeconds() << " s" << endl << endl;

    //check current cell is sane (and shift it slightly if it isn't)
    if( !validateCell( navMap, startCell, robotDiameterInCells(ogMap, robotDiameterMetres) ) ) {
        cout << "ERROR(pathplanutils.cpp): calcSimpleNavigation: startCell not valid. Returning..." << endl;
        return false;
    }

    // set utility of the start point to zero
    // U(q_origin)=0, initialize current list, insert q_origin into Lo
    navMap.setElement(startCell, 0.0);
    Lcurr.push_back( startCell );

    // flags used to determine if diagonal cell is reachable
    bool prev;
    bool first;

    // for i=0,1,..., until Li (Lnext) is empty
    while ( !Lcurr.empty() )
    {
        // for every q in Li starting at back
        while ( !Lcurr.empty() )
        {
            q = Lcurr.back();
            Lcurr.pop_back();

            // reset falgs
            prev=false;
            first=false;

            // for every 1-neighbor q' of q in GC_free
            for ( int k=0; k<4; k++ )
            {
                q1 = adjacentCell(q,k);
                if( !containsNan(navMap,q1) ) // i.e. is traversable and not too close to obstacle
                {
                    if( navMap.element( q1 ) > navMap.element( q ) + AC ) //+ OCCUP_PENALTY*mq2
                    {
                        navMap.setElement( q1, navMap.element( q ) + AC ); //+ OCCUP_PENALTY*mq2
                        Lnext.push_back( q1 );
                    }
                    // check diagonal
                    q1 = diagonalCell(q,k);
                    if( prev && !containsNan(navMap,q1) )  // other cells are traversable, diagonal is reachable
                    {
                        if( navMap.element( q1 ) > navMap.element( q ) + DC )  //+ OCCUP_PENALTY*mq2
                        {
                            navMap.setElement( q1, navMap.element( q ) + DC ); //+ OCCUP_PENALTY*mq2
                            if( !isIncluded(Lnext,q1) )
                            {
                                Lnext.push_back( q1 );
                            }
                        }
                    }
                    // separate test for diagonal when k=0
                    if( k==3 )
                    {
                        q1 = diagonalCell(q,0);
                        if( first && !containsNan(navMap,q1) )  // other cells are traversable, diagonal is reachable
                        {
                            if( navMap.element( q1 ) > navMap.element( q ) + DC )  //+ OCCUP_PENALTY*mq2
                            {
                                //update cell
                                navMap.setElement( q1, navMap.element( q ) + DC ); //+ OCCUP_PENALTY*mq2
                                if( !isIncluded(Lnext,q1) )
                                {
                                    Lnext.push_back( q1 );
                                }
                            }
                        }
                    }
                    prev=true;
                    if( k==0 ) first=true;
                }
                else //not traversable
                {
                    prev=false;
                    // don't need: if( k==0 ) first=false; since flag will stay false
                }
            }
        }
        Lcurr.swap(Lnext);
    }
    return true;
}

void
growObstaclesNavMap( const OgMap & ogMap, FloatMap & navMap, const double traversabilityThreshhold, const double robotDiameterMetres )
{
    Cell2DVector Lcurr,Lnext,Lold;
    Cell2D q, q1;

    // efficient for large radii
    // find all the edges of the known obstacles
    for ( int i=0; i<ogMap.numCellsX(); i++ )
    {
        for ( int j=0; j<ogMap.numCellsY(); j++ )
        {
            if (  !isTraversable( ogMap, i,j, traversabilityThreshhold )  &&
                   isTraversableNeighbors( ogMap, i,j, traversabilityThreshhold ) )
            {
                Lcurr.push_back( Cell2D( i,j ) );
            }
        }
    }

    int robotDiameterCells = robotDiameterInCells( ogMap, robotDiameterMetres );

    // extend them by robotDiameterCells cells
    for( int i=0; i<robotDiameterCells; i++ )
    {
        //cout << "Lcurr size: " << Lcurr.size()<<endl;
        while ( !Lcurr.empty() )
        {
            q = Lcurr.back();
            //cout << "Q: " << q << endl;
            Lcurr.pop_back();
            for ( int k=0; k<4; k++ )
            {
                q1 = adjacentCell(q, k);
                if ( !containsNan( navMap, q1 ) ) //selects traversable cell around q1
                {
                    navMap.setElement( q1, NAN );
                    Lnext.push_back( q1 );
                }
            }
        }
        Lcurr.swap(Lnext);
    }

}

void growObstaclesOgMap( OgMap        &ogMap,
                         const double  traversabilityThreshhold,
                         const int     robotDiameterCells )
{

    Cell2DVector Lcurr;
    Cell2D q, q1;

    // efficient for large radii
    // find all the edges of the known obstacles
    for ( int i=0; i<ogMap.numCellsX(); i++ )
    {
        for ( int j=0; j<ogMap.numCellsY(); j++ )
        {
            if (  !isTraversable( ogMap, i,j, traversabilityThreshhold )  &&
                   isTraversableNeighbors( ogMap, i,j, traversabilityThreshhold ) )
            {
                assert( ogMap.cellWithinMap( i,j ) );
                Lcurr.push_back( Cell2D( i,j ) );
            }
        }
    }

    // extend them by robotDiameterCells cells
    for( int i=0; i<robotDiameterCells; i++ )
    {
        Cell2DVector Lnext;
        //cout << "Lcurr size: " << Lcurr.size()<<endl;
        while ( !Lcurr.empty() )
        {
            q = Lcurr.back();
            //cout << "Q: " << q << endl;
            Lcurr.pop_back();
            for ( int k=0; k<4; k++ )
            {
                q1 = adjacentCell(q, k);

                // checking the boundaries
                if ( ogMap.cellWithinMap( q1.x(), q1.y() ) )
                {
                    const int thresh = (int)floor( (double)CELL_OCCUPIED*traversabilityThreshhold );
                    if ( ogMap.gridCell( q1.x(), q1.y() ) < thresh )
                    {
                        ogMap.gridCell( q1.x(), q1.y() ) = CELL_OCCUPIED;
                        Lnext.push_back( q1 );
                    }
                }
            }
        }
        Lcurr = Lnext;
    }
}




bool calcSkeletonNavigation( const orcaogmap::OgMap ogMap,
                             FloatMap navMap,
                             Cell2D startCell,
                             const double traversabilityThreshhold,
                             const double robotDiameterMetres )
{
    Cell2DVector skel;
    FloatMap distGrid;
    
    // 1. Compute skeleton for this ogmap
    if ( !computeSkeleton( ogMap,
                           navMap,
                           skel,
                           distGrid,
                           traversabilityThreshhold,
                           robotDiameterMetres ) ) return false;

    // 2. Connect start cell to skeleton 
    if( !connectCell2Skeleton( skel, startCell, navMap, distGrid, robotDiameterInCells(ogMap, robotDiameterMetres) ) ) return false;

    // 3. Compute potential function U along the skeleton
    if( !computePotentialSkeleton( ogMap, navMap, skel, startCell ) ) return false;

    // 4. Compute potential function U in the free space
    computePotentialFreeSpace( ogMap, navMap, skel, traversabilityThreshhold );

    return true;
}


void makeOgMapBoundary( orcaogmap::OgMap ogMap )
{
    int sizeX = ogMap.numCellsX();
    int sizeY = ogMap.numCellsY();
    int x,y;
    
    y=0;
    for (x=0; x<sizeX; x++ )
    {
        ogMap.gridCell( x,y ) = CELL_OCCUPIED;
    }
    
    y=sizeY-1;
    for (x=0; x<sizeX; x++ )
    {
        ogMap.gridCell( x,y ) = CELL_OCCUPIED;
    }
    
    x=0;
    for (y=0; y<sizeY; y++ )
    {
        ogMap.gridCell( x,y ) = CELL_OCCUPIED;
    }
    
    x=sizeX-1;
    for (y=0; y<sizeY; y++ )
    {
        ogMap.gridCell( x,y ) = CELL_OCCUPIED;
    }
}


bool computeSkeleton( const orcaogmap::OgMap &ogMap,
                      FloatMap               &navMap,
                      Cell2DVector           &skel,
                      FloatMap               &distGrid,
                      double                  traversabilityThreshhold,
                      double                  robotDiameterMetres )
{    
    assert( traversabilityThreshhold >= 0.0 && traversabilityThreshhold <= 1.0 );
    int robotDiameterCells = robotDiameterInCells( ogMap, robotDiameterMetres );

    int sx = ogMap.numCellsX();
    int sy = ogMap.numCellsY(); 

    double M = 2.0 * sx * sy;

    // initalise the distance grid
    distGrid.resize( sx, sy );
    distGrid.fill( 0 );

    // initialise the navMap
    navMap.resize( sx, sy );
    navMap.fill( NAN );
    
    // take robot size into account
    const int ALPHA = 2 * MAX(robotDiameterCells/2,2);

    Cell2DVector Lcurr, Lnext;
    Cell2D q, q1, c, c1;
    
    FloatMap zeroMap;
    zeroMap.resize( sx, sy );    
    // AlexB: added this just to be sure uninitialised elements aren't touched...
    zeroMap.fill( -1 );

    for ( int i=0; i<sx; i++ )
    {
        for ( int j=0; j<sy; j++ )
        {
            // if cell is in free space and not on the border of the map
            if ( isTraversable( ogMap, i, j, traversabilityThreshhold ) && !isBorderCell(ogMap,i,j) )
            {
                // for every cell q in GCfree do U(q)=M (large number)
                distGrid.setElement( i,j, M );
                
                // set the value grid to M - represents traversable and untouched
                navMap.setElement( i,j, M );
            }
            // cell is occupied
            else 
            {
                // if the cell is occupied and has at least one non-occupied neighbour
                // (ie it lies on a border)
                if ( isTraversableNeighbors( ogMap, i, j, traversabilityThreshhold ) )
                {
                    zeroMap.setElement( i,j, sub2ind( i,j, sx, sy ) );
                    Lcurr.push_back( Cell2D( i,j ) );
                } 
            } 
        }
    }

    // AlexB: I think:
    //        At this point:
    //        - LCurr    : the list of all cells within an obstacle but bordering GCfree
    //        - distGrid : every cell in GCfree = M, every other cell = 0
    //        - navMap   : every cell in GCfree = M, every other cell = nan
    //        - zeroMap  : every border cell (ie elements of LCurr): a unique ID
    //                     every other  cell: undefined
    //
    //        This next section:
    //        - sets distGrid to the 4-distance to the nearest obstacle
    //        - sets zeroMap: each cell will contain the ID of the border point that originated 
    //                        the wave which first reached that cell.
    //        - uses but doesn't modify navMap
    

    // FIND SKELETON
    // for i=0,1,..., until Li is empty
    while ( !Lcurr.empty() )
    {
        // for every q in Li
        for ( Cell2DVector::iterator it=Lcurr.begin(); it!=Lcurr.end(); it++ )
        {
            q = *it;

            // for every 1st neighbor q' of q in GC_free
            for ( int k=0; k<4; k++ )
            {
                q1 = adjacentCell(q, k);

                // if q1 is traversable ...
                if ( !containsNan( navMap, q1 ) )
                {
                    if ( distGrid.element( q1 )==M ) // i.e. traversable and untouched
                    {
                        distGrid.setElement( q1, distGrid.element( q ) + AC );
                        zeroMap.setElement( q1, zeroMap.element( q ) );
                        Lnext.push_back( q1 );
                    }
                    else
                    { 
                        // two waves meet
                        // Set c and c1 to the cells of the wave-originating border points.
                        c  = ind2sub( (int)zeroMap.element( q  ), sx, sy );
                        c1 = ind2sub( (int)zeroMap.element( q1 ), sx, sy );
                        // if distance between the two originating points is greater than ALPHA
                        if ( euclideanDistance( c, c1 ) > ALPHA )
                        {
                            // AlexB: How does this check work?
                            if ( distGrid.element( q ) <= distGrid.element( q1 ) 
                                 && !isIncluded( skel, q ) && !isIncluded( skel, q1 ) )
                            {
                                skel.push_back( q1 );
                            }
                        }
                    }
                }
            }
        }
        Lcurr.swap( Lnext );
        Lnext.clear();
    }

    if ( skel.size()==0 ) return false;

    return true;
}

bool
findClosestPointOnSkeleton( const Cell2DVector   &skel,
                            const FloatMap       &distGrid,
                            const Cell2D         &fromCell,
                            Cell2D               &closestCell )
{
    Cell2D currentCell = fromCell;
    Cell2DVector visitedCells;

    while ( !isIncluded( skel, currentCell ) )
    {
        visitedCells.push_back( currentCell );

        // select a neighbor having the largest value d1 i.e. move away from obstacles
        currentCell = max8Cells( distGrid, currentCell );

        if( isIncluded( visitedCells, currentCell ) )
        {
            // Can't reach skeleton
            cout<<"TRACE(pathplanutils.cpp): Couldn't reach skeleton from " << fromCell << endl;
            return false;
        }
    }
    closestCell = currentCell;
    return true;
}


bool
connectCell2Skeleton( Cell2DVector   &skel,
                      Cell2D         &cell,
                      const FloatMap &navMap,
                      const FloatMap &distGrid,
                      int             robotDiameterCells )
{
    // G - a list of configuration connection goal to S
    Cell2DVector G;

    // check current cell is sane (and shift it slightly if it isn't)
    if( !validateCell( navMap, cell, robotDiameterCells ) ) 
    {
        cout << "ERROR(pathplanutils.cpp, connectCell2Skeleton): cell is invalid" <<endl;
        return false;
    }
    
    // q <- q_goal
    Cell2D q = cell;
    
    // insert q_goal in G
    G.push_back( cell );
    
    // while q is not in S
    unsigned int count = 0;
    while ( !isIncluded( skel, q ) )
    {
        // select a neighbor q' having the largest value d1 i.e. move away from obstacles
        q = max8Cells( distGrid, q );

        if( !isIncluded( G, q ) )
        {
            G.push_back( q );

            //cout<<"("<<q.x()<<","<<q.y()<<")"<<endl;
            if ( ++count>navMap.sizeX() )
            {
                std::cout<<"ERROR(pathplanutils.cpp, connectCell2Skeleton): Can't reach skeleton from cell ("<<cell.x()<<","<<cell.y()<<") "<<"skeleton size:"<<skel.size()<<std::endl;
                return false;
            }
        }
    }
    
    // combine SIGMA and S ( drop the last element which is in both vectors )
    skel.insert( skel.end(), G.begin(), G.end() - 1 );
    
    return true;
}

bool
computePotentialSkeleton( const OgMap & ogMap, FloatMap & navMap, const Cell2DVector & skel, const Cell2D & startCell )
{


    // large number
    const double M = 2.0 * ogMap.numCellsX() * ogMap.numCellsY();

    // startCell must be on the skeleton
    if( !isIncluded( skel, startCell ) ) 
    {
        cout << "ERROR(pathplanutils.cpp, skeleton2SkeletonUtil): Start cell does not lie on the skeleton" << endl;
        return false;   
    }

    // U(q_goal) = 0
    navMap.setElement( startCell, 0.0 );

    // INSERT(startcell, Q)
    Cell2DList Q;
    Q.push_back( startCell );

    Cell2D q, q1;
    double qval, q1val;

    cout << "Size of the skel: " << skel.size() << endl;

    // while !EMPTY(Q)
    while ( !Q.empty() )
    {
        //cout << "Q size: " << Q.size() << endl;

        // q<-FIRST(Q) [assign Q(0) to q and remove it]
        q = Q.front(); //always pull off lowest utility valued cell i.e. cell that is closest to start
        Q.pop_front();
        
        qval = navMap.element(q);

        // for every m-neighbors q' of q in S (this is m-dim space, so 2nd neighbors)
        for ( unsigned int k=0; k<8; k++ )
        {
            q1 = surroundCell(q,k);
            
            // if cell is not part of the skeleton go back to beginning of for loop
            if ( !isIncluded( skel, q1 ) ) continue; 

            // if the potential equals to the large number we need to update it
            if ( navMap.element( q1 ) > M-1.0 ) // not sure if we can safely say ==M because of precision?
            {
                if ( k<4 ) {
                    q1val = qval + AC;  // adjacent cells (0-3)
                } else {
                    q1val = qval + DC;  // diagonal cells (4-7)
                }
                navMap.setElement( q1, q1val );
                
                bool hasInserted = false;
               
                // reverse the list, so we can traverse it with a normal iterator (and use insert)
                Q.reverse();
                for ( Cell2DList::iterator it=Q.begin(); it!=Q.end(); it++ )
                {
                    if ( q1val <= navMap.element( *it ) ) 
                    {
                        Q.insert( it, q1 ); //insert it just in front of *it
                        hasInserted = true;
                        break;
                    }
                }
                // we need this trick: if q1val wasn't smaller than any cell's potential in the list, it didn't insert it at all
                if (!hasInserted)
                {
                    Q.push_back( q1 );
                }
            }
            
        }//for
    }//while

    return true;
}

void computePotentialFreeSpace( const OgMap & ogMap, FloatMap & navMap, const Cell2DVector & skel, const double traversabilityThreshhold )
{
    double M = 2.0 * ogMap.numCellsX() * ogMap.numCellsY();

    Cell2DVector Lcurr, Lnext;
    Cell2D q, q1;

    // initialize Lo
    Lcurr = skel;

    // for i=0,1,..., until Li is empty
    while ( !Lcurr.empty() )
    {
        // for every q in Li
        for (  Cell2DVector::iterator it=Lcurr.begin(); it!=Lcurr.end(); it++  )
        {
            q = *it;

            for ( int k=0; k<4; k++ )
            {
                q1 = adjacentCell(q,k);
                if ( isTraversable( ogMap, q1, traversabilityThreshhold ) )
                {
                    if ( navMap.element( q1 ) == M  )
                    {
                        navMap.setElement( q1, navMap.element( q ) +AC  );
                        Lnext.push_back( q1 );
                    }
                }
            }
        }
        Lcurr.swap(Lnext);
        Lnext.clear();
    }
}

Result calcPath( const OgMap    &ogMap,
                 const FloatMap &navMap,
                 Cell2D         &goalCell,
                 Cell2DVector   &path,
                 double          robotDiameterMetres )
{
    
    // Make sure that navigation map is not empty and values are not all nans
    assert( navMap.sizeX()!=0 && navMap.sizeY()!=0 && !areAllNans(navMap) );
    
    int robotDiameterCells = robotDiameterInCells( ogMap, robotDiameterMetres );

    //check if target position is sane
    if( !validateCell( navMap, goalCell, robotDiameterCells ) )
    {
        cout<<"ERROR(pathplanutils.cpp, calcPath): Goal cell is not valid"<<endl;
        return PathDestinationNotValid;
    }

    cout << "INFO(pathplanutils.cpp, calcPath): Goal cell is validated" << endl;

    // start at target and work backwards
    Cell2D cnext = goalCell;
    Cell2D cprev;
    int count = 0;
    path.clear();
    path.push_back( cnext );

    // sanity check for path length
    //TODO: Magic number??
    const int MAX_PATH_LENGTH = 5*navMap.sizeX();

    // termination conditions: zero distance or max path length
    while ( navMap.element( cnext ) > 0.0 )
    {
        cprev = cnext;
        cnext = min8Cells( navMap, cnext );
        
//         //Debugging only
//         float worldX, worldY;
//         ogMap.getWorldCoords(cnext.x(),cnext.y(),worldX,worldY);
//         cout << cnext << ": (" << worldX << ", " << worldY <<"):" << navMap.element( cnext ) << endl;
        
        if ( cnext == cprev )
        {
            cout<<"ERROR(pathplanutils.cpp): Cannot calculate path, stuck in local minimum (error in previously calculated navigation function)"<<endl;
            return PathDestinationUnreachable;
        }
        if( count++ > MAX_PATH_LENGTH )
        {
            cout<<"ERROR(pathplanutils.cpp): Path to long"<<endl;
            return PathDestinationUnreachable;
        }
        path.push_back( cnext );
    }

    //path should start from starting point i.e. element( cnext ) = 0.0
    reverse(path.begin(),path.end());
    return PathOk;

}


void optimizePath( const OgMap        &ogMap,
                   double              traversabilityThreshhold,
                   const Cell2DVector &origPath,
                   Cell2DVector       &optimisedPath )
{
    OgLosTracer rayTracer( ogMap, (unsigned char)(traversabilityThreshhold*orcaogmap::CELL_OCCUPIED) );

#ifndef NDEBUG
    // Check that the original path is OK.
    if ( origPath.size() >= 2 )
    {
        for ( uint i=1; i < origPath.size(); i++ )
        {
            Cell2D wp1 = origPath[i-1];
            Cell2D wp2 = origPath[i];
            if ( !rayTracer.isClearCells( wp1.x(), wp1.y(), wp2.x(), wp2.y() ) )
            {
                cout<<"TRACE(pathplanutils.cpp): OptimisePath: original path was messed up:" << endl;
                cout<<"TRACE(pathplanutils.cpp): Couldn't get from wp "<<i-1<<" to "<<i<< endl;
                cout<<"TRACE(pathplanutils.cpp):   " << wp1 << " --> " << wp2 << endl;
                cout<<"TRACE(pathplanutils.cpp): entire origPath: " << endl;
                for ( uint j=0; j < origPath.size(); j++ )
                {
                    cout << "  " << j << ": " << origPath[j] << endl;
                }
                assert( false && "optimizePath received dodgy input path!" );
            }
        }
    }
#endif


    assert( origPath.size() > 0 );

    optimisedPath.push_back( origPath[0] );
    if ( origPath.size() >= 2 )
    {

        int priorI  = 0;
        int middleI = 1;

        //
        // In this while loop: middle and post are always contiguous wps from origPath.
        //
        while ( (middleI+1) < (int)(origPath.size()) )
        {
            const Cell2D &prior  = origPath[priorI];
            const Cell2D &middle = origPath[middleI];
            const Cell2D &post   = origPath[middleI+1];

#ifndef NDEBUG
            if ( !rayTracer.isClearCells( prior.x(), prior.y(), middle.x(), middle.y() ) )
            {
                cout << "ERROR(pathplanutils.cpp): Couldn't trace from prior to middle." << endl;
                cout<<"TRACE(pathplanutils.cpp): prior:  " << prior << endl;
                cout<<"TRACE(pathplanutils.cpp): middle: " << middle << endl;
                cout<<"TRACE(pathplanutils.cpp): post:   " << post << endl;
                cout<<"TRACE(pathplanutils.cpp): origPath: " << endl;
                for ( uint j=0; j < origPath.size(); j++ )
                    cout << "  " << j << ": " << origPath[j] << endl;
                assert( false );
            }
#endif

            if ( !rayTracer.isClearCells( prior.x(), prior.y(), post.x(), post.y() ) )
            {
                // There's a direct line from prior to middle, but _not_ from prior to post.
                // Therefore the middle-man is necessary.
                optimisedPath.push_back( middle );
                priorI  = middleI;
            }
            else
            {
                // skip the middle-man
            }
            middleI++;
        }
    }

    // Add the last cell
    optimisedPath.push_back( origPath.back() );

#ifndef NDEBUG
    // Just double-check that the path is OK.
    if ( optimisedPath.size() < 2 ) return;
    for ( uint i=1; i < optimisedPath.size(); i++ )
    {
        Cell2D wp1 = optimisedPath[i-1];
        Cell2D wp2 = optimisedPath[i];
        if ( !rayTracer.isClearCells( wp1.x(), wp1.y(), wp2.x(), wp2.y() ) )
        {
            cout<<"TRACE(pathplanutils.cpp): OptimisePath messed up:" << endl;
            cout<<"TRACE(pathplanutils.cpp): Couldn't get from wp "<<i-1<<" to "<<i<< endl;
            cout<<"TRACE(pathplanutils.cpp):   " << wp1 << " --> " << wp2 << endl;
            cout<<"TRACE(pathplanutils.cpp): entire optimised path: " << endl;
            for ( uint j=0; j < optimisedPath.size(); j++ )
            {
                cout << "  " << j << ": " << optimisedPath[j] << endl;
            }
            cout<<"TRACE(pathplanutils.cpp): origPath: " << endl;
            for ( uint j=0; j < origPath.size(); j++ )
            {
                cout << "  " << j << ": " << origPath[j] << endl;
            }
            assert( false && "optimizePath messed up!" );
        }
    }
#endif
}

}

