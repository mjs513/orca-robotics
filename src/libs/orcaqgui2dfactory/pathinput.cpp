/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <orcaqgui2d/paintutils.h>

#include <orcaqgui2dfactory/pathinput.h>
#include <orcaqgui2dfactory/waypointdialog.h>
#include <orcaobj/orcaobj.h>
#include <iostream>
#include <cmath>
#include <assert.h>
#include <orcaice/orcaice.h>
#include <orcaqgui/ihumanmanager.h>

#include <QPainter>
#include <QString>
#include <QMouseEvent>
#include <QDialog>
#include <QFile>
#include <QTextStream>

using namespace std;

namespace orcaqgui {
    
const int NUM_COLUMNS = 8;

float straightLineDist( QPointF line )
{
    return sqrt(line.x()*line.x() + line.y()*line.y());
}
    
WpTable::WpTable(PathInput *parent,
                 QPolygonF *waypoints, 
                 QVector<float> *times, 
                 QVector<float> *waitingTimes)
    : parent_(parent),
      waypoints_(waypoints),
      times_(times),
      waitingTimes_(waitingTimes),
      isLocked_(true)
{ 
    setColumnCount(NUM_COLUMNS);
    QObject::connect(this,SIGNAL(cellChanged(int,int)),this,SLOT(updateDataStorage(int,int)));
    QObject::connect(this,SIGNAL(cellClicked(int,int)),parent,SLOT(setWaypointFocus(int,int)));
}


void WpTable::refreshTable()
{
    isLocked_ = true;
    
    int size = waypoints_->size();
    clear();
    setRowCount(size);
    
    QString str;
    QTableWidgetItem *item;
    
    computeVelocities();
    
    for (int row=0; row<size; row++)
    {
        str.setNum(waypoints_->at(row).x(),'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, 0, item);
        
        str.setNum(waypoints_->at(row).y(),'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, 1, item);
        
        str.setNum(times_->at(row),'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, 2, item);
        
        str.setNum(waitingTimes_->at(row),'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, 3, item);
        
        str.setNum(velocities_[row],'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, 4, item);
    }
    
    isLocked_ = false;
}

void WpTable::computeVelocities()
{
    velocities_.resize(1);
    velocities_[0] = 0.0;
    for (int i=1; i<waypoints_->size(); i++)
    {
        float deltaS = straightLineDist( waypoints_->at(i) - waypoints_->at(i-1));
        float deltaT = times_->at(i) - (times_->at(i-1) + waitingTimes_->at(i-1));
        velocities_.push_back(deltaS/deltaT);
    }
}

void WpTable::updateDataStorage(int row, int column)
{
    if (isLocked_) return;
    
    QTableWidgetItem *item = this->item(row,column);
    
    switch (column)
    {
        case 0: 
        {       
            QPointF *data = waypoints_->data();
            data[row].setX( item->text().toDouble() );
            break;
        }
        case 1: 
        {       
            QPointF *data = waypoints_->data();
            data[row].setY( item->text().toDouble() );
            break;
        }
        case 2: 
        {       
            float *data = times_->data();
            data[row] = item->text().toDouble();
            refreshTable();
            break;
        }
        case 3: 
        {       
            float waitingTime =  item->text().toDouble();
            float *data = waitingTimes_->data();
            data[row] = waitingTime;
            // update all subsequent times
            data = times_->data();
            for (int i=row+1; i<rowCount(); i++)
            {
                data[i] = data[i] + waitingTime;    
            }
            refreshTable();
            break;
        }
        case 4: 
        {       
            float velocity =  item->text().toDouble();
            velocities_[row] = velocity;
            // update time to get to here
            float *data = times_->data();
            float deltaS = straightLineDist( waypoints_->at(row) - waypoints_->at(row-1) );
            data[row] = deltaS/velocity + (data[row-1] + waitingTimes_->at(row-1));
            refreshTable();
            break;
        }
        default: 
        {
            cout << "WpTable:: This column is currently not supported!" << endl;
        }
    }
        
    
}
    
PathInput::PathInput( WaypointSettings *wpSettings )
    : wpSettings_(wpSettings),
      waypointInFocus_(-99)
{
    wpTable_ = new WpTable( this,
                            &waypoints_, 
                            &times_, 
                            &waitingTimes_ );
}


PathInput::~PathInput() 
{ 
    delete wpTable_; 
}  


void PathInput::setWaypointFocus(int row, int column)
{
    waypointInFocus_ = row;    
}

void
PathInput::resizeData( int index )
{
    waypoints_.resize( index );
    headings_.resize( index );
    distTolerances_.resize( index );
    headingTolerances_.resize( index );
    maxSpeeds_.resize( index );
    maxTurnrates_.resize( index );
    times_.resize( index );
    waitingTimes_.resize( index );
}


void PathInput::paint( QPainter *p )
{
    QMatrix wm = p->matrix(); 
    wmInv_ = wm.inverted();
    
    if ( waypoints_.isEmpty() ) return;
    
    const float PATH_WIDTH = 0.05;
    
    QColor fillColor;
    QColor drawColor;
    
    if (useTransparency_) {
        fillColor = getTransparentVersion(Qt::green);
    } else {
        fillColor=Qt::green;
    }
    
    for ( int i=0; i<waypoints_.size(); i++)
    {
        p->save();
        p->translate( waypoints_[i].x(), waypoints_[i].y() );    // move to point

        if (i==waypointInFocus_) {
            drawColor = Qt::black;
        } else {
            drawColor = fillColor;
        }
        
        paintWaypoint( p, 
                       fillColor,
                       drawColor, 
                       headings_[i],
                       distTolerances_[i], 
                       headingTolerances_[i] );

        p->restore();
    }
    
    // draw connections between them
    if ( waypoints_.size()>1 )
    {
        p->setPen( QPen( fillColor, PATH_WIDTH ) );
        p->setBrush ( Qt::NoBrush );

        for ( int i=1; i<waypoints_.size(); ++i)
        {
            p->drawLine(waypoints_[i],waypoints_[i-1]);
        }
    }
}

void PathInput::updateWpSettings( WaypointSettings* wpSettings )
{
    wpSettings_ = wpSettings;   
}

void PathInput::processPressEvent( QMouseEvent* e)
{
    mouseDownPnt_ = e->pos();
    mouseMovePnt_ = mouseDownPnt_;  // initialisation of mouseMovePnt_ to same position   
}

void PathInput::processReleaseEvent( QMouseEvent* e)
{
    mouseUpPnt_ = e->pos();
    int button =  e->button();

    QPointF waypoint = wmInv_.map( mouseUpPnt_ );
    //cout << "mouseUpPnt_,waypoint: (" << mouseUpPnt_.x() << "," << mouseUpPnt_.y() <<")  (" << waypoint.x() << ","<< waypoint.y() << ")" << endl;
    
    if (button == Qt::LeftButton) 
    {         
        addWaypoint( waypoint );
    }
    else if (button == Qt::RightButton)
    {      
        removeWaypoint( waypoint );
    }
    else if (button == Qt::MidButton)
    {
        changeWpParameters( waypoint );    
    }
    if (wpTable_->isHidden()) wpTable_->show();
    wpTable_->refreshTable();
}

void PathInput::processDoubleClickEvent( QMouseEvent* e)
{
}

void PathInput::processMoveEvent( QMouseEvent* e)
{
}

void PathInput::addWaypoint (QPointF wp)
{      
    assert(wpSettings_!=NULL);
    
    int numWaypoints = waypoints_.size();
    
    waypoints_.append( wp );
     
    // by default, the heading is aligned with the line between the two last waypoints
    if (numWaypoints==0)
    {   // first waypoint is special
        headings_.append( 0 );
        headingTolerances_.append( 180*16 );
        times_.append( wpSettings_->timePeriod );
    }
    else 
    {
        // times
        times_.append( wpSettings_->timePeriod + times_[numWaypoints-1] + waitingTimes_[numWaypoints-1] );
        // heading    
        QPointF diff = waypoints_[numWaypoints] - waypoints_[numWaypoints - 1];
        int tmpHeading =(int)floor( atan2(diff.y(),diff.x() )/M_PI*180.0 );
        if (tmpHeading < 0) tmpHeading = tmpHeading + 360;
        headings_.append( tmpHeading*16 );
        headingTolerances_.append( wpSettings_->headingTolerance*16 );
        // correct past headings to the direction we're going
        if (numWaypoints>1)
        {
            headings_[numWaypoints-1] = tmpHeading*16;
        }
    }
    
    waitingTimes_.append( 0.0 );
    distTolerances_.append( wpSettings_->distanceTolerance );
    maxSpeeds_.append( wpSettings_->maxApproachSpeed );
    maxTurnrates_.append( wpSettings_->maxApproachTurnrate );
}

void PathInput::removeWaypoint( QPointF p1 )
{
    if ( waypoints_.isEmpty() ) return;   
    const double VICINITY = 1.0; // in meters
    
    // check if click is near the latest waypoint
    int lastIndex = waypoints_.size() - 1;
    QPointF p2;
    p2 = waypoints_[lastIndex];
    double dist = sqrt ( (p1.x()-p2.x()) * (p1.x()-p2.x()) + (p1.y()-p2.y()) * (p1.y()-p2.y()) );
    if (dist<VICINITY)
    {
        // delete the last element
        resizeData( lastIndex );
    }
    
}

void PathInput::changeWpParameters( QPointF p1 )
{

    if ( waypoints_.isEmpty() ) return;    
    const double VICINITY = 1.0; // in meters
    
    // check if click is near a waypoint
    for (int i=0; i<waypoints_.size(); i++)
    {
        QPointF p2;
        p2 = waypoints_[i];
        double dist = sqrt ( (p1.x()-p2.x()) * (p1.x()-p2.x()) + (p1.y()-p2.y()) * (p1.y()-p2.y()) );
        
        if (dist<VICINITY)
        {
            QDialog *myDialog = new QDialog;
            Ui::WaypointDialog ui;
            ui.setupUi(myDialog);
            
            // save waiting time
            float waitingTimeBefore = waitingTimes_[i];
            cout << "Waiting time before: " << waitingTimeBefore << endl;
            
            ui.xSpin->setValue( waypoints_[i].x() );
            ui.ySpin->setValue( waypoints_[i].y() );
            ui.headingSpin->setValue( headings_[i]/16 );
            ui.timeSpin->setValue( times_[i] );
            ui.waitingTimeSpin->setValue( waitingTimes_[i] );
            ui.distanceTolSpin->setValue( distTolerances_[i] );
            ui.headingTolSpin->setValue( headingTolerances_[i]/16 );
            ui.maxSpeedSpin->setValue( maxSpeeds_[i] );
            ui.maxTurnrateSpin->setValue( maxTurnrates_[i] );
            
            int ret = myDialog->exec();
            if (ret==QDialog::Rejected) return;
            
            waypoints_[i].setX( ui.xSpin->value() );
            waypoints_[i].setY( ui.ySpin->value() );
            headings_[i] = ui.headingSpin->value()*16;
            times_[i] = ui.timeSpin->value();
            waitingTimes_[i] = ui.waitingTimeSpin->value();
            distTolerances_[i] = ui.distanceTolSpin->value();
            headingTolerances_[i] = ui.headingTolSpin->value()*16;
            maxSpeeds_[i] = ui.maxSpeedSpin->value();
            maxTurnrates_[i] = ui.maxTurnrateSpin->value();
            
            // if waiting time has changed we need to update all subsequent times
            cout << "Waiting time after: " << waitingTimes_[i] << endl;
            cout << "Difference is: " << (waitingTimes_[i]-waitingTimeBefore) << endl;
            if ( (waitingTimes_[i]-waitingTimeBefore)<1e-5 || (i==waypoints_.size()-1)) return;
            cout << "Didn't return" << endl;
            for (int k=i+1; k<waypoints_.size(); k++)
            {
                times_[k] = times_[k] + waitingTimes_[i];
            }
            
        }
    }

}

void 
PathInput::savePath( const QString &fileName, IHumanManager *humanManager ) const
{
    int size=wpSettings_->numberOfLoops * waypoints_.size();
    
    if (size==0)
    {
        humanManager->showBoxMsg(Warning, "Path has no waypoints. File will be empty!");
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        humanManager->showBoxMsg(Error, "Cannot create file " + fileName );
        return;
    }
    
    const float timeOffset = times_[waypoints_.size()-1];

    QTextStream out(&file);
    for (int k=0; k<wpSettings_->numberOfLoops; k++)
    {
        for (int i=0; i<waypoints_.size(); i++)
        {
            out << waypoints_[i].x() << " " << waypoints_[i].y() << " "
                    << headings_[i] << " "
                    << times_[i]+k*timeOffset << " "
                    << distTolerances_[i] << " "
                    << headingTolerances_[i]<< " "
                    << maxSpeeds_[i]<< " "
                    << maxTurnrates_[i]<< "\n";
        }
    }
    
    file.close();
    humanManager->showStatusMsg(Information, "Path successfully saved to " + fileName );
}

PathFollowerInput::PathFollowerInput( WaypointSettings *wpSettings )
    : PathInput(wpSettings)
{
}

orca::PathFollower2dData
PathFollowerInput::getPath() const
{
    int size = wpSettings_->numberOfLoops * waypoints_.size();
//     cout << "DEBUG(pathinput.cpp): size is " << size << endl;
    
    orca::PathFollower2dData pathData;
    pathData.path.resize( size );
    int counter = -1;
    
    const float timeOffset = times_[waypoints_.size()-1];
    
    for (int k=0; k<wpSettings_->numberOfLoops; k++)
    {
        for (int i=0; i<waypoints_.size(); i++)
        {
            counter++;
            
            float heading = headings_[i]/16;
            if (heading>180.0) {
                heading = heading - 360.0;
            }
            float headingTolerance = headingTolerances_[i]/16;
            if (headingTolerance>180.0) {
                headingTolerance = headingTolerance - 360.0;
            }
    
            pathData.path[counter].target.p.x = waypoints_[i].x();
            pathData.path[counter].target.p.y = waypoints_[i].y();
            pathData.path[counter].target.o = heading/180.0 * M_PI;
            pathData.path[counter].distanceTolerance = distTolerances_[i];
            pathData.path[counter].headingTolerance = (float)headingTolerance/180.0*M_PI;      
            pathData.path[counter].timeTarget = orcaice::toOrcaTime( times_[i] + k*timeOffset );
                
            pathData.path[counter].maxApproachSpeed = maxSpeeds_[i];
            pathData.path[counter].maxApproachTurnrate = (float)maxTurnrates_[i]/180.0*M_PI;
        }
    }

    return pathData;
}

orca::PathPlanner2dTask
PathPlannerInput::getTask() const
{
    int size = waypoints_.size();
        
    orca::PathPlanner2dTask task;
    task.coarsePath.resize( size );
        
    for (int i=0; i<size; i++)
    {
        
        float heading = headings_[i]/16;
        if (heading>180.0) {
            heading = heading - 360.0;
        }
        float headingTolerance = headingTolerances_[i]/16;
        if (headingTolerance>180.0) {
            headingTolerance = headingTolerance - 360.0;
        }
        orca::Waypoint2d wp;
        wp.target.p.x = waypoints_[i].x();
        wp.target.p.y = waypoints_[i].y();
        wp.target.o = heading/180.0 * M_PI;
        wp.distanceTolerance = distTolerances_[i];
        wp.headingTolerance = (float)headingTolerance/180.0*M_PI;      
        wp.timeTarget = orcaice::toOrcaTime( times_[i] );
        wp.maxApproachSpeed = maxSpeeds_[i];
        wp.maxApproachTurnrate = (float)maxTurnrates_[i]/180.0*M_PI;
            
        task.coarsePath[i] = wp;
    }
    return task;
}

void PathFollowerInput::loadDataFromFile( QString* fileName )
{    
    resizeData(0);
    
    QFile file(*fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        waypoints_.append( QPointF(line.section(' ',0,0).toFloat(),line.section(' ',1,1).toFloat()) );
        headings_.append( line.section(' ',2,2).toInt() );
        times_.append( line.section(' ',3,3).toFloat() );
        distTolerances_.append( line.section(' ',4,4).toFloat() );
        headingTolerances_.append( line.section(' ',5,5).toInt() );
        maxSpeeds_.append( line.section(' ',6,6).toFloat() );
        maxTurnrates_.append( line.section(' ',7,7).toInt() );
    }
}


//////////////////////////////////////////////////////////////////////

WaypointSettings
readWaypointSettings( const Ice::PropertiesPtr & props, const std::string & tag )
{
    std::string prefix = tag + ".Config.Waypoints.";
    
    float timePeriod = orcaice::getPropertyAsDoubleWithDefault( props, prefix+"TimePeriod", 5.0 );
    float distanceTolerance = orcaice::getPropertyAsDoubleWithDefault( props, prefix+"DistanceTolerance", 1.0 );
    int headingTolerance = orcaice::getPropertyAsIntWithDefault( props, prefix+"HeadingTolerance", 90 );
    float maxApproachSpeed = orcaice::getPropertyAsDoubleWithDefault( props, prefix+"MaxApproachSpeed", 2e6 );
    int maxApproachTurnrate = orcaice::getPropertyAsIntWithDefault( props, prefix+"MaxApproachTurnRate", 6000000 );
    int numberOfLoops = orcaice::getPropertyAsIntWithDefault( props, prefix+"NumberOfLoops", 1 );
    WaypointSettings wpSettings(timePeriod, distanceTolerance, headingTolerance, maxApproachSpeed, maxApproachTurnrate, numberOfLoops);
    return wpSettings;
}

bool 
readActivateImmediately( const Ice::PropertiesPtr & props, const std::string & tag )
{
    std::string prefix = tag + ".Config.PathFollower2d.";
    return orcaice::getPropertyAsIntWithDefault( props, prefix+"ActivatePathImmediately", 1 );
}


}


