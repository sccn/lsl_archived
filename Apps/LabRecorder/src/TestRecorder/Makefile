CC=g++

TARGET=TestRecorder
QTINCLUDE= 
INCLUDE = -I../../../../LSL/liblsl/include -I../RecorderLib/RecorderLib
CPPFLAGS = -DWINDOWS='$(IS_WINDOWS)'
LIBPATH = -L../../../../LSL/liblsl/bin
LIBS = -llsl64 -lboost_thread -lboost_iostreams

SRCS = TestRecorder.cpp
OBJS = $(SRCS:.c=.o)


.PHONY = $(TARGET)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -g -o $@ $(OBJS) $(INCLUDE) $(CFLAGS) $(LIBPATH) $(LIBS) -Wl,-rpath=./linux_libs

.cpp.o:
	$(CC) $(INCLUDE) $(CPPFLAGS)  $(LIBPATH) $(LIBS) -c -o $@ $^



clean:
	rm *.o
	rm $(TARGET)
