# Overview #

The AmpServer application is included with the full LSL distribution. This program should include any amplifier that works with the AmpServer produced by EGI (http://www.egi.com/), however the program is currently untested. If you find that it works, please let us know so we can update the wiki.

# Usage #

  * Make sure that your AmpServer is running and can correctly record from its connected amplifier(s). To connect to the Amp Server you need to purchase the Amp Server Pro SDK, see: [ftp://ftp.egi.com/pub/documentation/placards/AS_guide_8409503-50_20100421.pdf](ftp://ftp.egi.com/pub/documentation/placards/AS_guide_8409503-50_20100421.pdf), otherwise the LSL application will not work.

  * Start the EGIAmpServer app. You should see a window like the following.
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/egiampserver.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/egiampserver.png)

  * Make sure that you have the correct IP address of the AmpServer assigned. The ports correspond to the default settings of the server and should not require a change.

  * If you have multiple amplifiers connected to the AmpServer and you would like to record from a specific one, you need to set the correct amplifier ID (these should be increasing from zero). Also make sure that you are using a supported number of channels and a supported sampling rate (the defaults should work).

  * To link the application to the LSL, click the "Link" button. If all goes well you should now have a new stream on the network with name "EGI NetAmp k" (k corresponding to the index of the amplifier) and type "EEG". If you get an error you might try to manually power on the desired Amp and try to link while it is either recording or stopped.

  * For subsequent uses you can save the desired settings from the GUI via File / Save Configuration. If the app is frequently used with different settings you might make a shortcut on the desktop that points to the app and appends to the Target field of the shortcut the snippet `-c name_of_config.cfg` to denote the name of the config file that should be loaded at startup.

# Acknowledgements #
This application was written to behave near-identically to the BCI2000 AmpServer module that was originally created by EGI.