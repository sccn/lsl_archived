cmake_minimum_required(VERSION 3.0)
project(Emotiv VERSION 0.0.1)

# load LSLAppBoilerplate if not done already
if(NOT TARGET LSL::lsl)
    if(NOT LSL_ROOT)
        set(LSL_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../build/lsl_Release/lslinstall/LSL")
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

# Boost
# Override by adding "-DBOOST_ROOT=C:\path\to\boost\ -DBOOST_LIBRARYDIR=C:\path\to\boost\lib64-msvc-14.0\" to your cmake command
IF (NOT BOOST_ROOT)
    SET(BOOST_ROOT "C:/local/boost_1_65_0/")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        SET(BOOST_LIBRARYDIR "${BOOST_ROOT}lib64-msvc-14.0/")
    else()
        SET(BOOST_LIBRARYDIR "${BOOST_ROOT}lib32-msvc-14.0/")
    endif()
ENDIF()
#set(Boost_DEBUG                OFF) #Switch this and next to ON for help debugging Boost problems.
#set(Boost_DETAILED_FAILURE_MSG OFF)
set(Boost_USE_STATIC_LIBS      ON) # only find static libs
#set(Boost_USE_MULTITHREADED    ON)
#set(Boost_USE_STATIC_RUNTIME   ON) #Not default. Use if app is linking against static runtime.
# Disable asio auto linking in date-time and regex
#add_definitions(-DBOOST_DATE_TIME_NO_LIB)
add_definitions(-DBOOST_REGEX_NO_LIB)
FIND_PACKAGE(Boost REQUIRED
    COMPONENTS
        thread
        regex
)

# Emotiv SDK
find_package(EmotivSDK)

# APPLICATION #

SET(LSLEmotiv_SRC)
SET(LSLEmotiv_INCL_DIRS)
SET(LSLEmotiv_REQ_LIBS)

# Sources
LIST(APPEND LSLEmotiv_SRC
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mainwindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mainwindow.h
    ${CMAKE_CURRENT_LIST_DIR}/mainwindow.ui
)

LIST(APPEND LSLEmotiv_INCL_DIRS
    ${Boost_INCLUDE_DIRS}
    ${EmotivSDK_INCLUDE_DIRS}
)

LIST(APPEND LSLEmotiv_REQ_LIBS
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Xml
    LSL::lsl
    ${Boost_LIBRARIES}
    ${EmotivSDK_LIBRARIES}
)


# TARGET #

SET(target LSLEmotiv)

add_executable(${target}
    ${LSLEmotiv_SRC}
)

target_include_directories(${target}
    PRIVATE
    ${LSLEmotiv_INCL_DIRS})

target_link_libraries(${target}
    PRIVATE
    ${LSLEmotiv_REQ_LIBS}
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
installLSLAuxFiles(${target} ${CMAKE_CURRENT_LIST_DIR}/emotiv_config.cfg)
installLSLAuxFiles(${target} ${EmotivSDK_BINARIES})