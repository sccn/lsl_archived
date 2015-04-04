# Overview #

The BrainAmpSeries application is part of the LSL distribution. The program should support any version of the Brain Products BrainAmp amplifier series.

# Usage #
  * Start the BrainAmpSeries app. You should see a window like the following.
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/braimampseries.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/braimampseries.png)

  * Make sure that you have correctly installed the drivers for your amplifier, and that the amplifier is plugged in, turned on, and batteries are charged (see also official brochure).

  * If you have multiple amplifiers plugged in, make sure that you pick the correct one under Device Number (1 is the first one according to USB port numbering). Select the number of channels that you want to record from and enter the channel labels according to your cap design; make sure that the number of channel labels matches the selected number of channels.

  * For most EEG experiments you can ignore the Chunk Size setting, but if you are developing a latency-critical real-time application (e.g., a P300 speller BCI), you can lower this setting to reduce the latency of your system. Also, for most applications it is recommended to leave the Impedance Mode and DC coupling options at their defaults. Further information is found in the amplifier's manual (and/or the BrainVision recorder manual).

  * If you have strong noise sources or you observe clipping of your recorded signal, you can change the resolution setting to a coarser stepping.

  * If you use the PolyBox, check the according box and prepend 8 channel labels at the beginning of the channel list (even if you only use a subset of them). Note that the PolyBox is not the same as the EMG box or other accessories.

  * Click the "Link" button. If all goes well you should now have a stream on your lab network that has name "BrainAmpSeries-0" (if you used device 0) and type "EEG", and a second one named "BrainAmpSeries-0-Markers" with type "Markers" that holds the event markers. Note that you cannot close the app while it is linked.

  * For subsequent uses you can save the settings in the GUI via File / Save Configuration. If the app is frequently used with different settings you might can also make a shortcut on the desktop that points to the app and appends to the Target field the snippet `-c name_of_config.cfg`, as in the following screen shot (here shown for another app):
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/cogionics-shortcut.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/cogionics-shortcut.png)