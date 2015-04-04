# Overview #

The gUSBamp application is part of the LSL distribution. The program should support any version of the g.USBamp amplifier series.

# Usage #
  * Start the gUSBamp app. You should see a window like the following.
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/gUSBamp.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/gUSBamp.png)

  * Make sure that you have correctly installed the drivers for your amplifier, and that the amplifier is plugged in and ready to use (see also official brochure).

  * If you have only one device plugged in, you can leave the Device Port or Serial setting at its default value. i.e., "(search)". If you have multiple devices you can either enter the device USB port number (starting from 0) or, better, the serial number of the device, which is a string of the form UX-XXXX.XX.XX.

  * Select the desired number of channels to record, and enter your channel labels (make sure that the number of provided labels matches the number of channels setting).

  * Use whatever sampling rate is appropriate for your experiment; you can use a lower rate if necessary to save network bandwidth (when transmitting on a slow network) or to reduce the disk space taken up by your recordings. Some analyses will require higher sampling rates than the default (e.g., measuring auditory brainstem responses). If you choose a high sampling rate, you should also increase the chunk size (the official recommendation is to use a power of two that is closest to sampling rate / 32 (e.g., if your sampling rate is 9600, 9600/32=300, so you pick 256 as the chunk size). You can pick a lower chunk size within some reasonable tolerance if you need particularly low latency access to the data (e.g., for real-time P300 spelling applications).

  * The typical setting for Common Ground and Common Reference is to have them both checked -- unchecking Common Ground means that each block of 4 channels has its own ground contact, which can be placed anywhere on the subject(s), and the same holds for Common Reference.

  * The Act as Slave setting is currently untested -- in theory it allows you to synchronize the clocks of multiple devices in hardware, but it requires some precautions. You need to declare one device the "master", and check Act as Slave in all other devices. Also, the master must be the first to be linked, and the last to be unlinked. If this is not checked, the synchronization will be done in software based on time stamps.

  * Click the "Link" button. If all goes well you should now have a stream on your lab network that has name "gUSBamp-0" (if you used device 0) and type "EEG", and a second one named "gUSBamp-0-Markers" with type "Markers" that holds the event markers. Note that you cannot close the app while it is linked.

  * For subsequent uses you can save the settings in the GUI via File / Save Configuration. If the app is frequently used with different settings you might can also make a shortcut on the desktop that points to the app and appends to the Target field the snippet `-c name_of_config.cfg`, as in the following screen shot (here shown for another app):
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/cogionics-shortcut.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/cogionics-shortcut.png)