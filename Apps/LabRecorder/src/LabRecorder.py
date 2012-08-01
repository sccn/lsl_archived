import sys, os, socket, time
from optparse import OptionParser

import pylsl
import RecorderLib

from PySide.QtCore import *
from PySide.QtGui import *

from ui_LabRecorder import Ui_MainWindow


class MainWindow(QMainWindow, Ui_MainWindow):
    def __init__(self,options,parent=None):
        """ Start up the main application. """
        super(MainWindow,self).__init__(parent)
        self.setupUi(self)
        
        # run the startup action
        self.startup(options)        

        # make GUI connections 
        self.actionQuit.triggered.connect(self.close)
        self.actionLoadConfig.triggered.connect(self.browseConfigDialog)
        self.browseButton.clicked.connect(self.browseLocationDialog)
        self.refreshButton.clicked.connect(self.refreshStreams)
        self.blockList.itemClicked.connect(self.blockSelected)
        self.streamList.itemClicked.connect(self.streamClicked)
        self.startButton.clicked.connect(self.startRecording)
        self.stopButton.clicked.connect(self.stopRecording)
        self.statusBar().showMessage("Ready")    
                
        # status bar timer...
        self.timer = QTimer()
        self.timer.timeout.connect(self.statusUpdate)
        self.timer.start(1000)
                
    def browseConfigDialog(self):
        """ Open a file browser to select the config file to load.""" 
        # browse for an existing configuration file
        fileName = QFileDialog.getOpenFileName(self,"Load configuration", "", "Configuration Files (*.cfg)")
        if fileName[0]:
            self.loadConfig(fileName[0])
        
    def browseLocationDialog(self):
        """ Open a file browser to select the storage location."""
        # browse for a (usually new) storage location
        fileName = QFileDialog.getSaveFileName(self,"Save recordings as...", "untitled.xdf", "XDF recordings (*.xdf);;XDF compressed recordings (*.xdfz)")
        if fileName[0]:
            self.StorageLocation = fileName[0]
            self.locationEdit.setText(fileName[0])
    
    def closeEvent(self,ev):
        """ Make sure that the user cannot close the program while a recording is running. """
        if self.CurrentlyRecording:
            ev.ignore()

    def statusUpdate(self):
        """ Update the status-bar message while recording. """
        if self.CurrentlyRecording:
            elapsed = int(time.time() - self.StartTime)
            hours, remainder = divmod(elapsed, 3600)
            minutes, seconds = divmod(remainder, 60)
            self.statusBar().showMessage("Recording ({0:02d}:{1:02d}:{2:02d}; {3:d}kb)...".format(hours, minutes, seconds, int(os.path.getsize(self.ActualFileName)/1000)))

    def startup(self,options):
        """ Perform (non-GUI) startup actions. """
        # load the initial configuration file 
        self.loadConfig(options.config_file)        
        self.CurrentlyRecording = False
        
        
    def loadConfig(self,filename):
        """ Load a configuration file. """
        
        print "loading config file",filename,"..."
        # apply built-in defaults        
        self.StorageLocation = "" 
        self.RequiredStreams = []
        self.SessionBlocks = []
        self.ExtraChecks = {}
        self.EnableScriptedActions = False
        # override selectively 
        execfile(filename,globals(),self.__dict__)

        if self.EnableScriptedActions:
            self.on_init(self)
        
        # do some minimal checking & fixes
        if not len(self.StorageLocation):
            self.StorageLocation = "C:\\Recordings\\CurrentStudy\\exp%n\\untitled.xdf"
        if not len(self.SessionBlocks):
            self.SessionBlocks = ["default"] 

        # deduce experiment number...
        abspath = os.path.abspath(self.StorageLocation)
        # if the storage location contains it as a placeholder...        
        if '%n' in abspath:
            # if the %n is only in the directory portion of the path
            if not ('%n' in os.path.split(abspath)[1]):
                abspath = os.path.dirname(abspath)
            # if the %b is in the path under consideration... 
            if '%b' in abspath:
                # substitute it by the first block
                abspath.replace('%b',self.SessionBlocks[0])
            self.ExperimentNumber = 9999
            # find the lowest number for which the file or directory does not yet exist
            for n in range(1,9999):
                if not os.path.exists(abspath.replace('%n',str(n))):
                    self.ExperimentNumber = n
                    break
        else:
            self.ExperimentNumber = 1

        self.SelectedBlock = None

        # update the GUI
        self.locationEdit.setText(self.StorageLocation)        
        self.blockList.clear()
        self.blockList.addItems(self.SessionBlocks)
        self.blockList.setCurrentRow(0)
        self.streamList.clear()
        self.enableScriptedActionsCheck.setChecked(self.EnableScriptedActions)
        self.experimentNumberSpin.setValue(self.ExperimentNumber)
        
        # refresh the streams...
        self.refreshStreams()


    def refreshStreams(self):
        """ Refresh the stream list. """
        # get all streams on the lab network
        self.ResolvedStreams = pylsl.resolve_streams(1.0)
        # get their names
        self.ResolvedNames = [];
        for k in range(len(self.ResolvedStreams)):
            self.ResolvedNames.append(self.ResolvedStreams[k].name())
        # find those that are required but missing
        missing = [n for n in self.RequiredStreams if (n not in self.ResolvedNames)]
        self.MissingStreams = missing
        allnames = []
        allnames += self.ResolvedNames
        allnames += missing
        allnames.sort()
        # update the listbox contents
        good_brush = QBrush()
        good_brush.setColor(QColor(0,128,0))
        bad_brush = QBrush()
        bad_brush.setColor(QColor(255,0,0))
        
        # keep track of what was unchecked before
        previously_unchecked = []
        for i in range(self.streamList.count()):
            item = self.streamList.item(i)
            if item.checkState() == Qt.Unchecked: 
                previously_unchecked.append(item.text())

        self.streamList.clear()
        for n in allnames:
            item = QListWidgetItem(n,self.streamList) 
            item.setForeground(bad_brush if n in missing else good_brush)
            item.setFlags(item.flags() & Qt.ItemIsUserCheckable)
            # check what's required (or all otherwise default), EXCEPT for what was specifically unchecked before...
            item.setCheckState(Qt.Checked if ((n in self.RequiredStreams) or len(self.RequiredStreams)==0) and not (n in previously_unchecked) else Qt.Unchecked)
            self.streamList.addItem(item)

    def startRecording(self):
        """ Start a new recording. """        
        if not self.CurrentlyRecording:
            # automatically refresh the streams
            self.refreshStreams()
            
            # determine whether all checked streams are actually present
            for i in range(self.streamList.count()):
                item = self.streamList.item(i)
                if item.checkState() == Qt.Checked and item.text() in self.MissingStreams:
                    msgBox = QMessageBox()
                    msgBox.setText("At least one of the streams that you checked seems to be offline.")
                    msgBox.setInformativeText("Do you want to start recording anyway?")
                    msgBox.setIcon(QMessageBox.Warning)
                    msgBox.setStandardButtons(QMessageBox.Yes | QMessageBox.No | QMessageBox.Cancel)
                    msgBox.setDefaultButton(QMessageBox.No)
                    if not (msgBox.exec_() == QMessageBox.Yes):
                        return
                    break;            
            
            # determine the selected experiment number
            self.ExperimentNumber = self.experimentNumberSpin.value()
            # determine the selected block name
            # if self.SelectedBlock is None:
            # and invoke the selection action if not yet done (e.g. just started up)
            self.blockSelected(self.blockList.currentItem())
            # invoke scripted action
            if self.enableScriptedActionsCheck.checkState():
                try:
                    self.on_startrecord(self,self.SelectedBlock,self.ExperimentNumber)
                except Exception as e:
                    msgBox = QMessageBox()
                    msgBox.setText("The scripted action associated with starting the recording failed with error " + str(e))
                    msgBox.setInformativeText("Do you want to start recording anyway?")
                    msgBox.setIcon(QMessageBox.Warning)
                    msgBox.setStandardButtons(QMessageBox.Yes | QMessageBox.No | QMessageBox.Cancel)
                    msgBox.setDefaultButton(QMessageBox.No)
                    if not (msgBox.exec_() == QMessageBox.Yes):
                        return

            # figure out file name
            filename = os.path.abspath(self.locationEdit.text()).replace('%n',str(self.ExperimentNumber)).replace('%b',self.SelectedBlock)
            self.ActualFileName = filename
        
            # rename existing file if necesary
            if os.path.exists(filename):
                # figure out new version of the filename
                lastdot = filename.rfind(".")
                # find the lowest number for which we can rename this to *_oldN.xdf 
                for n in range(1,9999):
                    rename_to = filename[:lastdot] + "_old" + str(n) + filename[lastdot:]
                    if not os.path.exists(rename_to):
                        try:
                            os.rename(filename, rename_to)
                        except:
                            QMessageBox.information(self,"Permissions issue", "Can not rename the file " + filename + " to " + rename_to + ". Please check your permissions.", QMessageBox.OK)
                            return
                        break
        
            # make directories if necessary
            targetdir = os.path.split(filename)[0]
            try:
                if not os.path.exists(targetdir):
                    os.makedirs(targetdir)
            except:
                QMessageBox.information(self,"Permissions issue", "Can not create the directory " + targetdir + ". Please check your permissions.", QMessageBox.Ok)
                return
                
            # create parameters to the recording function...

            into_streams = []
            into_watchfor = [] 
            for i in range(self.streamList.count()):
                item = self.streamList.item(i)                
                if item.checkState() == Qt.Checked:
                    if item.text() in self.MissingStreams:
                        into_watchfor.append("name='" + item.text() + "'")
                    elif item.text() in self.ResolvedNames:
                        into_streams.append(self.ResolvedStreams[self.ResolvedNames.index(item.text())])
            
            # determine the streams to record from...
            num_streams = len(into_streams)
            streams = RecorderLib.new_info(num_streams)
            for i in range(num_streams):
                RecorderLib.info_setitem(streams,i,into_streams[i].impl())
        
            # watchfor
            num_watchfor = len(into_watchfor)
            watchfor = RecorderLib.new_str(num_watchfor)
            for i in range(num_watchfor):
                RecorderLib.str_setitem(watchfor,i,into_watchfor[i].encode("utf-8"))
            
            # start recording
            self.CurrentRecording = RecorderLib.rl_start_recording(filename.encode("utf-8"),streams,num_streams,watchfor,num_watchfor,1)
            self.StartTime = time.time()            
            self.CurrentlyRecording = True
            self.stopButton.setEnabled(True)
            self.startButton.setEnabled(False)
            
        else:
            QMessageBox.information(self,"Already recording", "The recording is already running.", QMessageBox.Ok)
    
    
    def stopRecording(self):
        """ Stop an ongoing recording. """        
        if not self.CurrentlyRecording:
            QMessageBox.information(self,"Not recording", "There is no ongoing recording.", QMessageBox.Ok)
        else:
            # invoke scripted action...
            if self.enableScriptedActionsCheck.checkState():
                try:
                    self.on_stoprecord(self,self.SelectedBlock,self.ExperimentNumber)
                except Exception as e:
                    msgBox = QMessageBox()
                    msgBox.setText("The scripted action associated with stopping the recording failed with error " + str(e))
                    msgBox.setInformativeText("Do you want to stop the recording anyway?")
                    msgBox.setIcon(QMessageBox.Warning)                    
                    msgBox.setStandardButtons(QMessageBox.Yes | QMessageBox.No | QMessageBox.Cancel)
                    msgBox.setDefaultButton(QMessageBox.No)
                    if not (msgBox.exec_() == QMessageBox.Yes):
                        return
                    
            # stop the actual recording
            try:
                RecorderLib.rl_end_recording(self.CurrentRecording)
            finally:
                self.CurrentlyRecording = False
                self.startButton.setEnabled(True)
                self.stopButton.setEnabled(False)
                self.statusBar().showMessage("Stopped.")
        
    def blockSelected(self,item):
        """ Handle selection of a new block. """
        if self.CurrentlyRecording:
            QMessageBox.information(self,"Still recording", "Please stop recording before switching blocks.", QMessageBox.Ok)
        else:            
            self.SelectedBlock = item.text()
            if self.enableScriptedActionsCheck.checkState():
                try:
                    self.on_selectblock(self,self.SelectedBlock)
                except Exception as e:
                    QMessageBox.information(self,"Script issue", "Note: The scripted action associated with selecting a block failed with error " + str(e), QMessageBox.Ok)

    def streamClicked(self,item):
        if item.checkState():
            item.setCheckState(Qt.Unchecked)
        else:
            item.setCheckState(Qt.Checked)



# parse commandline arguments
parser = OptionParser()
parser.add_option("-c", "--config", dest="config_file", default="default_config.cfg", 
                  help="The configuration file to load.")
(options,args) = parser.parse_args()

# start GUI parts
app = QApplication(sys.argv)
frame = MainWindow(options)
frame.show()
app.exec_()
