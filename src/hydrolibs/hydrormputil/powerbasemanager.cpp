#include "powerbasemanager.h"
#include <iostream>

using namespace std;

namespace hydrormputil {

PowerbaseManager::PowerbaseManager( const std::string                         &name,
                                    int                                        powerbaseID,
                                    AggregatorCallback                        &aggregatorCallback,
                                    std::auto_ptr<hydrointerfaces::SegwayRmp> &hydroDriver,
                                    const hydroutil::Context                  &context )
    : hydroDriver_(hydroDriver),
      name_(name),
      powerbaseID_(powerbaseID),
      aggregatorCallback_(aggregatorCallback),
      stallType_(hydrormputil::NoStall),
      context_(context)
{
}

PowerbaseManager::~PowerbaseManager()
{
    gbxiceutilacfr::stopAndJoin( segwayRmpDriverThreadPtr_ );
}

void
PowerbaseManager::init( const hydrormputil::DriverThread::Config &cfg )
{
    //
    // Instantiate the segwayRmpDriverThread
    //
    std::string driverThreadName = name_+"-driverThread";
    segwayRmpDriverThread_ = new hydrormputil::DriverThread( cfg,
                                                            *hydroDriver_,
                                                            context_.tracer(),
                                                            context_.status(),
                                                            *this,
                                                            driverThreadName );
    segwayRmpDriverThreadPtr_ = segwayRmpDriverThread_;
}

void
PowerbaseManager::startThread()
{
    segwayRmpDriverThread_->start();
}

void
PowerbaseManager::setDesiredSpeed( const hydrointerfaces::SegwayRmp::Command &cmd )
{
    segwayRmpDriverThread_->setDesiredSpeed(cmd);
}

}

