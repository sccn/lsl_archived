This is the Python interface to the lab streaming layer. 
To use pylsl you need to make sure that the liblsl binary 
for your platform is found (e.g., in your application's working 
directory, or in a system path). 

Pylsl should work with any recent version of liblsl (although you need 
at least 1.10+ to use the particularly speedy chunk transfer functions), 
on any operating system and with any recent Python version, including 
2.5+ and 3.x. Let us know if you encounter any bugs.