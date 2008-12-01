ORCA_SET_COMPONENT_NAME( ImageView )
# SET( APP_CTRLC_HANDLER 0 )
GBX_ADD_LICENSE( LGPL )

FIND_PACKAGE( Qt4 )
FIND_PACKAGE( OpenGL )
FIND_PACKAGE( OpenCV )

# for some reason, when the sandbox is turned on, this flag turns
# this component off again.
# SET( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
SET( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} not_os_win "Windows OS is not  supported" )

SET( dep_libs OrcaIfaceImpl )
GBX_REQUIRE_TARGETS( build EXE ${APP_NAME} ${dep_libs} )

SET( dep_libs ${dep_libs} HydroDll )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} HYDRODLL_INSTALLED "libHydroDll not installed" )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} QT4_FOUND "Qt4 not found" )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} OPENGL_GLU_FOUND "glu not found" )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} OPENCV_FOUND "OpenCv not found" )

IF( OPENCV_FOUND )
    INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIRS} )
#    LINK_DIRECTORIES( ${OPENCV_LINK_DIRS} )
#    ADD_DEFINITIONS( -DOPENCV_FOUND )
ENDIF( OPENCV_FOUND )

SET ( BUILD_IMAGEVIEW FALSE )
IF( BUILD_IMAGEVIEW )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    SET( QT_USE_QTOPENGL TRUE )
    INCLUDE( ${QT_USE_FILE} )

    #
    # This gathers all the source files into the variable 'srcs'
    #
    FILE( GLOB human_srcs *.cpp )
    QT4_WRAP_CPP( moc_srcs imagequeue.h viewwidget.h )

    # build stand-alone component  
    ORCA_WRAP_MAINSUBSYSTEM_INTO_APP( ${APP_NAME} 
                                      ${APP_CTRLC_HANDLER} 
                                      ${COMP_NAMESPACE} 
                                      ${COMP_NAME} 
                                      ${human_srcs}
                                      ${moc_srcs} )
    
    TARGET_LINK_LIBRARIES( ${APP_NAME} 
                           ${dep_libs} 
                           ${QT_LIBRARIES} 
                           ${OPENGL_LIBRARIES}
                           ${OPENCV_LIBS} )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF( ORCA_BUILD_SANDBOX )