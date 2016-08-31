#-------------------------------------------------
#
# Project created by QtCreator 2012-04-25T03:50:34
#
#-------------------------------------------------

QT       += core gui

TARGET = BioSemi
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    biosemi_io.cpp

HEADERS  += mainwindow.h \
    biosemi_io.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../LSL/liblsl/bin/ -lliblsl32
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../LSL/liblsl/bin/ -lliblsl32-debug
else:symbian: LIBS += -lliblsl32
else:unix: LIBS += -L$$PWD/../../LSL/liblsl/bin/ -lliblsl32

INCLUDEPATH += $$PWD/../../LSL/liblsl/include \
    $$PWD/../../LSL/liblsl/external/
DEPENDPATH += $$PWD/../../LSL/liblsl/include

OTHER_FILES += \
    label_systems.txt \
    biosemi_config.cfg

symbian: LIBS += -llibboost_thread-vc90-mt-gd-1_47
else:unix|win32: LIBS += -L$$PWD/../../../boost_1_47/lib/ -llibboost_thread-vc90-mt-gd-1_47

INCLUDEPATH += $$PWD/../../../boost_1_47
DEPENDPATH += $$PWD/../../../boost_1_47
