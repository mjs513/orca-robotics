/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ODOMETRY_2D_INTERFACE_ICE
#define ORCA2_ODOMETRY_2D_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_odometry2d Odometry2d
    @brief Odometry of planar mobile robots.
    @{
*/

// define messages first

//! Odometry2d data structure
struct Odometry2dData
{
    //! Time when data was measured.
    Time timeStamp;

    //! Integrated odometry. The coordinate system has its origin at the point
    //! where the component was initialized.
    Frame2d pose;

    //! Instantanious translational and angular velocities in the vehicle CS.
    //! This means that Vx is forward speed and Vy is side speed
    //! (possible only for some platforms).
    Twist2d motion;
};

//! Static description of the mobile platform associated with this odometric device.
struct Odometry2dDescription
{
    //! Pose of the mobile base in the vehicle coordinate system.
    Frame2d offset;

    //! Dimensions of the mobile base.
    Size2d size;
};


//! Data consumer interface.
interface Odometry2dConsumer
{
    //! Transmits the data to the consumer.
    void setData( Odometry2dData obj );
};

/*!
    @brief Access to odometry of 2d mobile robotic bases.
*/
interface Odometry2d
{
    //! Returns the latest data.
    //! May raise DataNotExistException if the requested information is not available.
    //! May raise HardwareFailedException if there is some problem with hardware.
    nonmutating Odometry2dData getData()
            throws DataNotExistException, HardwareFailedException;
    
    //! Returns description         
    nonmutating Odometry2dDescription getDescription();

    /*!
     * Mimics IceStorm's subscribe(). @p subscriber is typically a direct proxy to the consumer object.
     * The implementation may choose to implement the push directly or use IceStorm.
     * This choice is transparent to the subscriber. The case when the @p subscriber is already subscribed
     * is quietly ignored.
     *
     * @see unsubscribe
     */
    void subscribe( Odometry2dConsumer* subscriber )
            throws SubscriptionFailedException;

    /*!
     * Unsubscribe an existing @p subscriber. The case when the @p subscriber is not subscribed
     * is quietly ignored.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( Odometry2dConsumer* subscriber );
};


//!  //@}
}; // module

#endif
