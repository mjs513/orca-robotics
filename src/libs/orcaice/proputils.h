/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PROPERTY_UTILITIES_H
#define ORCA2_PROPERTY_UTILITIES_H

#include <string>
#include <Ice/Ice.h>
#include <orca/orca.h>
#include <orca/bros1.h>

namespace orcaice
{

/*!
    *  @name Property functions
    */
//@{

/*!
 *  Platform-independent function to find Orca global configuration file.
 *
 *  1. Tries to load the file specified in Orca.GlobalConfig configuration parameter.
 *  2. If it is not defined (or empty), tries to load the file specified by the 
 *     environment variable @c ORCA2_GLOBAL_CONFIG
 *  3. If it is not defined (or empty), tries to load the defaultt file from a 
 *     system-dependent location:
 *          > Linux: $HOME/.orcarc
 *          > Win32: C:\orca.ini
 *  Throws orcaice::Exception if none of the three options work out.
 */
std::string getGlobalConfigFilename( const Ice::StringSeq & args );

/*!
 *  Platform-independent function to find Orca component configuration file.
 *
 *  1. Returns the file name specified in Orca.Config configuration parameter.
 *          $ mycomponent --Orca.Config=local.cfg
 *              returns "local.cfg"
 *  2. Returns the file name specified in Ice.Config configuration parameter.
 *          $ mycomponent --Ice.Config=local.cfg
 *              returns "local.cfg"
 *  3. If Orca.Config is not defined (or empty), and there is only one command line argument
 *     it is assumed to be the path to the config file.
 *          $ mycomponent local.cfg
 *              returns "local.cfg"
 *  4. File ./mycomponent.cfg when the component mycomponent is executed with 
 *     no command line arguments.
 *          $ mycomponent
 *          > mycomponent.exe
 *              return "mycomponent.cfg"
 *  
 *  Returns an empty string if everything else fails.
 */
std::string getApplicationConfigFilename( const Ice::StringSeq & args );

/*!
 *  Platform-independent function to find Orca component configuration file.
 *
 *  1. Returns the file name specified in Orca.Config configuration parameter.
 *          IceBox.Service.MyService=MyService:create --Orca.Config=local.cfg
 *              returns "local.cfg"
 *
 *  2. Returns the file name specified in Ice.Config configuration parameter.
 *          IceBox.Service.MyService=MyService:create --Ice.Config=local.cfg
 *              returns "local.cfg"
 *
 *  Throws orcaice::Exception if the argument list is empty. Returns an empty string if 
 *  the argument is non-empty but no suitable properties were found.
 */
std::string getServiceConfigFilename( const Ice::StringSeq & args );

//
// These guys are supposed to be simple wrappers for the Ice 'getPropertyXxxx' functions.
//
// In fact, things are a bit harder.  Ice's 'getPropertAsInt' returns '0' for 'property not found'.
// So everything goes through Ice's 'getProperty' instead, which returns an empty string on 'not found'.
//

//! Returns: 0 = property found (and set in value), non-zero = property not found.
int getPropertyAsDouble( const Ice::PropertiesPtr &, const ::std::string& key, Ice::Double &value );
//! Returns: 0 = property found (and set in value), non-zero = property not found.
int getPropertyAsInt(    const Ice::PropertiesPtr &, const ::std::string& key, int         &value );
//! Returns: 0 = property found (and set in value), non-zero = property not found.
int getProperty(         const Ice::PropertiesPtr &, const ::std::string& key, std::string &value );
//! Parses the value into a sequence of strings assuming ":" delimeters (like a sequence of endopoints).
//! Returns: 0 = property found (and set in value), non-zero = property not found or parsing failed.
int getPropertyAsStringSeq( const Ice::PropertiesPtr &, const ::std::string& key, Ice::StringSeq &value );
//! Parses the value into a Frame2d object assuming empty space delimeters. The format is assumed to
//! be: x[m] y[m] yaw[deg]. 
//! Returns: 0 = property found (and set in value), non-zero = property not found or parsing failed.
int getPropertyAsFrame2d( const Ice::PropertiesPtr &, const ::std::string& key, orca::Frame2d &value );
//! Parses the value into a Frame3d object assuming empty space delimeters. The format is assumed to
//! be: x[m] y[m] z[m] roll[deg] pitch[deg] yaw[deg]. 
//! Returns: 0 = property found (and set in value), non-zero = property not found or parsing failed.
int getPropertyAsFrame3d( const Ice::PropertiesPtr &, const ::std::string& key, orca::Frame3d &value );
//! Parses the value into a Size2d object assuming empty space delimeters.
//! Returns: 0 = property found (and set in value), non-zero = property not found or parsing failed.
int getPropertyAsSize2d( const Ice::PropertiesPtr &, const ::std::string& key, orca::Size2d &value );
//! Parses the value into a Size3d object assuming empty space delimeters.
//! Returns: 0 = property found (and set in value), non-zero = property not found or parsing failed.
int getPropertyAsSize3d( const Ice::PropertiesPtr &, const ::std::string& key, orca::Size3d &value );
//! Parses the value into a CartesianPoint object assuming empty space delimeters.
//! Returns: 0 = property found (and set in value), non-zero = property not found or parsing failed.
int getPropertyAsCartesianPoint( const Ice::PropertiesPtr &, const ::std::string& key, orca::CartesianPoint &value );
//! Parses the value into a Time object assuming ':' delimeters.
//! Returns: 0 = property found (and set in value), non-zero = property not found or parsing failed.
int getPropertyAsTimeDuration( const Ice::PropertiesPtr &, const ::std::string& key, orca::Time &value );

//! Returns the default value if key is not found or cannot be converted to a double.
Ice::Double getPropertyAsDoubleWithDefault( const Ice::PropertiesPtr &, const ::std::string& key, const Ice::Double defaultValue );
//! Returns the default value if key is not found or cannot be converted to an integer.
int         getPropertyAsIntWithDefault(    const Ice::PropertiesPtr &, const ::std::string& key, const int         defaultValue );
//! Returns the default value if key is not found.
std::string getPropertyWithDefault(         const Ice::PropertiesPtr &, const ::std::string& key, const std::string &defaultValue );
//! Returns the default value if key is not found or cannot be converted to a string sequence.
Ice::StringSeq getPropertyAsStringSeqWithDefault( const Ice::PropertiesPtr &, const ::std::string& key, const Ice::StringSeq &defaultValue );
//! Returns the default value if key is not found or cannot be converted to a Frame2d.
orca::Frame2d getPropertyAsFrame2dWithDefault( const Ice::PropertiesPtr &, const ::std::string& key, const orca::Frame2d &defaultValue );
//! Returns the default value if key is not found or cannot be converted to a Frame3d.
orca::Frame3d getPropertyAsFrame3dWithDefault( const Ice::PropertiesPtr &, const ::std::string& key, const orca::Frame3d &defaultValue );
//! Returns the default value if key is not found or cannot be converted to a Size3d.
orca::Size3d getPropertyAsSize2dWithDefault( const Ice::PropertiesPtr &, const ::std::string& key, const orca::Size2d &defaultValue );
//! Returns the default value if key is not found or cannot be converted to a Size3d.
orca::Size3d getPropertyAsSize3dWithDefault( const Ice::PropertiesPtr &, const ::std::string& key, const orca::Size3d &defaultValue );
//! Returns the default value if key is not found or cannot be converted to a CartesianPoint.
orca::CartesianPoint getPropertyAsCartesianPointWithDefault( const Ice::PropertiesPtr &, const ::std::string& key, const orca::CartesianPoint &defaultValue );
//! Returns the default value if key is not found or cannot be converted to a Time.
orca::Time getPropertyAsTimeDurationWithDefault( const Ice::PropertiesPtr &, const ::std::string& key, const orca::Time &d );

//! Returns standardized text which warns that a configuration property is not set.
std::string warnMissingProperty( const std::string & prop );
//! Returns standardized text which warns that a configuration property is not set.
//! Also informs that the specified default value is used instead.
std::string warnMissingPropertyWithDefault( const std::string & prop, const std::string & defaultValue );

//! Returns standardized text which warns that certain info is missing because a configuration property is not set
std::string warnMissingProperty( const std::string & info, const std::string & prop );
//! Returns standardized text which warns that a property is missing because a configuration property is not set. 
//! Also informs that the specified default value is used instead.
std::string warnMissingPropertyWithDefault( const std::string & info, const std::string & prop, const std::string & defaultValue );

//@}

} // namespace

#endif
