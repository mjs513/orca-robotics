/*
 *  Orca-Components: Components for robotics.
 *  
 *  Copyright (C) 2004
 *  
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef CANIO_USB_FTDI_H
#define CANIO_USB_FTDI_H

#include <queue>

#include "canpacket.h"
#include <ftd2xx.h>

class CanioUsbFtdi //: public DualCANIO
{
public:
    
    CanioUsbFtdi();
    
    ~CanioUsbFtdi();
    
    // we ignore this channel_freq
    int Init();
    int Shutdown();

    // This is the new read function; the original virtual one is not used
    int ReadPackets();

    // Returns 0 if copied a packet, 1 if the buffer was empty
    int GetNextPacket(CanPacket& pkt);
    
    int WritePacket(CanPacket &pkt);
    
    
private:
    
    // handle to the USB device inside our Segway
    FT_HANDLE ftHandle_;
    
    // a buffer to read USB stream into
    std::vector<unsigned char> charBuffer_;
    
    unsigned int residualBytes;
    
    // a buffer to store converted CAN messages
    std::queue<CanPacket> canBuffer_;
    
    int resetDevice();
    
    int readFromUsbToBuffer();
    
    int readFromBufferToQueue( int bytesInBuffer );
    
    int parseUsbToCan( CanPacket *pkt, unsigned char *bytes );
    
    int parseCanToUsb( CanPacket *pkt, unsigned char *bytes );
    
    unsigned char usbMessageChecksum( unsigned char *msg );
    
    int debugMsgCount_;
};

#endif
