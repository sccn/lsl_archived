# Frequently Asked Questions

## I want to check the most recent sample of a stream every few seconds. How do I do that?

Because the result of `inlet.pull_sample()` is the next sample in the order provided by the sender,
you first need to pull out all samples that have been buffered up in the inlet.

You can do this by calling `pull_sample()` with a timeout of 0.0 -- once it returns zero, there are
no more samples.
To speed this up, you can set a short buffer size when creating the inlet (e.g., one second).

## What clock does LSL use? / How do I relate LSL's `local_clock()` to my wall clock?

LSL's `local_clock()` function measures the time since the local machine was started, in seconds
(other system clocks usually do not have sufficient resolution for use with LSL).

The correct way to map its output to the time measured by your preferred system clock is to first
determine the constant offset between the two clocks, by reading them out at the same time, and then
to add that offset to the result of `local_clock()` whenever it is needed.
Also keep in mind that the time-stamps that are returned by `inlet.pull_sample()` will generally be
local to the sender's machine -- only after you add the time offset returned by
`inlet.time_correction()` to them you have them in your local domain.

## What is the latency of LSL? Does the chosen buffer size have anything to do with it?

The latency of LSL is typically under 0.1 milliseconds on a local machine, unless a very large
amount of data is transmitted (megabytes per sample).
The buffer size does not affect the latency unless you allow data to queue up by not querying it
for an extended period of time (or when the network connection is temporarily interrupted).
In such a case, the queued-up data will be quickly transmitted in a burst once network connectivity
is restored.
If you only need a limited backlog of data, you can set a shorter buffer size when creating the inlet.

## I want to transmit data with multiple data types (e.g., floats, ints) per sample. What is the best way to do that?

The simplest way is to use a channel format that can hold all numbers that you want to represent and
concatenate your data into a vector of that type -- the double64 format can store integers up to
53 bit, so it will hold virtually anything (floats, doubles, ints, etc.) that you want to store.

It is also possible to transmit raw structs, but note that this is generally unsafe and non-portable
(e.g., different compilers insert varying amount of padding between struct elements; also on
platforms with different endianness your data will be garbled).

In principle you can also send multiple streams and use the same time stamp when sending the samples,
but that will require some extra complexity at the receiving that is rarely worth the effort.

## I want to make an LSL driver for a piece of hardware. What is the fastest way to do that?

If a quick-and-dirty solution is fine, then the best way is to take one of the example programs for
your favorite language and extend it as needed.

If you want a graphical user interface and you know C++, you can copy
[the BestPracticesGUI application](https://github.com/labstreaminglayer/App-BestPracticesGUI/)
and adapt it as needed.

## I am making a driver for a piece of hardware and want to make sure that my time stamps are accurate. How to do that?

If your data comes from a separate device your samples will generally be a few milliseconds old.
If you know or have measured this delay, you can correct for it by submitting the time stamp as
`local_clock()-my_sample_age` when pushing a sample.

However, it is strongly recommended to log any constant offset (here: `my_sample_age`) in the
meta-data of the stream, otherwise it can be hard to later reconstruct what value was used,
especially if it is occasionally revised.

Aside from a delay, your time stamps will also have a jitter (due to OS multi-tasking).
It is difficult to smooth the jitter in real time correctly without introducing inadvertent clock
drift and therefore it is recommended to submit non-smoothed time stamps and leave it to the
receiver to smooth them if needed.

In particular, when you analyze the data offline (e.g., in MATLAB), you or the XDF importer can do
a much better job at linearizing the jitter post-hoc.

## I am transferring data at high sampling rate or bandwidth. What is the most efficient way to do this?

When sending the data it usually does not matter how you perform the sending (via `push_sample`,
`push_chunk`, or etc.), since the bottleneck at high bandwidth will typically be the operating
system's network stack.

You can call `push_sample` or `pull_sample` at least a million times per second without a
significant performance hit.

For small sample sizes (few channels), consider to send the data in chunks to avoid forcing
frequent OS calls and network transmission.
You can do this by either setting a chunk size when creating the outlet, or by using `push_chunk()`
instead of `push_sample()`, or by setting the pushthrough flag in `push_sample()` to false for every
sample except the last one in a batch.

Also, if you have a large number of channels (e.g., transferring image data), make sure that the
data type that you pass to the push function corresponds to the data type of the stream, otherwise
you pay extra for type casting.

When receiving data at very high rate (100KHz+) or bandwidth (20MBps+), it is faster to avoid the
basic `pull_chunk` functions and instead use `pull_chunk_multiplexed` with a pre-allocated buffer.
Make sure that you use a recent version of liblsl (1.10 or later offers a faster network protocol)
at both the sender and the receiver.
