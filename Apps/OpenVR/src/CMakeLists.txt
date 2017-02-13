# Collect sources, headers, and libs

SET(LSLOpenVR_SRC)
SET(LSLOpenVR_INCL_DIRS)
SET(LSLOpenVR_REQ_LIBS)

# Sources
LIST(APPEND LSLOpenVR_SRC
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mainwindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mainwindow.h
    ${CMAKE_CURRENT_LIST_DIR}/mainwindow.ui
    ${CMAKE_CURRENT_LIST_DIR}/openvrthread.h
    ${CMAKE_CURRENT_LIST_DIR}/openvrthread.cpp
)

LIST(APPEND LSL_OPENVR_INCL_DIRS
    ${LSL_INCLUDE_DIRS}
    ${OpenVR_INCLUDE_DIRS}
)

LIST(APPEND LSLOPENVR_REQ_LIBS
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Xml
    ${LSL_LIBRARIES}
    ${OpenVR_LIBRARIES}
)

##########
# TARGET #
##########

SET(target LSLOpenVR)

add_executable(${target}
    MACOSX_BUNDLE
    ${LSLOpenVR_SRC}
)

target_include_directories(${target}
    PRIVATE
    ${LSL_OPENVR_INCL_DIRS})

target_link_libraries(${target}
    PRIVATE
    ${LSLOPENVR_REQ_LIBS}
)

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
add_custom_command(TARGET ${target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${OpenVR_BINARIES}
        $<TARGET_FILE_DIR:${target}>)
if(APPLE)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_LIST_DIR}/../openvr_config.cfg"
            ${PROJECT_BINARY_DIR})
else()
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_LIST_DIR}/../openvr_config.cfg"
            $<TARGET_FILE_DIR:${target}>)
endif()
