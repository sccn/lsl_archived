#-------------------------------------------------
#
# Project created by QtCreator 2012-04-28T22:34:45
#
#-------------------------------------------------

QT       += core gui

TARGET = PhaseSpace
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += $$PWD/../../../../boost_1_47
DEPENDPATH += $$PWD/../../../../boost_1_47

INCLUDEPATH += $$PWD/../../LSL/liblsl/include
DEPENDPATH += $$PWD/../../LSL/liblsl/bin
