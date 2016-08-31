"""Example program that shows how to attach meta-data to a stream, and how to
later on retrieve the meta-data again at the receiver side."""

import time

from pylsl import StreamInfo, StreamOutlet, StreamInlet, resolve_stream

# create a new StreamInfo object which shall describe our stream
info = StreamInfo("MetaTester", "EEG", 8, 100, "float32", "myuid56872")

# now attach some meta-data (in accordance with XDF format,
# see also code.google.com/p/xdf)
chns = info.desc().append_child("channels")
for label in ["C3", "C4", "Cz", "FPz", "POz", "CPz", "O1", "O2"]:
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
outlet = StreamOutlet(info)

# (...normally here one might start sending data into the outlet...)

# === the following could run on another computer ===

# first we resolve a stream whose name is MetaTester (note that there are
# other ways to query a stream, too - for instance by content-type)
results = resolve_stream("name", "MetaTester")

# open an inlet so we can read the stream's data (and meta-data)
inlet = StreamInlet(results[0])

# get the full stream info (including custom meta-data) and dissect it
info = inlet.info()
print("The stream's XML meta-data is: ")
print(info.as_xml())
print("The manufacturer is: %s" % info.desc().child_value("manufacturer"))
print("Cap circumference is: %s" % info.desc().child("cap").child_value("size"))
print("The channel labels are as follows:")
ch = info.desc().child("channels").child("channel")
for k in range(info.channel_count()):
    print("  " + ch.child_value("label"))
    ch = ch.next_sibling()

time.sleep(3)
