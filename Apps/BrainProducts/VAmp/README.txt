== Usage ==

1. Make sure that you have correctly installed the drivers for your amplifier, and that the amplifier is plugged in and ready to use (see also official brochure).

2. If you have multiple amplifiers plugged in, make sure that you pick the correct one under Device Number (0 is the first one according to USB port numbering). Select the number of channels that you want to record from. The maximum number of channels is determined by the number of modules that are installed in your amplifier. Channels always count from the first module (the one closest to the front of the box). Enter the channel labels according to your cap design; make sure that the number of channel labels matches the selected number of channels.

3. For most EEG experiments you can ignore the Chunk Size setting, but if you are developing a latency-critical real-time application (e.g., a P300 speller BCI), you can lower this setting to reduce the latency of your system.

4. Use whatever sampling rate is appropriate for your experiment; you can use a lower rate if necessary to save network bandwidth (when transmitting on a slow network) or to reduce the disk space taken up by your recordings. If your analysis requires data to be unfiltered, particularly when you perform causal analysis (e.g., effective connectivity), you should strongly consider using a native sampling rate (e.g., 10KHz).

5. If you use AUX channels, check the according box and append 8 channel labels at the end of the channel list (even if you only use a subset of them).

6. Click the "Link" button. If all goes well you should now have a stream on your lab network that has name "ActiChamp-0" (if you used device 0) and type "EEG", and a second one named "ActiChamp-0-Markers" with type "Markers" that holds the event markers. Note that you cannot close the app while it is linked.

== Optional ==

The configuration settings can be saved to a .cfg file (see File / Save Configuration) and subsequently loaded from such a file (via File / Load Configuration). Importantly, the program can be started with a command-line argument of the form "ActiChamp.exe -c myconfig.cfg", which allows to load the config automatically at start-up. The recommended procedure to use the app in production experiments is to make a shortcut on the experimenter's desktop which points to a previously saved configuration customized to the study being recorded to minimize the chance of operator error.