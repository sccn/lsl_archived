# Overview #

The Neuroscan application is included with the full LSL distribution. This program should work with both the Synamp II and Synamp Wireless product series from Neuroscan (http://www.neuroscan.com/). It still needs to be tested, however.

Before you can use this program you need to obtain and install Neuroscan's Access SDK and the associated license dongle.

# Usage #

  * Make sure that your amp is turned on and correctly connected.

  * Start the Neuroscan app. You should see a window like the following.
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/neuroscan.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/neuroscan.png)

  * If you have a Synamp Wireless or other amp connected via a network connection you need to type the correct IP address in. Also select the desired number of channels and sampling rate.

  * To link the application to the LSL, click the "Link" button. If all goes well you should now have a new stream on the network with name "Neuroscan" and type "EEG".

  * For subsequent uses you can save the desired settings from the GUI via File / Save Configuration. If the app is frequently used with different settings you might make a shortcut on the desktop that points to the app and appends to the Target field of the shortcut the snippet `-c name_of_config.cfg` to denote the name of the config file that should be loaded at startup, as in the following screenshot:


> ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/neuroscan-screenshot.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/neuroscan-screenshot.png)