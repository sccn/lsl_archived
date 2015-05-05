import liblsl

# first resolve a marker stream on the lab network
print("looking for a marker stream...")
streams = liblsl.resolve_stream('type','Markers')

# create a new inlet to read from the stream
inlet = liblsl.stream_inlet(streams[0])

sample = liblsl.vectorstr()
while True:
	# get a new sample (you can also omit the timestamp part if you're not interested in it)
	timestamp = inlet.pull_sample(sample)
	print("got ",sample[0]," at time ",timestamp)
