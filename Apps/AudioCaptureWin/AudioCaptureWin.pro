#-------------------------------------------------
#
# Project created by QtCreator 2012-04-28T00:14:43
#
#-------------------------------------------------

QT       += core gui

TARGET = AudioCaptureWin
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

symbian: LIBS += -lliblsl32-debug
else:unix|win32: LIBS += -L$$PWD/../../LSL/liblsl/bin/ -lliblsl32-debug

INCLUDEPATH += $$PWD/../../LSL/liblsl/bin
DEPENDPATH += $$PWD/../../LSL/liblsl/bin
