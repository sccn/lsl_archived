# Overview #

The lab streaming layer distribution includes a default recording program, the LabRecorder, which can be used to record a collection of streams, including all their data (time-series / markers, meta-data) into a single XDF file. The XDF format (Extensible Data Format) is a general-purpose format for time series and their meta-data that was jointly developed with LSL to ensure compatibility, see [here](http://code.google.com/p/xdf/).

After a session has been recorded to disk using the LabRecorder or any other compatible recording application, it can be imported into MATLAB using the MATLAB Importer function (`load_xdf`). There are higher-level importers built on top of `load_xdf` that are integrated with several biosignal analysis toolboxes in MATLAB, including EEGLAB, BCILAB, and MoBILAB. These are available as separate plugins on the downloads page.

# Usage #
  * Start MATLAB and make sure that you have the function added to MATLAB's path. You can do this either through the GUI, under File / Set Path / Add Folder) or in the command line, by typing:

> `addpath('C:\\path\\to\\labstreaminglayer\\Apps\\MATLAB Importer')`

  * To load an .xdf file, type in the command line:

> `streams = load_xdf('your_file_name.xdf')`

  * After a few seconds it should return a cell array with one cell for every stream that was contained in the file. For each stream you get a struct that contains the entire meta-data (including channel descriptions and domain-specific information), as well as the time series data itself (numeric or cell-string array, depending on the value type of the stream), and the time stamps of each sample in the time series. All time stamps (across all streams, even if they were collected on different computers of the lab network) are in the same time domain, so they are synchronized. Note that time stamps from different .xdf files are generally not synchronized (although they will normally be in seconds since the recording machine was turned on).

# Documentation #
As usual in MATLAB, to get the documentation of the function, type `help load_xdf` or `doc load_xdf`. The latter will open a window like the following:

![http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/loadxdf-doc.png](http://wiki.labstreaminglayer.googlecode.com/hg/media/screenshots/loadxdf-doc.png)