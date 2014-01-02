== Driver Installation ==

Before you can use this app, you need to obtain the iView X SDK from www.smivision.com (note that this program requires the "iView X SDK", not the newer "iViewNG SDK"; there is a different app for the new SDK). At the time of this writing, the download page is: http://www.smivision.com/en/gaze-and-eye-tracking-systems/support/software-download.html (alternatively you might try to google for iViewX SDK). Please make sure that your eye tracking model is supported by the SDK. Note the installation location and after you have installed the drivers, copy the contents of the bin/, include/ and lib/ folders into this folder. You should now be able to run the app!

== Usage ==

To use this app you need to set the IP addresses and ports correctly (see tool tips). The iViewX server must be running and you need to be able to connect to it (make sure that your firewall allows this). Click Link to link the iViewX signal to the lab streaming layer. This program has been reported to work with iViewX remote eye tracking devices (iViewX RED). We are not sure if the eye tracking glasses or other head-mounted trackers work with this application (we have heard of at least one user who could not seem to get them to work).

== Optional ==

The configuration settings can be saved to a .cfg file (see File / Save Configuration) and subsequently loaded from such a file (via File / Load Configuration). Importantly, the program can be started with a command-line argument of the form "iViewX.exe -c myconfig.cfg", which allows to load the config automatically at start-up. The recommended procedure to use the app in production experiments is to make a shortcut on the experimenter's desktop which points to a previously saved configuration customized to the study being recorded to minimize the chance of operator error.