# Lab streaming layer library

The lab streaming layer is a simple all-in-one approach to streaming experiment
data between applications in a lab, e.g. instrument time series, event markers,
audio, and so on.

## Building liblsl

To compile the library yourself from source please follow the instructions in
the [main build documentation](https://github.com/labstreaminglayer/tree/master/doc/BUILD.md).

To build language bindings (e.g. for
[Matlab](https://github.com/labstreaminglayer/liblsl-Matlab) or
[Python](https://github.com/labstreaminglayer/liblsl-Python)) take a look at
the [corresponding directory](https://github.com/labstreaminglayer/labstreaminglayer/tree/master/LSL)
in the [main repository](https://github.com/labstreaminglayer/labstreaminglayer).

You might also be interested in
[apps to connect recording equipment](https://github.com/labstreaminglayer/labstreaminglayer/tree/master/Apps)
and the [LabRecorder](https://github.com/labstreaminglayer/App-LabRecorder).

Please stay put while we figure out how to best offer precompiled packages.

## Boost

liblsl uses boost (mainly Boost.ASIO and Boost.Thread) extensively.
Because Windows has timing problems with newer Boost versions and embedding liblsl
in an application that links to an other Boost version (notably Matlab) causes runtime
errors, we bundle two subset of boost, the older, tested Boost in `external` and the new
`lslboost` (with patches to fix the timing behavior on Windows).

To update the included lslboost, install Boost bcp and use the `update_lslboost.sh` script.

## Design goals

The design goals of the library are:
a) The interface shall be as simple as possible, allowing programs or drivers to send 
   or receive data in just 3-5 lines of code.
b) The library should be available for a variety of languages (currently C, C++, Matlab, Python, Java) 
   and platforms (Windows, Mac OS X, Linux, 32/64 bit) and be fully interoperable between them.
c) Data transmission should work "out of the box", even across networks with no need to configure 
   IP addresses / hostnames and such (thanks to on-the-fly service discovery), also time synchronization 
   and failure recovery should work out of the box.
d) The library should be fully featured. It should cover the relevant streaming data formats incl. 
   multi-channel signals, regular/irregular sampling rate and the major channel data types 
   (int8, int16, int32, float, double, string) in a simple interface. Generic stream meta-data should 
   be supported. Advanced transmission features should be available if desired (but not in the way for 
   simple uses), including custom ways of chunking and buffering the data. It should be possible to 
   configure and tune the behavior of the library (e.g. networking features) via configuration files 
   in a way that is transparent to the applications.
e) Network and processor overhead should be reasonably low to not get in the way.

Package overview:
* The API headers are in the include/ directory.
* The library source code is in the src/ directory.
* External dependencies needed to build the library are in the external/ directory.

To connect an application to the lab streaming layer:
* Include the header for your language (`lsl_c.h` for C, `lsl_cpp.h for C++`)
  (automatically done when using CMake) or get
  [bindings for your preferred language](https://github.com/labstreaminglayer/labstreaminglayer/tree/master/LSL)
* Make sure that the library file (`liblsl32`/`64``.dll`/`.so`/`.dylib`) is found by your application. 
  On Windows, it should be enough to put it in the same folder as your executable.
  When building a Windows app, also make sure that the liblsl64.lib (or liblsl32.lib) file is visible 
  to your build environment.
* To provide data, create a new streaminfo to describe your stream and create a new outlet with that info. 
  Push samples into the outlet as your app produces them. Destroy the outlet when you're done.
* To receive data, resolve a stream that matches your citeria (e.g. name or type), which gives you a 
  streaminfo and create a new inlet with that streaminfo. Pull samples from the inlet. 
  Destroy the inlet when you're done.
* Have a look at the example sources in the
  [examples/ folder](https://github.com/labstreaminglayer/App-Examples)

The library and example applications are licensed under the MIT license.  
The library uses code that is licensed under the Boost software license.
