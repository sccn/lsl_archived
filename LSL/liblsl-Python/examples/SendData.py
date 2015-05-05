import sys; sys.path.append('..') # help python find pylsl relative to this example program
from pylsl import StreamInfo, StreamOutlet
import random
import time

# first create a new stream info (here we set the name to BioSemi, the content-type to EEG, 8 channels, 100 Hz, and float-valued data)
# The last value would be the serial number of the device or some other more or less locally unique identifier for the stream as far as available (you could also omit it but interrupted connections wouldn't auto-recover).
info = StreamInfo('BioSemi','EEG',8,100,'float32','myuid34234');

# next make an outlet
outlet = StreamOutlet(info)

print("now sending data...")
while True:
	# make a new random 8-channel sample; this is converted into a pylsl.vectorf (the data type that is expected by push_sample)
	mysample = [random.random(),random.random(),random.random(),random.random(),random.random(),random.random(),random.random(),random.random()]
	# now send it and wait for a bit
	outlet.push_sample(mysample)
	time.sleep(0.01)
