import shutil
import urllib

import zipfile
import os

bin_addr = 'ftp://sccn.ucsd.edu/pub/LSL/lsl-dependencies'
cur_dir = os.path.dirname(os.path.realpath(__file__))
bin_dir = cur_dir+"/LSL/liblsl"
apps_dir = cur_dir+"/Apps/"
libs_dir = cur_dir+"/LSL/"
extern_dir = cur_dir+"/external_libs/"

# utilities for zipping/unzipping archives
def zip(src, dst):
    zf = zipfile.ZipFile("%s.zip" % (dst), "w", zipfile.ZIP_DEFLATED)
    abs_src = os.path.abspath(src)
    for dirname, subdirs, files in os.walk(src):
        for filename in files:
            absname = os.path.abspath(os.path.join(dirname, filename))
            arcname = absname[len(abs_src) + 1:]
            print 'zipping %s as %s' % (os.path.join(dirname, filename),
                                        arcname)
            zf.write(absname, arcname)
    zf.close()

def unzip(src, dst):
    with zipfile.ZipFile(src, "r") as z:
        z.extractall(dst)


# dealing with proper archive extraction
def unstrip_special_case(src, dst):
            # first retrieve the archive we want
            print("Downloading archive:")
            print(src)
            print("...into")
            print(dst)
            try:
                urllib.urlretrieve(src, dst)
            except OSError as detail: # TODO find out the OS so that we handle this exception correctly
                print("OSError", detail)

            # next extract it
            print("Extracting archive:")
            print (dst)
            unzip(dst, dst[:-4])

            # finally, remove the archive
            print("Deleting archive:")
            print(dst)
            os.remove(dst)


# dictionaries defining behavior

op_sys =  ["win32", "win64", "OSX", "linux"] # which os to unstrip -- win32 is the only one fully supported

# which apps are to be stripped/unstripped
apps = [
    "AMTIForcePlate",
    "AudioCaptureWin",
    "BAlert", # note -- App doesn't run on my computer because I am missing a certain 'SiUSBXp.dll' -- consider providing this? -- fixed: by downloading said .dll and copying into C:\Windows\System32
    "BioSemi",
    "BrainProducts/ActiChamp",
    "BrainProducts/BrainAmpSeries",
    "BrainProducts/BrainVisionRDA",
    "Cognionics",
    "EGIAmpServer",
    "EmbarcaderoXE/bin", 
    "EmbarcaderoXE/GazeStream", 
    "Enobio", # note -- enobio.dll is missing from my computer -- it seems this is proprietary - supposedly the user will own it?
    "EyeLink", # need to update the version of pyslsl
    "g.Tec/g.HIamp",
    "g.Tec/g.USBamp",
    "g.Tec/g.USBamp/misc",
    "GameController",
    "Keyboard",
    "KinectMocap/KinectMocap", # note -- kinect10.dll is missing from my computer -- fixed: one must install kinect runtime or the appropriate SDK
    "KinectMocap/KinectMocap/amd64",
    "KinectMocap/KinectMocap/x86", 
    "LabRecorder/src", 
    "LabRecorder/src/pylsl",
    "LabRecorder/src/TestRecorder",
    "MATLABViewer", 
    "MINDO",
    "Mouse",
    "MQTT", # note -- ask Christian about this -- at least a special case
    "MQTT/mqtt2lsl/Release",
    "MQTT/lsl2mqtt/Release",
    "Neuroscan",
    "OptiTrack/OptiTrack-v1.4",
    "OptiTrack/OptiTrack-v2.2",
    "PhaseSpace",
    "PupilPro", 
    "ReceiveVideo", # note -- ask Christian about this, I'm not convinved of how this actually works or what it does
    "SerialPort",
    "SMIEyetracker", 
    "SMIEyetracker/iViewNG-SDK", 
    "SMIEyetracker/iViewX-SDK", 
    "SMIEyetracker/Legacy-SDK", 
    "StaticInfo",
    "Tobii",
    "Wiimote/Wiimote", # note -- discovered some build problems when reorganizing the build directory -- consult with Christian
    "XDFBrowser"
    

] 

