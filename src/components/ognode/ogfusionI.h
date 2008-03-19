/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef OGFUSION_I_H
#define OGFUSION_I_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/ogfusion.h>

// for context()
#include <orcaice/orcaice.h>

// utilities
#include <gbxsickacfr/gbxiceutilacfr/buffer.h>

namespace ognode
{

class OgFusionI : public orca::OgFusion
{
public:
    OgFusionI( orca::OgFusionConfig &config, 
               gbxsickacfr::gbxiceutilacfr::Buffer<orca::OgFusionData> &OgFusionDataBuffer);

    virtual void setData(const orca::OgFusionData&, const Ice::Current&);
    virtual orca::OgFusionConfig getConfig(const ::Ice::Current &) const;

private:

    //config
    orca::OgFusionConfig config_;

    //buffer
    gbxsickacfr::gbxiceutilacfr::Buffer<orca::OgFusionData> &ogFusionDataBuffer_;

};

} // namespace

#endif
