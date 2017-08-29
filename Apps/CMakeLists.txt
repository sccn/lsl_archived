cmake_minimum_required(VERSION 3.5)

# Minimum C++ standard, apps can request more features with
# target_compile_features()
set(CMAKE_CXX_STANDARD 11)

# when building out of tree LSL_ROOT needs to be specified on the cmd line
if(NOT LSL_ROOT)
    message(FATAL_ERROR "When building out of tree you need to set LSL_ROOT ( cmake -DLSL_ROOT=/path/to/lsl/installation)")
else()
    list(APPEND CMAKE_MODULE_PATH "${LSL_ROOT}/cmake")
    include(LSLAppBoilerplate)
endif()

option(LSLAPPS_Examples         "Build example apps"                OFF)
option(LSLAPPS_LabRecorder      "Build the recorder app"            ON)
option(LSLAPPS_XDFBrowser       "Build the XDFBrowser app"          ON)
option(LSLAPPS_Wiimote          "Build the Wiimote app"             OFF)

# Windows-only apps
if(WIN32)
    option(LSLAPPS_BrainAmp     "Build the BrainAmp app"            OFF)
    option(LSLAPPS_Emotiv       "Build the Emotiv app"              OFF)
    option(LSLAPPS_gNEEDaccess  "Build the g.Tec NEEDaccess app"    OFF)
endif()

# add all Apps the user wants to build

if(LSLAPPS_Examples)
    add_subdirectory(Examples)
endif()

if(LSLAPPS_LabRecorder)
    add_subdirectory(LabRecorder)
endif()

if(LSLAPPS_XDFBrowser)
    add_subdirectory(XDFBrowser)
endif()

if(LSLAPPS_Wiimote)
    add_subdirectory(Wiimote)
endif()

if(LSLAPPS_BrainAmp)
    add_subdirectory(BrainProducts/BrainAmpSeries)
    #add_subdirectory(BrainProducts/BrainAmpImpedance)
endif()

if(LSLAPPS_Emotiv)
    add_subdirectory(emotiv)
endif()

if(LSLAPPS_gNEEDaccess)
    add_subdirectory(g.Tec/g.NEEDaccess)
endif()