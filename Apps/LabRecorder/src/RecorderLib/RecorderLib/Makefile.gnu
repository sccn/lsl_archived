CPP=g++
CC=gcc

CXXFLAGS = -fPIC
INCLUDE = -I../ -I../../../../../LSL/liblsl/include/ $(shell pkg-config --cflags python)
LIBPATH = -L../../../../../LSL/liblsl/bin -L/lib64/
LIBS = $(shell pkg-config --libs python) -lboost_system -lboost_thread -lboost_iostreams -llsl64

SRCS = RecorderLib.cpp RecorderLib_wrap.c
OBJS = $(SRCS:.cpp=.o) $(SRCS:.c=.o)

.PHONY: clean RecorderLib64.so 

all: RecorderLib64.so

.cpp.o:
	$(CPP) $(CXXFLAGS) $(LIBPATH) $(LIBS) $(INCLUDE) -c -o $@ $^

.c.o:
	$(CC) $(CXXFLAGS) $(LIBPATH) $(LIBS) $(INCLUDE) -c -o $@ $^

RecorderLib64.so: $(OBJS)
	$(CPP) -shared RecorderLib.o RecorderLib_wrap.o -o RecorderLib64.so $(LIBPATH) $(LIBS)
	mv RecorderLib64.so ../../

clean:
	rm -f *.so* *.o
