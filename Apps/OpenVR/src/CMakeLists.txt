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
    ${OpenVR_INCLUDE_DIRS}
)

LIST(APPEND LSLOPENVR_REQ_LIBS
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Xml
    LSL::lsl
    ${OpenVR_LIBRARIES}
)


# TARGET #

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


# Deployment #
 
installLSLApp(${target})  # Will handle Qt and LSL libraries
installLSLAuxFiles(${target} ${CMAKE_CURRENT_LIST_DIR}/../openvr_config.cfg)
installLSLAuxFiles(${target} ${OpenVR_BINARIES})
