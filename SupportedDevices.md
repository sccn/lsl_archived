# Overview #

The lab streaming layer was originally developed to facilitate human-subject experiments that involve multi-modal data acquisition, including both brain dynamics (primarily EEG), physiology (EOG, EMG, heart rate, respiration, skin conductance, etc.), as well as behavioral data (motion capture, eye tracking, touch interaction, facial expressions, etc.) and finally environmental and program state (for example, event markers).

# Supported EEG Hardware #
The majority of EEG systems on the market are currently compatible with LSL. The following systems are supported by programs included in the LSL distribution (untested systems marked with a (u)):
  * [ABM B-Alert X4/X10/X24 wireless](BAlert.md) (u)
  * [BioSemi Active II Mk1 and Mk2](Biosemi.md)
  * [Brain Products ActiChamp series](ActiChamp.md)
  * [Brain Products BrainAmp series](BrainAmpSeries.md)
  * [BrainVision RDA client](BrainVisionRDA.md)
  * [Cognionics dry/wireless](Cogionics.md)
  * [Enobio dry/wireless](Enobio.md) (u)
  * [g.Tec g.USBamp](GUSBAmp.md)
  * g.Tec g.HIamp (u)
  * [MINDO dry/wireless](MINDO.md)
  * [Neuroscan Synamp II and Synamp Wireless](Neuroscan.md) (u)
  * [EGI AmpServer](EGIAmpServer.md)

The following systems are also supported by a separate program, the [OpenViBE acquisition server](OVAS.md) (unstable systems marked with a (u)):
  * Brain Products QuickAmp, V-Amp, and BrainAmp series
  * g.USBamp
  * Emotiv EPOC
  * Micromed SD LTM
  * MindMedia NeXus32
  * OpenEEG ModularEEG and MonolithEEG
  * TMSi Porti32 and Refa32
  * ANT Neuro ASALAB EEG
  * EGI NetAmp (u)
  * Mitsar EEG 202 (u)
  * CTF/VSM (u)

# Supported Eye Tracking Hardware #
Several eye tracking systems are currently supported by LSL and included in the distribution (untested systems marked with a (u)):
  * [SMI iViewX](IViewX.md)
  * [SMI Eye Tracking Glasses](IViewNG.md)
  * [Tobii Eye trackers](Tobii.md) (u)
  * SR Research Eyelink (very basic)
  * Custom 2-camera eye trackers (with some hacking)

# Supported Human Interface Hardware #
A wide range of Windows-compatible input hardware is supported by LSL and included with the distribution:
  * [Computer mice, trackballs, presenters, etc.](Mouse.md)
  * [Computer keyboards](Keyboard.md)
  * [DirectX-compatible joysticks, wheels, gamepads and other controllers](GameController.md)
  * [Nintendo Wiimote and official expansions](Wiimote.md)

# Supported Motion Capture Hardware #
Several motion-capture systems are currently supported by LSL and included in the distribution. These are:
  * [PhaseSpace](PhaseSpace.md)
  * [NaturalPoint OptiTrack](OptiTrack.md) (some versions)
  * [Microsoft Kinect](KinectMocap.md)
  * [AMTI force plates with serial I/O](AMTIForcePlate.md)

# Supported Multimedia Hardware #
Support for standard Windows-compatible multimedia hardware is included:
  * [PC-compatible sound cards](AudioCaptureWin.md)
  * DirectShow-compatible video hardware

# Miscellaneous Hardware #
The following miscellaneous hardware is supported:
  * [Generic serial port](SerialPort.md)