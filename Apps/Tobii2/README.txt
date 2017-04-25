This is a stub for the development of LSL compatibility with the Tobii 2 Glasses SDK.

The file tobii2_lsl.py was a first attempt at interacting with the device. It works, but if you run the code and then attempt the analysis in tobii_test.m (no doubt paths and file names will need to be changed) you will see major problems with the time data.

Sent this code back to Tobii, and Estefania Dominuez returned the modified python script tobii2_lsl_sample_by_sample.py, along with the analysis code tobii_test_gp.m.

This appears to work correctly, but it hasn't been thoroughly tested. I'm mostly concerned that adjusting the time stamps handed out by LSL will affect the ability to synchronize later on, but I haven't had the time to test this or think about it carefully.

Furthermore, more bells and whistles would be great. E.g. video display (hard on Windows) as well as a calibration facility. This could all be done by the command line, but a GUI would be nice.