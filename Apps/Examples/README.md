# LSL examples

This repository contains small examples to demonstrate the basic usage of LSL.

## LSL Coding Fundamentals

There are essentially two types of programs interacting with LSL: programs that provide data, such
as a data source that represents a particular acquisition device, and programs that consume data
(and occasionally mixtures of the two), such as a viewer, a recorder, or a program that takes some
action based on real-time data.

The main difference between LSL and other data transport interfaces is that it is not
connection-oriented (although the underlying implementation uses connection-oriented transport).
It is closer to a "publish-subscribe" model. A data producer creates a named "outlet" object
(perhaps with meta-data) and pushes samples into it without ever caring about who is listening.
So functionally it is offering a stream of data plus some meta-data about it.

A data consumer who is interested in a particular type of stream queries the network for a matching
one ("resolves it"), for example based on the name, type or some other content-based query and then
creates an "inlet" object to receive samples from the stream.
It can also separately obtain the meta-data of the stream.
The consumer then pulls out samples from the stream.
The sequence of samples that the consumer sees is in order, without omissions (unless a buffer's
memory is exhausted), and type-safe.
The data is buffered at both endpoints if there are transmission delays anywhere (e.g., interrupted
network connection) but otherwise delivered as fast as the system allows.

The objects and functions to perform these tasks are provided by a single cross-platform library
(liblsl).
The library comes with a C header file, a C++ header file and wrapper classes for other languages.

Note: if you have trouble establishing communication between these programs across different
computers especially on Windows, take a look at the NetworkConnectivity page and read the
Network Troubleshooting section.

## [FAQs](https://github.com/labstreaminglayer/liblsl/blob/master/FAQ.md)

## C Example Programs: Basic to Advanced

These two example programs illustrate the bread-and-butter use of LSL as it is executing in almost
any device module that comes with the distribution:

- [Sending a multi-channel time series into LSL.](https://github.com/labstreaminglayer/App-Examples/blob/master/SendDataC.c)
- [Receiving a multi-channel time series from LSL.](https://github.com/labstreaminglayer/App-Examples/blob/master/ReceiveDataC.c)


These two example programs illustrate a more special-purpose use case, namely sending arbitrary
string-formatted data at irregular sampling rate. Such streams are used by programs that produce
event markers, for example:

- [Sending a stream of strings with irregular timing.](https://github.com/labstreaminglayer/App-Examples/blob/master/SendStringMarkersC.c)
- [Receiving a stream of strings with irregular timing.](https://github.com/labstreaminglayer/App-Examples/blob/master/ReceiveStringMarkersC.c)

The last example shows how to attach properly formatted meta-data to a stream, and how to read it
out again at the receiving end.
While meta-data is strictly optional, it is very useful to make streams self-describing.
LSL has adopted the convention to name meta-data fields according to the XDF file format
specification whenever the content type matches (for example EEG, Gaze, MoCap, VideoRaw, etc);
the spec is [here](https://github.com/sccn/xdf/wiki/Meta-Data).

Note that some older example programs (SendData/ReceiveData) predate this convention and name the
channels inconsistently.

- [Handling stream meta-data.](https://github.com/labstreaminglayer/App-Examples/blob/master/HandleMetaDataC.c)

## C++ Example Programs: Basic to Advanced

These two example programs illustrate the shortest amount of code that is necessary to get a C++
program linked to LSL:

- [Minimal data sending example.](https://github.com/labstreaminglayer/App-Examples/blob/master/SendDataSimple.cpp)
- [Minimal data receiving example.](https://github.com/labstreaminglayer/App-Examples/blob/master/ReceiveDataSimple.cpp)


These two example programs demonstrate how to write more complete LSL clients in C++ (they are 1:1
equivalents of the corresponding C programs):

- [Sending a multi-channel time series into LSL.](https://github.com/labstreaminglayer/App-Examples/blob/master/SendData.cpp)
- [Receiving a multi-channel time series from LSL.](https://github.com/labstreaminglayer/App-Examples/blob/master/ReceiveData.cpp)

These two programs transmit their data at the granularity of chunks instead of samples.
This is mostly a convenience matter, since inlets and outlets can be configured to automatically
batch samples into chunks for transmission. Note that for LSL the data is always a linear sequence
of samples and data that is pushed as samples can be pulled out as chunks or vice versa.

They also show how structs can be used to represent the sample data, instead of numeric arrays
(which is mostly a syntactic difference):

- [Sending a multi-channel time series at chunk granularity.](https://github.com/labstreaminglayer/App-Examples/blob/master/SendDataInChunks.cpp)
- [Receiving a multi-channel time series at chunk granularity.](https://github.com/labstreaminglayer/App-Examples/blob/master/ReceiveDataInChunks.cpp)

These two example programs illustrate a more special-purpose use case, namely sending arbitrary
string-formatted data at irregular sampling rate.
Such streams are used by programs that produce event markers, for example.

These are 1:1 equivalents of the corresponding C programs:

- [Sending a stream of strings with irregular timing.](https://github.com/labstreaminglayer/App-Examples/blob/master/SendStringMarkers.cpp)
- [Receiving a stream of strings with irregular timing.](https://github.com/labstreaminglayer/App-Examples/blob/master/ReceiveStringMarkers.cpp)

The last example shows how to attach properly formatted meta-data to a stream, and how to read it
out again at the receiving end.
While meta-data is strictly optional, it is very useful to make streams self-describing.
LSL has adopted the convention to name meta-data fields according to the XDF file format
specification whenever the content type matches (for example EEG, Gaze, MoCap, VideoRaw, etc);
the spec is [here](https://github.com/sccn/xdf/wiki/Meta-Data).

- [Handling stream meta-data.](https://github.com/labstreaminglayer/App-Examples/blob/master/HandleMetaData.cpp)

## C/C++ Special-Purpose Example Programs

These programs illustrate some special use cases of LSL that are also relevant for C programmers.
See the lsl\_c.h header for the corresponding C APIs (they are very similar to the C++ code shown here).

This example illustrates in more detail how streams can be resolved on the network:
- [Resolving all streams on the lab network, one-shot and continuous.](https://github.com/labstreaminglayer/App-Examples/blob/master/GetAllStreams.cpp)

This example shows how to query the full XML meta-data of a stream (which may be several megabytes large):
- [Retrieving the XML meta-data of a stream.](https://github.com/labstreaminglayer/App-Examples/blob/master/GetFullinfo.cpp)

This example shows how to obtain time-correction values for a given stream.
These time-correction values are offsets (in seconds) that are used to remap any stream's timestamps
into the own local clock domain (just by adding the offset to the timestamp):

- [Querying the time-correction information for a stream.](https://github.com/labstreaminglayer/App-Examples/blob/master/GetTimeCorrection.cpp)

## License

Since using the examples as a starting point is actively encouraged, they are
licensed under the [MIT](https://choosealicense.com/licenses/mit/) license.

Still, you might want to consider a less permissive license (i.e. with more
requirements to share modifications) like the
[MPL](https://choosealicense.com/licenses/mpl-2.0/) or
[GPL](https://choosealicense.com/licenses/gpl-3.0/).
