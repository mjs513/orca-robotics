include_directories( 
    ${PROJECT_SOURCE_DIR}/src/libs
)

#
# Platform-specific compiler and linker flags
#
if( NOT ORCA_OS_WIN )
    add_definitions( "-Wall" )
else( NOT ORCA_OS_WIN )
    add_definitions( "-Wall -D_CRT_SECURE_NO_DEPRECATE" )
endif( NOT ORCA_OS_WIN )
