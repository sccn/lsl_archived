"""
Example program to demonstrate how to send a multi-channel time series to LSL.

"""

import random
import time

import pylsl

# Number of channels for the stream to be generated
numchannels = 8

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
    source_id='myuid34234'
)

# next make an outlet
outlet = pylsl.StreamOutlet(info)

print("now sending data...")
while True:
    # make a new random 8-channel sample; this is converted into a
    # pylsl.vectorf (the data type that is expected by push_sample)
    mysample = [random.random() for _ in xrange(numchannels)]
    # now send it and wait for a bit
    outlet.push_sample(mysample)
    time.sleep(0.01)
