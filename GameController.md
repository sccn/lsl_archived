# Overview #

The GameController application comes with the full LSL distribution. This program should work out of the box from Windows Vista onward, and will run on older Windows versions if you have the DirectX 9.0c (or later) runtime installed (available from Microsoft at http://www.microsoft.com/en-us/download/details.aspx?id=35). The application should support any game controller (joysticks, steering wheels, gamepads) that is compatible with DirectX. The program provides two streams, one for button-press events and another one for the axis values.

# Usage #

  * Make sure that your controller is plugged in an recognized by Windows.

  * Start the GameController app. You should see a window like the following.
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/gamecontroller.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/gamecontroller.png)

  * Select the desired controller from the list (it should show any currently connected and supported controller).

  * Click the "Link" button to link the app to the lab network. If successful, the button should turn into "Unlink". If a firewall complains, allow the app to connect to the network.

  * You should now have two streams on your lab network, one with name "NameofyourdeviceButtons" and type "Markers" and another one with name "NameofyourdeviceAxes" and type "Position". Note that you cannot close the app while it is linked. The app by default streams all possible axes that a controller might have, although only those axes that are actually present in the device in will deliver non-zero values. The button events are of the form "ButtonN pressed" and "ButtonN released" where N ranges from 0 to the number of buttons on your device.

  * For subsequent uses you can save the desired settings from the GUI via File / Save Configuration. If you have multiple controllers connected and you frequently want to use the app with a specific one you might make a shortcut on the desktop that points to the app and appends to the Target field of the shortcut the snippet `-c name_of_config.cfg` to denote the name of the config file that should be loaded at startup.