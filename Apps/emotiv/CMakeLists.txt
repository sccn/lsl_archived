cmake_minimum_required(VERSION 3.0)
project(Emotiv VERSION 0.0.1)

##################
# GENERAL CONFIG #
##################

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

#########################
# THIRD PARTY LIBRARIES #
#########################

# LSL
set(LSL_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../../LSL/liblsl)
find_package(LSL REQUIRED)

# Qt
if(NOT CMAKE_PREFIX_PATH)
    SET(CMAKE_PREFIX_PATH "C:\\Qt\\5.8\\msvc2015_64" CACHE PATH "Qt path. e.g., C:\\Qt\\5.8\\msvc2015_64\\")
endif()
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

# Boost
# Default location of pre-compiled Boost for Windows
# Override by adding "-DBOOST_ROOT=C:\path\to\boost\ -DBOOST_LIBRARYDIR=C:\path\to\boost\lib64-msvc-14.0\" to your cmake command
IF (NOT BOOST_ROOT)
    SET(BOOST_ROOT "C:/local/boost_1_64_0/")
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

###############
# APPLICATION #
###############

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
    ${LSL_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${EmotivSDK_INCLUDE_DIRS}
)

LIST(APPEND LSLEmotiv_REQ_LIBS
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Xml
    ${LSL_LIBRARIES}
    ${Boost_LIBRARIES}
    ${EmotivSDK_LIBRARIES}
)

##########
# TARGET #
##########

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

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
SET_TARGET_PROPERTIES(${target}
    PROPERTIES
        LINK_FLAGS "/NODEFAULTLIB:\"liblsl64\""
)
else()
SET_TARGET_PROPERTIES(${target}
    PROPERTIES
        LINK_FLAGS "/NODEFAULTLIB:\"liblsl32\""
)
endif()

# Deployment
# 

# Copy Qt5 binaries
get_target_property(QT5_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
get_filename_component(QT5_WINDEPLOYQT_EXECUTABLE ${QT5_QMAKE_EXECUTABLE} PATH)
set(QT5_WINDEPLOYQT_EXECUTABLE "${QT5_WINDEPLOYQT_EXECUTABLE}/windeployqt.exe")
add_custom_command(TARGET ${target} POST_BUILD
    COMMAND ${QT5_WINDEPLOYQT_EXECUTABLE} --qmldir
        ${CMAKE_SOURCE_DIR}
        $<TARGET_FILE_DIR:${target}>)
# Copy LSL binaries
add_custom_command(TARGET ${target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LSL_BINARY_RELEASE}
        $<TARGET_FILE_DIR:${target}>)
add_custom_command(TARGET ${target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LSL_BINARY_DEBUG}
        $<TARGET_FILE_DIR:${target}>)
# Copy EmotivSDK binaries.
add_custom_command(TARGET ${target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${EmotivSDK_BINARIES}
        $<TARGET_FILE_DIR:${target}>)
# Copy config file.
add_custom_command(TARGET ${target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_LIST_DIR}/emotiv_config.cfg
        $<TARGET_FILE_DIR:${target}>)