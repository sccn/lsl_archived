# Overview #

The LabRecorder is the default recording program that comes with LSL. It allows to record all streams on the lab network (or a subset) into a single file, with time synchronization between streams.

# File Format #

The file format used by the LabRecorder is XDF. This is a new open general-purpose format that was designed concurrently with LSL and supports all features of LSL streams. The project page is [here](http://code.google.com/p/xdf/). There are importers for MATLAB (included with the distribution), EEGLAB, BCILAB, and MoBILAB (available separately on the Downloads page).


# Installation #

This program requires that you have installed either [Python-2.6](http://www.python.org/ftp/python/2.6/python-2.6.msi) and [PySide for Python 2.6](http://releases.qt-project.org/pyside/PySide-1.1.1qt474.win32-py2.6.exe) or [Python-2.7](http://www.python.org/ftp/python/2.7/python-2.7.msi) and [PySide for Python 2.7](http://releases.qt-project.org/pyside/PySide-1.1.1qt474.win32-py2.7.exe) (note: if any of the links are broken, just google for Python or Pyside, respectively). If you are using SNAP for experiment design, we recommend Python 2.7 (in fact you might want to install the SNAP runtime first, which includes the packages needed by the LabRecorder).

# Usage #

  * The LabRecorder displays a list of currently present device streams under "Record from Streams". If you have turned on a device after you have already started the recorder, click the "Refresh" button to update the list (this takes ca. 2 seconds). For testing there are several simple dummy devices in the folder labstreaminglayer/LSL/liblsl/bin (for example SendData, SendStringMarkers, and SendDataSimple); you can start any of these programs (and follow the on-screen instructions, if any) to bring a stream online. After you have done so, clicking refresh in the recorder should display the streams in green. If you cannot see streams that are provided on another computer, read the section Network Troubleshooting on the NetworkConnectivity page. You can select which streams you want to record from and which not by checking the check boxes next to them. By default any present stream will be initially checked, as in the follwing picture.
> ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/labrecorder-default.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/labrecorder-default.png)

  * The entry in "Storage Location" shows you the file name (or file name template) where your recording will be stored. You can change this by clicking the browse button. If this string contains any occurrence of the placeholder `%n`, it will be replaced by the value in the "Experiment Number" field. The value in that field is "smart": when you start the recorder or load a configuration file, this field will be set automatically to the lowest number for which no directory exists yet on disk. If the string contains any occurrence of `%b`, it will be substituted by the the selected string under "Current experiment block". This list of pre-defined block names is feature for advanced uses that can be customized by editing the `default_config.cfg` configuration file (or creating a new one). If the respective directory does not yet exist, it will be created automatically (except if you do not have the permissions to create it). If the file that you are trying to record to already exists, the existing file will be renamed (the string `_oldX` will be appended where X is the lowest number that is not yet occupied by another existing file). This way, it is impossible to accidentally overwrite data.

  * If the checkbox "Enable scripted actions" is checked, then scripted actions that are defined in your current config file will be automatically invoked when you click Start, Stop, or select a block. This check box is by normally unchecked unless you have custom-tailored a configuration to your experiment or experimentation environment.

  * Click "Start" to start a recording. If everything goes well, the status bar will now display the time since you started the recording, and more importantly, the current file size (the number before the kb) will grow slowly. This is a way to check whether you are still in fact recording data. The recording program cannot be closed while you are recording (as a safety measure). This looks like in the following picture.
> ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/labrecorder-running.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/labrecorder-running.png)

  * When you are done recording, click the "Stop" button. You can now close the program. You can now import and inspect your recording, see for example ImportingRecordingsInMatlab.

# Preparing a Full Study #

  * When preparing a new study, it is a good idea to make a custom configuration file which at least sets up a custom storage location for the study. See the documentation in the file `default_config.cfg` for how to do this -- it is very easy! The file `sample_config.cfg` contains a somewhat more advanced setup for reference. By default, the file `default_config.cfg` is loaded at startup. You can override this by making a shortcut for the LabRecorder program (e.g. on the desktop) and appending in its properties the command-line arguments `-c name_of_you_config.cfg`. You can also create a batch script. You can also load the config while the program is already running, but this can easily be forgotten during an experiment, so we recommend to follow the shortcut route.

  * In addition to the storage location, if your experiment has multiple blocks (e.g., SubjectTraining, PreBaseline, MainBlock, PostBaseline or the like) you can make the recording process even more straightforward for the experimenters by setting up a default list of block names. Again, take a look at the existing config files.

  * Since it is too easy to forget to turn on or check all necessary recording devices for a study, we recommend to also make a list of "required" streams (by their name) and put it into the config file. These streams will be pre-checked when starting the program, and any missing stream will be displayed in red. If such a stream is still not green when starting the recording, the experimenter will get a message box to confirm that he/she really wants to record without including the device.

  * Advanced users might consider automating further steps in the recording process (for example starting the experiment script itself), which can be done by assinging scripted actions in the configuration file. The file `sample_config.cfg` contains pre-defined actions to remote-control the SNAP experimentation environment. In particular, when you click a block, the corresponding SNAP .cfg file will be loaded, and when you click start, the module will be started. Keep in mind, however, that homegrown scripted actions carry the risk of accidentally crashing the recording program, so they need to be carefully tested. One subtle consideration is that the module definitions happen in the context of the main class of the LabRecorder.

  * The following picture shows the the recorder fully configured for a particular study. The BioSemi device is displayed in red because it is not yet turned on. The file naming scheme is customized, and there are four blocks pre-configured with associated scripted actions.
> ![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/labrecorder-study.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/labrecorder-study.png)

# Misc Features #

The LabRecorder has some useful features that can add robustness if things go wrong during the experiment:

If a network connectivity error happens while recording (e.g., a network cable pops out that connects to the source of a data stream), you have usually 6 minutes (think 5) to plug it back it in during which the data will be buffered on the sending machine. If it takes you longer to fix the problem, you will have some gap in your recording.

If a device program or computer crashes while recording, you will for sure lose data, but any device program that transmits an associated device serial number will be picked up automatically by the recorder when it comes back online (these programs are called "recoverable"), without a need to stop and re-start the recording.

You should check the health of your device to be sure, however, for example using an online stream viewer program (see, for example, ViewingStreamsInMatlab). Also, be sure to test whether it is in fact recoverable before relying on this feature (you can test this with a viewer by turning the device app off and back on).

If a device is displayed in red when you start recording (and it is checked), it will be added to the ongoing recording by the time when it comes online. This can be useful when a device can only be turned on while the recording is already in progress. Again, it is advisable to check that the device is in fact discoverable and added. The LabRecorder brings up a console window in the background which shows a list of all streams that are added to the recording -- this is a good place to check whether a late stream did get picked up successfully during a live recording.

# Note for SCCN Users #

If you are at the Swartz Center you will probably want to install our preferred Python distribution, in particular if you intend to be developing for SNAP (our stimulus presentation environment). The files and instructions can be found at [ftp://sccn.ucsd.edu/pub/software/third_party/build_environment/](ftp://sccn.ucsd.edu/pub/software/third_party/build_environment/)