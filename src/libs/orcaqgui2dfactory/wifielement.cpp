/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <QVector>
#include <QLabel>
#include <QProgressBar> 
#include <QLCDNumber>
#include <QVBoxLayout>

#include "wifielement.h"

using namespace std;
using namespace orca;
using namespace orcaqgui2d;

WifiWidget::WifiWidget( unsigned int numInterfaces, std::string proxyString )
    : numInterfaces_(numInterfaces)
{
    setupDisplay();
    setWindowTitle( QString(proxyString.c_str()) );
}

void
WifiWidget::refresh( WifiData &data )
{
    for (int i=0; i<interfaceLabels_.size(); i++) 
    {
        interfaceLabels_[i]->setText("<b>" + QString(data.interfaces[i].interfaceName.c_str()) + "</b>");
    }
    
    for (unsigned int i=0; i<numInterfaces_; i++)
    {
        WifiInterface &wifiInt = data.interfaces[i];
        lcdsSignal_[i]->display(wifiInt.signalLevel);
        lcdsNoise_[i]->display(wifiInt.noiseLevel);
        lcdsLink_[i]->display(wifiInt.linkQuality);
        
        if (wifiInt.linkType==LinkQualityTypeDbm) {
            lcdsMaxSignal_[i]->display("DB");
            lcdsMaxNoise_[i]->display("DB");
        } else {
            lcdsMaxSignal_[i]->display(wifiInt.maxSignalLevel);
            lcdsMaxNoise_[i]->display(wifiInt.maxNoiseLevel);
        }
        lcdsMaxLink_[i]->display(wifiInt.maxLinkQuality);
        
//         progressBars_[i]->setMinimum(0);
//         progressBars_[i]->setMaximum(wifiInt.maxLinkQuality);
//         progressBars_[i]->setValue(wifiInt.linkQuality);
    }
    
}

void WifiWidget::setupDisplay()
{
    const int numRowsPerInterface = 4; 
    const int numDigits = 3;
    
    QGridLayout *globalLayout = new QGridLayout(this);
    globalLayout->setColumnStretch(0,0);
    globalLayout->setColumnStretch(1,1);
    globalLayout->setColumnStretch(2,1);
    
    for (unsigned int i=0; i<numInterfaces_; i++)
    {
        QLabel *interfaceLabel = new QLabel;
        globalLayout->addWidget(interfaceLabel,numRowsPerInterface*i,0);
        interfaceLabels_.push_back( interfaceLabel );
        
        QLCDNumber *lcdSignalLevel = new QLCDNumber(numDigits, this);
        lcdSignalLevel->setSegmentStyle(QLCDNumber::Filled);
        lcdsSignal_.push_back(lcdSignalLevel);
        QLCDNumber *lcdMaxSignalLevel = new QLCDNumber(numDigits, this);
        lcdMaxSignalLevel->setSegmentStyle(QLCDNumber::Filled);
        lcdsMaxSignal_.push_back(lcdMaxSignalLevel);
        QLabel *signalLabel = new QLabel("Signal: ");
        globalLayout->addWidget( signalLabel,numRowsPerInterface*i+1,0);
        globalLayout->addWidget( lcdSignalLevel,numRowsPerInterface*i+1,1);
        globalLayout->addWidget( lcdMaxSignalLevel,numRowsPerInterface*i+1,2);
        
        QLCDNumber *lcdNoiseLevel = new QLCDNumber(numDigits, this);
        lcdNoiseLevel->setSegmentStyle(QLCDNumber::Filled);
        lcdsNoise_.push_back(lcdNoiseLevel);
        QLCDNumber *lcdMaxNoiseLevel = new QLCDNumber(numDigits, this);
        lcdMaxNoiseLevel->setSegmentStyle(QLCDNumber::Filled);
        lcdsMaxNoise_.push_back(lcdMaxNoiseLevel);
        QLabel *noiseLabel = new QLabel("Noise: ");
        globalLayout->addWidget( noiseLabel,numRowsPerInterface*i+2,0);
        globalLayout->addWidget( lcdNoiseLevel,numRowsPerInterface*i+2,1);
        globalLayout->addWidget( lcdMaxNoiseLevel,numRowsPerInterface*i+2,2);
        
        QLCDNumber *lcdLinkLevel = new QLCDNumber(numDigits, this);
        lcdLinkLevel->setSegmentStyle(QLCDNumber::Filled);
        lcdsLink_.push_back(lcdLinkLevel);
        QLCDNumber *lcdMaxLinkLevel = new QLCDNumber(numDigits, this);
        lcdMaxLinkLevel->setSegmentStyle(QLCDNumber::Filled);
        lcdsMaxLink_.push_back(lcdMaxLinkLevel);
        QLabel *linkLabel = new QLabel("Link quality: ");
        globalLayout->addWidget( linkLabel,numRowsPerInterface*i+3,0);
        globalLayout->addWidget( lcdLinkLevel,numRowsPerInterface*i+3,1);
        globalLayout->addWidget( lcdMaxLinkLevel,numRowsPerInterface*i+3,2);
        
    
//         QProgressBar *barLinkQuality = new QProgressBar;
//         progressBars_.push_back(barLinkQuality);
//         QLabel *linkLabel = new QLabel("Link quality level: ");
//         globalLayout->addWidget( linkLabel,3*i+2,0);
//         globalLayout->addWidget( barLinkQuality,3*i+2,1);
    }
    setLayout(globalLayout);
}

WifiElement::WifiElement( const orcaice::Context  &context,
                          const std::string       &proxyString,
                          int                      timeoutMs)
    : IceStormElement<WifiPainter,
                      orca::WifiData,
                      orca::WifiPrx,
                      orca::WifiConsumer,
                      orca::WifiConsumerPrx>(context, proxyString, painter_, timeoutMs ),
        wifiWidget_(0),
        proxyString_(proxyString)
{
}

WifiElement::~WifiElement()
{
    if (wifiWidget_!=0) delete wifiWidget_;
}


void
WifiElement::update()
{
    if ( !needToUpdate() ) {
        return;
    }
    
    assert( !listener_.buffer().isEmpty() );

    WifiData data;
    
    // get data from the buffer
    listener_.buffer().getAndPop( data );
    
    //cout << orcaice::toString(data) << endl;
    
    if (wifiWidget_==0) {
        wifiWidget_ = new WifiWidget( data.interfaces.size(), proxyString_ );
    }
    wifiWidget_->refresh( data );
    if (wifiWidget_->isHidden()) {
        wifiWidget_->show();
    }
}