# master dictionary and keys
apps_d={ 
    
    # first column is the root dirctory 
    'AMTIForcePlate':{
        # next comes the binaries and subdirectories to unstrip 
        'win32':["/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll","/liblsl/bin/liblsl32.dll"]
    },

    'AudioCaptureWin':{
        'win32':["/external_libs/Qt/QtNetwork4.dll", "/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl32.dll", "/liblsl/bin/liblsl32-debug.dll"]
    },

    'BAlert':{
        'win32':["/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll","/liblsl/bin/liblsl32.dll", "/external_libs/BAlert/BAlert.dll", "/external_libs/BAlert/BAlert.lib"]
    },

    'BrainProducts/ActiChamp':{
        'win32':["/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll",  "/external_libs/ActiChamp/ActiChamp_x86.dll", "/external_libs/ActiChamp/ActiChamp_x86.lib", "/liblsl/bin/liblsl32.dll"]
    },

    'BioSemi':{
        'win32':["/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl32.dll", "/liblsl/bin/liblsl32-debug.dll"],
        'win64':["/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl64.dll", "/liblsl/bin/liblsl64-debug.dll"],
        'OSX':["/liblsl/bin/liblsl32.dylib",  "/liblsl/bin/liblsl64.dylib"], 
        'linux':["/liblsl/bin/liblsl32.so", "/liblsl/bin/liblsl64.so"]
    },

    'BrainProducts/BrainAmpSeries':{
        'win32':["/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl32.dll"]
    },

    'BrainProducts/BrainAmpSeries':{
        'win32':["/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl32.dll"]
    },

    'BrainProducts/BrainVisionRDA':{
        'win32':["/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl32.dll"]
    },

    'Cognionics':{
        'win32':["/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl32.dll"]
    },


    'EGIAmpServer':{
        'win32':["/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl32.dll"]
    },

# TODO : EmbarcaderoXE
    'EmbarcaderoXE/bin':{
        'win32':["/external_libs/EmbarcaderoXE/bin/alleg44.dll", "/external_libs/EmbarcaderoXE/bin/alleg44.lib", "/external_libs/EmbarcaderoXE/bin/cvWrapper.dll", "/external_libs/EmbarcaderoXE/bin/cvWrapper.lib", "/external_libs/EmbarcaderoXE/bin/liblsl32.dll", "/external_libs/EmbarcaderoXE/bin/liblsl32.lib", "/external_libs/EmbarcaderoXE/bin/msvcp100.dll", "/external_libs/EmbarcaderoXE/bin/msvcr100.dll", "/external_libs/EmbarcaderoXE/bin/opencv_calib3d243.dll", "/external_libs/EmbarcaderoXE/bin/opencv_calib3d243d.dll", "/external_libs/EmbarcaderoXE/bin/opencv_contrib243.dll", "/external_libs/EmbarcaderoXE/bin/opencv_contrib243d.dll", "/external_libs/EmbarcaderoXE/bin/opencv_core243.dll", "/external_libs/EmbarcaderoXE/bin/opencv_core243d.dll", "/external_libs/EmbarcaderoXE/bin/opencv_features2d243.dll", "/external_libs/EmbarcaderoXE/bin/opencv_features2d243d.dll", "/external_libs/EmbarcaderoXE/bin/opencv_ffmpeg243.dll", "/external_libs/EmbarcaderoXE/bin/opencv_flann243.dll", "/external_libs/EmbarcaderoXE/bin/opencv_flann243d.dll", "/external_libs/EmbarcaderoXE/bin/opencv_gpu243.dll", "/external_libs/EmbarcaderoXE/bin/opencv_gpu243d.dll", "/external_libs/EmbarcaderoXE/bin/opencv_highgui243.dll", "/external_libs/EmbarcaderoXE/bin/opencv_highgui243d.dll", "/external_libs/EmbarcaderoXE/bin/opencv_imgproc243.dll", "/external_libs/EmbarcaderoXE/bin/opencv_imgproc243d.dll", "/external_libs/EmbarcaderoXE/bin/opencv_legacy243.dll", "/external_libs/EmbarcaderoXE/bin/opencv_legacy243d.dll", "/external_libs/EmbarcaderoXE/bin/opencv_ml243.dll", "/external_libs/EmbarcaderoXE/bin/opencv_ml243d.dll", "/external_libs/EmbarcaderoXE/bin/opencv_nonfree243.dll", "/external_libs/EmbarcaderoXE/bin/opencv_nonfree243d.dll", "/external_libs/EmbarcaderoXE/bin/opencv_objdetect243.dll", "/external_libs/EmbarcaderoXE/bin/opencv_objdetect243d.dll", "/external_libs/EmbarcaderoXE/bin/opencv_photo243.dll", "/external_libs/EmbarcaderoXE/bin/opencv_photo243d.dll", "/external_libs/EmbarcaderoXE/bin/opencv_stitching243.dll", "/external_libs/EmbarcaderoXE/bin/opencv_stitching243d.dll", "/external_libs/EmbarcaderoXE/bin/opencv_ts243.dll", "/external_libs/EmbarcaderoXE/bin/opencv_ts243d.dll", "/external_libs/EmbarcaderoXE/bin/opencv_video243.dll", "/external_libs/EmbarcaderoXE/bin/opencv_video243d.dll", "/external_libs/EmbarcaderoXE/bin/opencv_videostab243.dll", "/external_libs/EmbarcaderoXE/bin/opencv_videostab243d.dll", "/external_libs/EmbarcaderoXE/bin/pau.bmp", "/external_libs/EmbarcaderoXE/bin/pau-gr.bmp", "/external_libs/EmbarcaderoXE/bin/rec.bmp", "/external_libs/EmbarcaderoXE/bin/rec-gr.bmp"]
    },

    'EmbarcaderoXE/GazeStream':{
        'win32':["/external_libs/TVidGrab.lib"]
    },

    'Enobio':{
        'win32':["/external_libs/Enobio/EnobioDLL.lib", "/external_libs/Enobio/EnobioDLL.dll", "/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl32.dll"]
    },

    'EyeLink':{
        'win32':["/liblsl/bin/liblsl32.dll"]
    },

    'g.Tec/g.HIamp':{
        'win32':["/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl32.dll"]
    },

    'g.Tec/g.USBamp':{
        'win32':["/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl32.dll", "/external_libs/g.Tec/gUSBamp.dll"]
    },

    'g.Tec/g.USBamp/misc':{
        'win32':["/external_libs/g.Tec/misc/gUSBamp-for-2.0.dll", "/external_libs/g.Tec/misc/gUSBamp-for-3.0.dll"]
    },

    'GameController':{
        'win32':["/external_libs/GameController/dxguid.lib", "/external_libs/GameController/dinput8.lib", "/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl32.dll"]
    },

   'Keyboard':{
        'win32':["/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl32.dll"]
    },

   'KinectMocap/KinectMocap':{
        'win32':["/external_libs/Kinect/Microsoft.Kinect.dll", "/external_libs/Kinect/amd64/Kinect10.lib", "/external_libs/Kinect/x86/Kinect10.lib", "/liblsl/bin/liblsl32.dll"]
    },

    'KinectMocap/KinectMocap/amd64':{
        'win32':["/external_libs/Kinect/amd64/Kinect10.lib"]
    },

    'KinectMocap/KinectMocap/x86':{
        'win32':["/external_libs/Kinect/x86/Kinect10.lib"]
    },


    # this program is structured very differently, so it breaks from the mold
    'LabRecorder/src':{
        'win32':["/external_libs/LabRecorder/src/RecorderLib32.lib", "/external_libs/LabRecorder/src/RecorderLib32.dll", "/external_libs/LabRecorder/src/RecorderLib64.dylib", "/external_libs/LabRecorder/src/RecorderLib64.so"]
    }, 

    'LabRecorder/src/pylsl':{
        'win32':["/liblsl/bin/liblsl32.dll", "/liblsl/bin/liblsl32.dylib", "/liblsl/bin/liblsl64.dylib", "/liblsl/bin/liblsl64.so", "/liblsl-Python/pylsl.py"] # use the correct pylsl...
    }, 

    'LabRecorder/src/TestRecorder':{
        'win32':["/liblsl/bin/liblsl32.dll"],
        'win64':["/liblsl/bin/liblsl64.dll"],
        'linux':["/liblsl/bin/liblsl32.so", "/liblsl/bin/liblsl64.so"],
    }, 

    'MATLABViewer':{}, # come back to this one ...

    'MINDO':{
        'win32':["/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl32.dll"] 
    },

    'Mouse':{
        'win32':["/external_libs/Qt/QtNetwork4.dll", "/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl32.dll", "/liblsl/bin/liblsl32-debug.dll"]
    },

    'MQTT':{}, # empty
    'MQTT/mqtt2lsl/Release':{
        'win32':["/external_libs/MQTT/libmosquitto/mosquitto.dll", "/external_libs/MQTT/libmosquitto/mosquittopp.dll", "/external_libs/MQTT/pthreadVC.dll", "/liblsl/bin/liblsl32.dll"]
    },
    'MQTT/lsl2mqtt/Release':{
        'win32':["/external_libs/MQTT/libmosquitto/mosquitto.dll", "/external_libs/MQTT/libmosquitto/mosquittopp.dll", "/external_libs/MQTT/pthreadVC.dll", "/liblsl/bin/liblsl32.dll"]
    }, 
    
    'Neuroscan':{
        'win32':["/external_libs/Neuroscan/NSDevice.dll", "/external_libs/Neuroscan/NSDevice.lib", "/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl32.dll"]
    },

    'OptiTrack/OptiTrack-v1.4':{
        'win32':["/external_libs/OptiTrack/NatNetLib.lib", "/external_libs/OptiTrack/NatNetLib.dll",  "/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl32.dll"]
    }, 

    'OptiTrack/OptiTrack-v2.2':{
        'win32':["/external_libs/OptiTrack/NatNetLib.lib", "/external_libs/OptiTrack/NatNetLib.dll",  "/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl32.dll"]
    }, 

    'PhaseSpace':{
        'win32':["/external_libs/PhaseSpace/libowlsock.lib", "/external_libs/PhaseSpace/libowlsock.dll",  "/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl32.dll"]
    }, 

    'PupilPro':{
        'win32':["/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl32.dll", "/liblsl/bin/liblsl32-debug.dll"],
        'win64':["/external/zmq/libzmq-v90-mt-4_0_4.dll", "/external/zmq/libzmq-v90-mt-gd-4_0_4.dll", "/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl64.dll", "/liblsl/bin/liblsl64-debug.dll"],
        'OSX':["/liblsl/bin/liblsl32.dylib",  "/liblsl/bin/liblsl64.dylib"], 
        'linux':["/liblsl/bin/liblsl32.so", "/liblsl/bin/liblsl64.so"],
    },

   'ReceiveVideo':{
            'win32':["/external_libs/OpenCV/opencv_core220.dll", "/external_libs/OpenCV/opencv_core220.lib", "/external_libs/OpenCV/opencv_highgui220.dll", "/external_libs/OpenCV/opencv_highgui220.lib", "/external_libs/OpenCV/opencv_imgproc220.dll", "/external_libs/OpenCV/opencv_imgproc220.lib", "/liblsl/bin/liblsl32.dll", "/liblsl/bin/liblsl32.lib"]
   },

    'SerialPort':{
        'win32':["/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll"] 
    },

    'SMIEyetracker':{}, # just the special case where we grab the allegro-5.0.10-mscv-9.0 dir
   
    'SMIEyetracker/iViewNG-SDK':{
        'win32':["/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll" , "/liblsl/bin/liblsl32.dll", "/external_libs/OpenCV/opencv_core220.dll", "/external_libs/OpenCV/opencv_core220.lib", "/external_libs/OpenCV/opencv_imgproc220.dll", "/external_libs/OpenCV/opencv_imgproc220.lib"]
    }, 
    
    'SMIEyetracker/iViewX-SDK':{
        'win32':["/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll" , "/liblsl/bin/liblsl32.dll", "/external_libs/allegro/allegro-5.0.10-monolith-md.dll"]
    }, 
    
    'SMIEyetracker/Legacy-SDK':{
        'win32':["/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll" , "/liblsl/bin/liblsl32.dll"]
    }, 

    'StaticInfo':{
        'win32':["/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl32.dll"] 
    },

    'Tobii':{
        'win32':["/external_libs/Tobii/ttime.dll", "/external_libs/Tobii/ttime.lib", "/external_libs/Tobii/tet.dll", "/external_libs/Tobii/tet.lib", "/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/liblsl/bin/liblsl32.dll"] 
    },
    
    'Wiimote/Wiimote':{
        'win32':["/liblsl/bin/liblsl32.dll", "/liblsl/bin/liblsl32.lib", "/external_libs/Wiimote/wiiuse.lib", "/external_libs/Wiimote/wiiuse.dll"]
    }, # come back to this one

    'XDFBrowser':{
        'win32':["/external_libs/Qt/QtNetwork4.dll", "/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/external_libs/Qt/QtNetworkd4.dll", "/external_libs/Qt/QtCored4.dll", "/external_libs/Qt/QtGuid4.dll", "/liblsl/bin/liblsl32.dll"],
        'win64':["/external_libs/Qt/QtNetwork4.dll", "/external_libs/Qt/QtCore4.dll", "/external_libs/Qt/QtGui4.dll", "/external_libs/Qt/QtNetworkd4.dll", "/external_libs/Qt/QtCored4.dll", "/external_libs/Qt/QtGuid4.dll", "/liblsl/bin/liblsl64.dll"],
        'OSX':["/liblsl/bin/liblsl32.dylib",  "/liblsl/bin/liblsl64.dylib"], 
        'linux':["/liblsl/bin/liblsl32.so", "/liblsl/bin/liblsl64.so"],

    }
    
}


