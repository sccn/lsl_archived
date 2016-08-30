#ifndef BIOSEMI_IO_H
#define BIOSEMI_IO_H
#include <boost/cstdint.hpp>
#include <vector>
#include <string>

#ifdef _WIN32
    #define BIOSEMI_LINKAGE __cdecl
#else
    #define BIOSEMI_LINKAGE
#endif


class biosemi_io {
public:
    // raw sample from the BioSemi amp
    typedef std::vector<boost::int32_t> sample_t;
    // chunk of raw samples
    typedef std::vector<sample_t> chunk_t;

    // initialize amplifier connection
    biosemi_io();
    // shut down amplifier connection
    ~biosemi_io();

    // get a chunk of raw data
    void get_chunk(chunk_t &result);

    // get channel names
    const std::vector<std::string> &channel_labels() const { return channel_labels_; }
    // get channel types (Sync, Trigger, EEG, EXG,
    const std::vector<std::string> &channel_types() const { return channel_types_; }

    // query amplifier parameters
    bool is_mk2() const { return is_mk2_; }
    bool battery_low() const { return battery_low_; }
    int speed_mode() const { return speed_mode_; }
    int srate() const { return srate_; }
    int nbchan() const { return nbchan_; }
    int nbsync() const { return nbsync_; }
    int nbtrig() const { return nbtrig_; }
    int nbeeg() const { return nbeeg_; }
    int nbexg() const { return nbexg_; }
    int nbaux() const { return nbaux_; }
    int nbaib() const { return nbaib_; }

private:
    // function handle types for the library IO
    typedef void* (BIOSEMI_LINKAGE *OPEN_DRIVER_ASYNC_t)(void);
    typedef int (BIOSEMI_LINKAGE *USB_WRITE_t)(void*, const unsigned char*);
    typedef int (BIOSEMI_LINKAGE *READ_MULTIPLE_SWEEPS_t)(void*,char*,int);
    typedef int (BIOSEMI_LINKAGE *READ_POINTER_t)(void*,unsigned*);
    typedef int (BIOSEMI_LINKAGE *CLOSE_DRIVER_ASYNC_t)(void*);

    // amplifier parameters
    bool is_mk2_;       // whether the amp is a MK2 amplifier
    int speed_mode_;    // amplifier speed mode
    int srate_;         // native sampling rate
    int nbsync_;        // number of synchronization channels
    int nbtrig_;        // number of trigger channels
    int nbeeg_;         // number of EEG channels
    int nbexg_;         // number of ExG channels
    int nbaux_;			// number of AUX channels
    int nbaib_;			// number of AIB channels
    int nbchan_;        // total number of channels
    bool battery_low_;  // whether the battery is low

    // vector of channel labels (in BioSemi naming scheme)
    std::vector<std::string> channel_labels_;
    // vector of channel types (in LSL Semi naming scheme)
    std::vector<std::string> channel_types_;

    // ring buffer pointer (from the driver)
    boost::uint32_t *ring_buffer_;
    int last_idx_;

    // DLL handle
    void *hDLL_;
    // connection handle
    void *hConn_;
    // function pointers
    OPEN_DRIVER_ASYNC_t OPEN_DRIVER_ASYNC;
    USB_WRITE_t USB_WRITE;
    READ_MULTIPLE_SWEEPS_t READ_MULTIPLE_SWEEPS;
    READ_POINTER_t READ_POINTER;
    CLOSE_DRIVER_ASYNC_t CLOSE_DRIVER_ASYNC;
};

#endif // BIOSEMI_IO_H
