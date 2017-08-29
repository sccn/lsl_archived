cmake_minimum_required(VERSION 3.0)
project(gNEEDaccess VERSION 0.0.1)

# load LSLAppBoilerplate if not done already
if(NOT TARGET LSL::lsl)
    if(NOT LSL_ROOT)
        set(LSL_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../build/lsl_Release/lslinstall/LSL")
        get_filename_component(LSL_ROOT ${LSL_ROOT} ABSOLUTE)
        if(NOT EXISTS ${LSL_ROOT})
            message(FATAL_ERROR "When building a single app out of tree you need to set LSL_ROOT ( cmake -DLSL_ROOT=/path/to/installed/lsl)")
        endif()
    endif()
    list(APPEND CMAKE_MODULE_PATH "${LSL_ROOT}/cmake")
    include(LSLAppBoilerplate)
endif()

# GENERAL CONFIG #

set(CMAKE_INSTALL_PREFIX ${LSL_ROOT}/..)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# THIRD PARTY LIBRARIES #

# Qt
if(NOT Qt5_DIR)
    set(Qt5_DIR "C:\\Qt\\5.9.1\\msvc2015_64\\lib\\cmake\\Qt5")
endif()
find_package(Qt5 REQUIRED COMPONENTS Core Network Xml Gui Widgets)

# Emotiv SDK
find_package(gNEEDaccessSDK)


# APPLICATION #

SET(gNEEDaccess_SRC)
SET(gNEEDaccess_INCL_DIRS)
SET(gNEEDaccess_REQ_LIBS)

# Sources
LIST(APPEND gNEEDaccess_SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mainwindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mainwindow.h
    ${CMAKE_CURRENT_LIST_DIR}/src/mainwindow.ui
    ${CMAKE_CURRENT_LIST_DIR}/src/gdsthread.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gdsthread.cpp
)

LIST(APPEND gNEEDaccess_INCL_DIRS
    ${Boost_INCLUDE_DIRS}
    ${gNEEDaccessSDK_INCLUDE_DIRS}
)

LIST(APPEND gNEEDaccess_REQ_LIBS
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Xml
    LSL::lsl
    ${Boost_LIBRARIES}
    ${gNEEDaccessSDK_LIBRARIES}
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
installLSLAuxFiles(${target} ${gNEEDaccessSDK_BINARIES})