def unstrip(which, which_d, where, arg_op_sys):
    
    # don't know why this directory keeps dissapearing:
    if os.path.exists("./Apps/EmbarcaderoXE/bin")==False:
        os.makedirs("./Apps/EmbarcaderoXE/bin")
    if os.path.exists("./Apps/MQTT/mqtt2lsl/Release")==False:
        os.makedirs("./Apps/MQTT/mqtt2lsl/Release")
    if os.path.exists("./Apps/MQTT/lsl2mqtt/Release")==False:
        os.makedirs("./Apps/MQTT/lsl2mqtt/Release")


    for i in which: # i is which app we are unstripping
        for j in arg_op_sys: # j is which os we are unstripping
            try:
                tmp = which_d[i][j]
                for t in tmp:
                    #src_path = bin_dir + t
                    if where==apps_dir: # not fully general! how to fixit?
                        if j == "win32" or j == "win64":
                            dest_path = where + i + "/" + os.path.basename(t) 
                        elif j == "OSX" and os.path.isdir(where+i+"/OSX"):
                            dest_path = where + i + "/OSX/" + os.path.basename(t)
                        elif j == "linux" and os.path.isdir(where+i+"/linux"):
                            dest_path = where + i + "/linux/" + os.path.basename(t)
                    else:
                        dest_path = where + i + "/" + os.path.basename(t) 
                    if os.path.exists(dest_path)==False:
                        print("Attempting to copy:")
                        print(bin_addr+t)
                        print("...into:")
                        print(dest_path)
                        try:
                            urllib.urlretrieve(bin_addr+t, dest_path)
                            #shutil.copy(src_path, dest_path)
                        except IOError as detail:
                            print('File not copied. IOError: %s' %  detail)

            except KeyError as detail:
                print('KeyError: %s' %  detail)

        # special cases
        if i == "BioSemi" and os.path.exists(where+i+"/DLL")==False:
            unstrip_special_case(bin_addr+"/external_libs/BioSemi/DLL.zip", where+i+"/DLL.zip")
         
        if i == "EmbarcaderoXE/bin" and os.path.exists(where+i+"/../shared")==False:
            unstrip_special_case(bin_addr+"/external_libs/EmbarcaderoXE/shared.zip", where+i+"/../shared.zip")

        if i == "EyeLink" and os.path.exists(where+i+"/pylink")==False:
            unstrip_special_case(bin_addr+"/external_libs/pylink.zip", where+i+"/pylink.zip")

        if i == "MATLABViewer" and os.path.exists(where+i+"/liblsl-Matlab")==False:
            unstrip_special_case(bin_addr+"/liblsl-Matlab.zip", where+i+"/liblsl-Matlab.zip");

        if i == "MQTT" and os.path.exists(where+i+"/lsl2mqtt/liblsl")==False:
            unstrip_special_case(bin_addr+"/liblsl.zip", where+i+"/lsl2mqtt/liblsl.zip");
        if i == "MQTT" and os.path.exists(where+i+"/mqtt2lsl/liblsl")==False:
            unstrip_special_case(bin_addr+"/liblsl.zip", where+i+"/mqtt2lsl/liblsl.zip");
        if i == "MQTT" and os.path.exists(where+i+"/lsl2mqtt/libmosquitto")==False:
            unstrip_special_case(bin_addr+"/external_libs/MQTT/libmosquitto.zip", where+i+"/lsl2mqtt/libmosquitto.zip");
        if i == "MQTT" and os.path.exists(where+i+"/mqtt2lsl/libmosquitto")==False:
            unstrip_special_case(bin_addr+"/external_libs/MQTT/libmosquitto.zip", where+i+"/mqtt2lsl/libmosquitto.zip");

        if i == "SMIEyetracker" and os.path.exists(where+i+"/allegro-5.0.10-msvc-9.0.zip")==False:
            unstrip_special_case(bin_addr+"/external_libs/allegro-5.0.10-msvc-9.0.zip", where+i+"/allegro-5.0.10-msvc-9.0.zip");
       
        if i == "PupilPro" and os.path.exists(where+i+"/zmq")==False:
            unstrip_special_case(bin_addr+"/external_libs/zmq.zip", where+i+"/zmq.zip");



