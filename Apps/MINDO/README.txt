== Usage ==

To use this program, you first need to have your MINDO EEG system paired to your computer via Bluetooth. The pairing code for the device is 0000. 

(Side note: If necessary, please consult your operating system documentation on how to pair a device with your computer (on Windows, it essentially amounts to right-clicking the Bluetooth icon in the systray, selecting "Add Device...", picking the MINDO or "SPP Slave" device, selecting the "pair with a pairing code" option, typing in 0000, and waiting until the device is successfully added. As the device is getting paired, it is being assigned a COM port (a.k.a. serial port) number by your OS. On Windows, you see the assigned port number as a pop-up notification in your taskbar (saying something along the lines of "COM3" or some other number). You can also look it up in the device properties after it has been added.)

Now link the device into LSL by putting in the correct port number, the correct number of channels (important!), and the appropriate sampling rate. You should also provide a list of channel labels (although you can make up random labels if you do not require them to be correct).

== Optional ==

The configuration settings can be saved to a .cfg file (see File / Save Configuration) and subsequently loaded from such a file (via File / Load Configuration). Importantly, the program can be started with a command-line argument of the form "MINDO.exe -c myconfig.cfg", which allows to load the config automatically at start-up. The recommended procedure to use the app in production experiments is to make a shortcut on the experimenter's desktop which points to a previously saved configuration customized to the study being recorded to minimize the chance of operator error.