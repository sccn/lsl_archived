import liblsl
import random
import time

# first create a new stream info (here we set the name to BioSemi, the content-type to EEG, 8 channels, 100 Hz, and float-valued data)
# The last value would be the serial number of the device or some other more or less locally unique identifier for the stream as far as available (you could also omit it but interrupted connections wouldn't auto-recover).
info = liblsl.stream_info('BioSemi','EEG',8,100,liblsl.cf_float32,'dsffwerwer');

# next make an outlet
outlet = liblsl.stream_outlet(info)

print("now sending data...")
while True:
	# make a new random 8-channel sample; this is converted into a liblsl.vectorf (the data type that is expected by push_sample)
	mysample = liblsl.vectorf([random.random(),random.random(),random.random(),random.random(),random.random(),random.random(),random.random(),random.random()])
	# now send it and wait for a bit
	outlet.push_sample(mysample)
	time.sleep(0.01)
