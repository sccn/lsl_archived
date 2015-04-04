# Overview #

The MATLAB Viewer is a MATLAB function (vis\_stream) that is included with the full LSL distribution. It allows you to view the signal content of any stream on your lab network in real time. There is also an alternative viewer available that is written in C++ and does not require MATLAB (the StreamViewer).

# Usage #

  * Start MATLAB and add the MATLAB Viewer folder to the path, either through the GUI, under File / Set Path / Add Folder) or in the command line, by typing:

> `addpath('C:\\path\\to\\labstreaminglayer\\Apps\\MATLAB Viewer')`

  * Make sure that you have some stream online. For a quick test you might try the AudioCaptureWin app, or the example program SendData32.exe in labstreaminglayer/LSL/liblsl/bin.

  * Now type `vis_stream` to bring up the viewer GUI; it should give you a window that looks like the following. If you don't have a stream online it will finish with an error message.

> ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/visstream-gui.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/visstream-gui.png)

  * Select the stream that you want to display. If you cannot see a stream that should be running (e.g., on another computer), read the Network Troubleshooting section on the NetworkConnectivity page. For a quick test, you might click OK right away to bring up the real-time view, or you might configure some of the display parameters first. Upon clicking okay you will get a window like the following:

> ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/visstream-initial.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/visstream-initial.png)

> Most likely the graph scale is initially not quite right, so you probably want to resize the data by holding down either the cursor up (scale up) or cursor down (scale down) arrow. Once you have adapted the scales and window size your display will look closer to this:

> ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/visstream-adapted.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/visstream-adapted.png)

  * You can also adapt the time scale with the left/right cursor keys and if you have more channels than the ones that are displayed in one page, you can press page down or page up to flip through the channel pages. The number of channels per page and the initial range to display can be configured in the initial dialog.

  * The display updates in the background, so you can go ahead and open more viewports by calling vis\_stream again.


# GUI and Command-Line Options #
The function offers a series of options that can be assigned either in the GUI, or in the command-line. To see the documentation of the options click the Help button in the GUI, which should give you a window like the following:

![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/visstream-docs.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/visstream-docs.png)

The command-line options can be passed in like in the following example:

`vis_stream('StreamName','AudioCaptureWin', 'DataScale',0.01, 'SamplingRate',500)`

# Real-world Use #
You can have multiple viewports open in MATLAB, but if you have too many, your display will become sluggish or the dialog will take a long time to come up. This is because MATLAB will use only one core for all viewports; on a multicore machine you can open multiple MATLAB instances to view even more streams simultaneously.

If you add the folder to the path permanently (by clicking Save in MATLABs Set Path dialog), you can add a shortcut to vis\_stream in your MATLAB shotcuts bar (see documentation in add a shortcut), or on the Desktop by making a link to MATLAB and appending to the Target field of the shortcut the snippet `-r vis_stream`.

Since the function can be scripted via the command line, one can make a script that brings up all viewports that are needed to monitor a particular type of experiment with all initial parameters (including positions) pre-configured.

The MATLAB function is relatively simple and so can be customized quite easily.

# Caveats #
Currently, marker streams are not being displayed, and streams with irregular sampling rate will not have the correct time axis.