# Overview #

The Tobii application is included with the full LSL distribution. It should support all eye trackers made by Tobii Technology (http://www.tobii.com/), although it is currently untested.

# Usage #

  * Make sure that your eye tracker is running and can stream data. You need to have a calibration file (.cd file) created for your subject using an external application.

  * Start the Tobii app. This should give you a window like the following:
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/tobii.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/tobii.png)

  * If the Tobii server is running on another machine than this app, you need to set the correct IP address of that machine. The port should match the default settings of the Tobii server and only needs to be changed when the server is reconfigured. Type in the file name for your calibration file.

  * To link the application to the LSL, click the "Link" button. If all goes well you should now have a new stream on the network with name "Tobii" and type "Gaze".

  * For subsequent uses you can save the desired settings from the GUI via File / Save Configuration. If the app is frequently used with different settings you might make a shortcut on the desktop that points to the app and appends to the Target field of the shortcut the snippet `-c name_of_config.cfg` to denote the name of the config file that should be loaded at startup.