def strip_apps():
    print("stripping apps, apparently");
    for i in apps:
        win_files = [f for f in os.listdir(apps_dir+i)] 
        # find the dlls in the root folder -- windows is king here
        for f in win_files:
            if f[-3:] in ("dll", "lib", "bmp", "exe") or f[-5:] =="dylib" or f[-2:] == "so":
                print("Attempting to remove:")
                print(apps_dir+i+"/"+f)
                try:
                    os.remove(apps_dir+i+"/"+f)
                except OSError as detail: # TODO find out the OS so that we handle this exception correctly
                    print("OSError", detail)
                   
            # if there is a folder called OSX, go in and remove the dylibs
            if f == "OSX": # mac and linux are special
                osx_files = [g for g in os.listdir(apps_dir+i+"/OSX")] 
                # find the dlls in the root folder
                for g in osx_files:
                    if g[-5:] == "dylib" or g[-3:] == "bmp":
                        print("Removing:")
                        print(apps_dir+i+"/OSX/"+g)
                        os.remove(apps_dir+i+"/OSX/"+g)
                        
            # if there is a folder called linux, go in and remove the sos
            if f == "linux": 
                linux_files = [h for h in os.listdir(apps_dir+i+"/linux")] 
                # find the dlls in the root folder
                for h in linux_files:
                    if h[-2:] == "so" or h[-3:] == "bmp" or h[-4:] == "so.1" or h[-6:] == "so.1.0":
                        print("Removing:")
                        print(apps_dir+i+"/linux/"+h)
                        os.remove(apps_dir+i+"/linux/"+h)
                        
        # special cases

        if i == "BioSemi":
            print("Attempting to remove the directory:")
            print(apps_dir+i+"/DLL")
            try:
                if os.path.exists(apps_dir+i+"/DLL"):
                    shutil.rmtree(apps_dir+i+"/DLL")
            except OSError as detail: # TODO find out the OS so that we handle this exception correctly
                print("OSError", detail)
            
        if i == "EmbarcaderoXE/bin":
            print("Attempting to remove the directory:")
            print(apps_dir+i+"/../shared")
            try:
                if os.path.exists(apps_dir+i+"/../shared"):
                    shutil.rmtree(apps_dir+i+"/../shared")
            except OSError as detail: # TODO find out the OS so that we handle this exception correctly
                print("OSError", detail)

        if i == "EyeLink":
            print("Attempting to remove the directory:")
            print(apps_dir+i+"/pylink")
            try:
                if os.path.exists(apps_dir+i+"/pylink"):
                    shutil.rmtree(apps_dir+i+"/pylink")
            except OSError as detail: # TODO find out the OS so that we handle this exception correctly
                print("OSError", detail)

        if i =="MATLABViewer":
            print("Attempting to remove the directory:")
            print(apps_dir+i+"/liblsl-Matlab")
            try:
                if os.path.exists(apps_dir+i+"/liblsl-Matlab"):
                    shutil.rmtree(apps_dir+i+"/liblsl-Matlab")
            except OSError as detail: # TODO find out the OS so that we handle this exception correctly
                print("OSError", detail)
                        
        if i == "MQTT":
            try:
                print("Attempting to remove the directory:")
                print(apps_dir+i+"/lsl2mqtt/liblsl")
                if os.path.exists(apps_dir+i+"/lsl2mqtt/liblsl"):
                    shutil.rmtree(apps_dir+i+"/lsl2mqtt/liblsl")

                print("Attempting to remove the directory:")
                print(apps_dir+i+"/lsl2mqtt/liblsl")
                if os.path.exists(apps_dir+i+"/lsl2mqtt/libmosquitto"):
                    shutil.rmtree(apps_dir+i+"/lsl2mqtt/libmosquitto")

                print("Attempting to remove the directory:")
                print(apps_dir+i+"/lsl2mqtt/liblsl")
                if os.path.exists(apps_dir+i+"/mqtt2lsl/liblsl"):
                    shutil.rmtree(apps_dir+i+"/mqtt2lsl/liblsl")

                print("Attempting to remove the directory:")
                print(apps_dir+i+"/lsl2mqtt/liblsl")
                if os.path.exists(apps_dir+i+"/mqtt2lsl/libmosquitto"):
                    shutil.rmtree(apps_dir+i+"/mqtt2lsl/libmosquitto")

            except OSError as detail: # TODO find out the OS so that we handle this exception correctly
                print("OSError", detail)

        if i == "SMIEyetracker":
            try:
                print("Attempting to remove the directory:")
                print(apps_dir+i+"/allegro-5.0.10-msvc-9.0")
                if os.path.exists(apps_dir+i+"/allegro-5.0.10-msvc-9.0"):
                    shutil.rmtree(apps_dir+i+"/allegro-5.0.10-msvc-9.0")
            except OSError as detail: # TODO find out the OS so that we handle this exception correctly
                print("OSError", detail)

        if i == "PupilPro":
            print("Attempting to remove the directory:")
            print(apps_dir+i+"/zmq")
            try:
                if os.path.exists(apps_dir+i+"/zmq"):
                    shutil.rmtree(apps_dir+i+"/zmq")
            except OSError as detail: # TODO find out the OS so that we handle this exception correctly
                print("OSError", detail)

