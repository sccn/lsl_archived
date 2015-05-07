#!/bin/bash

#define the variables we need
PROJECT="XDFBrowser"

#this is the path to qmake -- sometimes it is /usr/bin
#or in /opt/local/bin -- which ever directory it is in, put it here:
QMAKEPATH="/usr/local/bin"

#add extra paths and libraries here
includepath="../../LSL/liblsl/include /usr/local/include"
libpath="-L./OSX -L/opt/local/lib -L/usr/local/lib -L./"
libs="-llsl64 -lboost_thread-mt -lboost_system-mt -lboost_chrono-mt -lzmq"

echo "adjusting runtime library paths"

#do the same for the local copy of liblsl 
install_name_tool -id "@executable_path/../Resources/liblsl64.dylib" ./liblsl64.dylib
install_name_tool -id "@executable_path/../Resources/liblsl32.dylib" ./liblsl32.dylib

#check if the app is already build
if [ -e $PROJECT.app ] 
  then 
    rm -r $PROJECT.app
    echo "Removing Previous Build..."
fi 


#go up to the project root
echo "switching to parent directory"
cd ../

# updating automatically generated files
echo "updating automatically generated files"
$QMAKEPATH/uic mainwindow.ui -o ui_mainwindow.h
$QMAKEPATH/moc mainwindow.h -o moc_mainwindow.cpp

#there might be a dangling app here as well, if so remove it
#check if the app is already build
if [ -e $PROJEC).app ] 
  then 
    rm -r $PROJECT.app
    echo "Removing Previous Build..."
fi 


#invoke qmake to make a project file
echo "creating makefile"
$QMAKEPATH/qmake -project

#shove in our library includes
echo 'INCLUDEPATH +=' $includepath >> $PROJECT.pro 
echo 'LIBS +=' $libpath $libs >> $PROJECT.pro 

#generate the makefile
$QMAKEPATH/qmake -spec macx-g++ $PROJECT.pro

#make it
echo
echo "building app..."
make
cd OSX




echo "cleaning up..."
#cleanup
rm ../*.o
rm ../Makefile
mv ../$PROJECT.app ./
#copy the libraries into the resources folder
cp ./liblsl64.dylib $PROJECT.app/Contents/Resources/
cp ./liblsl32.dylib $PROJECT.app/Contents/Resources/


echo "done"

