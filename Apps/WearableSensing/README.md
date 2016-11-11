#dsi2lsl#

Use this program to stream real-time data acquired by a [Wearable Sensing](http://www.wearablesensing.com) DSI headset to the local area network (LAN) via the [LSL](https://github.com/sccn/labstreaminglayer) library.
Below we list the build instructions (including Bluetooth troubleshooting steps) for GNU/Linux, Windows, and Mac. All commands need to be typed on a terminal.

For deployment just copy the subfolder inside Deploy/ that correspond to your OS.

Note: You only need to re-build the app if for some reason the binary in the Deploy folder doesn't run on your system.

#Build#

##GNU/Linux##

1. Move to the dsi2lsl/ folder. Substitute "/.." by the specific full path to the dsi2lsl folder in your file system.

```bash
cd /../dsi2lsl
```

2. Compile the project,

```bash
gcc -DDSI_PLATFORM=-Linux-x86_64  -o "Deploy/Linux/dsi2lsl" dsi2lsl.c  DSI_API_Loader.c -ldl  -L /../dsi2lsl/Deploy/Linux -llsl
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

1. Move to the dsi2lsl/ folder as before,

```bash
cd /../dsi2lsl
```

2. Compile,

```bash
gcc -DDSI_PLATFORM=-Darwin-x86_64  -o "Deploy/Mac/dsi2lsl" dsi2lsl.c  DSI_API_Loader.c -ldl  -L /../dsi2lsl/Deploy/Mac -llsl64
```

3. Add the DSI device through the Bluetooth manager, then find the port using the following command,

```bash
ls -l tty.*
```
      
for us this returned /dev/tty.DSI7-017-BluetoothSeria. We'll need this info later at run time.


##Windows##

1. Move to the dsi2lsl/ folder as before,

```bash
cd /../dsi2lsl
```

2. Compile the project. We used out of the box MinGW distribution of gcc as the compiler (see a tutorial [here](http://www.mingw.org/wiki/howto_install_the_mingw_gcc_compiler_suite)), although MS Visual C++ and others may work as well,

```bash
gcc -DDSI_PLATFORM=-Windows-x86_32 -o "Deploy/Windows/dsi2lsl.exe" dsi2lsl.c  DSI_API_Loader.c -ldl  -L /../dsi2lsl/Deploy/Windows -llsl32
```

3. Add the device using the Bluetooth manager, then find the port by right clicking on the device and clicking Properties/Hardware. For us the port was COM49. We'll need this info later at run time.


#Examples#

The app runs in the same way on the three platforms. Below we illustrate the different options that can be used. With the exception of the option --help, all the others should be given in --NAME=VALUE format. Use a terminal to run the examples.

Note:
      1. Linux users, since the app uses the serial port to exchange data with the device, your account may need to have superuser privileges (use sudo before any command).
      2. Linux and Mac users may need to add the path the the folder containing the binary and libraries to the system library path, if so use the following command
      
``` bash
export LD_LIBRARY_PATH=/.../dsi2lsl_deploy_folder:$LD_LIBRARY_PATH
```

### --port ###
Specify the serial port with the --port option. If not specified, the API will look for an environment variable called DSISerialPort. This example uses the serial port that the device is binded to on GNU/Linux, for other platforms see the previous secton.

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
