cmake_minimum_required(VERSION 3.0)
project(gNEEDaccess VERSION 0.0.1)

# load LSLAppBoilerplate if not done already
if(NOT TARGET LSL::lsl)
    if(NOT LSL_INSTALL_ROOT)
        # If building the app out of tree, the user is expected to supply LSL_INSTALL_ROOT.
        # For ease of Chadwick Boulay's development, he has added a default directory to check.
        # This default directory is probably wrong for you. Ignore it.
        # Just supply -DLSL_INSTALL_ROOT=/path/to/installed/lsl to the cmake command.
        set(LSL_INSTALL_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../build/install/lsl_Release/LSL")
        set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/../../../build/install/lsl_Release")
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
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../cmake")
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
# find_package(Boost REQUIRED)

# g.Tec gNEEDaccessSDK
find_package(gNEEDaccessSDK)


# APPLICATION #

# Collect requirements
SET(gNEEDaccess_SRC)
SET(gNEEDaccess_INCL_DIRS)
SET(gNEEDaccess_REQ_LIBS)

#...sources
LIST(APPEND gNEEDaccess_SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mainwindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mainwindow.h
    ${CMAKE_CURRENT_LIST_DIR}/src/mainwindow.ui
    ${CMAKE_CURRENT_LIST_DIR}/src/nautilus_dlg.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/nautilus_dlg.h
    ${CMAKE_CURRENT_LIST_DIR}/src/nautilus_dlg.ui
    ${CMAKE_CURRENT_LIST_DIR}/src/gUSB_dlg.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gUSB_dlg.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gUSB_dlg.ui
)

#...headers
#LIST(APPEND gNEEDaccess_INCL_DIRS
#    ${Boost_INCLUDE_DIRS}
#)

#...libraries
LIST(APPEND gNEEDaccess_REQ_LIBS
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Xml
    LSL::lsl
    ${Boost_LIBRARIES}
    gtec::gNEEDaccessSDK
)

# TARGET #
SET(target gNEEDaccess)
add_executable(${target}
    ${gNEEDaccess_SRC}
)
target_include_directories(${target}
    PRIVATE
    ${gNEEDaccess_INCL_DIRS})
target_link_libraries(${target}
    PRIVATE
    ${gNEEDaccess_REQ_LIBS}
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
installLSLAuxFiles(${target} ${CMAKE_CURRENT_LIST_DIR}/gneedaccess_config.cfg)
#installLSLAuxFiles(${target} ${gNEEDaccessSDK_BINARIES})  # Not necessary because g.Tec gNEEDaccess installer puts libraries on the path.
