# Overview #

The SerialPort application is part of the LSL distribution. The program reads a byte stream from a given COM port and emits it as an LSL stream. This program does not currently support sending a startup sequence to the device or any other kind of 2-way handshake.

# Usage #
  * Start the SerialPort app. You should see a window like the following.
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/serialport.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/serialport.png)

  * Make sure that your device is plugged in and that you know its COM port (you can usually check this in the Device Manage).

  * Select the COM port and the BAUD rate (data rate). If you know the nominal sampling rate of your stream (in bytes per second), you can set this in the Stream Sampling Rate (0 means irregular/unknown).

  * The output data will be batched into chunks of the given Chunk Size -- you can reduce or increase this setting according to your application's needs. The remaining settings can often be left untouched, except for devices with special requirements.

  * Click the "Link" button. If all goes well you should now have a stream on your lab network that has the name that you entered under Stream Name and type "Raw". Note that you cannot close the app while it is linked.

  * For subsequent uses you can save the settings in the GUI via File / Save Configuration. If the app is frequently used with different settings you might can also make a shortcut on the desktop that points to the app and appends to the Target field the snippet `-c name_of_config.cfg`, as in the following screen shot (here shown for another app):
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/cogionics-shortcut.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/cogionics-shortcut.png)