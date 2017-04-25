
""" Tobii 2 LSL interface
"
" This program will run a calibration routine on a connected Tobii Pro Glasses 2 device,
" then begin outputting eyetracking and sync data via an LSL outlet.
" video is excluded here since I am developing on Windows and  the python media libraries don't play well there
"
"""

import json
import time
import threading
import socket
import urllib2
import re


from pylsl import StreamInfo, StreamOutlet


# this is mostly copied from the tobii 2 sdk examples

# this is the address given in the tobii examples, but it seems not to work on my computer
#GLASSES_IP = "10.46.16.86"  # IPv4 address

# got this ip here http://www.tobiipro.com/learn-and-support/learn/faqs/what-is-the-ip-address-of-tobii-pro-glasses-2/
GLASSES_IP = "192.168.71.50"  # IPv4 address
PORT = 49152
timeout = 1
base_url = 'http://' + GLASSES_IP
# Keep-alive message content used to request live data and live video streams
KA_DATA_MSG = "{\"type\": \"live.data.unicast\", \"key\": \"some_GUID\", \"op\": \"start\"}"

# Create UDP socket
def mksock(peer):
    iptype = socket.AF_INET
    if ':' in peer[0]:
        iptype = socket.AF_INET6
    return socket.socket(iptype, socket.SOCK_DGRAM)


# Callback function
def send_keepalive_msg(socket, msg, peer):
    global running
    while running:
        socket.sendto(msg, peer)
        time.sleep(timeout)

def post_request(api_action, data=None):
    url = base_url + api_action
    req = urllib2.Request(url)
    req.add_header('Content-Type', 'application/json')
    data = json.dumps(data)
    response = urllib2.urlopen(req, data)
    data = response.read()
    json_data = json.loads(data)
    return json_data

def post_request(api_action, data=None):
    url = base_url + api_action
    req = urllib2.Request(url)
    req.add_header('Content-Type', 'application/json')
    data = json.dumps(data)
    response = urllib2.urlopen(req, data)
    data = response.read()
    json_data = json.loads(data)
    return json_data

def create_project():
    json_data = post_request('/api/projects')
    return json_data['pr_id']


def create_participant(project_id):
    data = {'pa_project': project_id}
    json_data = post_request('/api/participants', data)
    return json_data['pa_id']

def wait_for_status(api_action, key, values):
    url = base_url + api_action
    running = True
    while running:
        req = urllib2.Request(url)
        req.add_header('Content-Type', 'application/json')
        response = urllib2.urlopen(req, None)
        data = response.read()
        json_data = json.loads(data)
        if json_data[key] in values:
            running = False
        time.sleep(1)
    return json_data[key]

def create_calibration(project_id, participant_id):
    data = {'ca_project': project_id, 'ca_type': 'default', 'ca_participant': participant_id}
    json_data = post_request('/api/calibrations', data)
    return json_data['ca_id']

def create_recording(participant_id):
    data = {'rec_participant': participant_id}
    json_data = post_request('/api/recordings', data)
    return json_data['rec_id']

def start_recording(recording_id):
    post_request('/api/recordings/' + recording_id + '/start')

def start_calibration(calibration_id):
    post_request('/api/calibrations/' + calibration_id + '/start')
    
    
def stop_recording(recording_id):
    post_request('/api/recordings/' + recording_id + '/stop')
  



if __name__ == "__main__":
    running = True
    peer = (GLASSES_IP, PORT)

    # Create socket which will send a keep alive message for the live data stream
    data_socket = mksock(peer)
    # this is necessary on Windows, not sure about other OS (not in the original examples for Ubuntu)
    data_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    data_socket.bind(('', PORT))

    # I'm not happy with this threading technique because it won't exit gracefully
    td = threading.Timer(0, send_keepalive_msg, [data_socket, KA_DATA_MSG, peer])
    td.start()

    # todo provide support for calibrating elsewhere

    project_id = create_project()
    participant_id = create_participant(project_id)
    calibration_id = create_calibration(project_id, participant_id)
    print "Project: " + project_id, ", Participant: ", participant_id, ", Calibration: ", calibration_id, " "

    #sys_data = post_request('/api/system/status')
    #sr = sys_data['sys_et_freq']
    #serial_num = sys_data['sys_serial']
    sr = 50
    serial_num = "TG02B-080105045771"


    # create a new StreamInfo object which shall describe our stream
    info = StreamInfo("Tobii2", "EyeTracking", 4, sr, "float32", serial_num)

    # now attach some meta-data (in accordance with XDF format,
    # see also code.google.com/p/xdf)
    chns = info.desc().append_child("channels")
    for label in ["gaze_x", "gaze_y", "gaze_z", "ts"]:
        ch = chns.append_child("channel")
        ch.append_child_value("label", label)

    # create outlet for the stream
    outlet = StreamOutlet(info)

    input_var = raw_input("Press enter to calibrate")
    print ('Calibration started...')
    start_calibration(calibration_id)
    status = wait_for_status('/api/calibrations/' + calibration_id + '/status', 'ca_state', ['failed', 'calibrated'])

    if status == 'failed':
         print ('Calibration failed, using default calibration instead')
    else:
         print ('Calibration successful')

    recording_id = create_recording(participant_id)
    
    start_recording(recording_id)
    print ('Recording started...')


    n = 0;
    chunk_size = 10
    out = [[0 for x in range(4)] for y in range(chunk_size)]
    while running:
         # Read live data
         data, address = data_socket.recvfrom(1024)
         if 'gp' in data:
             if not 'gp3' in data:
                 now = pylsl.local_clock()
                 sample_age = json_data['l']/float(1000000)
                 collection_time = now - sample_age
                 print([now] + [sample_age] + [collection_time])
                 json_data = json.loads(data)
                 # if cnt == 1:
                 #     first_ts = json_data['ts']
                 #     print(first_ts)
                 #     cnt = 2
                 # print(json_data['ts'])
                 # print(first_ts)
                 # print(json_data['ts'] - first_ts)
                 #out = [data['ts'], data['gp3']]
                 #print([json_data['ts']+.0] + json_data['gp'] + [json_data['l']])
                 sample= [json_data['ts']] + json_data['gp'] + [json_data['l']]
                 outlet.push_sample(pylsl.vectord(sample), collection_time, True)

         #print (data)
    #    n = n+1
    stop_recording(recording_id)    
    print('Recording finished')
    data_socket.shutdown(1)
    data_socket.close()