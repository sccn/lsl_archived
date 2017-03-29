Deprecated Methodology. See LSL/liblsl-Android/Android Studio for current procedure.

Build Instructions for Android Examples run under visual studio:
Install Visual Studio 2013.
Install NVIDIA Codeworks for Android 1R3 (https://developer.nvidia.com/codeworks-android)

Connect Android device through USB
Go to settings -> About Tablet. Tap "build number" seven times.
Go to Settings -> Developer and activate "USB debugging".

Open Visual Studio.
Quick Debug: 
Select Debug and your processor type (ARM, x86, x64, or AArch64, if unsure go with ARM).
Hit the green arrow.
A stream of text markers should be emitted from the device and be viewable on your connected computer.

Quickish Release:
Select Release and your processor type.
Build -> Build Solution
Add C:\NVPACK\android-sdk-windows\platform-tools to your system path. (In View Advanced System Settings)
Open a command prompt.
Go to the release folder (LSL\liblsl\project\android-vs2013\SendStringMarkers\$(PLATFORM)\Release\
adb install -r SendStringMarkers.apk
Find SendStringMarkers on the device and select it.

Slow, Fat Release:
Select release.
Cycle through all processor types except for Tegra, right clicking on liblsl and selecting build for each.
Select Tegra-Android
Right click SendStringMarkers and select build.
Go to the release folder (LSL\liblsl\project\android-vs2013\SendStringMarkers\Tegra-Android\Release\
adb install -r SendStringMarkers.apk (this should work on any android platform).
Find SendStringMarkers on the device and select it.

Android, Visual Studio projects made by Matthew Grivich, Neurobehavioral Systems, matthew_grivich@neurobs.com
