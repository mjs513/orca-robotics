/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <gbxutilacfr/tokenise.h>
#include <hydrocolourtext/colourtext.h>
#include <orcaobj/stringutils.h>
#include "display.h"

using namespace std;

namespace systemstatusmon
{      

    // local non-member functions
    namespace 
    {  
    
    std::string toHorizontalLine( int platformCount, int nameWidth, int stateWidth )
    {
        stringstream ss;
        ss << hydroctext::toFixedWidth("-----------------------------------", nameWidth) << " +";
        for ( int i=0; i<platformCount; ++i )
            ss << hydroctext::toFixedWidth("-------------------------------------", stateWidth) << " +";
        return ss.str();
    }
    
    vector<string> extractPlatComp( const string &input )
    {
        vector<string> tokens = gbxutilacfr::tokenise( input.c_str(), "/");
        if (tokens.size()!=2)
        {
            stringstream ss;
            ss << "extractPlatForm: unexpected <platform/component> string: " << input;
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
        return tokens;
    }
    
    string extractPlatform( const string &input )
    {
        vector<string> tokens = extractPlatComp( input );
        return tokens[0];
    }
    
    string extractComponent( const string &input )
    {
        vector<string> tokens = extractPlatComp( input );
        return tokens[1];
    }
    
    unsigned int extractMaxNumComponents( const orca::SystemStatusData &data )
    {
        unsigned int maxNum=0;
        map<string,vector<orca::ComponentStatusData> >::const_iterator it;
        
        for ( it=data.begin(); it!=data.end(); ++it )
        {
            if ( (it->second.size()) > maxNum )
                maxNum = it->second.size();
        }
        
        return maxNum;
    }
    
    hydroctext::Style systemHealthStyle( const orca::ComponentStatusData& compData )
    { 
        if (compData.isDataStale)
            return hydroctext::Style( hydroctext::Reverse, hydroctext::White );
        
        const orca::SubsystemsStatus &subSysSt = compData.data.subsystems;
        map<string,orca::SubsystemStatus>::const_iterator it;
    
        orca::SubsystemHealth worstHealth = orca::SubsystemOk;           
        for (it=subSysSt.begin(); it!=subSysSt.end(); ++it)
        {
            if (it->second.health > worstHealth )
                worstHealth = it->second.health;
        }
        
        switch (worstHealth)
        {
            case orca::SubsystemOk: 
                return hydroctext::Style( hydroctext::Reverse, hydroctext::Green );
            case orca::SubsystemWarning:  
                return hydroctext::Style( hydroctext::Reverse, hydroctext::Yellow );
            case orca::SubsystemFault:  
                return hydroctext::Style( hydroctext::Reverse, hydroctext::Red );
            case orca::SubsystemStalled:  
                return hydroctext::Style( hydroctext::Reverse, hydroctext::Black );
            default:
                assert( false && "unknown health type" );
        }
    }
    
    std::string systemStateIcon( const orca::ComponentStatusData& compData )
    {
        if (compData.isDataStale)
            return hydroctext::emph( " ", hydroctext::Style( hydroctext::Reverse, hydroctext::White ) );
        
        const orca::SubsystemsStatus &subSysSt = compData.data.subsystems;
        map<string,orca::SubsystemStatus>::const_iterator it;
    
        orca::SubsystemState highestState = orca::SubsystemIdle;           
        for (it=subSysSt.begin(); it!=subSysSt.end(); ++it)
        {
            if (it->second.state > highestState )
                highestState = it->second.state;
        }
        
        switch (highestState)
        {
            case orca::SubsystemIdle: 
                return hydroctext::emph( "-", hydroctext::Style( hydroctext::Reverse, hydroctext::Yellow ) );
            case orca::SubsystemInitialising:
                return hydroctext::emph( "^", hydroctext::Style( hydroctext::Reverse, hydroctext::Blue ) );
            case orca::SubsystemWorking:
                return hydroctext::emph( " ", hydroctext::Style( hydroctext::Reverse, hydroctext::Green ) );
            case orca::SubsystemFinalising:
                return hydroctext::emph( "v", hydroctext::Style( hydroctext::Reverse, hydroctext::Blue ) );
            case orca::SubsystemShutdown:
                return hydroctext::emph( "x", hydroctext::Style( hydroctext::Reverse, hydroctext::Red ) );
            default:
                assert( false && "unknown state type" );
        }
    }
    
    std::string toShortString( const orca::ComponentStatusData& compData, int stateWidth )
    {
        string compPlat = orcaobj::toString(compData.data.name);
        
        stringstream ss;
        int stateUsedWidth = 1;
        
        ss << systemStateIcon(compData) 
        << hydroctext::emph(hydroctext::toFixedWidth(extractComponent(compPlat),stateWidth-stateUsedWidth), systemHealthStyle(compData) );
        
        return ss.str();
    }
    
    std::string toBoldString( const string &input )
    {
        return hydroctext::emph(input, hydroctext::Style( hydroctext::Bold ) );
    }
    
    std::string humanErrorMsgString( const orca::SystemStatusData &ssData )
    {
        
        stringstream ss;
        
        map<string,vector<orca::ComponentStatusData> >::const_iterator itCs;
        for ( itCs=ssData.begin(); itCs!=ssData.end(); ++itCs )
        {
            const vector<orca::ComponentStatusData> &components = itCs->second;
            
            // per component
            for (unsigned int i=0; i<components.size(); i++)
            {
                const orca::StatusData &statusData = components[i].data;
                string platComp = orcaobj::toString(statusData.name);
                
                const orca::SubsystemsStatus &subSysSt = statusData.subsystems;
                map<string,orca::SubsystemStatus>::const_iterator itSs;
                
                // per subsystem
                for ( itSs=subSysSt.begin(); itSs!=subSysSt.end(); ++itSs )
                {
                    if (itSs->second.health!=orca::SubsystemOk)
                        ss << toBoldString(platComp + ", " + itSs->first + ": ") << endl
                           << itSs->second.message << endl << endl;
                }
            }
        }
        
        return ss.str();
    }
    
    } // end of namespace
    

ColourTextDisplay::ColourTextDisplay( const orcaice::Context     &context,
                                      gbxiceutilacfr::SafeThread *thread)
    : context_(context)
{
    consumer_ = new orcaifaceimpl::BufferedSystemStatusConsumerImpl( -1, gbxiceutilacfr::BufferTypeCircular, context );
    consumer_->subscribeWithTag( "SystemStatus", thread );
}

void 
ColourTextDisplay::refresh()
{
    orca::SystemStatusData data;
    if ( !consumer_->buffer().isEmpty() )
    {
        consumer_->buffer().getAndPop( data );
        display( data );
    }
    else
    {
        context_.tracer().debug( "ColourTextDisplay:refresh(): systemstatus buffer is empty", 5 );
    }
}

    
void
ColourTextDisplay::display( const orca::SystemStatusData &data )
{   
    map<string,vector<orca::ComponentStatusData> >::const_iterator it;
    
    const string sep = " |";
    const int compNameWidth = 20;
    const int compStateWidth = 22;   

    // start with "clean" colour settings
    cout << hydroctext::reset();  
    
    // platform names
    cout << hydroctext::toFixedWidth( toBoldString("PLATFORMS"), compNameWidth ) << sep;
    for ( it=data.begin(); it!=data.end(); ++it )
    {
        cout << hydroctext::toFixedWidth( "    " + toBoldString(it->first), compStateWidth ) << sep;
    }
    cout << endl;
    
    // horizontal line
    cout << toHorizontalLine(data.size(),compNameWidth,compStateWidth) << endl;
    
    // find maximum number of components per platform -> number of rows
    unsigned int maxComp = extractMaxNumComponents( data );
    
    for (unsigned int compCounter=0; compCounter<maxComp; compCounter++)
    {

        // row label
        if (compCounter==0)
            cout << hydroctext::toFixedWidth( toBoldString("COMPONENTS"), compNameWidth ) << sep;
        else
            cout << hydroctext::toFixedWidth( " ", compNameWidth ) << sep;
        
        // per platform (column)
        for ( it=data.begin(); it!=data.end(); ++it )
        {
            const vector<orca::ComponentStatusData> &components = it->second;
            if ( compCounter >= components.size() )
                continue;
            
            cout << toShortString(components[compCounter],compStateWidth) << sep;
        }
        cout << endl;
    }
    
    // print the human-readable text, TODO: make configurable? or interactive?
    cout << endl;
    cout << humanErrorMsgString( data );
    
    // space between consecutive records
    cout << endl << endl;
}
        
}


        