import sys
from time import sleep
from pylsl import StreamInfo, StreamOutlet, local_clock
import pyxdf

[streams, fileheader] = pyxdf.load_xdf(sys.argv[1])

stream = streams[0]

name = stream['info']['name'][0]
type_ = stream['info']['type'][0]
channel_names = []
for ch in stream['info']['desc'][0]['channels'][0]['channel']:
    channel_names.append(ch['label'][0])
nominal_srate = int(stream['info']['nominal_srate'][0])
channel_format = stream['info']['channel_format'][0]
source_id = stream['info']['source_id'][0]

info = StreamInfo(name, type_, len(channel_names), nominal_srate, channel_format, source_id)
info.desc().append_child_value("manufacturer", stream['info']['desc'][0]['acquisition'][0]['manufacturer'][0])

channels = info.desc().append_child("channels")
for ch in stream['info']['desc'][0]['channels'][0]['channel']:
    channels.append_child("channel") \
        .append_child_value("label", ch['label'][0]) \
        .append_child_value("unit", ch['unit'][0]) \
        .append_child_value("type", ch['type'][0])

outlet = StreamOutlet(info)

samples = stream['time_series']

try:
    n = 0
    while True:
        stamp = local_clock()
        vec = []
        i = 0
        for c in channel_names:
            vec.append(samples[n][i])
            i = i + 1
        outlet.push_sample(vec, stamp)
        n = n + 1
        if n >= samples.size:
            n = 0
            print("looped")
        sleep(1.0 / nominal_srate)
except KeyboardInterrupt:
    pass
