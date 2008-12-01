IF( JAVA_FOUND AND ORCA_BUILD_JAVA )

MESSAGE( STATUS "Planning to build Java interfaces." )

PROJECT( java_interfaces Java )
# SET( CMAKE_VERBOSE_MAKEFILE 1 )

# TEMP HACKS
SET( PROJECT_INTERFACE_JAR orca.jar )
SET( PROJECT_INTERFACE_NAMESPACE orca )
SET( PROJECT_INTERFACE_TOKEN_FILE common.ice )
SET( PROJECT_INTERFACE_TOKEN_CLASS Time )

SET( SLICE2JAVA_COMMAND        ${ICE_HOME}/bin/slice2java${EXE_EXTENSION} )

SET( SLICE_SOURCE_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/../slice )
SET( SLICE_BINARY_DIR          ${CMAKE_CURRENT_BINARY_DIR}/../slice )
SET( SLICE2JAVA_BINARY_DIR     ${CMAKE_CURRENT_BINARY_DIR} )

# debian package splits off slice files into a different place
IF( ICE_HOME MATCHES /usr )
    SET( ice_slice_dir /usr/share/slice )
    MESSAGE( STATUS "This is a Debian Ice installation. Slice files are in ${ice_slice_dir}" )
ELSE ( ICE_HOME MATCHES /usr )
    SET( ice_slice_dir ${ICE_HOME}/slice )
    MESSAGE( STATUS "This is NOT a Debian Ice installation. Slice files are in ${ice_slice_dir}" )
ENDIF( ICE_HOME MATCHES /usr )

# note: compared to slice2cpp, slice2java automatically places generated files into 'namespace dir,
#       e.g. 'orca'. So the output dir is just 'java' not 'java/<namespace>'
SET( SLICE_ARGS -I${SLICE_SOURCE_DIR} -I${ice_slice_dir} --stream --output-dir ${SLICE2JAVA_BINARY_DIR} --meta "java:java5" )
# note: satelite projects need to include slice files from orca installation
IF( DEFINED ORCA_HOME )
    SET( SLICE_ARGS -I${ORCA_HOME}/slice ${SLICE_ARGS} )
ENDIF( DEFINED ORCA_HOME )

# ALL .java files for the .class files
ADD_CUSTOM_COMMAND(
    OUTPUT ${SLICE2JAVA_BINARY_DIR}/${PROJECT_INTERFACE_NAMESPACE}/${PROJECT_INTERFACE_TOKEN_CLASS}.java
    COMMAND ${SLICE2JAVA_COMMAND} 
    ARGS ${SLICE_SOURCE_DIR}/${PROJECT_INTERFACE_NAMESPACE}/*.ice ${SLICE_ARGS}
    MAIN_DEPENDENCY ${SLICE_SOURCE_DIR}/${PROJECT_INTERFACE_NAMESPACE}/${PROJECT_INTERFACE_TOKEN_FILE}
    COMMENT "-- Generating .java files from .ice sources." )

MESSAGE( STATUS "Will generate .java files from Slice definitions using this command:" )
MESSAGE( STATUS "${SLICE2JAVA_COMMAND} *.ice ${SLICE_ARGS}" )

# ALL .class files for the .jar file
ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_INTERFACE_NAMESPACE}/${PROJECT_INTERFACE_TOKEN_CLASS}.class
    COMMAND ${CMAKE_Java_COMPILER} 
    ARGS -classpath ${CMAKE_CURRENT_SOURCE_DIR} -classpath ${ICEJ_HOME}/Ice.jar -d ${CMAKE_CURRENT_BINARY_DIR} 
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_INTERFACE_NAMESPACE}/*.java
    MAIN_DEPENDENCY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_INTERFACE_NAMESPACE}/${PROJECT_INTERFACE_TOKEN_CLASS}.java
    COMMENT "-- Building Java objects ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_INTERFACE_NAMESPACE}/${PROJECT_INTERFACE_TOKEN_CLASS}/*.class" )

MESSAGE( STATUS "DEBUG: Will build Java objects using this command:" )
MESSAGE( STATUS "${CMAKE_Java_COMPILER} -classpath ${CMAKE_CURRENT_SOURCE_DIR} -classpath ${ICEJ_HOME}/Ice.jar -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_INTERFACE_NAMESPACE}/*.java" )

# the .jar file for the target
ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_INTERFACE_JAR}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_INTERFACE_NAMESPACE}/${PROJECT_INTERFACE_TOKEN_CLASS}.class
    COMMAND ${CMAKE_COMMAND}
    ARGS -E chdir ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_Java_ARCHIVE} 
        -cf ${PROJECT_INTERFACE_JAR} ${PROJECT_INTERFACE_NAMESPACE}/*.class
#         -cf ${PROJECT_INTERFACE_JAR} ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_INTERFACE_NAMESPACE}/*.class
    COMMENT "-- Creating archive ${PROJECT_INTERFACE_JAR} from .class files." )

# the target
ADD_CUSTOM_TARGET( ${PROJECT_INTERFACE_JAR} ALL 
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_INTERFACE_JAR})

GBX_ADD_ITEM( ${PROJECT_INTERFACE_JAR} )
INSTALL_FILES( ${GBX_LIB_INSTALL_DIR} FILES ${PROJECT_INTERFACE_JAR} )
MESSAGE( STATUS "Will install archive ${PROJECT_INTERFACE_JAR} into ${GBX_LIB_INSTALL_DIR}" )

ELSE ( JAVA_FOUND AND ORCA_BUILD_JAVA )
    MESSAGE( STATUS "Will not build Java interfaces : JAVA_FOUND=${JAVA_FOUND}, ORCA_BUILD_JAVA=${ORCA_BUILD_JAVA}" )
ENDIF( JAVA_FOUND AND ORCA_BUILD_JAVA )