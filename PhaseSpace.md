# Overview #

The PhaseSpace application is included with the full LSL distribution. It should support all motion capture systems produced by PhaseSpace (http://www.phasespace.com), including Impulse and Improv.

# Usage #

  * Make sure that the PhaseSpace server is running, and that also the PhaseSpace Master application is running, connected to the server, and tracking markers properly. The filtering settings that you set in the PhaseSpace master apply also to the streams recorded by this application, even though they are received directly from the server machine.

  * Start the PhaseSpace app. This should give you a window like the following:
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/phasespace.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/phasespace.png)

  * Set the correct address of the PhaseSpace server machine (this should be the same as the one that is set in the master program).

  * You may optionally assign names to your markers and rigid bodies (in the order in which they are defined) by filling in the respective list boxes (one line per item). For markers or rigids that were not labeled the names Marker1...MarkerN and Rigid1...RigidN will be assumed. You can also name the current marker setup for your own record-keeping.

  * To link the application to the LSL, click the "Link" button. If all goes well you should now have a new stream on the network with name "PhaseSpace" and type "Mocap".

  * For subsequent uses you can save the desired settings from the GUI via File / Save Configuration. If the app is frequently used with different settings you might make a shortcut on the desktop that points to the app and appends to the Target field of the shortcut the snippet `-c name_of_config.cfg` to denote the name of the config file that should be loaded at startup.

# Further Info #
The meta-data associated with PhaseSpace streams is fairly comprehensive and describes how the various channels are related to scene objects (e.g. what objects are the various position channels referring to, etc.) -- for up-to-date information, study the meta-data that is included in your stream (e.g. after importing a test file).

The One stream per rigid body checkbox is only for cases where you have a simple program that wants to track just one of your scene's rigid bodies without having to pick it out of the channel list. It is currently untested.