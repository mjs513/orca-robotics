/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_PROPERTY_UTILITIES_H
#define ORCAICE_PROPERTY_UTILITIES_H

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
 *     environment variable @c ORCAICE_GLOBAL_CONFIG
 *  3. If it is not defined (or empty), tries to load the defaultt file from a 
 *     system-dependent location:
 *          > Linux: $HOME/.orcarc
 *          > Win32: C:\orca.ini
 *  Throws hydroutil::Exception if none of the three options work out.
 */
std::string getGlobalConfigFilename( const Ice::PropertiesPtr &props );

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
 *  Throws hydroutil::Exception if the argument list is empty. Returns an empty string if 
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
//! Throws a hydroutil::Exception if the key is found but the value cannot be converted to the desired type.
int getPropertyAsDouble( const Ice::PropertiesPtr &, const ::std::string& key, Ice::Double &value );
//! Returns: 0 = property found (and set in value), non-zero = property not found.
//! Throws a hydroutil::Exception if the key is found but the value cannot be converted to the desired type.
int getPropertyAsInt(    const Ice::PropertiesPtr &, const ::std::string& key, int         &value );
//! Returns: 0 = property found (and set in value), non-zero = property not found.
//! Note that the Ice PropertiesI::load() method has a line size limitation of 1024 characters 
//! but quietly fails so there is no way of knowing if you have exceeded this limitation
int getProperty(         const Ice::PropertiesPtr &, const ::std::string& key, std::string &value );
//! Parses the value into a vector of ints assuming empty space delimeters. Elements will continue
//! to be added to the end of the vector until the end of the line is reached.
//! Returns: 0 = property found (and set in value), non-zero = property not found.
//! Note that the Ice PropertiesI::load() method has a line size limitation of 1024 characters.
//! but quietly fails so there is no way of knowing if you have exceeded this limitation
//! Throws a hydroutil::Exception if the key is found but the value cannot be converted to the desired type.
int getPropertyAsIntVector( const Ice::PropertiesPtr &, const ::std::string& key, std::vector<int> &value );
//! Parses the value into a vector of doubles assuming empty space delimeters. Elements will continue
//! to be added to the end of the vector until the end of the line is reached.
//! Returns: 0 = property found (and set in value), non-zero = property not found.
//! Note that the Ice PropertiesI::load() method has a line size limitation of 1024 characters 
//! but quietly fails so there is no way of knowing if you have exceeded this limitation.
//! Throws a hydroutil::Exception if the key is found but the value cannot be converted to the desired type.
int getPropertyAsDoubleVector( const Ice::PropertiesPtr &, const ::std::string& key, std::vector<double> &value );

//! Returns the default value if key is not found.
//! Throws a hydroutil::Exception if the key is found but the value cannot be converted to the desired type.
Ice::Double getPropertyAsDoubleWithDefault( const Ice::PropertiesPtr &, const ::std::string& key, const Ice::Double defaultValue );
//! Returns the default value if key is not found.
//! Throws a hydroutil::Exception if the key is found but the value cannot be converted to the desired type.
int         getPropertyAsIntWithDefault(    const Ice::PropertiesPtr &, const ::std::string& key, const int         defaultValue );
//! Returns the default value if key is not found.
std::string getPropertyWithDefault(         const Ice::PropertiesPtr &, const ::std::string& key, const std::string &defaultValue );
//! Returns the default value if key is not found or cannot be converted to a vector<int>.
std::vector<int> getPropertyAsIntVectorWithDefault( const Ice::PropertiesPtr &, const ::std::string& key, const std::vector<int> &defaultValue );
//! Returns the default value if key is not found.
//! Throws a hydroutil::Exception if the key is found but the value cannot be converted to the desired type.
std::vector<double> getPropertyAsDoubleVectorWithDefault( const Ice::PropertiesPtr &, const ::std::string& key, const std::vector<double> &defaultValue );

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
