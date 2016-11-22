This is an updated version of the eyelink app for python 2.6. In order for this to work, python needs to be correctly configured so that pylsl is on the PYTHONPATH environment variable. 

There is some metadata and the eyelink timestamps is recorded as one of the channels. 

This program will also write the data using EyeLinks native EDF format. A good way to control this (in Windows) is to make a shortcut, right click and open the properties dialogue. For the target, specify the full path to python 2.6, then the path to the program to which your shortcut is linked, eg:

C:\Python26\python.exe C:\Users\ces\Desktop\EyeLink\eyelink.py

then in the Start in field, specify the directory into which you would like your EDF written. The file will be called TRIAL.edf, so you will have to move/rename it after each trial. Obviously, this system can be improved with more code, perhaps a GUI.