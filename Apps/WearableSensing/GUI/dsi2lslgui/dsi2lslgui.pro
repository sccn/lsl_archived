#-------------------------------------------------
#
# Project created by QtCreator 2016-11-07T10:39:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dsi2lslgui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    DSI_API_Loader.c

HEADERS  += mainwindow.h \
    DSI.h \
    lsl_cpp.h

FORMS    += mainwindow.ui
LIBS        += -ldl -L "/home/ale/Projects/dsi2lsl/GUI/build-dsi2lslgui-Desktop-Debug/" -llsl
INCLUDEPATH += "/home/ale/Projects/dsi2lsl/GUI/build-dsi2lslgui-Desktop-Debug/"
DEPENDPATH  += "/home/ale/Projects/dsi2lsl/GUI/build-dsi2lslgui-Desktop-Debug/"

LIBS        += -ldl -L "/home/ale/Projects/dsi2lsl/GUI/build-dsi2lslgui-Desktop-Debug/" -lDSI-Linux-x86_64
INCLUDEPATH += "/home/ale/Projects/dsi2lsl/GUI/build-dsi2lslgui-Desktop-Debug/"
DEPENDPATH  += "/home/ale/Projects/dsi2lsl/GUI/build-dsi2lslgui-Desktop-Debug/"
