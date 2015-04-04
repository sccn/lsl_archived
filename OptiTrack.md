# Overview #

The OptiTrack application is included with the full LSL distribution. There are two versions of this program, one for Arena 2.x and one for Arena 1.x by NaturalPoint (http://www.naturalpoint.com/).

# Usage #

  * Start Arena and make sure that you are recording, you have a scene set up, data streaming is enabled, and that you are streaming the desired items (marker sets, rigid bodies, etc.).

  * Start the OptiTrack app for your version of Arena. The only difference between the two apps is that in version 1.4 there is an extra field for the IP address of the computer that runs this app. You should see a window like the following:
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/optitrack14.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/optitrack14.png)     ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/optitrack22.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/optitrack22.png)

  * Set the correct address of the Arena server. Under some circumstances you may be able to leave the field blank (but you will need to test this). For 1.4, if you are running on a computer other than the one that hosts Arena, put in the IP address of your computer; note that you may have multiple IPs if you have multiple networks or network cards attached -- you need the one that is visible to Arena.

  * Select the type of camera that you are using. This determines the frame rate at which you are recording, among other things. Contact us if your camera is not in the list.

  * To link the application to the LSL, click the "Link" button. If all goes well you should now have a new stream on the network with name "OptiTrack" and type "Mocap".

  * For subsequent uses you can save the desired settings from the GUI via File / Save Configuration. If the app is frequently used with different settings you might make a shortcut on the desktop that points to the app and appends to the Target field of the shortcut the snippet `-c name_of_config.cfg` to denote the name of the config file that should be loaded at startup.

# Further Info #
The meta-data associated with OptiTrack streams is fairly comprehensive and describes how the various channels are related to scene objects (e.g. how are the rigid bodies arranged into skeletons, etc.) -- for up-to-date information, study the meta-data that is included in your stream (e.g. after importing a test file).