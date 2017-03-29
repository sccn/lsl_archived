# if this script should fail due to ftp server/client errors (i.e. 'File not copied. IOError: [Errno ftp error] 200 Switching to Binary mode.')
# or 'Oops! Source <path> not found' (which will resul from the download error above occuring previously, 
# you may have to copy the folders into the subdirectory 'dependencies' with filezilla or some such other application
# the url for the server to connect to is ftp://sccn.ucsd.edu/pub/LSL/lsl-dependencies/
# from there simply copy external_libs, liblsl-xxx, and LSL-zips into the folder <path>/labstreaminglayer/dependencies
# the script first downloads these files into dependencies (which you may have to do by hand)
# then it access that local copy to place binaries into the correct folders.
# so, once the dependencies folder exists and is populated with the correct files, run the script again and it should work


import shutil
import urllib
try:
    import urllib.request  # Python 3.x
except:
    pass

import zipfile
import os
import sys

bin_addr = 'ftp://sccn.ucsd.edu/pub/LSL/lsl-dependencies'
if '__file__' in locals():  # Run from file
    cur_dir = os.path.dirname(os.path.realpath(__file__))
else:  # Interactive
    cur_dir = os.getcwd()
bin_dir = os.path.join(cur_dir, "LSL", "liblsl")
apps_dir = os.path.join(cur_dir, "Apps")
libs_dir = os.path.join(cur_dir, "LSL")
cache_dir = os.path.join(cur_dir, "dependencies")
extern_dir = os.path.join(cur_dir, "external_libs")
cur_ver = "1.12"  # current lsl version
cur_lsl = "/liblsl-"+cur_ver+"/"


# utilities for zipping/unzipping archives
def zip(src, dst):
    zf = zipfile.ZipFile("%s.zip" % (dst), "w", zipfile.ZIP_DEFLATED)
    abs_src = os.path.abspath(src)
    for dirname, subdirs, files in os.walk(src):
        for filename in files:
            absname = os.path.abspath(os.path.join(dirname, filename))
            arcname = absname[len(abs_src) + 1:]
            print('zipping %s as %s' % (os.path.join(dirname, filename),
                                        arcname))
            zf.write(absname, arcname)
    zf.close()


def unzip(src, dst):
    with zipfile.ZipFile(src, "r") as z:
        z.extractall(dst)


# dictionaries defining behavior

op_sys = ["win32", "win64", "OSX", "linux"]  # which os to unstrip -- win32 is the only one fully supported

libs = [
    "liblsl-Android/libs/armeabi",
    "liblsl-Csharp",
    "liblsl-Java",
    "liblsl-Matlab",
    "liblsl-Matlab/bin",
    "liblsl-Python/pylsl",
    "liblsl/bin"
]

std_libs = {
    'win32': [
        cur_lsl+"bin/liblsl32.dll",
        cur_lsl+"bin/liblsl32.lib",
        cur_lsl+"bin/liblsl32-debug.dll",
        cur_lsl+"bin/liblsl32-debug.lib"
    ],
    'win64': [
        cur_lsl+"bin/liblsl64.dll",
        cur_lsl+"bin/liblsl64.lib",
        cur_lsl+"bin/liblsl64-debug.dll",
        cur_lsl+"bin/liblsl64-debug.lib"
    ],
    'OSX': [
        cur_lsl+"bin/liblsl32.dylib",
        cur_lsl+"bin/liblsl64.dylib",
        cur_lsl+"bin/liblsl32-debug.dylib",
        cur_lsl+"bin/liblsl64-debug.dylib"
    ],
    'linux': [
        cur_lsl+"bin/liblsl32.so",
        cur_lsl+"bin/liblsl64.so",
        cur_lsl+"bin/liblsl32-debug.so",
        cur_lsl+"bin/liblsl64-debug.so"
    ],
}

libs_d = {
    'liblsl-Android/libs/armeabi': {
        'win32': ["/external_libs/android/liblslAndroid.so"],
        'win64': ["/external_libs/android/liblslAndroid.so"],
        'OSX': ["/external_libs/android/liblslAndroid.so"],
        'linux': ["/external_libs/android/liblslAndroid.so"],
        },
    'liblsl-Csharp': std_libs,
    'liblsl-Java': std_libs,
    'liblsl-Matlab': {},
    'liblsl-Matlab/bin': std_libs,
    'liblsl-Python/pylsl': std_libs,
    'liblsl/bin': std_libs
}

