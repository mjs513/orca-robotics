set( _lib_name ${GBX_PROJECT_NAME_CAP}IfaceLog )
set( _lib_namespace ${GBX_PROJECT_NAME_LOWER}ifacelog )
set( _lib_version ${GBX_PROJECT_VERSION} )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${_lib_name} ON )

# using Exception
set( _dep_libs GbxUtilAcfr )
GBX_REQUIRE_LIBS( build LIB ${_lib_name} ${_dep_libs} )

if( build )
    #
    # Defines macros for parsing Slice source files.
    #
    include( ${ORCA_CMAKE_DIR}/Slice2Log.cmake )
    
    #
    # All Slice source files are defined in one place (loads ORCA_SLICE_SOURCE_FILES)
    #
    include( ${PROJECT_SOURCE_DIR}/src/interfaces/slice/${GBX_PROJECT_NAME_LOWER}/slice_sources.cmake )
    
    #
    # Work out the list of generated files from the list of slice sources
    #
    ORCA_GENERATE_SLICE2LOG_RULES( _gen_sources _gen_headers ${ORCA_SLICE_SOURCE_FILES} )
    
    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${GEARBOX_CMAKE_DIR}/UseGearbox.cmake )
    include( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    include( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    
    if( NOT ORCA_MOTHERSHIP )
        include( ${ORCA_CMAKE_DIR}/UseOrca.cmake )
    endif( NOT ORCA_MOTHERSHIP )

    # add templated utility files
    set( util_h_file ${CMAKE_CURRENT_BINARY_DIR}/util.h )
    set( util_cpp_file ${CMAKE_CURRENT_BINARY_DIR}/util.cpp )
    
    if ( NOT EXISTS ${util_h_file} OR NOT EXISTS ${util_cpp_file} )
      configure_file( ${ORCA_CMAKE_DIR}/ifacelogutil.h.in ${util_h_file} )
      configure_file( ${ORCA_CMAKE_DIR}/ifacelogutil.cpp.in ${util_cpp_file} )
    endif ( NOT EXISTS ${util_h_file} OR NOT EXISTS ${util_cpp_file} )

    # IceStorm is not included in UseIce.cmake
    set( _dep_libs ${_dep_libs} IceStorm )
    
    GBX_ADD_LIBRARY( ${_lib_name} DEFAULT ${_lib_version} ${_gen_sources} ${util_cpp_file} )
    target_link_libraries( ${_lib_name} ${_dep_libs} )    

    # add dependency on the generator executable when compiling Orca (satellite projects use installed version)
    # OR
    # when everything is built from one super-project.
    if( ORCA_MOTHERSHIP OR IS_SUPER_PROJECT )
        add_dependencies( ${_lib_name} slice2orca )
    endif( ORCA_MOTHERSHIP OR IS_SUPER_PROJECT )

    # for satellite projects inside a super-project, need to wait until the Orca library is build
    if( NOT ORCA_MOTHERSHIP AND IS_SUPER_PROJECT )
        add_dependencies( ${_lib_name} OrcaIfaceLog )
    endif( NOT ORCA_MOTHERSHIP AND IS_SUPER_PROJECT )

    if( ORCA_MOTHERSHIP )
        # only Orca installs generated headers
        foreach( _gen_header ${_gen_headers} )
            set( slice_generated_headers_full ${slice_generated_headers_full} ${CMAKE_CURRENT_BINARY_DIR}/${_gen_header} )
        endforeach( _gen_header )
        
        GBX_ADD_HEADERS( ${_lib_namespace} ${slice_generated_headers_full} ${util_h_file} )

        if( ORCA_BUILD_TESTS )
            add_subdirectory( test )
        endif( ORCA_BUILD_TESTS )
    endif( ORCA_MOTHERSHIP )

endif( build )
