cmake_minimum_required(VERSION 3.0)
project(LSLOpenVR VERSION 0.0.1)

##################
# GENERAL CONFIG #
##################

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(ExternalProject)
# Set policies
cmake_policy(SET CMP0028 NEW) # ENABLE CMP0028: Double colon in target name means ALIAS or IMPORTED target.
cmake_policy(SET CMP0054 NEW) # ENABLE CMP0054: Only interpret if() arguments as variables or keywords when unquoted.
cmake_policy(SET CMP0042 NEW) # ENABLE CMP0042: MACOSX_RPATH is enabled by default.
cmake_policy(SET CMP0063 NEW) # ENABLE CMP0063: Honor visibility properties for all target types.
# Generate folders for IDE targets (e.g., VisualStudio solutions)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(IDE_FOLDER "")
#
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
# Set runtime path
set(CMAKE_SKIP_BUILD_RPATH            FALSE) # Add absolute path to all dependencies for BUILD
set(CMAKE_BUILD_WITH_INSTALL_RPATH    FALSE) # Use CMAKE_INSTALL_RPATH for INSTALL
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE) # Do NOT add path to dependencies for INSTALL
# Runtime should find binaries relative to itself
if(APPLE)
    set(CMAKE_INSTALL_RPATH "@loader_path/../../../${INSTALL_LIB}")
else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN/${INSTALL_LIB}")       
endif()

#########################
# THIRD PARTY LIBRARIES #
#########################

# LSL
set(LSL_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../../LSL/liblsl)
find_package(LSL REQUIRED)

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

# OpenVR
IF(NOT OpenVR_ROOT_DIR)
    message(STATUS "OpenVR_ROOT_DIR not set.")
    message("\tUse cmake [...] -DOpenVR_ROOT_DIR=<path/to/openvr>")
    message("\tAttempting Chad's default rel directory...")
    SET(OpenVR_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../VR/openvr")
ENDIF()
find_package(OpenVR)
IF(NOT OpenVR_FOUND)
    message(STATUS "OpenVR not found. It will be downloaded from GitHub during configure. This can be very slow.")
    ExternalProject_Add(openvr
        PREFIX ${CMAKE_CURRENT_LIST_DIR}/deps/openvr
        DOWNLOAD_NO_PROGRESS 0  # Do not disable download progress.
        GIT_REPOSITORY https://github.com/ValveSoftware/openvr.git
        GIT_SHALLOW 1
        BUILD_COMMAND "" # Disable build step.
        INSTALL_COMMAND "" # Disable install step.
    )
    ExternalProject_Get_Property(openvr SOURCE_DIR)
    # Cannot use find_package because the project is not downloaded yet.
    # Instead, assume file locations.
    set(SIZEOF_VOIDP ${CMAKE_SIZEOF_VOID_P})
    if((NOT APPLE) AND (CMAKE_SIZEOF_VOID_P EQUAL 8))
      set(PROCESSOR_ARCH "64")
    else()
      set(PROCESSOR_ARCH "32")
    endif()
    if(WIN32)
        set(PLATFORM_NAME "win")
    elseif(UNIX AND NOT APPLE)
      if(CMAKE_SYSTEM_NAME MATCHES ".*Linux")
        set(PLATFORM_NAME "linux")
      endif()
    elseif(APPLE)
      if(CMAKE_SYSTEM_NAME MATCHES ".*Darwin.*" OR CMAKE_SYSTEM_NAME MATCHES ".*MacOS.*")
        set(PLATFORM_NAME "osx")
      endif()
    endif()
    SET(OpenVR_INCLUDE_DIRS ${SOURCE_DIR}/headers)
    SET(OpenVR_BINARIES ${SOURCE_DIR}/bin/${PLATFORM_NAME}${PROCESSOR_ARCH}/${CMAKE_SHARED_LIBRARY_PREFIX}openvr_api${CMAKE_SHARED_LIBRARY_SUFFIX})
    SET(OpenVR_LIBRARIES ${SOURCE_DIR}/lib/${PLATFORM_NAME}${PROCESSOR_ARCH}/${CMAKE_SHARED_LIBRARY_PREFIX}openvr_api${CMAKE_IMPORT_LIBRARY_SUFFIX})
ENDIF()

###############
# APPLICATION #
###############
add_subdirectory(src) 
