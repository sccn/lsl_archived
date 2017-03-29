# CMake version
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

##################
# GENERAL CONFIG #
##################

# Include cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Set policies
cmake_policy(SET CMP0028 NEW) # ENABLE CMP0028: Double colon in target name means ALIAS or IMPORTED target.
cmake_policy(SET CMP0054 NEW) # ENABLE CMP0054: Only interpret if() arguments as variables or keywords when unquoted.
cmake_policy(SET CMP0042 NEW) # ENABLE CMP0042: MACOSX_RPATH is enabled by default.
cmake_policy(SET CMP0063 NEW) # ENABLE CMP0063: Honor visibility properties for all target types.

# Meta information about the project
set(META_PROJECT_NAME        "LabRecorder")
set(META_PROJECT_DESCRIPTION "Record LabStreamingLayer streams to XDF data file.")
set(META_AUTHOR_ORGANIZATION "David Medine -- SCCN")
set(META_AUTHOR_DOMAIN       "https://github.com/sccn/labstreaminglayer/tree/master/Apps/LabRecorder")
set(META_AUTHOR_MAINTAINER   "")
set(META_VERSION_MAJOR       "0")
set(META_VERSION_MINOR       "1")
set(META_VERSION_PATCH       "0")
set(META_VERSION             "${META_VERSION_MAJOR}.${META_VERSION_MINOR}.${META_VERSION_PATCH}")
set(META_NAME_VERSION        "${META_PROJECT_NAME} v${META_VERSION}")

# Generate folders for IDE targets (e.g., VisualStudio solutions)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(IDE_FOLDER "")

# Declare project
project(${META_PROJECT_NAME} C CXX)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# Set runtime path
set(CMAKE_SKIP_BUILD_RPATH            FALSE) # Add absolute path to all dependencies for BUILD
set(CMAKE_BUILD_WITH_INSTALL_RPATH    FALSE) # Use CMAKE_INSTALL_RPATH for INSTALL
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE) # Do NOT add path to dependencies for INSTALL

if(NOT SYSTEM_DIR_INSTALL)
    # Find libraries relative to binary
    if(APPLE)
        set(CMAKE_INSTALL_RPATH "@loader_path/../../../${INSTALL_LIB}")
    else()
        set(CMAKE_INSTALL_RPATH "$ORIGIN/${INSTALL_LIB}")       
    endif()
endif()


#########################
# THIRD PARTY LIBRARIES #
#########################

# Qt
FIND_PROGRAM(QT_QMAKE_EXECUTABLE_FINDQT
    NAMES
        qmake qmake5 qmake-qt5
    PATHS
        "${QT_SEARCH_PATH}/bin"
        "$ENV{QTDIR}/bin"
)
SET(QT_QMAKE_EXECUTABLE ${QT_QMAKE_EXECUTABLE_FINDQT} CACHE PATH "Qt qmake program.")
EXEC_PROGRAM(${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_PREFIX" OUTPUT_VARIABLE CMAKE_PREFIX_PATH)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network)
find_package(Qt5Xml)
find_package(Qt5Gui)
find_package(Qt5Widgets)
set(CMAKE_INCLUDE_CURRENT_DIR ON)  # Because the ui_mainwindow.h file.
# Enable automoc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(AUTOMOC_MOC_OPTIONS PROPERTIES FOLDER CMakeAutomocTargets)
set_property(GLOBAL PROPERTY AUTOMOC_FOLDER CMakeAutomocTargets)
# ENABLE CMP0020: Automatically link Qt executables to qtmain target on Windows.
cmake_policy(SET CMP0020 NEW)
#TODO: Make sure the architecture of the Qt binaries matches the current context architecture.

# Boost
IF(MSVC)
    # Default location of pre-compiled Boost for Windows
    # Override by adding "-DBOOST_ROOT=C:\path\to\boost\ -DBOOST_LIBRARYDIR=C:\path\to\boost\lib32-msvc-14.0\" to your cmake command
    IF (NOT BOOST_ROOT)
        SET(BOOST_ROOT "C:/local/boost_1_63_0/")
        SET(BOOST_LIBRARYDIR "C:/local/boost_1_63_0/lib64-msvc-14.0/")
    ENDIF()
ENDIF()
SET(Boost_DEBUG                OFF) #Switch this and next to ON for help debugging Boost problems.
SET(Boost_DETAILED_FAILURE_MSG OFF)
set(Boost_USE_STATIC_LIBS      ON)
set(Boost_USE_MULTITHREADED    ON)
set(Boost_USE_STATIC_RUNTIME   OFF)
IF(MSVC)
    find_package(Boost REQUIRED COMPONENTS filesystem system thread chrono system date_time atomic iostreams regex zlib)
    # Disable asio auto linking in date-time and regex
    add_definitions(-DBOOST_DATE_TIME_NO_LIB)
    add_definitions(-DBOOST_REGEX_NO_LIB)
    set(boost_plus_libs ${Boost_LIBRARIES})
ELSE()
    find_package(Boost REQUIRED COMPONENTS filesystem system thread chrono system date_time atomic iostreams regex)
    find_package(zlib)
    set(boost_plus_libs ${Boost_LIBRARIES} ${ZLIB_LIBRARIES})
ENDIF()

# LSL
set(LSL_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../../LSL/liblsl)
find_package(LSL REQUIRED)

 
###############
# APPLICATION #
###############

# Target name
set(target LabRecorder)

# Sources
set(sources
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    recording.h
    targetver.h
)

# Build executable
add_executable(${target}
    MACOSX_BUNDLE
    ${sources}
)

# Project options
set_target_properties(${target}
    PROPERTIES
    FOLDER "${IDE_FOLDER}"
)

target_include_directories(${target}
    PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${LSL_INCLUDE_DIRS})

target_link_libraries(${target}
    PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
    ${boost_plus_libs}
    ${LSL_LIBRARIES}
)


# 
# Deployment
# 

if(WIN32)
    get_target_property(QT5_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(QT5_WINDEPLOYQT_EXECUTABLE ${QT5_QMAKE_EXECUTABLE} PATH)
    set(QT5_WINDEPLOYQT_EXECUTABLE "${QT5_WINDEPLOYQT_EXECUTABLE}/windeployqt.exe")
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${QT5_WINDEPLOYQT_EXECUTABLE} --qmldir
            ${CMAKE_SOURCE_DIR}
            $<TARGET_FILE_DIR:${target}>)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${LSL_BINARY_RELEASE}
            $<TARGET_FILE_DIR:${target}>)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${LSL_BINARY_DEBUG}
            $<TARGET_FILE_DIR:${target}>)
endif(WIN32)
if(APPLE)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_LIST_DIR}/default_config.cfg"
            ${PROJECT_BINARY_DIR})
else()
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_LIST_DIR}/default_config.cfg"
            $<TARGET_FILE_DIR:${target}>)
endif()