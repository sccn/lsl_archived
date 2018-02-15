cmake_minimum_required(VERSION 3.0)
project(BioSemi VERSION 0.0.1)

# load LSLAppBoilerplate if not done already
if(NOT TARGET LSL::lsl)
    if(NOT LSL_INSTALL_ROOT)
        # If building the app out of tree, the user is expected to supply LSL_INSTALL_ROOT.
        # This default directory is may be wrong for you. Ignore it.
        # Just supply -DLSL_INSTALL_ROOT=/path/to/installed/lsl to the cmake command.
        set(LSL_INSTALL_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../build/install/lsl_Release/LSL")
        set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/../../build/install/lsl_Release")
        get_filename_component(LSL_INSTALL_ROOT ${LSL_INSTALL_ROOT} ABSOLUTE)
        if(NOT EXISTS ${LSL_INSTALL_ROOT})
            message(FATAL_ERROR "When building a single app out of tree you need to set LSL_INSTALL_ROOT ( cmake -DLSL_INSTALL_ROOT=/path/to/installed/lsl)")
        endif()
    endif()
    if(NOT IS_ABSOLUTE ${LSL_INSTALL_ROOT})
        message(FATAL_ERROR "LSL_INSTALL_ROOT needs to be an absolute path")
    endif()
    file(TO_CMAKE_PATH ${LSL_INSTALL_ROOT} LSL_INSTALL_ROOT)
    list(APPEND CMAKE_MODULE_PATH "${LSL_INSTALL_ROOT}/cmake")
    include(LSLAppBoilerplate)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../cmake")
    include(LSLSaneDefaults)
endif()

# GENERAL CONFIG #
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# THIRD PARTY LIBRARIES #

# Qt
# If Qt5_DIR is not supplied, use default directory defined in ${CMAKE_CURRENT_LIST_DIR}/../../../cmake/LSLSaneDefaults
find_package(Qt5 REQUIRED COMPONENTS Core Xml Gui Widgets)

# Boost
find_package(Boost REQUIRED COMPONENTS thread)

# APPLICATION #

# Collect requirements
SET(BioSemi_SRC)
SET(BioSemi_INCL_DIRS)
SET(BioSemi_REQ_LIBS)

#...sources
LIST(APPEND BioSemi_SRC
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mainwindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mainwindow.h
    ${CMAKE_CURRENT_LIST_DIR}/mainwindow.ui
    ${CMAKE_CURRENT_LIST_DIR}/biosemi_io.cpp
    ${CMAKE_CURRENT_LIST_DIR}/biosemi_io.h
    ${CMAKE_CURRENT_LIST_DIR}/Resampler.h
)

#...headers
LIST(APPEND BioSemi_INCL_DIRS
    ${Boost_INCLUDE_DIRS}
)

#...libraries
LIST(APPEND BioSemi_REQ_LIBS
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Xml
    ${Boost_LIBRARIES}
    LSL::lsl
)

# TARGET #
SET(target BioSemi)
add_executable(${target}
    MACOSX_BUNDLE
    WIN32
    ${BioSemi_SRC}
)
target_include_directories(${target}
    PRIVATE
    ${BioSemi_INCL_DIRS})
target_link_libraries(${target}
    PRIVATE
    ${BioSemi_REQ_LIBS}
)

#if(CMAKE_SIZEOF_VOID_P EQUAL 8)
#SET_TARGET_PROPERTIES(${target}
#    PROPERTIES
#        LINK_FLAGS "/NODEFAULTLIB:\"liblsl64\""
#)
#else()
#SET_TARGET_PROPERTIES(${target}
#    PROPERTIES
#        LINK_FLAGS "/NODEFAULTLIB:\"liblsl32\""
#)
#endif()


# Deployment # 
installLSLApp(${target})  # Will handle Qt and LSL libraries
installLSLAuxFiles(${target} ${CMAKE_CURRENT_LIST_DIR}/biosemi_config.cfg)
installLSLAuxFiles(${target} "directory" ${CMAKE_CURRENT_LIST_DIR}/DLL)
