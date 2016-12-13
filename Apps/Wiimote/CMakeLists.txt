# CMake version
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
include(ExternalProject)

# Include cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(cmake/Custom.cmake)

# Set policies
set_policy(CMP0028 NEW) # ENABLE CMP0028: Double colon in target name means ALIAS or IMPORTED target.
set_policy(CMP0054 NEW) # ENABLE CMP0054: Only interpret if() arguments as variables or keywords when unquoted.
set_policy(CMP0042 NEW) # ENABLE CMP0042: MACOSX_RPATH is enabled by default.
set_policy(CMP0063 NEW) # ENABLE CMP0063: Honor visibility properties for all target types.

# Meta information about the project
set(META_PROJECT_NAME        "Wiimote")
set(META_PROJECT_DESCRIPTION "Send Wii data streams (Wiimote, balanceboard, etc) to stream.")
set(META_AUTHOR_ORGANIZATION "?")
set(META_AUTHOR_DOMAIN       "https://github.com/sccn/labstreaminglayer/tree/master/Apps/Wiimote")
set(META_AUTHOR_MAINTAINER   "Chadwick Boulay -- chadwick<dot>boulay<at>gmail<dot>com")
set(META_VERSION_MAJOR       "0")
set(META_VERSION_MINOR       "1")
set(META_VERSION_PATCH       "0")
set(META_VERSION             "${META_VERSION_MAJOR}.${META_VERSION_MINOR}.${META_VERSION_PATCH}")
set(META_NAME_VERSION        "${META_PROJECT_NAME} v${META_VERSION}")

# Project options
option(OPTION_SELF_CONTAINED "Create a self-contained install with all dependencies." OFF)

# Generate folders for IDE targets (e.g., VisualStudio solutions)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(IDE_FOLDER "")

# Declare project
project(${META_PROJECT_NAME} C CXX)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# Compiler settings and options
include(cmake/CompileOptions.cmake)
 
 
## Deployment/installation setup 

# Get project name
set(project ${META_PROJECT_NAME})

# Check for system dir install
set(SYSTEM_DIR_INSTALL FALSE)
if("${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr" OR "${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr/local")
    set(SYSTEM_DIR_INSTALL TRUE)
endif()

# Installation paths
if(UNIX AND SYSTEM_DIR_INSTALL)
    # Install into the system (/usr/bin or /usr/local/bin)
    set(INSTALL_ROOT      "share/${project}")       # /usr/[local]/share/<project>
    set(INSTALL_CMAKE     "share/${project}/cmake") # /usr/[local]/share/<project>/cmake
    set(INSTALL_EXAMPLES  "share/${project}")       # /usr/[local]/share/<project>
    set(INSTALL_DATA      "share/${project}")       # /usr/[local]/share/<project>
    set(INSTALL_BIN       "bin")                    # /usr/[local]/bin
    set(INSTALL_SHARED    "lib")                    # /usr/[local]/lib
    set(INSTALL_LIB       "lib")                    # /usr/[local]/lib
    set(INSTALL_INCLUDE   "include")                # /usr/[local]/include
    set(INSTALL_DOC       "share/doc/${project}")   # /usr/[local]/share/doc/<project>
    set(INSTALL_SHORTCUTS "share/applications")     # /usr/[local]/share/applications
    set(INSTALL_ICONS     "share/pixmaps")          # /usr/[local]/share/pixmaps
    set(INSTALL_INIT      "/etc/init")              # /etc/init (upstart init scripts)
else()
    # Install into local directory
    set(INSTALL_ROOT      ".")                      # ./
    set(INSTALL_CMAKE     "cmake")                  # ./cmake
    set(INSTALL_BIN       ".")                      # ./
endif()

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


## Target config

# External dependencies

set(bitness 32)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(bitness 64)  
endif()

#LSL
set(LSL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../LSL/liblsl)
set(libext .dylib)
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(libext .lib)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(libext .so)
endif()
if(MSVC)
    set(liblsl ${LSL_DIR}/bin/liblsl${bitness}${libext})
else()
    set(liblsl
        debug ${LSL_DIR}/bin/liblsl${bitness}-debug${libext}
        optimized ${LSL_DIR}/bin/liblsl${bitness}${libext})
endif()

# Qt
IF (NOT CMAKE_PREFIX_PATH)
    IF (MSVC)
        IF (CMAKE_SIZEOF_VOID_P EQUAL 8)
            SET(CMAKE_PREFIX_PATH "C:\\Qt\\5.7\\msvc2015_64")
        ELSE()
            SET(CMAKE_PREFIX_PATH "C:\\Qt\\5.7\\msvc2015")
        ENDIF()
    ELSEIF(APPLE)
        SET(CMAKE_PREFIX_PATH "/usr/local/Cellar/qt5/5.7.0")
    ENDIF()
