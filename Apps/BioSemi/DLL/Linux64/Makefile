ifndef PKG_CONFIG_PATH
ifndef LIBUSB_PREFIX
$(error Environment variable PKG_CONFIG_PATH or LIBUSB_PREFIX must be defined)
endif
endif

ALL: liblabview_dll.so  Labview_DLL_SyncTest

labview_dll_synctest.o: labview_dll_synctest.cpp
	$(CXX) -Wunused-variable -c -o $@ $^

Labview_DLL_SyncTest: labview_dll_synctest.o
	$(CXX)  -o $@ $^ -L. -llabview_dll

labview_dll.o: labview_dll.cpp
	$(CXX) -Wunused-variable -fPIC -c -o $@ $^

liblabview_dll.so.0.0.1: labview_dll.o bsif_libusb.o
ifdef PKG_CONFIG_PATH
	$(CXX) -o $@ -fPIC -lc -shared -Wl,-soname,liblabview_dll.so.0 \
		$^ `pkg-config --libs libusb-1.0`
else
	$(CXX) -o $@ -fPIC -lc -shared -Wl,-soname,liblabview_dll.so.0 \
		$^ -L${LIBUSB_PREFIX}/lib -l usb-1.0
endif

# make the linker name and soname pointers
liblabview_dll.so: liblabview_dll.so.0.0.1
	ln -sf $^ liblabview_dll.so
	/sbin/ldconfig -n .

#bsif_libusb.o: bsif_libusb.cpp
#ifdef PKG_CONFIG_PATH
#	$(CXX) -Wunused-variable -fPIC -c -o $@ $^ `pkg-config --cflags libusb-1.0`
#else
#	$(CXX) -Wunused-variable -fPIC -c -o $@ $^ -I${LIBUSB_PREFIX}/include
#endif

clean:
#	rm -f bsif_libusb.o
	rm -f labview_dll.o
	rm -f labview_dll_synctest.o
	rm -f liblabview_dll.so.0
	rm -f liblabview_dll.so
	rm -f liblabview_dll.so.0.0.0
	rm -f Labview_DLL_SyncTest
