"""Example program to demonstrate how to send string-valued markers into LSL."""

import random
import time

from pylsl import StreamInfo, StreamOutlet

# first create a new stream info (here we set the name to MyMarkerStream,
# the content-type to Markers, 1 channel, irregular sampling rate,
# and string-valued data) The last value would be the locally unique
# identifier for the stream as far as available, e.g.
# program-scriptname-subjectnumber (you could also omit it but interrupted
# connections wouldn't auto-recover). The important part is that the
# content-type is set to 'Markers', because then other programs will know how
#  to interpret the content
info = StreamInfo('MyMarkerStream', 'Markers', 1, 0, 'string', 'myuidw43536')

# next make an outlet
outlet = StreamOutlet(info)

print("now sending markers...")
markernames = ['Test', 'Blah', 'Marker', 'XXX', 'Testtest', 'Test-1-2-3']
while True:
    # pick a sample to send an wait for a bit
    outlet.push_sample([random.choice(markernames)])
    time.sleep(random.random()*3)
