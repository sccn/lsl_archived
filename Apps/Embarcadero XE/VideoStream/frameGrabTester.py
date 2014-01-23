import sys; sys.path.append('..')  # make sure that pylsl is found (note: in a normal program you would bundle pylsl with the program)
import pylsl
import random
import time
from PIL import Image
import struct

info = pylsl.stream_info('VideoFrameCommandStream','Markers',1,0,pylsl.cf_string,'dgeyurtutu567sdf');

outlet = pylsl.stream_outlet(info)

print("looking for video frame stream...")
streams = pylsl.resolve_stream('type','VideoRaw')

# create a new inlet to read from the stream
inlet = pylsl.stream_inlet(streams[0])

print("now sending markers...")

flatFrame = pylsl.vectori()
i=0
while True:
	# choose a marker string randomly and store it as a pylsl.vectorstr (note that this is actually a list since there can be multiple channels in the sample, even though it is of little use for markers)
	mysample = pylsl.vectorstr([ 'Request Image' ])
	# now send it and wait for a bit
	outlet.push_sample(mysample)

	if(inlet.pull_sample(flatFrame,1)):
		print 'got frame'
		buf = struct.pack('b'*len(flatFrame), *flatFrame)
		#It is possible to load the width and height from the stream header, but not implemented here.
		img = Image.frombuffer('RGB', (640,480), buf)
		if i==0:
			#this opens your default graphics program to show the image. Not wonderful.
			img.show()
		

		time.sleep(1)
