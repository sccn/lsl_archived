#!/bin/sh
# Build Octave bindings on 64bit Linux
# You need the package liboctave-dev *or* matlab-support installed and
# either liblsl64.so or a link to it in the bin folder

# This builds with matlab by default, but you can suppress the check by
# setting OCT (e.g. OCT=1 ./build_linux.sh)

if [ ! -e bin/liblsl64.so ]; then
	echo 'liblsl64.so not found in bin/'
	exit 1;
fi

if [ -z "$OCT" ] && [ `which matlab-mex` ]; then
	echo 'Building with matlab-mex'
	buildcmd='matlab-mex'
elif [ `which mkoctfile` ]; then
	echo 'Building with mkoctfile'
	buildcmd='mkoctfile --mex'
else
	echo 'No build command found, exiting...'
	exit 1;
fi

cd bin
for i in ../mex/*.c; do $buildcmd -I../../liblsl/include -L. -llsl64 -ldl ../mex/$i; done
cd ..

octave-cli -p bin examples/LSLVersion.m