libs = [

    "liblsl-Android/libs/armeabi",
    "liblsl-Csharp",
    "liblsl-Java",
    "liblsl-Matlab/bin",
    "liblsl-Python",
    "liblsl/bin"

]

std_libs = {
    'win32':["/liblsl/bin/liblsl32.dll", "/liblsl/bin/liblsl32.lib"],
    'win64':["/liblsl/bin/liblsl64.dll", "/liblsl/bin/liblsl64.lib"],
    'OSX':["/liblsl/bin/liblsl32.dylib", "/liblsl/bin/liblsl64.dylib"],
    'linux':["/liblsl/bin/liblsl32.so", "/liblsl/bin/liblsl64.so"],
}

libs_d = {

    'liblsl-Android/libs/armabi':{
        'win32':["/external_libs/android/liblslAndroid.so"],
        'win64':["/external_libs/android/liblslAndroid.so"],
        'OSX':["/external_libs/android/liblslAndroid.so"],
        'linux':["/external_libs/android/liblslAndroid.so"],
        },

    'liblsl-Csharp':std_libs,

    'liblsl-Java':std_libs,

    'liblsl-Matlab/bin':std_libs,

    'liblsl-Python':std_libs,
    
    'liblsl/bin':std_libs
}

def strip_libs():
    for i in libs:
        files = [f for f in os.listdir(libs_dir+i)] 
        
        for f in files:
            if f[-3:] == "exe" or f[-3:]=="mac" or f[-5:] == "linux" or f[-3:] == "dll" or f[-5:] == "dylib" or f[-2:] == "so" or f[-3:] == "lib":
             try:
                 os.remove(libs_dir+i+"/"+f)
             except OSError as detail: # TODO find out the OS so that we handle this exception correctly
                 print("OSError", detail)


def strip_all():
    strip_apps()
    strip_libs()

def unstrip_all():
    unstrip(libs, libs_d, libs_dir, op_sys)
    unstrip(apps, apps_d, apps_dir, op_sys)
    

