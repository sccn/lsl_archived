#!/bin/bash

# define the variables we need
# project name
PROJECT="LabRecorder"

# location of qt4/bin on this computer
QMAKEPATH="/lib64/qt4/bin"

# add extra paths and libraries here
# these should be relative to the directory ../ (the one above this one
includepath="../../LSL/liblsl/include/ /usr/include/QtNetwork /usr/include/QtCore /usr/include/QtGui"
libpath="-L./linux -L./"
libs="-llsl64 -lboost_thread -lboost_chrono -lboost_system -lboost_iostreams -lboost_filesystem -lQtGui -lQtNetwork -lQtCore"

#go up to the project root
cd ../

# updating automatically generated files
echo "updating automatically generated files"
$QMAKEPATH/uic mainwindow.ui -o ui_mainwindow.h
echo "$QMAKEPATH/moc mainwindow.h -o moc_mainwindow.cpp"
$QMAKEPATH/moc -o moc_mainwindow.cpp mainwindow.h

#invoke qmake to make a project file
$QMAKEPATH/qmake -project

#modify the .pro file with our custom settings from above
echo 'INCLUDEPATH +=' $includepath >> $PROJECT.pro 
echo 'LIBS +=' $libpath $libs >> $PROJECT.pro 
echo 'CXXFLAGS += -std=c++11' >> $PROJECT.pro
#generate the makefile
$QMAKEPATH/qmake

#make it
make

#cleanup
cd linux
rm ../*.o
rm ../*.pro
rm ../Makefile
mv ../$PROJECT ./

# copy the config file to this directory
cp ../default_config.cfg ./

