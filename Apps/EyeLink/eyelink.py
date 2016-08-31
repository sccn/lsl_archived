'''
Created on Jul 10, 2012

@author: SCCN
'''

from pylink import *
import pylsl,socket, time
from msvcrt import getch

if __name__ == '__main__':

    outlet = None 
    edfFileName = "Trial.edf"
    SR = 500
    try:
        info = pylsl.stream_info("EyeLink","Gaze",11,SR,pylsl.cf_float32,"eyelink-" + socket.gethostname());
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
                break
            except:
                tracker = EyeLink("100.1.1.1")
                print "Established a primary connection with the eye tracker."
                break
            beginRealTimeMode(100)
            #getEYELINK().startRecording(1, 1, 1, 1)   
            # uncomment if you want to get pupil size in diameter, otherwise it is the area
            # tracker.setPupilSizeDiameter(YES)            
            print "Now reading samples..."
            while True: 
                sample = getEYELINK().getNewestSample()
                if sample is not None:
                    now = pylsl.local_clock()
                    ppd = sample.getPPD()
                    values = [0,0, 0,0, 0, 0, sample.getTargetX(),sample.getTargetY(),sample.getTargetDistance(), ppd[0],ppd[1]]
                    if (sample.isLeftSample()) or (sample.isBinocular()):
                        values[0:2] = sample.getLeftEye().getGaze()
                        values[4] = sample.getLeftEye().getPupilSize()
                    if (sample.isRightSample()) or (sample.isBinocular()):
                        values[2:4] = sample.getRightEye().getGaze()
                        values[5] = sample.getRightEye().getPupilSize()
                    print values
                    outlet.push_sample(pylsl.vectord(values), now, True)
                    time.sleep(.9/SR)
        except Exception, e:		
            print "connection broke off: ", e

