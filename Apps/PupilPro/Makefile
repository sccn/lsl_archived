CC=g++

TARGET=PupilPro
QTINCLUDE= 
INCLUDE = -I../../LSL/liblsl/include -I./zmq/include -I/usr/include/QtGui -I./ 
CPPFLAGS = -DWINDOWS='$(IS_WINDOWS)'
LIBPATH = -L../../LSL/liblsl/bin -L./zmq/bin
LIBS = -llsl64 -lboost_thread -lQtGui -lQtNetwork -lQtCore -lzmq

SRCS = mainwindow.cpp moc_mainwindow.cpp main.cpp
OBJS = $(SRCS:.c=.o)


.PHONY = $(TARGET)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -g -o $@ $(OBJS) $(INCLUDE) $(CFLAGS) $(LIBPATH) $(LIBS) -Wl,-rpath=../linux_libs -Wl,-rpath=./zmq/bin/linux

.cpp.o:
	$(CC) $(INCLUDE) $(CPPFLAGS)  $(LIBPATH) $(LIBS) -c -o $@ $^



clean:
	rm *.o
	rm $(TARGET)
