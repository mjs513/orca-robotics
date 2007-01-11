/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>
#include <orcaobj/timeutils.h>
#include <orcaimage/imageutils.h>

#include "camerareplayer.h"

#ifdef OPENCV_FOUND
    #include <opencv/highgui.h>
#endif

using namespace std;
using namespace orcalogfactory;

int CameraReplayer::_counter = 0;

CameraReplayer::CameraReplayer( const std::string      &format,
                                const std::string      &filename,
                                const orcaice::Context &context )
    : orcalog::Replayer("Camera", format, filename, context)
{
#ifndef OPENCV_FOUND
    // check that we support this format
    if ( format_=="ice" )
    {
        // this one is supported
    }
    else if ( format_=="jpeg" )
    {
        context_.tracer()->info( "Images can only be logged in 'jpeg' format if you have OpenCV." );
        context_.tracer()->info( "Please have a look at the documentation for ImageServer component for installing OpenCv." );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: 'jpeg' format not supported because OpenCV is not installed." );
    }
    else
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
#endif


#ifdef OPENCV_FOUND
    // check that we support this format
    if ( format_!="ice" && format_!="jpeg" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
#endif     

    stringstream ss;
    ss << "camera" << _counter;
    interfaceName_ = ss.str();
    _counter++;
}

CameraReplayer::~CameraReplayer()
{
}

void 
CameraReplayer::initInterfaces()
{
//  cout << "INFO(camerareplayer.cpp): createInterface" << endl;
    topic_ = orcaice::connectToTopicWithString( context_, publisher_, interfaceName_ );

    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithString( context_, obj, interfaceName_ );
}

// served out the data to the client (it was stored here by the driver at the last read)
orca::CameraData
CameraReplayer::getData(const Ice::Current& current) const
{
//    std::cout << "Sending data back" << std::endl;

    // we don't need to pop the data here because we don't block on it.
    if ( dataPipe_.isEmpty() )
    {
        throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
    }

    // create a null pointer. data will be cloned into it.
    orca::CameraData data;
    dataPipe_.get( data );

    return data;
}


orca::CameraDescription
CameraReplayer::getDescription(const Ice::Current& current) const
{    
//     std::cout << "Sending config back" << std::endl;

    if ( descrPipe_.isEmpty() )
    {
        throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
    }
    orca::CameraDescription obj;
    descrPipe_.get( obj );
    return obj;
}

void 
CameraReplayer::subscribe(const ::orca::CameraConsumerPrx &subscriber, const ::Ice::Current&)
{
//    cout<<"INFO(camerareplayer.cpp): subscription request"<<endl;
    IceStorm::QoS qos;
    topic_->subscribe( qos, subscriber );
}


void 
CameraReplayer::unsubscribe(const ::orca::CameraConsumerPrx &subscriber, const ::Ice::Current&)
{
//    cout<<"INFO(camerareplayer.cpp): unsubscription request"<<endl;
    topic_->unsubscribe( subscriber );
}


void 
CameraReplayer::loadHeaderIce()
{
    orca::CameraDescription obj;

    orcalog::IceReadHelper helper( context_.communicator(), file_ );
    ice_readCameraDescription( helper.stream_, obj );
    helper.read();

    descrPipe_.push( obj );  
}

void 
CameraReplayer::initDescription()
{
    if (format_=="ice")
    {
        loadHeaderIce();
    }
    else if (format_=="jpeg")
    {
//         readConfigFromFileJose();
    }
    else
    {
        throw orcalog::FormatNotSupportedException( ERROR_INFO, "Unknown format: "+format_ );
    }
}

void 
CameraReplayer::replayData( int index, bool isTest )
{
    checkIndex( index );
    
    if (format_=="ice") {
        loadDataIce( index );
    }
    else if (format_=="jpeg") {
        loadDataJpeg( index );
    }
    else {
        throw orcalog::FormatNotSupportedException( ERROR_INFO, "Unknown format: "+format_ );
    }

    // push to buffer for direct remote access
    dataPipe_.push( data_ );

    if ( !isTest ) 
    {   
        // push to IceStorm
        publisher_ -> setData( data_ );
    }
}

void 
CameraReplayer::loadDataIce( int index )
{
    while (index != (dataCounter_) )
    {
        orcalog::IceReadHelper helper( context_.communicator(), file_ );
        ice_readCameraData( helper.stream_, data_ );
        helper.read();

        dataCounter_++;
    }
}

void 
CameraReplayer::loadDataJpeg( int index )
{
//     for (int i=0; i<index; i++)
    while (index != (dataCounter_) )
    {
        std::vector<Ice::Byte> byteData;
        size_t length;
        file_->read( (char*)&length, sizeof(length) );
        checkFile();

//         cout << "TRACE(camerareplayer.cpp): before resize, length: " << length << endl;
        byteData.resize( length );
        file_->read( (char*)&byteData[0], length );
        checkFile();

//         cout << "TRACE(cameraplayer.cpp): byteData, size: " << byteData.size() << endl;
        Ice::InputStreamPtr iceInputStreamPtr = Ice::createInputStream( context_.communicator(), byteData );
        if ( !byteData.empty() )
        {
            // TODO: Make this so that it an option is read from the logged master file
            // for reading back as an ice binary file or as separate images        
            // ice_readCameraData( iceInputStreamPtr, data_ );
            orca_readCameraData( iceInputStreamPtr, index );
        }
        dataCounter_++;
    }
}

void 
CameraReplayer::orca_readCameraData( Ice::InputStreamPtr iceInputStreamPtr, int index )
{
    // User defined demarshalling as we can't use the ice demarshalling as it expects the actual
    // image to be in the object whereas the image has been saved separately by the logger
    data_.imageWidth = iceInputStreamPtr->readInt();
    data_.imageHeight = iceInputStreamPtr->readInt();
    data_.format = (orca::ImageFormat)iceInputStreamPtr->readInt();
    data_.compression = (orca::ImageCompression)iceInputStreamPtr->readInt();
    double t = iceInputStreamPtr->readDouble();
    data_.timeStamp = orcaice::toOrcaTime( t );
    std::string fname = iceInputStreamPtr->readString();
    
    #ifdef OPENCV_FOUND
        if (index == 1)
        {    
            // only setup the opencv struct on the first loop
            nChannels_ = orcaimage::numChannels( data_.format );
        }
        // load compressed image into opencv struct
        cvImage_ = cvLoadImage( fname.c_str(), -1);
        
        // resize object buffer to fit image
        int imageSize = (int)ceil( nChannels_ * data_.imageHeight * data_.imageWidth );
        data_.image.resize( imageSize );
        
        // load image from opencv struct to orca object
        memcpy( &data_.image[0], cvImage_->imageData, data_.image.size() );
    #endif
}       