# which apps are to be stripped/unstripped
apps = [
    "AMTIForcePlate",
    "AudioCaptureWin",
    "BAlert",  
    "BioSemi",
    "BrainProducts/ActiChamp",
    "BrainProducts/BrainAmpSeries",
    "BrainProducts/BrainVisionRDA",
    "BrainProducts/VAmp",
    "BrainProducts/LiveAmp",
    "Cognionics",
    "EGIAmpServer",
    "EmbarcaderoXE/bin",
    "EmbarcaderoXE/GazeStream",
    "Enobio",  # note -- enobio.dll is missing from my computer -- it seems this is proprietary - supposedly the user will own it?
    "EyeLink",  # need to update the version of pyslsl
    "EyeTribe",
    #"EyeTribe/lib", # this is for a single .a library -- don't know what is going on there, so I exclude it...
    "g.Tec/g.HIamp",
    "g.Tec/g.USBamp",
    "g.Tec/g.USBamp/misc",
    "GameController",
    "Keyboard",
    "KinectMocap/KinectMocap",  # note -- kinect10.dll is missing from my computer -- fixed: one must install kinect runtime or the appropriate SDK
    "KinectMocap/KinectMocap/amd64",
    "KinectMocap/KinectMocap/x86",
    "LabRecorder",
    "MATLABImporter",
    "MATLABViewer",
    "MINDO",
    "Mouse",
    "MQTT",  # note -- ask Christian about this -- at least a special case
    "MQTT/mqtt2lsl/Release",
    "MQTT/lsl2mqtt/Release",
    "Neuroscan",
    "OptiTrack/OptiTrack-v1.4",
    "OptiTrack/OptiTrack-v2.2",
    "PhaseSpace",
    "PupilPro",
    "ReceiveVideo",  # note -- ask Christian about this, I'm not convinved of how this actually works or what it does
    "SerialPort",
    "SMIEyetracker",
    "SMIEyetracker/iViewNG-SDK",
    "SMIEyetracker/iViewX-SDK",
    "SMIEyetracker/Legacy-SDK",
    "StaticInfo",
    "Tobii",
    "Wiimote/Wiimote",  # note -- discovered some build problems when reorganizing the build directory -- consult with Christian
    "XDFBrowser"
]

