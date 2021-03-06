/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef IMAGEVIEW_OCV_VIEWER_H
#define IMAGEVIEW_OCV_VIEWER_H

#include <orcaice/context.h>
#include <orca/image.h>

// opencv headers
#include <highgui.h>
#include <cv.h>

#include <string>

namespace imageviewocv {
//! @brief Class for viewing images using OpenCV. This class sets up the OpenCV structures for storing image formats and displaying them.
class Viewer
{
public:
    Viewer( const int width,
            const int height,
            const std::string& format,
            const orcaice::Context& context );
    ~Viewer();
    
    //! function that actually displays the image in a window
    void display( orca::ImageDataPtr& image );

private:
    //!
    //! opencv structures
    //!
    //! image structure that stores the image in BGR8 format
    //! for display
    IplImage* cvDisplayImage_;
    // image structure that stores the original image format
    IplImage* cvSrcImage_;
    
    //! handle to the opencv window
    const char* name_;
    
    //! display the frame rate of the received images
    void displayFrameRate();
    
    //! resize IplImages to the same size as the orca image
    void resize( orca::ImageDataPtr& image );
    
    //! time variables for calculating number of frames per second 
    orca::Time oldFrameTime_;
    orca::Time currentFrameTime_;
    double diff_;
    double fps_;
    //! variable for displaying fps
    CvFont font_;

    //! OpenCV pads byte arrays for machine efficiency (see IplImage::widthstep). 
    //! Orca does not.
    //! If image width is a multiple of 4, then padding does not occur and
    //! the opencv image data is identical to orca image data.
    //! If not, we have to do an inefficient copy line be line from orca data
    //! to opencv data.
    //! This variable flags if opencv has padded the array not.
    bool isPadded_;
        
    //! Number of bytes in one row of the orca image data structure 
    //! i.e. the width in number of bytes 
    int32_t orcaByteWidth_;
    
    orcaice::Context context_;
};

} //namespace

#endif
