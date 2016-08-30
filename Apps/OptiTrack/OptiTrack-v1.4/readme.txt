== Usage ==

The OptiTrack connector should run out of the box without further dependencies. To operate it, an OptiTrack server program (e.g. Arena) must be running and streaming data.
This program connects (upon clicking link) to the given IP address of the OptiTrack server.
WARNING: This program has not yet been tested with real data (for lack of actual hardware) -- please review the collected data and meta-data before doing production experiments with it.

== Optional ==

The configuration settings can be saved to a .cfg file (see File / Save Configuration) and subsequently loaded from such a file (via File / Load Configuration). Importantly, the OptiTrack program can be started with a command-line argument of the form "OptiTrack.exe -c myconfig.cfg", which allows to load the config automatically at start-up. The recommended procedure to use the app in production experiments is to make a shortcut on the experimenter's desktop which points to a previously saved configuration customized to the study being recorded to minimize the chance of operator error.