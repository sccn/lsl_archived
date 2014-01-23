The only external dependency is boost 1.55 or newer. The external folder is only used if you build liblsl as a standalone library with boost compiled in (on Linux and Mac you can alternatively compile and link against shared system-provided boost headers and libraries). 

Note that, when you upgrade your boost libraries you might inadvertently break backwards compatibility with old versions of liblsl (<1.10). In particular, you should check whether boost.serialization has received a breaking change since 1.55.

To build a standalone library with a newer version of boost compiled in, delete the boost, lslboost and src folders and copy the folders boost and libs (not lib!) from boost_1_xx_0 to liblsl/external. Then you have two options: 

a) The recommended way is to run the python script premangle_boost.py from the command line in the external folder, which will prefix the boost namespace internally with the string lsl (this is done transparently by also providing a namespace alias called boost). This step allows you to link liblsl into applications that ship with their own incompatible version of boost without having to care about compiler/linker flags that can strip out conflicting symbols (these can otherwise can cause runtime crashes).

b) The alternative way is to use an unmodified version of boost (if you don't care about conflicts with 3rd party applications) by just renaming the libs folder to src manually.

Notes:
* It is possible that in a future version of boost some of the source files will be named differently than in 1.50 or that extra libraries are required. In this case you will have to update the CMakeLists.txt in this folder or the corresponding project files in order to accomodate the changes.
* liblsl needs only a subset of boost libraries, so if you plan to redistribute the updated source code you might want to delete some unused header and library files to save space.
