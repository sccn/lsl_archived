# Overview #

The BrainVisionRDA application is part of the LSL distribution (original version kindly provided by Brain Products). The program should be able to connect to any version of the Brain Products BrainVision recorder software.

# Usage #
  * Start the BrainVisionRDA app. You should see a window like the following.
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/brainvisionrda.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/brainvisionrda.png)

  * Make sure that your BrainVision recorder is started, and that remote data access (RDA) is enabled in the settings/preferences (see manual).

  * If your BrainVision recorder is running on the same machine as this program, there is no need to change the RDA Server Address -- otherwise you need to enter the IP address of the respective machine.

  * Click the "Link" button. If all goes well you should now have a stream on your lab network that has name "BrainVision RDA" and type "EEG", and another stream called "BrainVision RDA Markers" of type "Markers" which holds even markers. Note that you cannot close the app while it is linked.

  * For subsequent uses you can save the settings in the GUI via File / Save Configuration. If the app is frequently used with different settings you might can also make a shortcut on the desktop that points to the app and appends to the Target field the snippet `-c name_of_config.cfg`, as in the following screen shot (here shown for another app):
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/cogionics-shortcut.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/cogionics-shortcut.png)