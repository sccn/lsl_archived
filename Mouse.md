# Overview #

The Mouse application comes with the full LSL distribution. This program should work out of the box on Windows 2000 or later. The program provides two streams, one for button-press events and another one for the cursor position. Keep in mind that this program must run on the stimulus presentation PC.

# Usage #

  * Start the Mouse app. You should see a window like the following.
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/mouse.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/mouse.png)

  * Click the "Link" button to link the app to the lab network. If successful, the button should turn into "Unlink". If a firewall complains, allow the app to connect to the network.

  * You should now have two streams on your lab network, one with name "MouseButtons" and type "Markers" and another one with name "MousePosition" and type "Position". Note that you cannot close the app while it is linked.


# Button Event Names #
The following list contains the names assigned to the various button codes. The events are of the form "ButtonName pressed" or "ButtonName released": MouseButtonLeft, MouseButtonRight, MouseButtonMiddle, MouseButtonX1, MouseButtonX2, MouseButtonX3, MouseButtonX4, MouseWheelUp, MouseWheelDown, MouseWheelLeft, MouseWheelRight