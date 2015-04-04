# Overview #

The AudioCaptureWin application comes with the full LSL distribution. This program should work out of the box on Windows Vista or later. It uses the native WASAPI audio interface of the operating system which (in theory) allows for accurate time-stamping of every collected sample. Note that the actual accuracy of the time stamps is up to the used sound card and card drivers, so it is recommended to use known-to-be-good hardware or at least test the ms-level synchronicity of the audio prior to relying on it too much. In the worst case the offset is not greater than the total latency of the audio chain (which may be between 30ms and 80ms depending on hardware). A timing test performed by a collaborator has shown an synchronization offset w.r.t. the BioSemi module of less than 5ms. The number of output channels can be one or two depending on the connected microphone.

# Usage #
Using this app is very simple:

  * Make sure that you have connected a microphone to your computer and that it is selected as the "default recording device" (you should be able to set this in your Control Panel).

  * Start the AudioCaptureWin app. You should see a window like the following.
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/audiocapturewin.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/audiocapturewin.png)

  * Click the "Link" button to link the app to the lab network. If successful, the button should turn into "Unlink". If a firewall complains, allow the app to connect to the network.

  * You should now have a stream on your lab network that has type "Audio" and its name is "AudioCaptureWin". Note that you cannot close the app while it is linked.


# Further Notes #
For Windows XP there is an older LSL audio recording app available on request; it uses the [irrKlang](http://www.ambiera.com/irrklang/) audio library, which in turn uses DirectX audio on Windows. That application does not support support accurate time synchronization and is therefore deprecated.