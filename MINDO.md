# Overview #

The MINDO application is included with the full LSL distribution. The program should support the current generation of dry/wireless EEG systems by MINDO (http://mindo.com.tw) that use the Bluetooth serial port profile.

# Usage #
  * Make sure that the headset is powered on and paired with the computer (pairing code 0000). If necessary, please consult your operating system documentation on how to pair a device with your computer (on Windows, it essentially amounts to right-clicking the Bluetooth icon in the systray, selecting "Add Device...", picking the MINDO or "SPP Slave" device, selecting the "pair with a pairing code" option, typing in 0000, and waiting until the device is successfully added. As the device is getting paired, it is being assigned a COM port (a.k.a. serial port) number by your OS. On Windows, you see the assigned port number as a pop-up notification in your taskbar (saying something along the lines of "COM3" or some other number). You can also look it up in the device properties after it has been added.

  * Start the MINDO app. You should see a window like the following.
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/MINDO.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/MINDO.png)

  * Make sure that you have the right COM port (you should see this in your Control Panel / System / Device Manager / Ports / COMx system setting), the correct number of channels, and a supported sampling rate set up (the default should be fine). The number of channels in the channels list must match the numeric value (but you can use dummy names for a quick test). If you are unsure about the protocol version, you most likely have Version 1 (the default). The bit depth and gain can also be left at their defaults.

  * Click the "Link" button. If all goes well you should now have a stream on your lab network that has name "MINDO" and type "EEG". Note that you cannot close the app while it is linked.

  * For subsequent uses you can save the settings in the GUI via File / Save Configuration. If the app is frequently used with different settings you might can also make a shortcut on the desktop that points to the app and appends to the Target field the snippet `-c name_of_config.cfg`, as in the following screen shot:
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/MINDO-shortcut.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/MINDO-shortcut.png)