# master dictionary and keys
apps_d = {
    # first column is the root dirctory
    'AMTIForcePlate': {
        # next comes the binaries and subdirectories to unstrip
        'win32': [
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll"]
    },

    'AudioCaptureWin': {
        'win32': [
            "/external_libs/Qt/QtNetwork4.dll",
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll",
            cur_lsl+"bin/liblsl32-debug.dll"]
    },

    'BAlert': {
        'win32': [
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll",
            "/external_libs/BAlert/BAlert.dll",
            "/external_libs/BAlert/BAlert.lib",
            "/external_libs/BAlert/SiUSBXp.dll"]
    },

    'BrainProducts/ActiChamp': {
        'win32': [
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            "/external_libs/ActiChamp/ActiChamp_x86.dll",
            "/external_libs/ActiChamp/ActiChamp_x86.lib",
            "/external_libs/ActiChamp/ActiChamp_x64.dll",
            "/external_libs/ActiChamp/ActiChamp_x64.lib",
            cur_lsl+"bin/liblsl32.dll",
            cur_lsl+"bin/liblsl64.dll"]
    },

    'BrainProducts/VAmp': {
        'win32': [
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            "/external_libs/VAmp/FirstAmp32.dll",
            "/external_libs/VAmp/FilterButterworth64D.lib",
            "/external_libs/VAmp/FilterButterworth64R.lib",
            cur_lsl+"bin/liblsl32.dll"]
    },

    'BrainProducts/LiveAmp': {
        'win32': [
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            "/external_libs/Qt/QtNetwork4.dll",
            "/external_libs/LiveAmp/LiveAmpLib2.dll",
            "/external_libs/LiveAmp/LiveAmpLib2.lib",
            cur_lsl+"bin/liblsl32.dll"]
    },

    'BrainProducts/BrainAmpSeries': {
        'win32': [
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll"]
    },


    'BrainProducts/BrainVisionRDA': {
        'win32': [
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll"]
    },

    'BioSemi': {
        'win32': [
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll",
            cur_lsl+"bin/liblsl32-debug.dll"],
        'win64': [
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl64.dll",
            cur_lsl+"bin/liblsl64-debug.dll"],
        'OSX': [
            cur_lsl+"bin/liblsl32.dylib",
            cur_lsl+"bin/liblsl64.dylib"],
        'linux': [
            cur_lsl+"bin/liblsl32.so",
            cur_lsl+"bin/liblsl64.so"]
    },

    'Cognionics': {
        'win32': [
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll"]
    },


    'EGIAmpServer': {
        'win32': [
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll"]
    },

# TODO : EmbarcaderoXE
    'EmbarcaderoXE/bin': {
        'win32': [
            "/external_libs/EmbarcaderoXE/bin/alleg44.dll",
            "/external_libs/EmbarcaderoXE/bin/alleg44.lib",
            "/external_libs/EmbarcaderoXE/bin/cvWrapper.dll",
            "/external_libs/EmbarcaderoXE/bin/cvWrapper.lib",
            "/external_libs/EmbarcaderoXE/bin/liblsl32.dll",
            "/external_libs/EmbarcaderoXE/bin/liblsl32.lib",
            "/external_libs/EmbarcaderoXE/bin/msvcp100.dll",
            "/external_libs/EmbarcaderoXE/bin/msvcr100.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_calib3d243.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_calib3d243d.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_contrib243.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_contrib243d.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_core243.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_core243d.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_features2d243.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_features2d243d.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_ffmpeg243.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_flann243.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_flann243d.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_gpu243.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_gpu243d.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_highgui243.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_highgui243d.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_imgproc243.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_imgproc243d.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_legacy243.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_legacy243d.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_ml243.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_ml243d.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_nonfree243.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_nonfree243d.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_objdetect243.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_objdetect243d.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_photo243.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_photo243d.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_stitching243.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_stitching243d.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_ts243.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_ts243d.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_video243.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_video243d.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_videostab243.dll",
            "/external_libs/EmbarcaderoXE/bin/opencv_videostab243d.dll",
            "/external_libs/EmbarcaderoXE/bin/pau.bmp",
            "/external_libs/EmbarcaderoXE/bin/pau-gr.bmp",
            "/external_libs/EmbarcaderoXE/bin/rec.bmp",
            "/external_libs/EmbarcaderoXE/bin/rec-gr.bmp"]
    },

    'EmbarcaderoXE/GazeStream': {
        'win32': ["/external_libs/TVidGrab.lib"]
    },

    'Enobio': {
        'win32': [
            "/external_libs/Enobio/EnobioDLL.lib",
            "/external_libs/Enobio/EnobioDLL.dll",
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll"]
    },

    'EyeLink': {
        'win32': [cur_lsl+"bin/liblsl32.dll"]
    },

    'EyeTribe': {
        'win32': [cur_lsl+"bin/liblsl32.dll"]
    },

#    'EyeTribe/lib': {
#        'win32': ["/external_libs/EyeTrib/libGazeApiLib.a"]
#    },

    'g.Tec/g.HIamp': {
        'win32': [
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            "/external_libs/g.Tec/gHIamp.lib",
            cur_lsl+"bin/liblsl32.dll"]
    },

    'g.Tec/g.USBamp': {
        'win32': [
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll",
            "/external_libs/g.Tec/gUSBamp.dll"]
    },

    'g.Tec/g.USBamp/misc': {
        'win32': [
            "/external_libs/g.Tec/misc/gUSBamp-for-2.0.dll",
            "/external_libs/g.Tec/misc/gUSBamp-for-3.0.dll"]
    },

    'GameController': {
        'win32': [
            "/external_libs/GameController/dxguid.lib",
            "/external_libs/GameController/dinput8.lib",
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll"]
    },

    'Keyboard': {
        'win32': [
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll"]
    },

    'KinectMocap/KinectMocap': {
        'win32': [
            "/external_libs/Kinect/Microsoft.Kinect.dll",
            "/external_libs/Kinect/amd64/Kinect10.lib",
            "/external_libs/Kinect/x86/Kinect10.lib",
            cur_lsl+"bin/liblsl32.dll"]
    },

    'KinectMocap/KinectMocap/amd64': {
        'win32': ["/external_libs/Kinect/amd64/Kinect10.lib"]
    },

    'KinectMocap/KinectMocap/x86': {
        'win32': ["/external_libs/Kinect/x86/Kinect10.lib"]
    },


    'LabRecorder': {
        'win32': [cur_lsl+"bin/liblsl32.dll"],
        'win64': [cur_lsl+"bin/liblsl64.dll"],
        'OSX': [cur_lsl+"bin/liblsl64.dylib"],
        'linux': [cur_lsl+"bin/liblsl64.so", cur_lsl+"bin/liblsl32.so"],
    },

    'MATLABImporter': {
        'win32': ["/external_libs/MATLABImporter/load_xdf_innerloop.mexw32"],
        'win64': ["/external_libs/MATLABImporter/load_xdf_innerloop.mexw64"],
        'OSX': ["/external_libs/MATLABImporter/load_xdf_innerloop.mexmaci64"],
        'linux': ["/external_libs/MATLABImporter/load_xdf_innerloop.mexa64"],
    },

    'MATLABViewer': {},  # come back to this one ...

    'MINDO': {
        'win32': [
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll"]
    },

    'Mouse': {
        'win32': [
            "/external_libs/Qt/QtNetwork4.dll",
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll",
            cur_lsl+"bin/liblsl32-debug.dll"]
    },

    'MQTT': {},  # empty
    'MQTT/mqtt2lsl/Release': {
        'win32': [
            "/external_libs/MQTT/libmosquitto/mosquitto.dll",
            "/external_libs/MQTT/libmosquitto/mosquittopp.dll",
            "/external_libs/MQTT/pthreadVC.dll",
            cur_lsl+"bin/liblsl32.dll"]
    },

    'MQTT/lsl2mqtt/Release': {
        'win32': [
            "/external_libs/MQTT/libmosquitto/mosquitto.dll",
            "/external_libs/MQTT/libmosquitto/mosquittopp.dll",
            "/external_libs/MQTT/pthreadVC.dll",
            cur_lsl+"bin/liblsl32.dll"]
    },

    'Neuroscan': {
        'win32': [
            "/external_libs/Neuroscan/NSDevice.dll",
            "/external_libs/Neuroscan/NSDevice.lib",
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll"]
    },

    'OptiTrack/OptiTrack-v1.4': {
        'win32': [
            "/external_libs/OptiTrack/NatNetLib.lib",
            "/external_libs/OptiTrack/NatNetLib.dll",
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll"]
    },

    'OptiTrack/OptiTrack-v2.2': {
        'win32': [
            "/external_libs/OptiTrack/NatNetLib.lib",
            "/external_libs/OptiTrack/NatNetLib.dll",
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll"]
    },

    'PhaseSpace': {
        'win32': [
            "/external_libs/PhaseSpace/libowlsock.lib",
            "/external_libs/PhaseSpace/libowlsock.dll",
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll"]
    },

    'PupilPro': {
        'win32': [
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll",
            cur_lsl+"bin/liblsl32-debug.dll"],
        'win64': [
            "/external_libs/zmq/libzmq-v90-mt-4_0_4.dll",
            "/external_libs/zmq/libzmq-v90-mt-gd-4_0_4.dll",
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl64.dll",
            cur_lsl+"bin/liblsl64-debug.dll"],
        'OSX': [
            cur_lsl+"bin/liblsl32.dylib",
            cur_lsl+"bin/liblsl64.dylib"],
        'linux': [
            cur_lsl+"bin/liblsl32.so",
            cur_lsl+"bin/liblsl64.so"],
    },

    'ReceiveVideo': {
        'win32': [
            "/external_libs/OpenCV/opencv_core220.dll",
            "/external_libs/OpenCV/opencv_core220.lib",
            "/external_libs/OpenCV/opencv_highgui220.dll",
            "/external_libs/OpenCV/opencv_highgui220.lib",
            "/external_libs/OpenCV/opencv_imgproc220.dll",
            "/external_libs/OpenCV/opencv_imgproc220.lib",
            cur_lsl+"bin/liblsl32.dll",
            cur_lsl+"bin/liblsl32.lib"]
    },

    'SerialPort': {
        'win32': [
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll"]
    },

    'SMIEyetracker': {},  # just the special case where we grab the allegro-5.0.10-mscv-9.0 dir

    'SMIEyetracker/iViewNG-SDK': {
        'win32': [
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll",
            "/external_libs/OpenCV/opencv_core220.dll",
            "/external_libs/OpenCV/opencv_core220.lib",
            "/external_libs/OpenCV/opencv_imgproc220.dll",
            "/external_libs/OpenCV/opencv_imgproc220.lib"]
    },

    'SMIEyetracker/iViewX-SDK': {
        'win32': [
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll",
            "/external_libs/allegro/allegro-5.0.10-monolith-md.dll"]
    },

    'SMIEyetracker/Legacy-SDK': {
        'win32': [
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll"]
    },

    'StaticInfo': {
        'win32': [
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll"]
    },

    'Tobii': {
        'win32': [
            "/external_libs/Tobii/ttime.dll",
            "/external_libs/Tobii/ttime.lib",
            "/external_libs/Tobii/tet.dll",
            "/external_libs/Tobii/tet.lib",
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            cur_lsl+"bin/liblsl32.dll"]
    },

    'Wiimote/Wiimote': {
        'win32': [
            cur_lsl+"bin/liblsl32.dll",
            cur_lsl+"bin/liblsl32.lib",
            "/external_libs/Wiimote/wiiuse.lib",
            "/external_libs/Wiimote/wiiuse.dll"]
    },  # come back to this one

    'XDFBrowser': {
        'win32': [
            "/external_libs/Qt/QtNetwork4.dll",
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            "/external_libs/Qt/QtNetworkd4.dll",
            "/external_libs/Qt/QtCored4.dll",
            "/external_libs/Qt/QtGuid4.dll",
            cur_lsl+"bin/liblsl32.dll"],
        'win64': [
            "/external_libs/Qt/QtNetwork4.dll",
            "/external_libs/Qt/QtCore4.dll",
            "/external_libs/Qt/QtGui4.dll",
            "/external_libs/Qt/QtNetworkd4.dll",
            "/external_libs/Qt/QtCored4.dll",
            "/external_libs/Qt/QtGuid4.dll",
            cur_lsl+"bin/liblsl64.dll"],
        'OSX': [
            cur_lsl+"bin/liblsl32.dylib",
            cur_lsl+"bin/liblsl64.dylib"],
        'linux': [
            cur_lsl+"bin/liblsl32.so",
            cur_lsl+"bin/liblsl64.so"],
    }

}

apps_libs_special = {
    # Key is name of app/lib. Its value is an array of dicts, where
    # each key is the remote location of the file,
    # and each value is the local location of the file relative to the app/lib
    "BioSemi": [
        {"/external_libs/BioSemi/DLL.zip": "DLL.zip"},
    ],
    "EmbarcaderoXE/bin": [
        {"/external_libs/EmbarcaderoXE/shared.zip": "../shared.zip"},
    ],
    "EyeLink": [
        {"/external_libs/pylink.zip": "pylink.zip"},
    ],
    "MATLABViewer": [
        {"/LSL-zips/liblsl-Matlab-"+cur_ver+"-unstrip.zip": "liblsl-Matlab.zip"},
    ],
    "MQTT": [
        {"/LSL-zips/liblsl-"+cur_ver+"-mqtt-unstrip.zip": "lsl2mqtt/liblsl.zip"},
        {"/LSL-zips/liblsl-"+cur_ver+"-mqtt-unstrip.zip": "mqtt2lsl/liblsl.zip"},
        {"/external_libs/MQTT/libmosquitto.zip": "lsl2mqtt/libmosquitto.zip"},
        {"/external_libs/MQTT/libmosquitto.zip": "mqtt2lsl/libmosquitto.zip"}
    ],
    "SMIEyetracker": [
        {"/external_libs/allegro-5.0.10-msvc-9.0.zip": "allegro-5.0.10-msvc-9.0.zip"},
    ],
    "PupilPro": [
        {"/external_libs/zmq.zip": "zmq.zip"},
    ],
    "liblsl-Matlab": [
        {"/LSL-zips/mex/build-bluffing-vm.zip": "mex/build-bluffing-vm.zip"},
        {"/LSL-zips/mex/build-client64-80.sdsc.edu.zip": "mex/build-client64-80.sdsc.edu.zip"},
        {"/LSL-zips/mex/build-demoing.zip": "mex/build-demoing.zip"},
        {"/LSL-zips/mex/build-evolving.zip": "mex/build-evolving.zip"},
        {"/LSL-zips/mex/build-john-HP-Pavilion-dv5-Notebook-PC.zip": "mex/build-john-HP-Pavilion-dv5-Notebook-PC.zip"},
        {"/LSL-zips/mex/build-Jordan.zip": "mex/build-Jordan.zip"},
        {"/LSL-zips/mex/build-sccn-andrey-mac.ucsd.edu.zip": "mex/build-sccn-andrey-mac.ucsd.edu.zip"},
        {"/LSL-zips/mex/build-seeding.ucsd.edu.zip": "mex/build-seeding.ucsd.edu.zip"},
        {"/LSL-zips/mex/build-untitled.zip": "mex/build-untitled.zip"},
    ]
}


def cpy_file(src, dst):
    dst = os.path.normpath(dst)
    src = os.path.normpath(src)

    print("Copying file:")
    print(src)
    print("...into:")
    print(dst)

    if not os.path.exists(os.path.dirname(dst)):
        os.makedirs(os.path.dirname(dst))

    if not os.path.exists(src):
        print("Oops! Source %s not found." % src)
    else:
        try:
            shutil.copy2(src, dst)
        except OSError as detail:  # TODO detect OS, handle error
            print("OSError", detail)


def dl_file(src, dst):
    dst = os.path.normpath(dst)
#    dest = os.path.join(
#        os.path.normcase(os.path.dirname(dst)),
#        os.path.basename(dst))

    # Create destination directory if it does not exist.
    dst_dir = os.path.dirname(dst)
    if not os.path.exists(dst_dir):
        os.makedirs(dst_dir)

    # Download file
    if not os.path.exists(dst):
        print("Attempting to copy:")
        print(src)
        print("...into:")
        print(dst)
        try:
            if hasattr(urllib,'urlretrieve'):
                urllib.urlretrieve(src, dst)
            else:
                urllib.request.urlretrieve(src, dst)
        except IOError as detail:
            print('File not copied. IOError: %s' % detail)


def dl_deps(which, which_d, arg_op_sys):

    if not os.path.exists(cache_dir):
        os.makedirs(cache_dir)

    for i in which:  # i is which app/lib we are unstripping
        for j in arg_op_sys:
            try:
                tmp = which_d[i][j]
                for t in tmp:
                    dl_file(bin_addr + t, os.path.normpath(cache_dir + t))
            except KeyError as detail:
                print('KeyError: %s' % detail)

        # special cases
        if i in apps_libs_special:
            for sub_dict in apps_libs_special[i]:
                for k in sub_dict:
                    dl_file(bin_addr + k, os.path.normpath(cache_dir + k))


# dealing with proper archive extraction
def unstrip_special_case(src, dst):
    cpy_file(src, dst)

    # next extract it
    print("Extracting archive:")
    print (dst)
    unzip(dst, dst[:-4])

    # finally, remove the archive
    print("Deleting archive copy:")
    print(dst)
    os.remove(dst)


def unstrip(which, which_d, where, arg_op_sys):

    # don't know why this directory keeps dissapearing:
    if os.path.exists("./Apps/EmbarcaderoXE/bin") == False:
        os.makedirs("./Apps/EmbarcaderoXE/bin")
    if os.path.exists("./Apps/MQTT/mqtt2lsl/Release") == False:
        os.makedirs("./Apps/MQTT/mqtt2lsl/Release")
    if os.path.exists("./Apps/MQTT/lsl2mqtt/Release") == False:
        os.makedirs("./Apps/MQTT/lsl2mqtt/Release")
    if os.path.exists("./Apps/g.Tec/g.USBamp/misc") == False:
        os.makedirs("./Apps/g.Tec/g.USBamp/misc")
    if os.path.exists("./Apps/KinectMocap/Kinect/Mocap/x86") == False:
        os.makedirs("./Apps/KinectMocap/Kinect/Mocap/x86")

    for i in which:  # i is which app we are unstripping
        for j in arg_op_sys:  # j is which os we are unstripping
            try:
                tmp = which_d[i][j]
                for t in tmp:
                    src_path = os.path.normpath(cache_dir + t)
                    extra_dir = j if j in ["OSX", "linux"] and where == apps_dir else ""
                    dest_path = os.path.join(where, i, extra_dir, os.path.basename(t))
                    cpy_file(src_path, dest_path)
            except KeyError as detail:
                print('KeyError: %s' % detail)

        # special cases
        if i in apps_libs_special:
            for sub_dict in apps_libs_special[i]:
                for k in sub_dict:
                    local_path = sub_dict[k]
                    unstrip_special_case(
                        os.path.normpath(cache_dir + k),
                        os.path.join(where, i, local_path)
                    )


def strip(which, which_d, where, arg_op_sys):
    for i in which:  # i is which app we are unstripping
        for j in arg_op_sys:  # j is which os we are unstripping
            try:
                tmp = which_d[i][j]
                for t in tmp:
                    extra_dir = j if j in ["OSX", "linux"] and where == apps_dir else ""
                    doomed_path = os.path.join(where, i, extra_dir, os.path.basename(t))
                    print("Attempting to remove:")
                    print(doomed_path)
                    try:
                        os.remove(doomed_path)
                    except OSError as detail:  # TODO detect OS, handle error
                        if detail.errno == 2:
                            pass  # Suppress message that file is not there.
                        else:
                            print("OSError", detail)
            except KeyError as detail:
                print('KeyError: %s' % detail)
        # special cases
        if i in apps_libs_special:
            for sub_dict in apps_libs_special[i]:
                for k in sub_dict:  # Only 1, but this is easy...
                    doomed_dir = os.path.join(apps_dir, i, sub_dict[k], os.path.basename(k)[:-4])
                    print("Attempting to remove the directory:")
                    print(doomed_dir)
                    try:
                        if os.path.exists(doomed_dir):
                            shutil.rmtree(doomed_dir)
                    except OSError as detail:  # TODO detect OS, handle error
                        print("OSError", detail)


def strip_all():
    strip(libs, libs_d, libs_dir, op_sys)
    strip(apps, apps_d, apps_dir, op_sys)
    # The only reason you would strip is if you need newer binaries and plan
    # to unstrip again, so go ahead and delete the local (old?) files.
    try:
        print("Attempting to remove "+cache_dir)
        os.remove(cache_dir)
    except OSError as detail:  # TODO detect OS, handle error
        print("OSError", detail)


def unstrip_all():
    # Only download files for this OS
    my_op_sys = op_sys  # ["win32", "win64", "OSX", "linux"]
    
    if sys.platform == "linux" or sys.platform == "linux32":
        my_op_sys = ["linux"]
    elif sys.platform == "darwin":
        my_op_sys = ["OSX"]
    elif sys.platform == "win32":
        my_op_sys = ["win32", "win64"]

    # uncomment if you want to be os specific
    my_op_sys = op_sys  # ["win32", "win64", "OSX", "linux"]

        
    dl_deps(libs, libs_d, my_op_sys)
    dl_deps(apps, apps_d, my_op_sys)
    unstrip(libs, libs_d, libs_dir, my_op_sys)
    unstrip(apps, apps_d, apps_dir, my_op_sys)
    
    # comment if you want to keep the dependencies folder 
    #if os.path.exists("./dependencies"):
    #    shutil.rmtree("./dependencies")
