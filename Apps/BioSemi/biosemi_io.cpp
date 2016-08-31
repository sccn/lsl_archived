#include "biosemi_io.h"

#include <iostream>
#include <exception>
#include <vector>
#include <time.h>

#include <boost/lexical_cast.hpp>

// DLL location, depending on platform
#ifdef _WIN32
	#ifdef _WIN64
		const char *dllpath = "DLL\\Win64\\Labview_DLL.dll";
	#else
		const char *dllpath = "DLL\\Win32\\Labview_DLL.dll";
	#endif
#else
	#ifdef __macosx__
		const char *dllpath = "DLL/Mac/liblabview_dll.0.0.1.dylib";
	#else
		#ifdef _LP64
			const char *dllpath = "DLL/Linux64/liblabview_dll.so";
		#else
			const char *dllpath = "DLL/Linux32/liblabview_dll.so";
		#endif
	#endif
#endif


// maximum waiting time when trying to connect
const float max_waiting_time = 3.0;
// size of the initial probing buffer
const int buffer_bytes = 8*1024*1024;
// preferred size of final buffer (note: must be a multiple of 512; also, ca. 32MB is a good size according to BioSemi)
const int buffer_samples = 60*512;

// 64 zero bytes
const unsigned char msg_enable[64] = {0};
// 0xFF followed by 63 zero bytes
const unsigned char msg_handshake[64] = {255,0};

// library handling
#ifdef _WIN32
#include <windows.h>
#define LOAD_LIBRARY(lpath) LoadLibraryA(lpath)
#define LOAD_FUNCTION(lhandle,fname) GetProcAddress((HMODULE)lhandle,fname)
#define FREE_LIBRARY(lhandle) FreeLibrary((HMODULE)lhandle)
#else
#include <dlfcn.h>
#define LOAD_LIBRARY(lpath) dlopen(lpath,RTLD_NOW | RTLD_LOCAL)
#define LOAD_FUNCTION(lhandle,fname) dlsym(lhandle,fname)
#define FREE_LIBRARY(lhandle) dlclose(lhandle)
#endif


