# Overview #

The BAlert application is included with the full LSL distribution. The program is currently untested by should work with the B-Alert X4, X10 and X24 systems produced by Advanced Brain Monitoring (http://www.b-alert.com/).


# Usage #
  * Make sure that the device is turned on an properly connected.

  * Start the BAlert app. You should see a window like the following.
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/balert.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/balert.png)

  * Click the "Link" button. If all goes well you should now have a stream on your lab network that has name "BAlert Xn" (n can be 4, 10 or 24) and type "EEG".

  * The filter setting can be saved in the config file for subsequent use via File / Save Configuration. It is also possible to make a desktop shortcut that appends to the shortcut's Target field the snippet `-c name_of_config.cfg`.