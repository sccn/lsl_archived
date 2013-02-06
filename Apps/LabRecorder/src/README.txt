
=== Basic Information ===
The LabRecorder is a Python application to record from devices present on the lab streaming layer.
The program records into files in XDF format (see http://code.google.com/p/xdf/ for the ongoing effort 
to build an ecosystem around this file format). The resulting recordings can be imported into MATLAB 
using the load_xdf.m function. If you have a data analysis environment (such as EEGLAB, BCILAB, 
MoBILAB, or FieldTrip) you will most likely want to import the data using higher-level importer function.

See "Getting Started" for how to collect your first data.

=== Getting Started ===
The LabRecorder displays a list of currently present device streams under "Record from Streams". 

* Click the "Refresh" button to update the list after you have started a device.

  For testing several simple dummy devices are in the folder LSL/liblsl/bin (for example SendData, 
  SendStringMarkers, SendDataSimple); you can start any of these programs (and follow the on-screen
  instructions, if any) to bring a stream online. After you ahve done so, clicking refresh in the recorder
  should display the streams in green. 

* You select which streams you want to record from by checking the check boxes next to them.

* The entry in "Storage Location" shows you the file name (or file name template) where your recording
  will be stored. You can change this by clicking the browse button. 
  
  If this string contains any occurrence of %n, it will be replaced by the value in the "Experiment Number"
  field. When you start the recorder or load a configuration file, this field will be set automatically to 
  the lowest number for which no directory exists yet.

  If the string contains any occurrence of %b, it will be substituted by the the selected string under 
  "Current experiment block". This list of pre-defined block names is an advanced feature that can be 
  customized by editing the default_config.cfg configuration file (or creating a new one).
  
  If the respective directory does not yet exist, it will be created automatically (except if you do not
  have the permissions to create it). If the file that you are trying to record to already exists, the 
  existing file will be renamed (the string _oldX will be appended where X is the lowest number that is 
  not yet occupied by another existing file). This way, it is impossible to accidentally overwrite data.  
  
* If the checkbox "Enable scripted actions" is checked, the scripted actions that are defined in your
  current config file will be automatically invoked when you click Start, Stop, or select a block.
  This check box will be disabled except if you are working with a configuration that is custom-tailored 
  to your experimentation environment.
  
* Click "Start" to start a recording. If everything goes well, the status bar will now display the time since
  you started the recording, and more importantly, the current file size (the number before the kb) will grow
  slowly. This is a way to check whether you are still in fact recording data.
  
  The recording program cannot be closed while you are recording (as a safety measure).
  
* When you are done recording, click the "Stop" button. You can now close the program.

  You can inspect your recording in MATLAB by opening the environment, making sure that you have a version of the
  importer function at hand (http://xdf.googlecode.com/files/load_xdf-1.00.zip), and execute the line:
  streams = load_xdf('C:\\Recordings\\CurrentStudy\\exp1\\untitled.xdf')
  
  The variable streams should now contain the data that you just recorded.


=== Getting devices ===

* To record from a particular device with this program, you need a program that makes it accessible to the 
  lab streaming layer. If no such program is available for your device, it would be a good idea to ask
  a programmer and point him/her to the LSL distribution and contained documentation - there are many code 
  examples for how implement a new device component in C or C++. There is also a Python and a MATLAB interface
  for the truly lazy or time-pressured.


=== Preparing a Study ===

* When preparing a new study, it is a good idea to make a custom configuration file which at least sets up a 
  custom storage location for the study. See the documentation in the file default_config.cfg for how to do this --
  it is very easy! The file sample_config.cfg contains a somewhat more advanced setup for reference.

  By default, the file default_config.cfg is loaded by the program upon startup. You can override this default
  by a) making a shortcut for the LabRecorder program (e.g. on the desktop) and b) adding in its properties
  the command-line arguments "-c name_of_you_config.cfg" (without the quotes). You can also create a batch script.
  
  You can also load the config while the program is already running, but this can easily be forgotten during an
  experiment, so we recommend to follow the shortcut route.
  
* In addition to the storage location, if your experiment has multiple blocks (e.g., SubjectTraining, PreBaseline, 
  MainBlock, PostBaseline or the like) you can make the recording process even more straightforward for the 
  experimenters by setting up a default list of block names. Again, take a look at the existing config files.
  
* Because it is very easy to forget to start or check all necessary recording devices for a study, we recommend
  to also make a list of "required" streams (by their name) and put it into the config file. These streams will
  be pre-checked when starting the program, and any missing stream will be displayed in red. If the stream is 
  still not green when starting the recording, the experimenter will get a message box to confirm that he/she
  wants to record without including the device.

* Advanced users might consider automating further steps in the recording process (for example starting the experiment
  script itself), which can be done by assinging scripted actions in the configuration file. The file sample_config.cfg 
  contains pre-defined actions to remote-control the SNAP experimentation environment. In particular, when you 
  click a block, the corresponding SNAP .cfg file will be loaded, and when you click start, the module will be started.
  
  Note, however, that scripted actions carry the risk of accidentally crashing the recording program, so they 
  need to be carefully tested. One subtle considerations is that the module definitions happen in the context 
  of the main class of the LabRecorder.
  

=== Other Features ===

* The LabRecorder has some useful features that can add robustness if things go wrong during the experiment:
  
  If a network connectivity error happens while recording (e.g., a network cable pops out that connects to the source
  of a data stream), you have 6 minutes (think 5) to plug it back it in during which the data will be buffered on
  the sending machine. If it takes you longer to fix the problem, you will have some gap in your recording.
  
  If a device program or computer crashes while recording, you will for sure lose data, but any device program
  that transmits an associated device serial number will be picked up automatically by the recorder when it comes
  back online (these programs are called "recoverable"), without a need to stop and re-start the recording.
  
  You should check the health of your device to be sure, however, for example using an online stream viewer program.
  Also, be sure to test whether it is in fact recoverable before relying on this feature.
  
  If a device is displayed in red when you start recording (and it is checked), it will be added to the ongoing 
  recording by the time when it comes online. This can be useful when a device necessarily starts while the 
  recording is already ongoing. Again, it is advisable to check that the device is in fact discoverable and added.
  The LabRecorder brings up a console window in the background which shows a list of all streams that are added
  to the recording -- this is a good place to check whether a late stream did get picked up successfully during a 
  live recording.
  
