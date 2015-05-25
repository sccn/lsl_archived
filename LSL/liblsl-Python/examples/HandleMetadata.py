"""
Example program that shows how to attach meta-data to a stream, and how to
later on retrieve the meta-data again at the receiver side.

"""

import time

import pylsl

# Number of channels for the stream to be generated
numchannels = 8
# Name of channels to be generated
channelsname = ["C3", "C4", "Cz", "FPz", "POz", "CPz", "O1", "O2"]

# create a new StreamInfo object which shall describe our stream
info = pylsl.StreamInfo(
    name='MetaTester',
    type='EEG',
    channel_count=numchannels,
    nominal_srate=100,
    channel_format=pylsl.cf_float32,
    source_id='myuid56872'
)

# now attach some meta-data (in accordance with XDF format,
# see also code.google.com/p/xdf)
chns = info.desc().append_child("channels")
for label in channelsname:
    ch = chns.append_child("channel")
    ch.append_child_value("label", label)
    ch.append_child_value("unit", "microvolts")
    ch.append_child_value("type", "EEG")

info.desc().append_child_value("manufacturer", "SCCN")

cap = info.desc().append_child("cap")
cap.append_child_value("name", "EasyCap")
cap.append_child_value("size", "54")
cap.append_child_value("labelscheme", "10-20")

# create outlet for the stream
outlet = pylsl.StreamOutlet(info)

# (...normally here one might start sending data into the outlet...)

# === the following could run on another computer ===

# first we resolve a stream whose name is MetaTester (note that there are
# other ways to query a stream, too - for instance by content-type)
results = pylsl.resolve_stream("name", "MetaTester")

# open an inlet so we can read the stream's data (and meta-data)
inlet = pylsl.StreamInlet(results[0])

# get the full stream info (including custom meta-data) and dissect it
info = inlet.info()
desc = info.desc()
print("The stream's XML meta-data is: ")
print(info.as_xml())
print("The manufacturer is: %s" % desc.child_value("manufacturer"))
print("Cap circumference is: %s" % desc.child("cap").child_value("size"))
print("The channel labels are as follows:")
ch = desc.child("channels").child("channel")
while not ch.empty():
    print("  " + ch.child_value("label"))
    ch = ch.next_sibling()

time.sleep(3)
