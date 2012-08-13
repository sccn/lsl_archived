'''
Created on Jul 10, 2012

@author: SCCN
'''

from pylink import *
import pylsl,socket, time

if __name__ == '__main__':

    outlet = None 
    try:
        info = pylsl.stream_info("EyeLink","Gaze",9,100,pylsl.cf_float32,"eyelink-" + socket.gethostname());
        outlet = pylsl.stream_outlet(info)
        print "Established LSL outlet."
    except:
        print "Could not create LSL outlet."
    

    
    while True:
        try:
            print "Trying to connect to EyeLink tracker..."
            try:
                tracker = EyeLink("255.255.255.255")
                print "Established a passive connection with the eye tracker."
            except:
                tracker = EyeLink("100.1.1.1")
                print "Established a primary connection with the eye tracker."
            beginRealTimeMode(100)
            getEYELINK().startRecording(1, 1, 1, 1)                
            print "Now reading samples..."
            while True: 
                sample = getEYELINK().getNewestSample()
                if sample is not None:
                    now = pylsl.local_clock()
                    ppd = sample.getPPD()
                    values = [0,0, 0,0, sample.getTargetX(),sample.getTargetY(),sample.getTargetDistance(), ppd[0],ppd[1]]
                    if (sample.isLeftSample()) or (sample.isBinocular()):
                        values[0:2] = sample.getLeftEye().getGaze()
                    if (sample.isRightSample()) or (sample.isBinocular()):
                        values[2:4] = sample.getRightEye().getGaze()
                    print values
                    outlet.push_sample(pylsl.vectord(values), now, True)
                    time.sleep(1.0/250)
        except Exception, e:		
            print "connection broke off: ", e
            