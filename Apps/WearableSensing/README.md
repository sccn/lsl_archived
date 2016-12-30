#dsi2lsl#

Use this program to stream data acquired by a [Wearable Sensing](http://www.wearablesensing.com) DSI headset to the local area network (LAN) via the [LSL](https://github.com/sccn/labstreaminglayer) library.
Below we list the build instructions (including Bluetooth troubleshooting steps) for GNU/Linux, Windows, and Mac operating systems.

In addition, in the folder **dsi2lslgui/**, we provide a Qt GUI layer that can be deployed on top of the command line app **dsi2lsl**. After built, place **dsi2lslgui** binary in the same folder where **dsi2lsl** and all the other libraries are.


#Build#

Note: Before we can build the project, we need to download Wearable Sensing's API library and save it in this folder. For example, we would need libDSI-Linux-x86_64.so on GNU/Linux, libDSI-Darwin-x86_64.dylib on Mac, and/or libDSI-Windows-x86_32.dll on Windows.

##GNU/Linux##

1. Open a terminal and move to this folder.

2. Compile the project,
```bash
gcc -DDSI_PLATFORM=-Linux-x86_64  -o "dsi2lsl" dsi2lsl.c  DSI_API_Loader.c -ldl -L ../../LSL/liblsl/build/src/ -llsl
```

3. Configure Bluetooth. Pair the DSI device using the Bluetooth device manager. If you don't have the Bluetooth stack properly installed do so using the following command,
```bash
sudo apt-get install bluez bluez-tools
```

4. Find the serial port that corresponds to the device. To accomplish this we need to find the device's address with hcitool,
  and then bind to it,
``` bash
hcitool scan
rfcomm bind /dev/rfcomm0 xx:xx:xx:xx:xx:xx 1
```
where xx:xx:xx:xx:xx:xx represents the output of hcitool (see [this](http://www.westernwillow.com/cms/blog/franco/creating-bluetooth-serial-port-ubuntu) example). We'll need this info later at run time.


##Mac##

1. Open a terminal and move to this folder.

2. Compile the project,
```bash
gcc -DDSI_PLATFORM=-Darwin-x86_64  -o "dsi2lsl" dsi2lsl.c  DSI_API_Loader.c -ldl -L ../../LSL/liblsl/build/src/ -llsl
```

3. Add the DSI device through the Bluetooth manager, then find the port using the following command,
```bash
ls -l tty.*
```
for us this returned /dev/tty.DSI7-017-BluetoothSeria. We'll need this info later at run time.


##Windows##

1. Open a command prompt and move to this folder.

2. Compile the project. We used MinGW distribution of gcc as the compiler (see a tutorial [here](http://www.mingw.org/wiki/howto_install_the_mingw_gcc_compiler_suite)), out of the box install instructions worked. Other compilers such as MS Visual C++ may work as well.
```bash
gcc -DDSI_PLATFORM=-Windows-x86_32 -o "dsi2lsl.exe" dsi2lsl.c  DSI_API_Loader.c -ldl -L ../../LSL/liblsl/build/src/ -llsl32
```

3. Add the DSI device using the Bluetooth manager, then find the port by right clicking on the device icon and clicking Properties/Hardware. For us the port was COM49. We'll need this info later at run time.


#Examples#

The app runs in the same way on the three platforms. Below we illustrate the different options that can be used. With the exception of the option --help, all the others should be given in --NAME=VALUE format. Use a terminal to run the examples.

Tip: Linux and Mac users
      1. Since the app uses the serial port to exchange data with the device, your account may need to have superuser privileges (use sudo before any command).
      2. You may need to add this folder to the system's library path, to do so use the following command
``` bash
export LD_LIBRARY_PATH=/.../dsi2lsl_deploy_folder:$LD_LIBRARY_PATH
```

### --port ###
Specify the serial port with the --port option. If not specified, the API will look for an environment variable called DSISerialPort. This example uses the serial port that the device is binded to on GNU/Linux, for other platforms see the previous section.

``` bash
./dsi2lsl --port=/dev/rfcomm0
```

### --lsl-stream ###
Specify the name of the LSL outlet that will stream the data to the local network. If omitted, the default name WS-default will be used

``` bash
./dsi2lsl --port=/dev/rfcomm0 --lsl-stream-name=mystream
```

### --montage ###
Specify the montage, a list of channel specifications, comma-separated without spaces, (can also be space-delimited, but then you would need to enclose the option in quotes on the command-line). If omitted all available channels will be used.

``` bash
./dsi2lsl --port=/dev/rfcomm0 --lsl-stream-name=mystream --montage=F3,C3,P3,P4,C4,F4,Pz
```

### --reference ###
Specify the reference, the name of sensor (or linear combination of sensors, without spaces) to be used as reference. Defaults to a \"traditional\" averaged-ears or averaged-mastoids reference if available, or the factory reference (typically Pz) if these sensors are not available.

``` bash
./dsi2lsl --port=/dev/rfcomm0 --lsl-stream-name=mystream --montage=F3,C3,P3,P4,C4,F4,Pz --reference Pz
```
