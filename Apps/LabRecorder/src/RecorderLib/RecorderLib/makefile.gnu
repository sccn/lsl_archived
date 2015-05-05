CPP=g++
CC=gcc

CXXFLAGS = -fPIC
INCLUDE = -I../ -I../../../../../../LSL/liblsl/include/ $(shell pkg-config --cflags python)
LIBPATH = -L../../../../../LSL/liblsl/bin -L/lib64/
LIBS = $(shell pkg-config --libs python) -lboost_system -lboost_thread -lboost_iostreams -llsl64 #-lpthread -lc -llibstdc++

SRCS = RecorderLib.cpp RecorderLib_wrap.c
OBJS = $(SRCS:.cpp=.o) $(SRCS:.c=.o)

.PHONY: clean RecorderLib64.so 

#all: RecorderLib64.so rlib.o
#all: rlib.o rlibwrap.o
all: RecorderLib64.so

#rlib.o: RecorderLib.cpp #RecorderLib.h
#	$(CPP) $(LIBPATH) $(LIBS) RecorderLib.cpp -Wl,-rpath=./

#rlibwrap.o: RecorderLib_wrap.c
#	$(CPP) $(INCLUDE) $(LIBS) RecorderLib_wrap.c RecorderLib.cpp

#%.o: %.cpp
.cpp.o:
	$(CPP) $(CXXFLAGS) $(LIBPATH) $(LIBS) $(INCLUDE) -c -o $@ $^

.c.o:
	$(CC) $(CXXFLAGS) $(LIBPATH) $(LIBS) $(INCLUDE) -c -o $@ $^

RecorderLib64.so: $(OBJS)
	$(CPP) -shared RecorderLib.o RecorderLib_wrap.o -o RecorderLib64.so.1.0 $(LIBPATH) $(LIBS)
	ln -s ./RecorderLib64.so.1.0 RecorderLib64.so.1
	ln -s ./RecorderLib64.so.1 RecorderLib64.so

clean:
	rm -f *.so* *.o