ENDIF()
find_package(Qt5Core)
find_package(Qt5Network)
find_package(Qt5Gui)
find_package(Qt5Widgets)
set(CMAKE_INCLUDE_CURRENT_DIR ON)  # Because the ui_mainwindow.h file.
# Enable automoc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(AUTOMOC_MOC_OPTIONS PROPERTIES FOLDER CMakeAutomocTargets)
set_property(GLOBAL PROPERTY AUTOMOC_FOLDER CMakeAutomocTargets)
# ENABLE CMP0020: Automatically link Qt executables to qtmain target on Windows.
set_policy(CMP0020 NEW)

#Wiiuse
set(WIIUSE_INSTALL_DIR ${CMAKE_CURRENT_LIST_DIR}/deps/local)
ExternalProject_Add(wiiuse
    PREFIX ${CMAKE_CURRENT_LIST_DIR}/deps/wiiuse
    INSTALL_DIR ${WIIUSE_INSTALL_DIR}
    GIT_REPOSITORY https://github.com/cboulay/wiiuse.git
    GIT_TAG updates
    GIT_SHALLOW 1
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DCMAKE_INCLUDE_PATH=${WIIUSE_INSTALL_DIR}/include
        -DCMAKE_LIBRARY_PATH=${WIIUSE_INSTALL_DIR}/lib
        -DBUILD_WIIUSE_SHARED_LIB:BOOL=OFF
        -DBUILD_EXAMPLE:BOOL=OFF
        -DBUILD_EXAMPLE_SDL:BOOL=OFF
)
IF(APPLE)
    set(WIIUSE_LIB ${WIIUSE_INSTALL_DIR}/lib/libwiiuse.a)
ELSEIF(WIN32)
    add_definitions(-DWIIUSE_STATIC)
    set(WIIUSE_LIB
        debug ${WIIUSE_INSTALL_DIR}/lib/wiiuse_debug.lib
        optimized ${WIIUSE_INSTALL_DIR}/lib/wiiuse.lib)
ENDIF()
# Platform libraries required by Wiiuse
set(PLATFORM_LIBS)
if(APPLE)
    find_library(IOBLUETOOTH_FRAMEWORK
        NAMES
        IOBluetooth)
    find_library(COREFOUNDATION_FRAMEWORK
        NAMES
        CoreFoundation)
    find_library(FOUNDATION_FRAMEWORK
        NAMES
        Foundation)
    list(APPEND PLATFORM_LIBS ${IOBLUETOOTH_FRAMEWORK} ${COREFOUNDATION_FRAMEWORK} ${FOUNDATION_FRAMEWORK})
else(WIN32)
    find_package(WinHID REQUIRED)
    include_directories(${WINHID_INCLUDE_DIRS})
    list(APPEND PLATFORM_LIBS ws2_32 setupapi ${WINHID_LIBRARIES})
endif(APPLE)


## Executable name and options

# Target name
set(target Wiimote)

# Exit here if required dependencies are not met
# TODO: Check more dependencies.
if (NOT Qt5Core_FOUND)
    message(STATUS "${target} skipped: Qt5 not found")
    return()
else()
    message(STATUS "${target}")
endif()

# Sources
set(sources
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

# Build executable
add_executable(${target}
    MACOSX_BUNDLE
    ${sources}
)

# Project options

add_dependencies(${target} wiiuse)

set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
)

target_include_directories(${target}
    PRIVATE
    ${LSL_DIR}/include
    ${WIIUSE_INSTALL_DIR}/include)

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LIBRARIES}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
    ${liblsl}
    ${WIIUSE_LIB}
    ${PLATFORM_LIBS}
)

# 
# Compile definitions
# 

target_compile_definitions(${target}
    PRIVATE
    ${DEFAULT_COMPILE_DEFINITIONS}
)

# 
# Compile options
# 

target_compile_options(${target}
    PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)

# 
# Linker options
# 

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LINKER_OPTIONS}
)

# 
# Deployment
# 
if(WIN32)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${LSL_DIR}/bin/liblsl${bitness}.dll"
            $<TARGET_FILE_DIR:${target}>)
    get_target_property(QT5_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(QT5_WINDEPLOYQT_EXECUTABLE ${QT5_QMAKE_EXECUTABLE} PATH)
    set(QT5_WINDEPLOYQT_EXECUTABLE "${QT5_WINDEPLOYQT_EXECUTABLE}/windeployqt.exe")
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${QT5_WINDEPLOYQT_EXECUTABLE} --qmldir
            ${CMAKE_SOURCE_DIR}
            $<TARGET_FILE_DIR:${target}>)
endif(WIN32)

#install(TARGETS ${target}
#    RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT examples
#    BUNDLE  DESTINATION ${INSTALL_BIN} COMPONENT examples
#)
