# Overview #

The OpenViBE acquisition server (OVAS) is an open-source program that is part of the OpenViBE distribution and OpenViBE project (http://openvibe.inria.fr/) developed at INRIA. The OVAS supports a wide range of EEG hardware by itself, and is almost entirely complementary to what the LSL distribution is offering. Together both distributions cover the majority of the EEG market. While the OpenViBE project is unrelated to LSL as of this writing, we are offering a special build of the OVAS in the [Downloads](http://code.google.com/p/labstreaminglayer/downloads/list) section which has support for LSL streaming added in (including the respective source code).

# Usage #

The usage of this OVAS build is identical to the standard version, so the [official documentation](http://openvibe.inria.fr/acquisition-server/) applies without restriction. The only addition is that the OVAS, while connected and playing, is visible on the lab network under the name of the device that is being streamed (e.g., "Generic Oscillator" or "BrainAmp series"). For real-time use it is important that you minimize the buffering latency, see Minimizing Latency below.

# In a Nutshell #

  * After you have downloaded the application, double-click the file openvibe-acquisition-server-withlsl.cmd and wait for it to start up. This should give you a window like the following.
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/OVAS-main.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/OVAS-main.png)

  * Select your source hardware and configure the module if necessary by clicking on "Driver Properties", which is specific to the particular driver, but looks similar to the following window:
> > ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/ovas-driverprops.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/ovas-driverprops.png)


> For a quick test using the Generic Oscillator source is fine and does not require configuration.

  * Connect to the device by pressing "Connect", and then start acquisition by pressing "Play". If all goes well, you should now have a new stream on the lab network that is named after your source hardware  and has type "EEG". Note that merely connecting to the device does not yet bring the stream up on the network.

# Advanced Features #
The OVAS provides some advanced features for drift correction (i.e. when the device has a different effective sampling rate than its nominal sampling rate).  You will only need this feature if you have a data analysis application that cannot properly handle odd sampling rates (e.g. 250.1 Hz). In most other cases, especially offline analysis, you will probably want to disable it, since it can affect your data quality to some extent.

# Minimizing Latency #
The OVAS outputs the data in equal-sized chunks, and the chunk size determines most of the latency of the program. To minimize this latency, use the lowest possible setting under "Sample count per sent block". Some device drivers also offer options that indirectly affect the latency, for example the "Brain Products BrainAmp series" driver has an option called "Decimation factor", which serves to reduce the effective sampling rate (default=10x). The length of the output buffer (in seconds) is proportional to this factor, so a lower value will give you lower latency.