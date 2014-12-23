#-------------------------------------------------
#
# Project created by QtCreator 2012-04-28T00:14:43
#
#-------------------------------------------------

QT       += core gui

TARGET = ClickMe
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/mac_libs/release/ -llsl64
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/mac_libs/debug/ -llsl64
else:unix: LIBS += -L$$PWD/mac_libs/ -llsl64

INCLUDEPATH += $$PWD/../../LSL/liblsl/include
DEPENDPATH += $$PWD/../../LSL/liblsl/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../opt/local/lib/release/ -lboost_system-mt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../opt/local/lib/debug/ -lboost_system-mt
else:unix: LIBS += -L$$PWD/../../../../../../opt/local/lib/ -lboost_system-mt

INCLUDEPATH += $$PWD/../../../../../../opt/local/include
DEPENDPATH += $$PWD/../../../../../../opt/local/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../opt/local/lib/release/libboost_system-mt.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../opt/local/lib/debug/libboost_system-mt.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../opt/local/lib/release/boost_system-mt.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../opt/local/lib/debug/boost_system-mt.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../../../opt/local/lib/libboost_system-mt.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../opt/local/lib/release/ -lboost_thread-mt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../opt/local/lib/debug/ -lboost_thread-mt
else:unix: LIBS += -L$$PWD/../../../../../../opt/local/lib/ -lboost_thread-mt

INCLUDEPATH += $$PWD/../../../../../../opt/local/include
DEPENDPATH += $$PWD/../../../../../../opt/local/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../opt/local/lib/release/libboost_thread-mt.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../opt/local/lib/debug/libboost_thread-mt.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../opt/local/lib/release/boost_thread-mt.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../opt/local/lib/debug/boost_thread-mt.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../../../opt/local/lib/libboost_thread-mt.a
