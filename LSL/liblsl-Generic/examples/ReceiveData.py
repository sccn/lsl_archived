import liblsl

# first resolve an EEG stream on the lab network
print("looking for an EEG stream...")
streams = liblsl.resolve_stream('type','EEG')

# create a new inlet to read from the stream
inlet = liblsl.stream_inlet(streams[0])

sample = liblsl.vectorf()
while True:
	# get a new sample (you can also omit the timestamp part if you're not interested in it)
	timestamp = inlet.pull_sample(sample)	
	print(timestamp, list(sample))
