== Usage ==

This program should run out of the box without further dependencies. To operate it, the PhaseSpace Master program must be running and connected, and PhaseSpace must have a configuration loaded.

The program connects (upon clicking link) to the given IP address of the PhaseSpace server.

== Optional ==

It is possible to assign custom names to the markers as well as rigid bodies provided by the PhaseSpace server. To do this, just type the names (one per line) in the order of appearance in the PhaseSpace Master. Any unassigned or blank lines will be taken as labels such as Marker1, Marker2, etc.

The setup itself can also be given a name.

The one stream per rigid body allows to export a separate stream for each body, which can make it a bit easier for a client program that is only interested in the movement of that body (e.g. for head tracking).

The configuration settings can be saved to a .cfg file (see File / Save Configuration) and subsequently loaded from such a file (via File / Load Configuration). Importantly, the PhaseSpace program can be started with a command-line argument of the form "PhaseSpace.exe -c myconfig.cfg", which allows to load the config automatically at start-up. The recommended procedure to use the app in production experiments is to make a shortcut on the experimenter's desktop which points to a previously saved configuration customized to the study being recorded to minimize the chance of operator error.