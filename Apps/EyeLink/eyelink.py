'''
Created on Jul 10, 2012
Updated Aug 17, 2016
@author: SCCN
'''


from pylink import *
import pylsl,socket, time
import msvcrt

if __name__ == '__main__':

    outlet = None 
    SR = 500
    edfFileName = "TRIAL.edf"
    try:
        #info = pylsl.stream_info("EyeLink","Gaze",9,500,pylsl.cf_float32,"eyelink-" + socket.gethostname());
        info = pylsl.stream_info("EyeLink","Gaze",10,SR,pylsl.cf_double64,"eyelink-" + socket.gethostname());
        channels = info.desc().append_child("channels")

        channels.append_child("channel") \
            .append_child_value("label", "leftEyeX") \
            .append_child_value("type", "eyetracking")
        channels.append_child("channel") \
            .append_child_value("label", "leftEyeY") \
            .append_child_value("type", "eyetracking")

        channels.append_child("channel") \
            .append_child_value("label", "rightEyeX") \
            .append_child_value("type", "eyetracking")
        channels.append_child("channel") \
            .append_child_value("label", "rightEyeY") \
            .append_child_value("type", "eyetracking")
        channels.append_child("channel") \
            .append_child_value("label", "leftPupilArea") \
            .append_child_value("type", "eyetracking")
        channels.append_child("channel") \
            .append_child_value("label", "rightPupilArea") \
            .append_child_value("type", "eyetracking")
        channels.append_child("channel") \
            .append_child_value("label", "pixelsPerDegreeX") \
            .append_child_value("type", "eyetracking")
        channels.append_child("channel") \
            .append_child_value("label", "pixelsPerDegreeY") \
            .append_child_value("type", "eyetracking")
        channels.append_child("channel") \
            .append_child_value("label", "eyelink_timestamp") \
            .append_child_value("type", "eyetracking")
        channels.append_child("channel") \
            .append_child_value("label", "LSL_timestamp") \
            .append_child_value("type", "eyetracking")



            
        outlet = pylsl.stream_outlet(info)
        print "Established LSL outlet."
    except:
        print "Could not create LSL outlet."
    

    quit = 0
    isVerbose = 0 
    while quit!= 1 :
        try:
            print "Trying to connect to EyeLink tracker..."
            try:
                tracker = EyeLink("255.255.255.255")
                print "Established a passive connection with the eye tracker."
            except:
                tracker = EyeLink("100.1.1.1")
                print "Established a primary connection with the eye tracker."
            # uncomment if you want to get pupil size in diameter, otherwise it is the area
            # tracker.setPupilSizeDiameter(YES)  
            # tracker.setVelocityThreshold(22)
            beginRealTimeMode(100)
            getEYELINK().openDataFile(edfFileName)	
            getEYELINK().startRecording(1, 1, 1, 1)                
            print "Now reading samples..."
            print "Press \'Esc\' to quit"

            while quit != 1: 
                sample = getEYELINK().getNewestSample()
                quit = getEYELINK().escapePressed();
                if sample is not None:
                    now = pylsl.local_clock()
                    ppd = sample.getPPD()
                    #values = [0,0, 0,0, 0, 0, sample.getTargetX(),sample.getTargetY(),sample.getTargetDistance(), ppd[0],ppd[1]]
                    values = [0,0, 0,0, 0, 0, ppd[0],ppd[1], sample.getTime(), now]
                    if (sample.isLeftSample()) or (sample.isBinocular()):
                        values[0:2] = sample.getLeftEye().getGaze()
                        values[4] = sample.getLeftEye().getPupilSize() 
                    if (sample.isRightSample()) or (sample.isBinocular()):
                        values[2:4] = sample.getRightEye().getGaze()
                        values[5] = sample.getRightEye().getPupilSize()
                        

                    
                    outlet.push_sample(pylsl.vectord(values), now, True)
                    time.sleep(1.0/SR)
                    
                    x = msvcrt.kbhit()
                    if x:
                        # getch acquires the character encoded in binary ASCII
                        # check if quit
                        if msvcrt.getch() == chr(27):
                            quit = 1

        
                    if quit==1 :
                        getEYELINK().setOfflineMode()                         
                        msecDelay(500);                 

                        # Close the file and transfer it to Display PC
                        getEYELINK().closeDataFile()
                        getEYELINK().receiveDataFile(edfFileName, edfFileName)
                        getEYELINK().close();    
        
        except Exception, e:		
            print "connection broke off: ", e

        
    