"""
Example program to demonstrate how to send a multi-channel time-series with
proper meta-data to LSL.

"""

import random
import time

import pylsl

# Number of channels for the stream to be generated
numchannels = 8
# Name of channels to be generated
channelsname = ["C3", "C4", "Cz", "FPz", "POz", "CPz", "O1", "O2"]

# first create a new stream info (here we set the name to BioSemi,
# the content-type to EEG, 8 channels, 100 Hz, and float-valued data) The
# last value would be the serial number of the device or some other more or
# less locally unique identifier for the stream as far as available (you
# could also omit it but interrupted connections wouldn't auto-recover).
info = pylsl.StreamInfo(
    name='BioSemi',
    type='EEG',
    channel_count=numchannels,
    nominal_srate=100,
    channel_format=pylsl.cf_float32,
    source_id='myuid2424'
)


# append some meta-data
info.desc().append_child_value("manufacturer", "BioSemi")
channels = info.desc().append_child("channels")
for c in channelsname:
    channels.append_child("channel") \
        .append_child_value("label", c) \
        .append_child_value("unit", "microvolts") \
        .append_child_value("type", "EEG")

# next make an outlet; we set the transmission chunk size to 32 samples and
# the outgoing buffer size to 360 seconds (max.)
outlet = pylsl.StreamOutlet(
    info,
    chunk_size=32,
    max_buffered=360
)

print("now sending data...")
while True:
    # make a new random 8-channel sample; this is converted into a
    # pylsl.vectorf (the data type that is expected by push_sample)
    mysample = [random.random() for _ in xrange(numchannels)]
    # get a time stamp in seconds (we pretend that our samples are actually
    # 125ms old, e.g., as if coming from some external hardware)
    stamp = pylsl.local_clock() - 0.125
    # now send it and wait for a bit
    outlet.push_sample(mysample, stamp)
    time.sleep(0.01)
