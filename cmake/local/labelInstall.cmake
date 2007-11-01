# labelInstall.cmake
# Author Duncan Mercer

# This script is only called during the installation phase.

# When we are installing the system using 'make install'
# we want to label the installation date / time for 
# identification purposes. 

IF ( NOT WIN32 )

  # Specify where to write the information generated by the labelInstall.sh script
  # Note the this is the ./bin/ directory of the install
  SET (LABEL_OUTPUT_FILE "${CMAKE_INSTALL_PREFIX}/bin/orcainstallinfo.txt")

  # Setup the path to the script. Note that this is in the HYDRO install
  IF (EXISTS $ENV{HYDRO_SRC})
    SET (LABEL_SCRIPT "$ENV{HYDRO_SRC}/scripts/shell/labelInstall.sh")  
    MESSAGE("-- Labeling and time stamping the install")
    EXECUTE_PROCESS(COMMAND ${LABEL_SCRIPT}  $ENV{ORCA_SRC} 
                    RESULT_VARIABLE SCRIPT_RESULT
                    OUTPUT_VARIABLE SCRIPT_DATA)
                  
    # Flag it if there was a failure.
    IF ( ${SCRIPT_RESULT} )              
      MESSAGE("WARNING: Unable to timestamp the install. Continuing anyway.")
      MESSAGE("${SCRIPT_DATA}")
    ENDIF ( ${SCRIPT_RESULT} )              

    #Write file even if failed above. Ensures that failure gets recorded
    FILE(WRITE ${LABEL_OUTPUT_FILE} ${SCRIPT_DATA})

  ELSE (EXISTS $ENV{HYDRO_SRC})

    MESSAGE("WARNING: Env var HYDRO_SRC not found Unable to timestamp the install. Continuing anyway.")

  ENDIF (EXISTS $ENV{HYDRO_SRC})

  

ENDIF ( NOT WIN32 )
