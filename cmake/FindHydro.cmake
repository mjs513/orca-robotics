# Locate Hydro installation

# This module defines
# HYDRO_HOME where to find include, lib, bin, etc.
# HYDRO_FOUND, If set to 0, don't try to use Hydro libraries, scripts, etc.

# start with 'not found'
SET( HYDRO_FOUND 0 CACHE BOOL "Do we have Hydro?" )

FIND_PATH( HYDRO_HOME hydro_manifest.cmake
  $ENV{HYDRO_HOME}
  # Test standard installation points
  /opt/hydro
  /opt/hydro-${PROJECT_VERSION}   # this will work in Orca which has the same version as Hydro
  C:/hydro
  C:/hydro-${PROJECT_VERSION}
  )
# MESSAGE( STATUS "DEBUG: manifest.cmake is apparently found in : ${HYDRO_HOME}" )

# NOTE: if HYDRO_HOME is set to *-NOTFOUND it will evaluate to FALSE
IF ( HYDRO_HOME )
    SET( HYDRO_FOUND 1 CACHE BOOL "Do we have Hydro?" FORCE )
ENDIF ( HYDRO_HOME )

# do we need it in cache?
# SET( HYDRO_HOME ${HYDRO_HOME} CACHE PATH "Hydro installed directory" FORCE )
#   MESSAGE( "Setting Hydro installed directory to ${HYDRO_HOME}" )
