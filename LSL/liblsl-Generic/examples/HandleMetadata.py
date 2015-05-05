import liblsl
import time

# create a new StreamInfo and declare some meta-data (in accordance with XDF format)
info = liblsl.stream_info("MetaTester","EEG",8,100,liblsl.cf_float32,"myuid56872")
chns = info.desc().append_child("channels")
for label in ["C3","C4","Cz","FPz","POz","CPz","O1","O2"]:
    ch = chns.append_child("channel")
    ch.append_child_value("label",label)
    ch.append_child_value("unit","microvolts")
    ch.append_child_value("type","EEG")
info.desc().append_child_value("manufacturer","SCCN")
cap = info.desc().append_child("cap")
cap.append_child_value("name","EasyCap")
cap.append_child_value("size","54")
cap.append_child_value("labelscheme","10-20")

# create outlet for the stream
outlet = liblsl.stream_outlet(info)


# === the following could run on another computer ===

# resolve the stream and open an inlet
results = liblsl.resolve_stream("name","MetaTester")
inlet = liblsl.stream_inlet(results[0])
# get the full stream info (including custom meta-data) and dissect it
inf = inlet.info()
print "The stream's XML meta-data is: "
print inf.as_xml()
print "The manufacturer is: " + inf.desc().child_value("manufacturer")
print "The cap circumference is: " + inf.desc().child("cap").child_value("size")
print "The channel labels are as follows:" 
ch = inf.desc().child("channels").child("channel")
for k in range(info.channel_count()):
    print "  " + ch.child_value("label")
    ch = ch.next_sibling()

time.sleep(3)