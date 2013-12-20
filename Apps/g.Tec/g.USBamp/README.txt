== Installation ==

Before you can use this program you must have installed the drivers for your g.USBamp. This program requires a 32-bit driver. If your program is unable to start and you get an error message about a dll file not being found before the GUI even comes up, but your driver is installed then the reason might be that your driver was for 64-bit, or is of a version older than 2.x or newer than 3.x. If your amp is either 2.x or 3.x, you might attempt to copy the correct file under misc/ into this folder and rename it to gUSBamp.dll, but there is no guarantee that this will work.

Note that this program has not yet been tested. 

== Optional ==

The configuration settings can be saved to a .cfg file (see File / Save Configuration) and subsequently loaded from such a file (via File / Load Configuration). Importantly, the program can be started with a command-line argument of the form "gUSBamp.exe -c myconfig.cfg", which allows to load the config automatically at start-up. The recommended procedure to use the app in production experiments is to make a shortcut on the experimenter's desktop which points to a previously saved configuration customized to the study being recorded to minimize the chance of operator error.