== Usage ==

To use this program, you first need to have the Wearable Sensing DSI device paired to your computer via Bluetooth.
In this example we used a Debian GNU/Linux system. If you haven't installed the Bluetooth stack you can do so with
the following commands on the terminal:

sudo apt-get install bluez bluez-tools

Once the Bluetooth Manager shows up in your Control Center menu, click on it and hit "Search" to discover the DSI device.
Then, select the DSI device from the list, hit "Setup", and follow the steps (the setup is quite straightforward).


If you need to recompile the project use the following commands:

# 1- Move to the labstreaminglayer/ folder. Observe that you have to substitute "/.." by the specific full path in your file system.
cd /../labstreaminglayer

# 2- Compile with gcc. Observe that after the -L option you have to substitute "/.." by the specific full path in your file system.
gcc -DDSI_PLATFORM=-Linux-x86_64  -o "dsi2lsl" dsi2lsl.c  DSI_API_Loader.c -ldl  -L /../labstreaminglayer/LSL/liblsl/output/linux/src -llsl


Running the app:
With the exception of the option --help, all the others should be given in --NAME=VALUE format. Since the app uses the serial port to exchange
data with the device, make sure that your account can execute sudo commands.

Examples (run in a terminal):

# Specify the serial port with the --port option. If not specified, the API will look for an environment variable called DSISerialPort.
sudo ./dsi2lsl --port=/dev/rfcomm0

# Specify the name of the LSL outlet that will stream the data to the local network. If omitted, the default name WS-default will be used.
sudo ./dsi2lsl --port=/dev/rfcomm0 --lsl-stream-name=mystream

# Specify the montage, a list of channel specifications, comma-separated without spaces, (can also be space-delimited, but then you would
  need to enclose the option in quotes on the command-line). If omitted all available channels will be used.
sudo ./dsi2lsl --port=/dev/rfcomm0 --lsl-stream-name=mystream --montage==F3,C3,P3,P4,C4,F4,Pz

# Specify the reference, the name of sensor (or linear combination of sensors, without spaces) to be used as reference. Defaults to a
  \"traditional\" averaged-ears or averaged-mastoids reference if available, or the factory reference (typically Pz) if these sensors
  are not available.
sudo ./dsi2lsl --port=/dev/rfcomm0 --lsl-stream-name=mystream --montage==F3,C3,P3,P4,C4,F4,Pz --reference Pz
