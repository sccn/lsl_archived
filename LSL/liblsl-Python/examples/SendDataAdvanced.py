import sys; sys.path.append('..')  # make sure that pylsl is found (note: in a normal program you would bundle pylsl with the program)
import pylsl
import random
import time

# first create a new stream info (here we set the name to BioSemi, the content-type to EEG, 8 channels, 100 Hz, and float-valued data)
# The last value would be the serial number of the device or some other more or less locally unique identifier for the stream as far as available (you could also omit it but interrupted connections wouldn't auto-recover).
info = pylsl.stream_info('BioSemi','EEG',8,100,pylsl.cf_float32,'dsffwerwer');

# append some meta-data
info.desc().append_child_value("manufacturer","BioSemi")
channels = info.desc().append_child("channels")
for c in ["C3","C4","Cz","FPz","POz","CPz","O1","O2"]:
	channels.append_child("channel").append_child_value("name",c).append_child_value("unit","microvolts").append_child_value("type","EEG")

# next make an outlet; we set the outgoing buffer size to 360 seconds (max.) and the transmission chunk size to 32 samples
outlet = pylsl.stream_outlet(info,360,32)

print("now sending data...")
while True:
	# make a new random 8-channel sample; this is converted into a pylsl.vectorf (the data type that is expected by push_sample)
	mysample = pylsl.vectorf([random.random(),random.random(),random.random(),random.random(),random.random(),random.random(),random.random(),random.random()])
    # get a time stamp in seconds (we might modify this time stamp based on the true age of the sample, e.g. if it came from a measurement device, in case we can determine it)
	now = pylsl.local_clock()
	# now send it and wait for a bit
	outlet.push_sample(mysample,now)
	time.sleep(0.01)
