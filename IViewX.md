# Overview #

The iViewX application is included with the full LSL distribution. It should support all eye trackers that are compatible with the iViewX server application by SMI (http://www.smivision.com/), although it is currently untested.

# Usage #

  * Make sure that your iViewX server is running, has a calibration loaded and can track gaze. Make sure that streaming of gaze packets is enabled and take note of the ports that are configured for sending and receiving data.

  * Start the iViewX app. This should give you a window like the following:
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/iviewx.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/iviewx.png)

  * Set the correct IP address of the iViewX server application and make sure that you have the right ports assigned. The second address field should only be needed if you have multiple networks / network cards attached and you need a specific one to talk to the server.

  * To link the application to the LSL, click the "Link" button. If all goes well you should now have a new stream on the network with name "iViewX" and type "Gaze".

  * For subsequent uses you can save the desired settings from the GUI via File / Save Configuration. If the app is frequently used with different settings you might make a shortcut on the desktop that points to the app and appends to the Target field of the shortcut the snippet `-c name_of_config.cfg` to denote the name of the config file that should be loaded at startup.