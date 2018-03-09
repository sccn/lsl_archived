#-------------------------------------------------
#
# Project created by QtCreator 2018-03-05T13:52:27
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NeuroScanAcquireLSL
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS QT_NO_DEBUG_OUTPUT

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11


SOURCES += \
        main.cpp \
        widget.cpp \
    nsatransporter.cpp

HEADERS += \
        widget.h \
    NeuroscanAcquireProtocol.h \
    nsatransporter.h

LIBS += \
        -llsl64

FORMS += \
        widget.ui



INCLUDEPATH += /home/bot/Documents/labstreaminglayer/build/install/lsl_Release/LSL/include
LIBS += -L/home/bot/Documents/labstreaminglayer/build/install/lsl_Release/LSL/lib
