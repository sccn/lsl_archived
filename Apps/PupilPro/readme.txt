This program is meant to translate messages from the pupil_pro python application into LSL streams. pupil_pro will output various messages depending on how the source code is written, see https://github.com/pupil-labs/pupil/wiki/Pupil-Capture#pupil-server for more information. As of now, this applicaiton outputs the following data, in the following order:


confidence
norm_gazeX
norm_gazeY
apparent_pupil_size
norm_pupilX
norm_pupilY
timestamp (pupil_pro's timestamp, that is)

This application builds using Visual Studio 2008 and the provided makefile will work on most Linux distros (provided the correct dependencies are installed). You will need the Qt sdk installed as well as labstreaminglayer. Coming soon is a Macintosh XCode project so that it will build on Macs as well.