biosemi_io::biosemi_io() {
    boost::uint32_t start_idx; // index of the first sample that was recorded
    boost::uint32_t cur_idx;   // index past the current sample

    // === load the library & obtain DLL functions ===

    std::cout << "Loading BioSemi driver dll..." << std::endl;
    hDLL_ = LOAD_LIBRARY(dllpath);
    if (!hDLL_)
        throw std::runtime_error("Could not load BioSemi driver DLL.");
    OPEN_DRIVER_ASYNC = (OPEN_DRIVER_ASYNC_t)LOAD_FUNCTION(hDLL_,"OPEN_DRIVER_ASYNC");
    if (!OPEN_DRIVER_ASYNC) {
        FREE_LIBRARY(hDLL_);
        throw std::runtime_error("Did not find function OPEN_DRIVER_ASYNC() in the BisoSemi driver DLL.");
    }
    USB_WRITE = (USB_WRITE_t)LOAD_FUNCTION(hDLL_,"USB_WRITE");
    if (!USB_WRITE) {
        FREE_LIBRARY(hDLL_);
        throw std::runtime_error("Did not find function USB_WRITE() in the BisoSemi driver DLL.");
    }
    READ_MULTIPLE_SWEEPS = (READ_MULTIPLE_SWEEPS_t)LOAD_FUNCTION(hDLL_,"READ_MULTIPLE_SWEEPS");
    if (!READ_MULTIPLE_SWEEPS) {
        FREE_LIBRARY(hDLL_);
        throw std::runtime_error("Did not find function READ_MULTIPLE_SWEEPS() in the BisoSemi driver DLL.");
    }
    READ_POINTER = (READ_POINTER_t)LOAD_FUNCTION(hDLL_,"READ_POINTER");
    if (!READ_POINTER) {
        FREE_LIBRARY(hDLL_);
        throw std::runtime_error("Did not find function READ_POINTER() in the BisoSemi driver DLL.");
    }
    CLOSE_DRIVER_ASYNC = (CLOSE_DRIVER_ASYNC_t)LOAD_FUNCTION(hDLL_,"CLOSE_DRIVER_ASYNC");
    if (!CLOSE_DRIVER_ASYNC) {
        FREE_LIBRARY(hDLL_);
        throw std::runtime_error("Did not find function CLOSE_DRIVER_ASYNC() in the BisoSemi driver DLL.");
    }


    // === initialize driver ===

    // connect to driver
    std::cout << "Connecting to driver..." << std::endl;
    hConn_ = OPEN_DRIVER_ASYNC();
    if (!hConn_) {
        FREE_LIBRARY(hDLL_);
        throw std::runtime_error("Could not open connection to BioSemi driver.");
    }

    // initialize USB2 interface
    std::cout << "Initializing USB interface..." << std::endl;
    if (!USB_WRITE(hConn_,&msg_enable[0])) {
        CLOSE_DRIVER_ASYNC(hConn_);
        FREE_LIBRARY(hDLL_);
        throw std::runtime_error("Could not initialize BioSemi USB2 interface.");
    }


    // === allocate ring buffer and begin acquisiton ===

    // initialize the initial (probing) ring buffer
    std::cout << "Initializing ring buffer..." << std::endl;
    ring_buffer_ = new boost::uint32_t[buffer_bytes];
    if (!ring_buffer_) {
        CLOSE_DRIVER_ASYNC(hConn_);
        FREE_LIBRARY(hDLL_);
        throw std::runtime_error("Could not allocate ring buffer (out of memory).");
    }
    memset(ring_buffer_,0,buffer_bytes);

    // begin acquisition
    READ_MULTIPLE_SWEEPS(hConn_,(char*)ring_buffer_,buffer_bytes);

    // enable handshake
    std::cout << "Enabling handshake..." << std::endl;
    if (!USB_WRITE(hConn_,&msg_handshake[0])) {
        CLOSE_DRIVER_ASYNC(hConn_);
        FREE_LIBRARY(hDLL_);
        delete ring_buffer_;
        throw std::runtime_error("Could not enable handshake with BioSemi USB2 interface.");
    }


    // === read the first sample ===

    // obtain buffer head pointer
    std::cout << "Querying buffer pointer..." << std::endl;
    if (!READ_POINTER(hConn_,&start_idx)) {
        USB_WRITE(hConn_, &msg_enable[0]);
        CLOSE_DRIVER_ASYNC(hConn_);
        FREE_LIBRARY(hDLL_);
        delete ring_buffer_;
        throw std::runtime_error("Can not obtain ring buffer pointer from BioSemi driver.");
    }

    // check head pointer validity
    if (start_idx > buffer_bytes) {
        USB_WRITE(hConn_, &msg_enable[0]);
        CLOSE_DRIVER_ASYNC(hConn_);
        FREE_LIBRARY(hDLL_);
        delete ring_buffer_;
        throw std::runtime_error("Buffer pointer returned by BioSemi driver is not in the valid range.");
    }

    // read the first sample
    std::cout << "Waiting for data..." << std::endl;
    clock_t start_time = clock();
    while (1) {
        // error checks...
        if (!READ_POINTER(hConn_, &cur_idx)) {
            USB_WRITE(hConn_, &msg_enable[0]);
            CLOSE_DRIVER_ASYNC(hConn_);
            FREE_LIBRARY(hDLL_);
            delete ring_buffer_;
            throw std::runtime_error("Ring buffer handshake with BioSemi driver failed unexpectedly.");
        }
        if ( ((double)(clock() - start_time))/CLOCKS_PER_SEC > max_waiting_time) {
            USB_WRITE(hConn_, &msg_enable[0]);
            CLOSE_DRIVER_ASYNC(hConn_);
            FREE_LIBRARY(hDLL_);
            delete ring_buffer_;
            if (cur_idx - start_idx < 8)
                throw std::runtime_error("BioSemi driver does not transmit data. Is the box turned on?");
            else
                throw std::runtime_error("Did not get a sync signal from BioSemi driver. Is the battery charged?");
        }

		if ((cur_idx - start_idx >= 8) && (ring_buffer_[0] == 0xFFFFFF00)) {
			// got a sync signal on the first index...
			start_idx = 0;
			break;
		}
        if ((cur_idx - start_idx >= 8) && (ring_buffer_[start_idx/4] == 0xFFFFFF00))
            // got the sync signal!
            break;
    }


    // === parse amplifier configuration ===

    // read the trigger channel data ...
    std::cout << "Checking status..." << std::endl;
    boost::uint32_t status = ring_buffer_[start_idx/4+1] >> 8;
    std::cout << "  status: " << status << std::endl;

    // determine channel configuration
    is_mk2_ = ((status&(1<<23)) != 0);
    std::cout << "  MK2: " << is_mk2_ << std::endl;

    // check speed mode
    speed_mode_ = ((status&(1<<17))>>17) + ((status&(1<<18))>>17) + ((status&(1<<19))>>17) + ((status&(1<<21))>>18);
    std::cout << "  speed mode: " << speed_mode_ << std::endl;

    // check for problems...
    if (speed_mode_ > 9) {
        USB_WRITE(hConn_, &msg_enable[0]);
        CLOSE_DRIVER_ASYNC(hConn_);
        FREE_LIBRARY(hDLL_);
        delete ring_buffer_;
        if (is_mk2_)
            throw std::runtime_error("BioSemi amplifier speed mode wheel must be between positions 0 and 8 (9 is a reserved value); recommended for typical use is 4.");
        else
            throw std::runtime_error("BioSemi amplifier speed mode wheel must be between positions 0 and 8 (9 is a reserved value); recommended for typical use is 4.");
    }

    // determine sampling rate (http://www.biosemi.com/faq/adjust_samplerate.htm)
    switch (speed_mode_ & 3) {
    case 0: srate_ = 2048; break;
    case 1: srate_ = 4096; break;
    case 2: srate_ = 8192; break;
    case 3: srate_ = 16384; break;
    }
	// speed modes lower than 3 are special on Mk2 and are for daisy-chained setups (@2KHz)
	bool multibox = false;
	if (is_mk2_ && speed_mode_ < 4) {
		srate_ = 2048;
		multibox = true;
	}
    std::cout << "  sampling rate: " << srate_ << std::endl;

    // determine channel configuration -- this is written according to:
	//   http://www.biosemi.com/faq/make_own_acquisition_software.htm
	//   http://www.biosemi.com/faq/adjust_samplerate.htm
    if (is_mk2_) {
		// in an Mk2 the speed modes 0-3 are for up to 4 daisy-chained boxes; these are 
		// multiplexed, have 128ch EEG each and 8ch EXG each, plus 16 extra channels each (howdy!)
        switch (speed_mode_) {
        case 0:
        case 1:
        case 2:
        case 3:
            nbeeg_ = 4*128; nbexg_ = 4*8; nbaux_ = 4*16; nbaib_ = 0; nbtrig_ = 1; nbsync_ = 1; nbchan_ = 610; break;
		// spd modes 4-7 are the regular ones and have 8 EXG's added in
        case 4: nbeeg_ = 256; nbexg_ = 8; nbaux_ = 16; nbaib_ = 0; nbtrig_ = 1; nbsync_ = 1; nbchan_ = 282; break;
        case 5: nbeeg_ = 128; nbexg_ = 8; nbaux_ = 16; nbaib_ = 0; nbtrig_ = 1; nbsync_ = 1; nbchan_ = 154; break;
        case 6: nbeeg_ = 64; nbexg_ = 8; nbaux_ = 16; nbaib_ = 0; nbtrig_ = 1; nbsync_ = 1; nbchan_ = 90; break;
        case 7: nbeeg_ = 32; nbexg_ = 8; nbaux_ = 16; nbaib_ = 0; nbtrig_ = 1; nbsync_ = 1; nbchan_ = 58; break;
		// spd mode 8 adds
        case 8: nbeeg_ = 256; nbexg_ = 8; nbaux_ = 16; nbaib_ = 32; nbtrig_ = 1; nbsync_ = 1; nbchan_ = 314; break;
        }
    } else {
		// in a Mk1 the EXG's are off in spd mode 0-3 and on in spd mode 4-7 (but subtracted from the EEG channels)
        switch (speed_mode_) {
        // these are all-EEG modes
		case 0: nbeeg_ = 256; nbexg_ = 0; nbaux_ = 0; nbaib_ = 0; nbtrig_ = 1; nbsync_ = 1; nbchan_ = 258; break;
        case 1: nbeeg_ = 128; nbexg_ = 0; nbaux_ = 0; nbaib_ = 0; nbtrig_ = 1; nbsync_ = 1; nbchan_ = 130; break;
        case 2: nbeeg_ = 64; nbexg_ = 0; nbaux_ = 0; nbaib_ = 0; nbtrig_ = 1; nbsync_ = 1; nbchan_ = 66; break;
        case 3: nbeeg_ = 32; nbexg_ = 0; nbaux_ = 0; nbaib_ = 0; nbtrig_ = 1; nbsync_ = 1; nbchan_ = 34; break;
		// in these modes there are are 8 EXGs and 16 aux channels
		case 4: nbeeg_ = 232; nbexg_ = 8; nbaux_ = 16; nbaib_ = 0; nbtrig_ = 1; nbsync_ = 1; nbchan_ = 258; break;
        case 5: nbeeg_ = 104; nbexg_ = 8; nbaux_ = 16; nbaib_ = 0; nbtrig_ = 1; nbsync_ = 1; nbchan_ = 130; break;
        case 6: nbeeg_ = 40; nbexg_ = 8; nbaux_ = 16; nbaib_ = 0; nbtrig_ = 1; nbsync_ = 1; nbchan_ = 66; break;
        case 7: nbeeg_ = 8; nbexg_ = 8; nbaux_ = 16; nbaib_ = 0; nbtrig_ = 1; nbsync_ = 1; nbchan_ = 34; break;
		// in spd mode 8 there are 32 AIB channels from an Analog Input Box (AIB) multiplexed in (EXGs are off)
		case 8: nbeeg_ = 256; nbexg_ = 0; nbaux_ = 0; nbaib_ = 32; nbtrig_ = 1; nbsync_ = 1; nbchan_ = 290; break;
        }
    }
    std::cout << "  channels: " << nbchan_ << "(" << nbsync_ << "xSync, " << nbtrig_ << "xTrigger, " << nbeeg_ << "xEEG, " << nbexg_ << "xExG, " << nbaux_ << "xAUX, " << nbaib_ << "xAIB)" << std::endl;

    // check for additional problematic conditions
    battery_low_ = (status & (1<<22)) != 0;
    if (battery_low_)
        std::cout << "  battery: low" << std::endl;
    else
        std::cout << "  battery: good" << std::endl;
    if (battery_low_)
        std::cout << "The BioSemi battery is low; amplifier will shut down within 30-60 minutes." << std::endl;

    // compute a proper buffer size (needs to be a multiple of 512, a multiple of nbchan, as well as ~32MB in size)
    std::cout << "Reallocating optimized ring buffer..." << std::endl;


    // === shutdown current coonnection ===

    // shutdown current connection
    std::cout << "Sending the enable message again..." << std::endl;
    if (!USB_WRITE(hConn_, &msg_enable[0])) {
        CLOSE_DRIVER_ASYNC(hConn_);
        FREE_LIBRARY(hDLL_);
        throw std::runtime_error("Error while disabling the handshake.");
    }
    std::cout << "Closing the driver..." << std::endl;
    if (!CLOSE_DRIVER_ASYNC(hConn_)) {
        FREE_LIBRARY(hDLL_);
        throw std::runtime_error("Error while disconnecting.");
    }

    // reset to a new ring buffer
    std::cout << "Freeing the ring buffer..." << std::endl;
    delete ring_buffer_;


    // === reinitialize acquisition ===

    std::cout << "Allocating a new ring buffer..." << std::endl;
    ring_buffer_ = new boost::uint32_t[buffer_samples*nbchan_];
    if (!ring_buffer_) {
        FREE_LIBRARY(hDLL_);
        throw std::runtime_error("Could not reallocate ring buffer (out of memory?).");
    }
    std::cout << "Zeroing the ring buffer..." << std::endl;

    memset(ring_buffer_,0,buffer_samples*4*nbchan_);

    // reconnect to driver
    std::cout << "Opening the driver..." << std::endl;
    hConn_ = OPEN_DRIVER_ASYNC();
    if (!hConn_) {
        FREE_LIBRARY(hDLL_);
        throw std::runtime_error("Could not open connection to BioSemi driver.");
    }
    // reinitialize USB2 interface
    std::cout << "Reinitializing the USB interface..." << std::endl;
    if (!USB_WRITE(hConn_,&msg_enable[0])) {
        CLOSE_DRIVER_ASYNC(hConn_);
        FREE_LIBRARY(hDLL_);
        throw std::runtime_error("Could not initialize BioSemi USB2 interface.");
    }
    // begin acquisition
    std::cout << "Starting data acquisition..." << std::endl;
    READ_MULTIPLE_SWEEPS(hConn_,(char*)ring_buffer_,buffer_samples*4*nbchan_);
    // enable handshake
    std::cout << "Enabling handshake..." << std::endl;
    if (!USB_WRITE(hConn_,&msg_handshake[0])) {
        CLOSE_DRIVER_ASYNC(hConn_);
        FREE_LIBRARY(hDLL_);
        delete ring_buffer_;
        throw std::runtime_error("Could not reenable handshake with BioSemi USB2 interface.");
    }


    // === check for correctness of new data ===

    // make sure that we can read the buffer pointer
    std::cout << "Attempt to read buffer pointer..." << std::endl;
    if (!READ_POINTER(hConn_,&start_idx)) {
        USB_WRITE(hConn_, &msg_enable[0]);
        CLOSE_DRIVER_ASYNC(hConn_);
        FREE_LIBRARY(hDLL_);
        delete ring_buffer_;
        throw std::runtime_error("Can not obtain ring buffer pointer from BioSemi driver.");
    }

    std::cout << "Verifying channel format..." << std::endl;
    start_time = clock();
    while (1) {
        // error checks
        if (!READ_POINTER(hConn_, &cur_idx)) {
            USB_WRITE(hConn_, &msg_enable[0]);
            CLOSE_DRIVER_ASYNC(hConn_);
            FREE_LIBRARY(hDLL_);
            delete ring_buffer_;
            throw std::runtime_error("Ring buffer handshake with BioSemi driver failed unexpectedly.");
        }
        if ( ((double)(clock() - start_time))/CLOCKS_PER_SEC > max_waiting_time) {
            USB_WRITE(hConn_, &msg_enable[0]);
            CLOSE_DRIVER_ASYNC(hConn_);
            FREE_LIBRARY(hDLL_);
            delete ring_buffer_;
            if (cur_idx - start_idx < 8)
                throw std::runtime_error("BioSemi driver does not transmit data. Is the box turned on?");
            else
                throw std::runtime_error("Did not get a sync signal from BioSemi driver. Is the battery charged?");
        }
		if ((cur_idx - start_idx >= 4*nbchan_) && (ring_buffer_[0] == 0xFFFFFF00))
			// got a sync signal on the first index
			start_idx = 0;
        if ((cur_idx - start_idx >= 4*nbchan_) && (ring_buffer_[start_idx/4] == 0xFFFFFF00)) {
            if (ring_buffer_[start_idx/4+nbchan_] != 0xFFFFFF00) {
                USB_WRITE(hConn_, &msg_enable[0]);
                CLOSE_DRIVER_ASYNC(hConn_);
                FREE_LIBRARY(hDLL_);
                delete ring_buffer_;
                throw std::runtime_error("Sync signal did not show up at the expected position.");
            } else {
                std::cout << "Channel format is correct..." << std::endl;
                // all correct
                break;
            }
        }
    }


    // === derive channel labels ===

    channel_labels_.clear();
    channel_types_.clear();
    for (int k=1;k<=nbsync_;k++) {
        channel_labels_.push_back(std::string("Sync") += boost::lexical_cast<std::string>(k));
        channel_types_.push_back("Sync");
    }
    for (int k=1;k<=nbtrig_;k++) {
        channel_labels_.push_back(std::string("Trig") += boost::lexical_cast<std::string>(k));
        channel_types_.push_back("Trigger");
    }
	if (multibox) {
		// multi-box setup
		for (int b=0;b<=3;b++) {
			const char *boxid[] = {"_Box1","_Box2","_Box3","_Box4"};
			for (int k=1;k<=nbeeg_/4;k++) {
				std::string tmp = "A"; tmp[0] = 'A'+(k-1)/32;
				channel_labels_.push_back((std::string(tmp) += boost::lexical_cast<std::string>(1+(k-1)%32)) += boxid[b]);
				channel_types_.push_back("EEG");
			}
			for (int k=1;k<=nbexg_/4;k++) {
				channel_labels_.push_back((std::string("EX") += boost::lexical_cast<std::string>(k)) += boxid[b]);
				channel_types_.push_back("EXG");
			}
			for (int k=1;k<=nbaux_/4;k++) {
				channel_labels_.push_back((std::string("AUX") += boost::lexical_cast<std::string>(k)) += boxid[b]);
				channel_types_.push_back("AUX");
			}
			for (int k=1;k<=nbaib_/4;k++) {
				channel_labels_.push_back((std::string("AIB") += boost::lexical_cast<std::string>(k)) += boxid[b]);
				channel_types_.push_back("Analog");
			}
		}
	} else {
		// regular setup
		for (int k=1;k<=nbeeg_;k++) {
			std::string tmp = "A"; tmp[0] = 'A'+(k-1)/32;
			channel_labels_.push_back(std::string(tmp) += boost::lexical_cast<std::string>(1+(k-1)%32));
			channel_types_.push_back("EEG");
		}
		for (int k=1;k<=nbexg_;k++) {
			channel_labels_.push_back(std::string("EX") += boost::lexical_cast<std::string>(k));
			channel_types_.push_back("EXG");
		}
		for (int k=1;k<=nbaux_;k++) {
			channel_labels_.push_back(std::string("AUX") += boost::lexical_cast<std::string>(k));
			channel_types_.push_back("AUX");
		}
		for (int k=1;k<=nbaib_;k++) {
			channel_labels_.push_back(std::string("AIB") += boost::lexical_cast<std::string>(k));
			channel_types_.push_back("Analog");
		}
	}

    last_idx_ = 0;
}

