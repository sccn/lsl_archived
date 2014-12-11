CC=g++

TARGET=ClickMe
QTINCLUDE= 
INCLUDE = -I../../LSL/liblsl/include -I./zmq/include -I/usr/include/QtGui -I./ 
CPPFLAGS = -DWINDOWS='$(IS_WINDOWS)'
LIBPATH = -L../../LSL/liblsl/bin 
LIBS = -llsl64 -lboost_thread -lQtGui -lQtNetwork -lQtCore 

SRCS = mainwindow.cpp moc_mainwindow.cpp main.cpp
OBJS = $(SRCS:.c=.o)


.PHONY = $(TARGET)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -g -o $@ $(OBJS) $(INCLUDE) $(CFLAGS) $(LIBPATH) $(LIBS) -Wl,-rpath=../linux_libs -Wl,-rpath=../linuxlibs

.cpp.o:
	$(CC) $(INCLUDE) $(CPPFLAGS)  $(LIBPATH) $(LIBS) -c -o $@ $^



clean:
	rm *.o
	rm $(TARGET)


