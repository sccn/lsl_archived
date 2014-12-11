CC=g++

TARGET=ClickMe
CPPFLAGS = -DWINDOWS='$(IS_WINDOWS)'
INCLUDE = -I/home/dmedine/SCCN/labstreaminglayer/LSL/liblsl/inlude
LIBPATH = -L/home/dmedine/SCCN/labstreaminglayer/LSL/liblsl/bin 
LIBS = -llsl64 -lboost_thread -lQtGui -lQtNetwork -lQtCore

.PHONY = mainwindow.o $(TARGET)

all: mainwindow.o $(TARGET)

moc_mainwindow.o: moc_mainwindow.cpp
	$(CC) $(CPPFLAGS ) $(INLCUDE) $(LIBPATH) $(LIBS) -c -o $@ $^

mainwindow.o: mainwindow.cpp
	$(CC) $(CPPFLAGS ) $(INLCUDE) $(LIBPATH) $(LIBS) -c -o $@ $^ 

$(TARGET): main.o mainwindow.o moc_mainwindow.o
	$(CC) -g -o $@ $^ $(CFLAGS) $(LIBPATH) $(LIBS) -Wl,-rpath=../linux_libs 

clean:
	rm *.o
	rm $(TARGET)