biosemi_io::~biosemi_io() {
    /* close driver connection */
    if (!USB_WRITE(hConn_,&msg_enable[0]))
        std::cout << "The handshake while shutting down the BioSemi driver gave an error." << std::endl;
    if (!CLOSE_DRIVER_ASYNC(hConn_))
        std::cout << "Closing the BioSemi driver gave an error." << std::endl;

    // close the DLL
    FREE_LIBRARY(hDLL_);

    // free the ring buffer
    delete ring_buffer_;
}

void biosemi_io::get_chunk(chunk_t &result) {
    // get current buffer offset
    int cur_idx;
    if (!READ_POINTER(hConn_, (unsigned*)&cur_idx))
        throw std::runtime_error("Reading the updated buffer pointer gave an error.");
    cur_idx = cur_idx/4;

    // forget about incomplete sample data
    cur_idx = cur_idx - cur_idx % nbchan_;
    if (cur_idx < 0)
        cur_idx = cur_idx + buffer_samples*nbchan_;

    result.clear();
    if (cur_idx != last_idx_) {
        if (cur_idx > last_idx_) {
            // sequential read: copy intermediate part between offsets
            int chunklen = (cur_idx - last_idx_) / nbchan_;
            result.resize(chunklen);
            for (int k=0;k<chunklen;k++) {
                result[k].resize(nbchan_);
                memcpy(&result[k][0], &ring_buffer_[last_idx_ + k*nbchan_], nbchan_*4);
            }
        } else {
            // wrap-around read: concatenate two parts
            int chunklen = (cur_idx + buffer_samples*nbchan_ - last_idx_) / nbchan_;
            result.resize(chunklen);
            int first_section = buffer_samples - last_idx_/nbchan_;
            for (int k=0;k<first_section;k++) {
                result[k].resize(nbchan_);
                memcpy(&result[k][0], &ring_buffer_[last_idx_ + k*nbchan_], nbchan_*4);
            }
            int second_section = chunklen - first_section;
            for (int k=0;k<second_section;k++) {
                result[first_section+k].resize(nbchan_);
                memcpy(&result[first_section+k][0], &ring_buffer_[k*nbchan_], nbchan_*4);
            }
        }
        // update status flags
        boost::uint32_t status = ring_buffer_[cur_idx] >> 8;
        battery_low_ = (status & (1<<22)) != 0;
    }

    // update last buffer pointer
    last_idx_ = cur_idx;
}
