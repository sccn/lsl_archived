# Overview #

The Wiimote application comes with the full LSL distribution. This program should work with any Wiimote and any official expansion (Nunchuk, Classic Controller, Guitar Hero 3 Controller, Balance Board) by Nintendo (http://www.nintendo.com/). The program provides two streams, one for button-press events and another one for the position and motion sensors. The program uses Bluetooth to connect to the Wiimote, so your computer must have a working Bluetooth adapter (e.g., dongle). We recommend that you use Bluetooth hardware that is natively supported by your Windows, instead of installing a third-party Bluetooth stack (such as BlueSoleil, although it should work as well). In theory the program should work with multiple remotes simultaneously, although this has not yet been tested.

# Usage #

  * First you need to pair your Wiimote with the computer. For this, right-click the Bluetooth icon in the task bar and select "Add a Device". Press and release Buttons 1 and 2 of your Wiimote simultaneously, which makes the remote discoverable for ca. 20 seconds. Its four LEDs will flash periodically during this time and the device should show up in the list of discovered devices (the name is most likely "Nintendo RVL-CNT-01"). Add this device and click "pair without using a code". If you get an error while trying to do this it is most likely due to the 20-second timeout, so just retry.

  * Start the Wiimote app. If your Wiimote had been paired successfully the remote will emit a short hum and you will see a console window like the following.

> ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/wiimote-success.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/wiimote-success.png)

> Otherwise the app will keep prompting you to first pair the Wiimote and then confirm with Enter until the bluetooth connection has succeeded.

> ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/wiimote-retry.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/wiimote-retry.png)

  * Your app should now be linked to the lab network. If a firewall complains, allow the app to connect to the network.

  * You should now have two streams on your lab network, one with name "WiimoteMarkers0" and type "Markers" and another one with name "!wiimoteSensors0" and type "Mocap".


# Further Info #
The marker stream has a single channel that contains string-formatted samples (at irregular rate). The events are of the form "ButtonName pressed" and "ButtonName released". The sensors stream also has irregular rate (since the remote only sends updates when the values have actually changed) and the number of channels depends on the expansions that you have connected. Take a look at the stream meta-data to identify their meaning.

# Acknowledgements #
This program uses the Wiiuse library.