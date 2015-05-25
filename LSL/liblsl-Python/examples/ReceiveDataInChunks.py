"""
Example program to demonstrate how to read a multi-channel time-series
from LSL in a chunk-by-chunk manner (which is more efficient).

"""

import pylsl

# first resolve an EEG stream on the lab network
print("looking for an EEG stream...")
streams = pylsl.resolve_stream('type', 'EEG')

# create a new inlet to read from the stream
inlet = pylsl.StreamInlet(streams[0])

while True:
    # get a new sample (you can also omit the timestamp part if you're not
    # interested in it)
    chunk, timestamps = inlet.pull_chunk()
    if timestamps:
        print(timestamps, chunk)
