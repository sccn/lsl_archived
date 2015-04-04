# LSL Coding Fundamentals #

There are essentially two types of programs interacting with LSL: programs that provide data, such as a data source that represents a particular acquisition device, and programs that consume data (and occasionally mixtures of the two), such as a viewer, a recorder, or a program that takes some action based on real-time data.

The main difference between LSL and other data transport interfaces is that it is not connection-oriented (although the underlying implementation uses connection-oriented transport). It is closer to a "publish-subscribe" model. A data producer creates a named "outlet" object (perhaps with meta-data) and pushes samples into it without ever caring about who is listening. So functionally it is offering a stream of data plus some meta-data about it.

A data consumer who is interested in a particular type of stream queries the network for a matching one ("resolves it"), for example based on the name, type or some other content-based query and then creates an "inlet" object to receive samples from the stream. It can also separately obtain the meta-data of the stream. The consumer then pulls out samples from the stream. The sequence of samples that the consumer sees is in order, without omissions (unless a buffer's memory is exhausted), and type-safe. The data is buffered at both endpoints if there are transmission delays anywhere (e.g., interrupted network connection) but otherwise delivered as fast as the system allows.

The objects and functions to perform these tasks are provided by a single cross-platform library (liblsl). The library comes with a C header file, a C++ header file and wrapper classes for other languages.

Note: if you have trouble establishing communication between these programs across different computers especially on Windows, take a look at the NetworkConnectivity page and read the Network Troubleshooting section.

# API Documentation #

It is recommended that you clone the repository to get the respective code (or check the [SDK mirror](ftp://sccn.ucsd.edu/pub/software/LSL/SDK/) at SCCN). The documentation is at the following locations:
  * C: [C header file](https://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl/include/lsl_c.h)
  * C++: [C++ header file](https://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl/include/lsl_cpp.h)
  * Python: [pylsl module](https://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Python/pylsl.py)
  * Java: [JavaDocs](https://labstreaminglayer.googlecode.com/hg/LSL/liblsl-Java/javadoc/index.html)
  * C#: [LSL module](https://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Csharp/LSL.cs)
  * MATLAB: [class files](https://code.google.com/p/labstreaminglayer/source/browse/#hg%2FLSL%2Fliblsl-Matlab).
  * Other languages (R, Octave, Ruby, Lua, Perl, Go): [SWIG interfaces](https://code.google.com/p/labstreaminglayer/source/browse/#hg%2FLSL%2Fliblsl-Generic) (the C or C++ header file is the API reference).

The API documentation covers all classes, functions and types and should hopefully leave no questions unanswered. Note that a typical application will only need a small subset of the API (as used in the example programs).

# C Example Programs: Basic to Advanced #

These two example programs illustrate the bread-and-butter use of LSL as it is executing in almost any device module that comes with the distribution:
  * [Sending a multi-channel time series into LSL.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl/examples/C/SendDataC/SendDataC.c)
  * [Receiving a multi-channel time series from LSL.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl/examples/C/ReceiveDataC/ReceiveDataC.c)


These two example programs illustrate a more special-purpose use case, namely sending arbitrary string-formatted data at irregular sampling rate. Such streams are used by programs that produce event markers, for example:
  * [Sending a stream of strings with irregular timing.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl/examples/C/SendStringMarkersC/SendStringMarkersC.c)
  * [Receiving a stream of strings with irregular timing.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl/examples/C/ReceiveStringMarkersC/ReceiveStringMarkersC.c)

The last example shows how to attach properly formatted meta-data to a stream, and how to read it out again at the receiving end. While meta-data is strictly optional, it is very useful to make streams self-describing. LSL has adopted the convention to name meta-data fields according to the XDF file format specification whenever the content type matches (for example EEG, Gaze, MoCap, VideoRaw, etc); the spec is [here](http://code.google.com/p/xdf/wiki/MetaData). Note that some older example programs (SendData/ReceiveData) predate this convention and name the channels inconsistently.
  * [Handling stream meta-data.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl/examples/C/HandleMetaDataC/HandleMetaDataC.c)

# C++ Example Programs: Basic to Advanced #

These two example programs illustrate the shortest amount of code that is necessary to get a C++ program linked to LSL:
  * [Minimal data sending example.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl/examples/C%2B%2B/SendDataSimple/SendDataSimple.cpp)
  * [Minimal data receiving example.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl/examples/C%2B%2B/ReceiveDataSimple/ReceiveDataSimple.cpp)


These two example programs demonstrate how to write more complete LSL clients in C++ (they are 1:1 equivalents of the corresponding C programs):
  * [Sending a multi-channel time series into LSL.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl/examples/C%2B%2B/SendData/SendData.cpp)
  * [Receiving a multi-channel time series from LSL.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl/examples/C%2B%2B/ReceiveData/ReceiveData.cpp)

These two programs transmit their data at the granularity of chunks instead of samples. This is mostly a convenience matter, since inlets and outlets can be configured to automatically batch samples into chunks for transmission. Note that for LSL the data is always a linear sequence of samples and data that is pushed as samples can be pulled out as chunks or vice versa. They also show how structs can be used to represent the sample data, instead of numeric arrays (which is mostly a syntactic difference):
  * [Sending a multi-channel time series at chunk granularity.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl/examples/C%2B%2B/SendDataInChunks/SendDataInChunks.cpp)
  * [Receiving a multi-channel time series at chunk granularity.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl/examples/C%2B%2B/ReceiveDataInChunks/ReceiveDataInChunks.cpp)

These two example programs illustrate a more special-purpose use case, namely sending arbitrary string-formatted data at irregular sampling rate. Such streams are used by programs that produce event markers, for example. These are 1:1 equivalents of the corresponding C programs:
  * [Sending a stream of strings with irregular timing.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl/examples/C%2B%2B/SendStringMarkers/SendStringMarkers.cpp)
  * [Receiving a stream of strings with irregular timing.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl/examples/C%2B%2B/ReceiveStringMarkers/ReceiveStringMarkers.cpp)

The last example shows how to attach properly formatted meta-data to a stream, and how to read it out again at the receiving end. While meta-data is strictly optional, it is very useful to make streams self-describing. LSL has adopted the convention to name meta-data fields according to the XDF file format specification whenever the content type matches (for example EEG, Gaze, MoCap, VideoRaw, etc); the spec is [here](http://code.google.com/p/xdf/wiki/MetaData). Note that some older example programs (SendData/ReceiveData) predate this convention and name the channels inconsistently.
  * [Handling stream meta-data.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl/examples/C%2B%2B/HandleMetaData/HandleMetaData.cpp)


# C/C++ Special-Purpose Example Programs #
These programs illustrate some special use cases of LSL that are also relevant for C programmers. See the lsl\_c.h header for the corresponding C APIs (they are very similar to the C++ code shown here).

This example illustrates in more detail how streams can be resolved on the network:
  * [Resolving all streams on the lab network, one-shot and continuous.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl/examples/C%2B%2B/GetAllStreams/GetAllStreams.cpp)

This example shows how to query the full XML meta-data of a stream (which may be several megabytes large):
  * [Retrieving the XML meta-data of a stream.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl/examples/C%2B%2B/GetFullinfo/GetFullinfo.cpp)

This example shows how to obtain time-correction values for a given stream. These time-correction values are offsets (in seconds) that are used to remap any stream's timestamps into the own local clock domain (just by adding the offset to the timestamp):
  * [Querying the time-correction information for a stream.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl/examples/C%2B%2B/GetTimeCorrection/GetTimeCorrection.cpp)

# Python Example Programs: Basic to Advanced #
These examples show how to transmit a numeric multi-channel time series through LSL:
  * [Sending a multi-channel time series into LSL.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Python/examples/SendData.py)
  * [Receiving a multi-channel time series from LSL.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Python/examples/ReceiveData.py)

The following examples show how to send and receive data in chunks, which can be more convenient. The data sender also demonstrates how to attach meta-data to the stream.
  * [Sending a multi-channel time series in chunks.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Python/examples/SendDataAdvanced.py)
  * [Receiving a multi-channel time series in chunks.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Python/examples/ReceiveDataInChunks.py)

These examples show a special-purpose use case that is mostly relevant for stimulus-presentation programs or other applications that want to emit 'event' markers or other application state. The stream here is single-channel and has irregular sampling rate, but the value per channel is a string:
  * [Sending string-formatted irregular streams.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Python/examples/SendStringMarkers.py)
  * [Receiving string-formatted irregular streams.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Python/examples/ReceiveStringMarkers.py)

The last example shows how to attach properly formatted meta-data to a stream, and how to read it out again at the receiving end. While meta-data is strictly optional, it is very useful to make streams self-describing. LSL has adopted the convention to name meta-data fields according to the XDF file format specification whenever the content type matches (for example EEG, Gaze, MoCap, VideoRaw, etc); the spec is [here](http://code.google.com/p/xdf/wiki/MetaData). Note that some older example programs (SendData/ReceiveData) predate this convention and name the channels inconsistently.
  * [Handling stream meta-data.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Python/examples/HandleMetadata.py)

# MATLAB Example Programs: Basic to Advanced #
These examples show how to transmit a numeric multi-channel time series through LSL:
  * [Sending a multi-channel time series into LSL.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Matlab/examples/SendData.m)
  * [Receiving a multi-channel time series from LSL.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Matlab/examples/ReceiveData.m)

These examples do the same as before, but now transmit the data at the granularity of chunks. For the purposes of network transmission the same effect can be achieved by creating the inlet or outlet with an extra argument to indicate that multiple samples should be batched into a chunk for transmission. However, since MATLAB's interpreter is relatively slow, the library calls should be made in a vectorized manner, i.e. at chunk granularity, whenever possible (at least for high-rate streams). Note that for LSL the data is always a linear sequence of samples and data that is pushed as samples can be pulled out as chunks or vice versa:
  * [Sending data at chunk granularity.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Matlab/examples/SendDataInChunks.m)
  * [Receiving data at chunk granularity.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Matlab/examples/ReceiveDataInChunks.m)

These examples show a special-purpose use case that is mostly relevant for stimulus-presentation programs or other applications that want to emit 'event' markers or other application state. The stream here is single-channel and has irregular sampling rate, but the value per channel is a string:
  * [Sending string-formatted irregular streams.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Matlab/examples/SendStringMarkers.m)
  * [Receiving string-formatted irregular streams.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Matlab/examples/ReceiveStringMarkers.m)

The last example shows how to attach properly formatted meta-data to a stream, and how to read it out again at the receiving end. While meta-data is strictly optional, it is very useful to make streams self-describing. LSL has adopted the convention to name meta-data fields according to the XDF file format specification whenever the content type matches (for example EEG, Gaze, MoCap, VideoRaw, etc); the spec is [here](http://code.google.com/p/xdf/wiki/MetaData). Note that some older example programs (SendData/ReceiveData) predate this convention and name the channels inconsistently.
  * [Handling stream meta-data.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Matlab/examples/HandleMetaData.m)

# Java Example Programs: Basic to Advanced #
These examples show how to transmit a numeric multi-channel time series through LSL:
  * [Sending a multi-channel time series into LSL.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Java/examples/SendData.java)
  * [Receiving a multi-channel time series from LSL.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Java/examples/ReceiveData.java)

The following examples show how to transmit data in form of chunks instead of samples, which can be more convenient.
  * [Sending a multi-channel time series in chunks.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Java/examples/SendDataInChunks.java)
  * [Receiving a multi-channel time series in chunks.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Java/examples/ReceiveDataInChunks.java)

These examples show a special-purpose use case that is mostly relevant for stimulus-presentation programs or other applications that want to emit 'event' markers or other application state. The stream here is single-channel and has irregular sampling rate, but the value per channel is a string:
  * [Sending string-formatted irregular streams.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Java/examples/SendStringMarkers.java)
  * [Receiving string-formatted irregular streams.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Java/examples/ReceiveStringMarkers.java)

The last example shows how to attach properly formatted meta-data to a stream, and how to read it out again at the receiving end. While meta-data is strictly optional, it is very useful to make streams self-describing. LSL has adopted the convention to name meta-data fields according to the XDF file format specification whenever the content type matches (for example EEG, Gaze, MoCap, VideoRaw, etc); the spec is [here](http://code.google.com/p/xdf/wiki/MetaData). Note that some older example programs (SendData/ReceiveData) predate this convention and name the channels inconsistently.
  * [Handling stream meta-data.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Java/examples/HandleMetaData.java)

# C# Example Programs: Basic to Advanced #
These examples show how to transmit a numeric multi-channel time series through LSL:
  * [Sending a multi-channel time series into LSL.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Csharp/examples/SendData.cs)
  * [Receiving a multi-channel time series from LSL.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Csharp/examples/ReceiveData.cs)

The following examples show how to transmit data in form of chunks instead of samples, which can be more convenient.
  * [Sending a multi-channel time series in chunks.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Csharp/examples/SendDataInChunks.cs)
  * [Receiving a multi-channel time series in chunks.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Csharp/examples/ReceiveDataInChunks.cs)

These examples show a special-purpose use case that is mostly relevant for stimulus-presentation programs or other applications that want to emit 'event' markers or other application state. The stream here is single-channel and has irregular sampling rate, but the value per channel is a string:
  * [Sending string-formatted irregular streams.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Csharp/examples/SendStringMarkers.cs)
  * [Receiving string-formatted irregular streams.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Csharp/examples/ReceiveStringMarkers.cs)

The last example shows how to attach properly formatted meta-data to a stream, and how to read it out again at the receiving end. While meta-data is strictly optional, it is very useful to make streams self-describing. LSL has adopted the convention to name meta-data fields according to the XDF file format specification whenever the content type matches (for example EEG, Gaze, MoCap, VideoRaw, etc); the spec is [here](http://code.google.com/p/xdf/wiki/MetaData). Note that some older example programs (SendData/ReceiveData) predate this convention and name the channels inconsistently.
  * [Handling stream meta-data.](http://code.google.com/p/labstreaminglayer/source/browse/LSL/liblsl-Csharp/examples/HandleMetaData.cs)

# Real-World Example Programs #
These sample codes are from actual 'production' software that is used to do data transmission:
  * [Kinect: multi-channel signal with body joint positions and meta-data.](http://code.google.com/p/labstreaminglayer/source/browse/Apps/KinectMocap/KinectMocap/KinectMocap.cpp)
  * [Keyboard: irregular marker stream based on keyboard inputs.](http://code.google.com/p/labstreaminglayer/source/browse/Apps/Keyboard/mainwindow.cpp)
  * [B-Alert: reading from an EEG device in a separate thread.](http://code.google.com/p/labstreaminglayer/source/browse/Apps/BAlert/mainwindow.cpp)
  * [EyeLink: reading from an eye tracker in Python.](http://code.google.com/p/labstreaminglayer/source/browse/Apps/EyeLink/eyelink.py)

Also, all applications in the Apps directory are open-source and can serve as examples, and most of them are very similar in how they pass on data to LSL.


# Frequently Asked Questions #

**I want to check the most recent sample of a stream every few seconds. How do I do that?** Because the result of inlet.pull\_sample() is the next sample in the order provided by the sender, you first need to pull out all samples that have been buffered up in the inlet. You can do this by calling pull\_sample() with a timeout of 0.0 -- once it returns zero, there are no more samples. To speed this up, you can set a short buffer size when creating the inlet (e.g., one second).

**What clock does LSL use? / How do I relate LSL's local\_clock() to my wall clock?** LSL's local\_clock() function measures the time since the local machine was started, in seconds (other system clocks usually do not have sufficient resolution for use with LSL). The correct way to map its output to the time measured by your preferred system clock is to first determine the constant offset between the two clocks, by reading them out at the same time, and then to add that offset to the result of local\_clock() whenever it is needed. Also keep in mind that the time-stamps that are returned by inlet.pull\_sample() will generally be local to the sender's machine -- only after you add the time offset returned by inlet.time\_correction() to them you have them in your local domain.

**What is the latency of LSL? Does the chosen buffer size have anything to do with it?** The latency of LSL is typically under 0.1 milliseconds on a local machine, unless a very large amount of data is transmitted (megabytes per sample). The buffer size does not affect the latency unless you allow data to queue up by not querying it for an extended period of time (or when the network connection is temporarily interrupted). In such a case, the queued-up data will be quickly transmitted in a burst once network connectivity is restored. If you only need a limited backlog of data, you can set a shorter buffer size when creating the inlet.

**I want to transmit data with multiple data types (e.g., floats, ints) per sample. What is the best way to do that?** The simplest way is to use a channel format that can hold all numbers that you want to represent and concatenate your data into a vector of that type -- the double64 format can store integers up to 53 bit, so it will hold virtually anything (floats, doubles, ints, etc.) that you want to store. It is also possible to transmit raw structs, but note that this is generally unsafe and non-portable (e.g., different compilers insert varying amount of padding between struct elements; also on platforms with different endianness your data will be garbled). In principle you can also send multiple streams and use the same time stamp when sending the samples, but that will require some extra complexity at the receiving that is rarely worth the effort.

**I want to make an LSL driver for a piece of hardware. What is the fastest way to do that?** If a quick-and-dirty solution is fine, then the best way is to take one of the example programs for your favorite language and extend it as needed. If you want a graphical user interface and you know C++ and are on Windows, you can copy one of the applications in the LSL distribution and adapt it as needed.

**I am making a driver for a piece of hardware and want to make sure that my time stamps are accurate. How to do that?** If your data comes from a separate device your samples will generally be a few milliseconds old. If you know or have measured this delay, you can correct for it by submitting the time stamp as local\_clock()-my\_sample\_age when pushing a sample. However, it is strongly recommended to log any constant offset (here: my\_sample\_age) in the meta-data of the stream, otherwise it can be hard to later reconstruct what value was used, especially if it is occasionally revised. Aside from a delay, your time stamps will also have a jitter (due to OS multi-tasking). It is difficult to smooth the jitter in real time correctly without introducing inadvertent clock drift and therefore it is recommended to submit non-smoothed time stamps and leave it to the receiver to smooth them if needed. In particular, when you analyze the data offline (e.g., in MATLAB), you or the XDF importer can do a much better job at linearizing the jitter post-hoc.

**I am transferring data at high sampling rate or bandwidth. What is the most efficient way to do this?** When sending the data it usually does not matter how you perform the sending (via push\_sample, push\_chunk, or etc.), since the bottleneck at high bandwidth will typically be the operating system's network stack. You can call push\_sample or pull\_sample at least a million times per second without a significant performance hit. For small sample sizes (few channels), consider to send the data in chunks to avoid forcing frequent OS calls and network transmission. You can do this by either setting a chunk size when creating the outlet, or by using push\_chunk() instead of push\_sample(), or by setting the pushthrough flag in push\_sample() to false for every sample except the last one in a batch. Also, if you have a large number of channels (e.g., transferring image data), make sure that the data type that you pass to the push function corresponds to the data type of the stream, otherwise you pay extra for type casting. When receiving data at very high rate (100KHz+) or bandwidth (20MBps+), it is faster to avoid the basic pull\_chunk functions and instead use pull\_chunk\_multiplexed with a pre-allocated buffer. Make sure that you use a recent version of liblsl (1.10 or later offers a faster network protocol) at both the sender and the receiver.