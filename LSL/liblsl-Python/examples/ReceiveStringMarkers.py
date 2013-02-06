import sys; sys.path.append('..')  # make sure that pylsl is found (note: in a normal program you would bundle pylsl with the program)
import pylsl

# first resolve a marker stream on the lab network
print("looking for a marker stream...")
streams = pylsl.resolve_stream('type','Markers')

# create a new inlet to read from the stream
inlet = pylsl.stream_inlet(streams[0])

sample = pylsl.vectorstr()
while True:
	# get a new sample (you can also omit the timestamp part if you're not interested in it)
	timestamp = inlet.pull_sample(sample)
	print("got ",sample[0]," at time ",timestamp)
