CC=g++

vpath %.c ./
vpath %.h ./

SRCS=RecorderLib.cpp RecorderLib_wrap.cpp
OBJS=$(SRCS:.cpp=.o)
OBJ_PATH=objs

LIBDIR=-L../../../../../LSL/liblsl/bin
LIBS=-llsl64 -lpython2.7 -lboost_system -lboost_iostreams -lboost_thread -lboost_date_time -lboost_chrono -lboost_filesystem -lboost_regex -lboost_atomic -lboost_serialization -lboost_program_options

INCLUDE=-I../../../../../LSL/liblsl/include

CFLAGS=-fPIC -c

.PHONY: RecorderLib64.so

all: RecorderLib_wrap.o RecorderLib.o RecorderLib64.so

RecorderLib_wrap.o: 
	$(CC) RecorderLib_wrap.cpp $(CFLAGS) $(INCLUDE) $(LIBDIR) $(LIBS) -o RecorderLib_wrap.o

RecorderLib.o: 
	$(CC) RecorderLib.cpp $(CFLAGS) $(INCLUDE) $(LIBDIR) $(LIBS) -o RecorderLib.o

RecorderLib64.so:
	$(CC) -shared -g3 -fPIC -Wl,-soname,RecorderLib.so \
	-o RecorderLib64.so $(OBJS) -lc $(LIBDIR) $(LIBS)
	mv *.o $(OBJ_PATH)
	mv RecorderLib64.so ../../
