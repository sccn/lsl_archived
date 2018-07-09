# MATLAB Example Programs: Basic to Advanced

These examples show how to transmit a numeric multi-channel time series through LSL:
  * [Sending a multi-channel time series into LSL.](SendData.m)
  * [Receiving a multi-channel time series from LSL.](ReceiveData.m)

These examples do the same as before, but now transmit the data at the granularity of chunks.
For the purposes of network transmission the same effect can be achieved by creating the inlet or
outlet with an extra argument to indicate that multiple samples should be batched into a chunk for
transmission.

However, since MATLAB's interpreter is relatively slow, the library calls should be made in a
vectorized manner, i.e. at chunk granularity, whenever possible (at least for high-rate streams).

Note that for LSL the data is always a linear sequence of samples and data that is pushed as
samples can be pulled out as chunks or vice versa:

  * [Sending data at chunk granularity.](SendDataInChunks.m)
  * [Receiving data at chunk granularity.](ReceiveDataInChunks.m)

These examples show a special-purpose use case that is mostly relevant for stimulus-presentation
programs or other applications that want to emit 'event' markers or other application state.

The stream here is single-channel and has irregular sampling rate, but the value per channel is a
string:

  * [Sending string-formatted irregular streams.](SendStringMarkers.m)
  * [Receiving string-formatted irregular streams.](ReceiveStringMarkers.m)

The last example shows how to attach properly formatted meta-data to a stream, and how to read it
out again at the receiving end.
While meta-data is strictly optional, it is very useful to make streams self-describing.
LSL has adopted the convention to name meta-data fields according to the XDF file format
specification whenever the content type matches (for example EEG, Gaze, MoCap, VideoRaw, etc);
the spec is [here](https://github.com/sccn/xdf/wiki/Meta-Data).

Note that some older example programs (SendData/ReceiveData) predate this convention and name the
channels inconsistently.

  * [Handling stream meta-data.](HandleMetaData.m)

## Running the examples 
To run these examples, you must have added the folder `include/lsl_matlab` to your MATLAB path (recursively).
You can do this either via the File/Path... menu or via the command `addpath(genpath('your_path_to/liblsl-1.xx/include/lsl_matlab'))`
Also, if you have rebuilt the library, make sure that `liblsl-1.xx/include/lsl_matlab/bin` contains a copy of the library files in the `liblsl-1.xx/bin`.
