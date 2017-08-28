#ifndef LSL_C_H
#define LSL_C_H

/**
* C API for the lab streaming layer.
* 
* The lab streaming layer provides a set of functions to make instrument data accessible 
* in real time within a lab network. From there, streams can be picked up by recording programs, 
* viewing programs or custom experiment applications that access data streams in real time.
*
* The API covers two areas:
* - The "push API" allows to create stream outlets and to push data (regular or irregular measurement 
*   time series, event data, coded audio/video frames, etc.) into them.
* - The "pull API" allows to create stream inlets and read time-synched experiment data from them 
*   (for recording, viewing or experiment control).
*
* To use this library you need to link to either the liblsl32 or liblsl64 shared library that comes with
* this header. Under Visual Studio the library is linked in automatically.
*/

#ifdef __MINGW32__
    #ifdef LIBLSL_STATIC
        #define LIBLSL_C_API
    #elif defined (LIBLSL_EXPORTS)
        #define LIBLSL_C_API __declspec(dllexport)
    #else
        #define LIBLSL_C_API __declspec(dllimport)
    #endif
#elif defined (_WIN32)
    #ifdef LIBLSL_STATIC
        #define LIBLSL_C_API
    #elif defined (LIBLSL_EXPORTS)
        #define LIBLSL_C_API __declspec(dllexport)
    #else
        #ifndef _DEBUG
            #ifdef _WIN64
                #pragma comment (lib,"liblsl64.lib")
            #else
                #pragma comment (lib,"liblsl32.lib")
            #endif
        #else
            #ifdef LSL_DEBUG_BINDINGS
                #ifdef _WIN64
                    #pragma comment (lib,"liblsl64-debug.lib")
                #else
                    #pragma comment (lib,"liblsl32-debug.lib")
                #endif
            #else
                #ifdef _WIN64
                    #pragma comment (lib,"liblsl64.lib")
                #else
                    #pragma comment (lib,"liblsl32.lib")
                #endif
            #endif
        #endif
        #define LIBLSL_C_API __declspec(dllimport)
    #endif
    #pragma warning (disable:4275)
#else
    #pragma GCC visibility push(default)
    #define LIBLSL_C_API
#endif


#ifdef __cplusplus
extern "C" {
#endif



/* =========================== */
/* ==== Defined constants ==== */
/* =========================== */

/**
* Constant to indicate that a stream has variable sampling rate.
*/
#define LSL_IRREGULAR_RATE 0.0

/**
* Constant to indicate that a sample has the next successive time stamp.
* This is an optional optimization to transmit less data per sample.
* The stamp is then deduced from the preceding one according to the stream's sampling rate 
* (in the case of an irregular rate, the same time stamp as before will is assumed).
*/
#define LSL_DEDUCED_TIMESTAMP -1.0

/**
* A very large time value (ca. 1 year); can be used in timeouts.
*/
#define LSL_FOREVER 32000000.0

/**
* Constant to indicate that there is no preference about how a data stream shall be chunked for transmission.
* (can be used for the chunking parameters in the inlet or the outlet).
*/
#define LSL_NO_PREFERENCE 0

/**
* Data format of a channel (each transmitted sample holds an array of channels).
*/
typedef enum {
    cft_float32 = 1,    /* For up to 24-bit precision measurements in the appropriate physical unit */
                        /* (e.g., microvolts). Integers from -16777216 to 16777216 are represented accurately. */
    cft_double64 = 2,   /* For universal numeric data as long as permitted by network & disk budget. */
                        /* The largest representable integer is 53-bit. */
    cft_string = 3,     /* For variable-length ASCII strings or data blobs, such as video frames, */
                        /* complex event descriptions, etc. */
    cft_int32 = 4,      /* For high-rate digitized formats that require 32-bit precision. Depends critically on */
                        /* meta-data to represent meaningful units. Useful for application event codes or other coded data. */
    cft_int16 = 5,      /* For very high rate signals (40Khz+) or consumer-grade audio */ 
                        /* (for professional audio float is recommended). */
    cft_int8 = 6,       /* For binary signals or other coded data. */
                        /* Not recommended for encoding string data. */
    cft_int64 = 7,      /* For now only for future compatibility. Support for this type is not yet exposed in all languages. */
                        /* Also, some builds of liblsl will not be able to send or receive data of this type. */
    cft_undefined = 0   /* Can not be transmitted. */
} lsl_channel_format_t;

/**
* Post-processing options for stream inlets. 
*/
typedef enum {
	proc_none = 0,			/* No automatic post-processing; return the ground-truth time stamps for manual post-processing */
							/* (this is the default behavior of the inlet). */
	proc_clocksync = 1,		/* Perform automatic clock synchronization; equivalent to manually adding the time_correction() value */
							/* to the received time stamps. */
	proc_dejitter = 2,		/* Remove jitter from time stamps. This will apply a smoothing algorithm to the received time stamps; */
							/* the smoothing needs to see a minimum number of samples (30-120 seconds worst-case) until the remaining */
							/* jitter is consistently below 1ms. */
	proc_monotonize = 4,	/* Force the time-stamps to be monotonically ascending (only makes sense if timestamps are dejittered). */
	proc_threadsafe = 8,    /* Post-processing is thread-safe (same inlet can be read from by multiple threads); uses somewhat more CPU. */
	proc_ALL = 1|2|4|8		/* The combination of all possible post-processing options. */
} lsl_processing_options_t;

/**
* Possible error codes.
*/
typedef enum {
    lsl_no_error = 0,           /* No error occurred */
    lsl_timeout_error = -1,     /* The operation failed due to a timeout. */
    lsl_lost_error = -2,        /* The stream has been lost. */
    lsl_argument_error = -3,    /* An argument was incorrectly specified (e.g., wrong format or wrong length). */
    lsl_internal_error = -4     /* Some other internal error has happened. */
} lsl_error_code_t;




/* ===================================================== */
/* ==== Objects provided by the lab streaming layer ==== */
/* ===================================================== */

/**
* Handle to a stream info object. Stores the declaration of a data stream.
* Represents the following information:
*  a) stream data format (#channels, channel format)
*  b) core information (stream name, content type, sampling rate)
*  c) optional meta-data about the stream content (channel labels, measurement units, etc.)
*
* Whenever a program wants to provide a new stream on the lab network it will typically first 
* create an lsl_streaminfo to describe its properties and then construct an lsl_outlet with it to create
* the stream on the network. Other parties who discover/resolve the outlet on the network can query the 
* stream info; it is also written to disk when recording the stream (playing a similar role as a file header).
*/
typedef struct lsl_streaminfo_struct_* lsl_streaminfo;

/**
* A stream outlet handle.
* Outlets are used to make streaming data (and the meta-data) available on the lab network.
*/
typedef struct lsl_outlet_struct_* lsl_outlet;

/**
* A stream inlet handle.
* Inlets are used to receive streaming data (and meta-data) from the lab network.
*/
typedef struct lsl_inlet_struct_* lsl_inlet;

/**
* A lightweight XML element tree handle; models the description of a streaminfo object.
* XML elements behave like advanced pointers into memory that is owned by some respective streaminfo.
* Has a name and can have multiple named children or have text content as value; attributes are omitted.
* Insider note: The interface is modeled after a subset of pugixml's node type and is compatible with it.
* Type-casts between pugi::xml_node_struct* and lsl_xml_ptr are permitted (in both directions) since the types 
* are binary compatible.
* See also pugixml.googlecode.com/svn/tags/latest/docs/manual/access.html for additional documentation.
*/
typedef struct lsl_xml_ptr_struct_* lsl_xml_ptr;

/** 
* Handle to a convenience object that resolves streams continuously in the background throughout 
* its lifetime and which can be queried at any time for the set of streams that are currently 
* visible on the network.
*/
typedef struct lsl_continuous_resolver_* lsl_continuous_resolver;




/* ============================================================ */
/* ==== Free functions provided by the lab streaming layer ==== */
/* ============================================================ */

/**
* Protocol version.
* The major version is protocol_version() / 100;
* The minor version is protocol_version() % 100;
* Clients with different minor versions are protocol-compatible with each other 
* while clients with different major versions will refuse to work together.
*/
extern LIBLSL_C_API int lsl_protocol_version();

/**
* Version of the liblsl library.
* The major version is library_version() / 100;
* The minor version is library_version() % 100;
*/
extern LIBLSL_C_API int lsl_library_version();


/**
* Obtain a local system time stamp in seconds. The resolution is better than a millisecond.
* This reading can be used to assign time stamps to samples as they are being acquired. 
* If the "age" of a sample is known at a particular time (e.g., from USB transmission 
* delays), it can be used as an offset to lsl_local_clock() to obtain a better estimate of 
* when a sample was actually captured. See lsl_push_sample() for a use case.
*/
extern LIBLSL_C_API double lsl_local_clock();

/**
* Resolve all streams on the network.
* This function returns all currently available streams from any outlet on the network.
* The network is usually the subnet specified at the local router, but may also include 
* a multicast group of machines (given that the network supports it), or a list of hostnames.
* These details may optionally be customized by the experimenter in a configuration file 
* (see page Network Connectivity in the LSL wiki).
* This is the default mechanism used by the browsing programs and the recording program.
* @param buffer A user-allocated buffer to hold the resolve results.
*               Note: it is the user's responsibility to either destroy the resulting streaminfo 
*               objects or to pass them back to the LSL during during creation of an inlet.
*               Note 2: The stream_info's returned by the resolver are only short versions that do not
*               include the .desc() field (which can be arbitrarily big). To obtain the full
*               stream information you need to call .info() on the inlet after you have created one.
* @param buffer_elements The user-provided buffer length.
* @param wait_time The waiting time for the operation, in seconds, to search for streams.
*                  The recommended wait time is 1 second (or 2 for a busy and large recording operation).
*                  Warning: If this is too short (<0.5s) only a subset (or none) of the 
*                           outlets that are present on the network may be returned.
* @return The number of results written into the buffer (never more than the provided # of slots) 
*         or a negative number if an error has occurred (values corresponding to lsl_error_code_t).
*/
extern LIBLSL_C_API int lsl_resolve_all(lsl_streaminfo *buffer, unsigned buffer_elements, double wait_time);

/**
* Resolve all streams with a given value for a property.
* If the goal is to resolve a specific stream, this method is preferred over resolving all streams and then selecting the desired one.
* @param buffer A user-allocated buffer to hold the resolve results.
*               Note: it is the user's responsibility to either destroy the resulting streaminfo 
*               objects or to pass them back to the LSL during during creation of an inlet.
*               Note 2: The stream_info's returned by the resolver are only short versions that do not
*               include the .desc() field (which can be arbitrarily big). To obtain the full
*               stream information you need to call .info() on the inlet after you have created one.
* @param buffer_elements The user-provided buffer length.
* @param prop The streaminfo property that should have a specific value ("name", "type", "source_id", or, e.g., "desc/manufaturer" if present).
* @param value The string value that the property should have (e.g., "EEG" as the type).
* @param minimum Return at least this number of streams.
* @param timeout Optionally a timeout of the operation, in seconds (default: no timeout).
*                 If the timeout expires, less than the desired number of streams (possibly none) will be returned.
* @return The number of results written into the buffer (never more than the provided # of slots) 
*         or a negative number if an error has occurred (values corresponding to lsl_error_code_t).
*/
extern LIBLSL_C_API int lsl_resolve_byprop(lsl_streaminfo *buffer, unsigned buffer_elements, char *prop, char *value, int minimum, double timeout);

/**
* Resolve all streams that match a given predicate.
* Advanced query that allows to impose more conditions on the retrieved streams; the given string is an XPath 1.0 
* predicate for the <info> node (omitting the surrounding []'s), see also http://en.wikipedia.org/w/index.php?title=XPath_1.0&oldid=474981951.
* @param buffer A user-allocated buffer to hold the resolve results.
*               Note: it is the user's responsibility to either destroy the resulting streaminfo 
*               objects or to pass them back to the LSL during during creation of an inlet.
*               Note 2: The stream_info's returned by the resolver are only short versions that do not
*               include the .desc() field (which can be arbitrarily big). To obtain the full
*               stream information you need to call .info() on the inlet after you have created one.
* @param buffer_elements The user-provided buffer length.
* @param pred The predicate string, e.g. "name='BioSemi'" or "type='EEG' and starts-with(name,'BioSemi') and count(info/desc/channel)=32"
* @param minimum Return at least this number of streams.
* @param timeout Optionally a timeout of the operation, in seconds (default: no timeout).
*                 If the timeout expires, less than the desired number of streams (possibly none) will be returned.
* @return The number of results written into the buffer (never more than the provided # of slots) 
*         or a negative number if an error has occurred (values corresponding to lsl_error_code_t).
*/
extern LIBLSL_C_API int lsl_resolve_bypred(lsl_streaminfo *buffer, unsigned buffer_elements, char *pred, int minimum, double timeout);

/** 
* Deallocate a string that has been transferred to the application.
* Rarely used: the only use case is to deallocate the contents of 
* string-valued samples received from LSL in an application where
* no free() method is available (e.g., in some scripting languages).
*/
extern LIBLSL_C_API void lsl_destroy_string(char *s);



/* =============================== */
/* ==== lsl_streaminfo object ==== */
/* =============================== */

/**
* Construct a new streaminfo object.
* Core stream information is specified here. Any remaining meta-data can be added later.
* @param name Name of the stream. Describes the device (or product series) that this stream makes available 
*             (for use by programs, experimenters or data analysts). Cannot be empty.
* @param type Content type of the stream. Please see https://github.com/sccn/xdf/wiki/Meta-Data (or web search for:
*             XDF meta-data) for pre-defined content-type names, but you can also make up your own.
*             The content type is the preferred way to find streams (as opposed to searching by name).
* @param channel_count Number of channels per sample. This stays constant for the lifetime of the stream.
* @param nominal_srate The sampling rate (in Hz) as advertised by the data source, if regular (otherwise set to IRREGULAR_RATE).
* @param channel_format Format/type of each channel. If your channels have different formats, consider supplying 
*                       multiple streams or use the largest type that can hold them all (such as cft_double64). A good default is cft_float32.
* @param source_id Unique identifier of the source or device, if available (such as the serial number). 
*                  Allows recipients to recover from failure even after the serving app or device crashes.
*                  May in some cases also be constructed from device settings.
* @return A newly created streaminfo handle or NULL in the event that an error occurred.
*/
extern LIBLSL_C_API lsl_streaminfo lsl_create_streaminfo(char *name, char *type, int channel_count, double nominal_srate, lsl_channel_format_t channel_format, char *source_id);

/**
* Destroy a previously created streaminfo object.
*/
extern LIBLSL_C_API void lsl_destroy_streaminfo(lsl_streaminfo info);

/**
* Copy an existing streaminfo object (rarely used).
*/
extern LIBLSL_C_API lsl_streaminfo lsl_copy_streaminfo(lsl_streaminfo info);

/**
* Name of the stream.
* This is a human-readable name. For streams offered by device modules, it refers to the type of device or product series 
* that is generating the data of the stream. If the source is an application, the name may be a more generic or specific identifier.
* Multiple streams with the same name can coexist, though potentially at the cost of ambiguity (for the recording app or experimenter).
* @return A library-owned pointer to the string value. Modification is not permitted.
*/
extern LIBLSL_C_API char *lsl_get_name(lsl_streaminfo info);

/**
* Content type of the stream.
* The content type is a short string such as "EEG", "Gaze" which describes the content carried by the channel (if known). 
* If a stream contains mixed content this value need not be assigned but may instead be stored in the description of channel types.
* To be useful to applications and automated processing systems using the recommended content types is preferred. 
* Content types usually follow those pre-defined in https://github.com/sccn/xdf/wiki/Meta-Data (or web search for: XDF meta-data).
* @return A library-owned pointer to the string value. Modification is not permitted.
*/
extern LIBLSL_C_API char *lsl_get_type(lsl_streaminfo info);

/**
* Number of channels of the stream.
* A stream has at least one channels; the channel count stays constant for all samples.
*/
extern LIBLSL_C_API int lsl_get_channel_count(lsl_streaminfo info);

/**
* Sampling rate of the stream, according to the source (in Hz).
* If a stream is irregularly sampled, this should be set to IRREGULAR_RATE.
*
* Note that no data will be lost even if this sampling rate is incorrect or if a device has temporary 
* hiccups, since all samples will be recorded anyway (except for those dropped by the device itself). However, 
* when the recording is imported into an application, a good importer may correct such errors more accurately 
* if the advertised sampling rate was close to the specs of the device.
*/
extern LIBLSL_C_API double lsl_get_nominal_srate(lsl_streaminfo info);

/**
* Channel format of the stream.
* All channels in a stream have the same format. However, a device might offer multiple time-synched streams 
* each with its own format.
*/
extern LIBLSL_C_API lsl_channel_format_t lsl_get_channel_format(lsl_streaminfo info);

/**
* Unique identifier of the stream's source, if available.
* The unique source (or device) identifier is an optional piece of information that, if available, allows that
* endpoints (such as the recording program) can re-acquire a stream automatically once it is back online.
* @return A library-owned pointer to the string value. Modification is not permitted.
*/
extern LIBLSL_C_API char *lsl_get_source_id(lsl_streaminfo info);

/**
* Protocol version used to deliver the stream.
*/
extern LIBLSL_C_API int lsl_get_version(lsl_streaminfo info);

/**
* Creation time stamp of the stream.
* This is the time stamp when the stream was first created
* (as determined via local_clock() on the providing machine).
*/
extern LIBLSL_C_API double lsl_get_created_at(lsl_streaminfo info);

/**
* Unique ID of the stream outlet (once assigned).
* This is a unique identifier of the stream outlet, and is guaranteed to be different
* across multiple instantiations of the same outlet (e.g., after a re-start).
* @return A library-owned pointer to the string value. Modification is not permitted.
*/
extern LIBLSL_C_API char *lsl_get_uid(lsl_streaminfo info);

/**
* Session ID for the given stream.
* The session id is an optional human-assigned identifier of the recording session.
* While it is rarely used, it can be used to prevent concurrent recording activitites 
* on the same sub-network (e.g., in multiple experiment areas) from seeing each other's streams 
* (assigned via a configuration file by the experimenter, see Network Connectivity on the LSL wiki).
* @return A library-owned pointer to the string value. Modification is not permitted.
*/
extern LIBLSL_C_API char *lsl_get_session_id(lsl_streaminfo info);

/**
* Hostname of the providing machine (once bound to an outlet). Modification is not permitted.
*/
extern LIBLSL_C_API char *lsl_get_hostname(lsl_streaminfo info);

/**
* Extended description of the stream.
* It is highly recommended that at least the channel labels are described here. 
* See code examples on the LSL wiki. Other information, such as amplifier settings, 
* measurement units if deviating from defaults, setup information, subject information, etc., 
* can be specified here, as well. Meta-data recommendations follow the XDF file format project
* (github.com/sccn/xdf/wiki/Meta-Data or web search for: XDF meta-data).
*
* Important: if you use a stream content type for which meta-data recommendations exist, please 
* try to lay out your meta-data in agreement with these recommendations for compatibility with other applications.
*/
extern LIBLSL_C_API lsl_xml_ptr lsl_get_desc(lsl_streaminfo info);

/**
* Retrieve the entire streaminfo in XML format.
* This yields an XML document (in string form) whose top-level element is <info>. The info element contains
* one element for each field of the streaminfo class, including:
*  a) the core elements <name>, <type>, <channel_count>, <nominal_srate>, <channel_format>, <source_id>
*  b) the misc elements <version>, <created_at>, <uid>, <session_id>, <v4address>, <v4data_port>, <v4service_port>, <v6address>, <v6data_port>, <v6service_port>
*  c) the extended description element <desc> with user-defined sub-elements.
* @return A pointer to a copy of the XML text or NULL in the event that an error occurred.
*         Note: It is the user's responsibility to deallocate this string when it is no longer needed.
*/
extern LIBLSL_C_API char *lsl_get_xml(lsl_streaminfo info);

/// Number of bytes occupied by a channel (0 for string-typed channels).
extern LIBLSL_C_API int lsl_get_channel_bytes(lsl_streaminfo info);

/// Number of bytes occupied by a sample (0 for string-typed channels).
extern LIBLSL_C_API int lsl_get_sample_bytes(lsl_streaminfo info);

/// Create a streaminfo object from an XML representation
extern LIBLSL_C_API lsl_streaminfo lsl_streaminfo_from_xml(char *xml);




/* =========================== */
/* ==== lsl_outlet object ==== */
/* =========================== */

/**
* Establish a new stream outlet. This makes the stream discoverable.
* @param info The stream information to use for creating this stream. Stays constant over the lifetime of the outlet.
*             Note: the outlet makes a copy of the streaminfo object upon construction (so the old info should still be destroyed.)
* @param chunk_size Optionally the desired chunk granularity (in samples) for transmission. If specified as 0,
*                   each push operation yields one chunk. Stream recipients can have this setting bypassed.
* @param max_buffered Optionally the maximum amount of data to buffer (in seconds if there is a nominal 
*                     sampling rate, otherwise x100 in samples). A good default is 360, which corresponds to 6 minutes of data. 
*                     Note that, for high-bandwidth data you will almost certainly want to use a lower value here to avoid 
*                     running out of RAM. 
* @return A newly created lsl_outlet handle or NULL in the event that an error occurred.
*/
extern LIBLSL_C_API lsl_outlet lsl_create_outlet(lsl_streaminfo info, int chunk_size, int max_buffered);

/**
* Destroy an outlet.
* The outlet will no longer be discoverable after destruction and all connected inlets will stop delivering data.
*/
extern LIBLSL_C_API void lsl_destroy_outlet(lsl_outlet out);

/**
* Push a pointer to some values as a sample into the outlet.
* Handles type checking & conversion.
* @param out The lsl_outlet object through which to push the data.
* @param data A pointer to values to push. The number of values pointed to must be no less than the number of channels in the sample.
* @param lengths For lsl_push_sample_buf*, a pointer the number of elements to push for each channel (string lengths). 
* @param timestamp Optionally the capture time of the sample, in agreement with lsl_local_clock(); if omitted, the current time is used.
* @param pushthrough Whether to push the sample through to the receivers instead of buffering it with subsequent samples.
*                    Note that the chunk_size, if specified at outlet construction, takes precedence over the pushthrough flag.
* @return Error code of the operation or lsl_no_error if successful (usually attributed to the wrong data type).
*/
extern LIBLSL_C_API int lsl_push_sample_f(lsl_outlet out, float *data);
extern LIBLSL_C_API int lsl_push_sample_ft(lsl_outlet out, float *data, double timestamp);
extern LIBLSL_C_API int lsl_push_sample_ftp(lsl_outlet out, float *data, double timestamp, int pushthrough);
extern LIBLSL_C_API int lsl_push_sample_d(lsl_outlet out, double *data);
extern LIBLSL_C_API int lsl_push_sample_dt(lsl_outlet out, double *data, double timestamp);
extern LIBLSL_C_API int lsl_push_sample_dtp(lsl_outlet out, double *data, double timestamp, int pushthrough);
extern LIBLSL_C_API int lsl_push_sample_l(lsl_outlet out, long *data);
extern LIBLSL_C_API int lsl_push_sample_lt(lsl_outlet out, long *data, double timestamp);
extern LIBLSL_C_API int lsl_push_sample_ltp(lsl_outlet out, long *data, double timestamp, int pushthrough);
extern LIBLSL_C_API int lsl_push_sample_i(lsl_outlet out, int *data);
extern LIBLSL_C_API int lsl_push_sample_it(lsl_outlet out, int *data, double timestamp);
extern LIBLSL_C_API int lsl_push_sample_itp(lsl_outlet out, int *data, double timestamp, int pushthrough);
extern LIBLSL_C_API int lsl_push_sample_s(lsl_outlet out, short *data);
extern LIBLSL_C_API int lsl_push_sample_st(lsl_outlet out, short *data, double timestamp);
extern LIBLSL_C_API int lsl_push_sample_stp(lsl_outlet out, short *data, double timestamp, int pushthrough);
extern LIBLSL_C_API int lsl_push_sample_c(lsl_outlet out, char *data);
extern LIBLSL_C_API int lsl_push_sample_ct(lsl_outlet out, char *data, double timestamp);
extern LIBLSL_C_API int lsl_push_sample_ctp(lsl_outlet out, char *data, double timestamp, int pushthrough);
extern LIBLSL_C_API int lsl_push_sample_str(lsl_outlet out, char **data);
extern LIBLSL_C_API int lsl_push_sample_strt(lsl_outlet out, char **data, double timestamp);
extern LIBLSL_C_API int lsl_push_sample_strtp(lsl_outlet out, char **data, double timestamp, int pushthrough);
extern LIBLSL_C_API int lsl_push_sample_buf(lsl_outlet out, char **data, unsigned *lengths);
extern LIBLSL_C_API int lsl_push_sample_buft(lsl_outlet out, char **data, unsigned *lengths, double timestamp);
extern LIBLSL_C_API int lsl_push_sample_buftp(lsl_outlet out, char **data, unsigned *lengths, double timestamp, int pushthrough);
extern LIBLSL_C_API int lsl_push_sample_v(lsl_outlet out, void *data);
extern LIBLSL_C_API int lsl_push_sample_vt(lsl_outlet out, void *data, double timestamp);
extern LIBLSL_C_API int lsl_push_sample_vtp(lsl_outlet out, void *data, double timestamp, int pushthrough);


/**
* Push a chunk of multiplexed samples into the outlet. One timestamp per sample is provided.
* IMPORTANT: Note that the provided buffer size is measured in channel values (e.g., floats) rather than in samples.
* Handles type checking & conversion.
* @param out The lsl_outlet object through which to push the data.
* @param data A buffer of channel values holding the data for zero or more successive samples to send.
* @param lengths For lsl_push_chunk_buf*, a pointer the number of elements to push for each value (string lengths). 
* @param timestamp Optionally the capture time of the most recent sample, in agreement with local_clock(); if omitted, the current time is used.
*                   The time stamps of other samples are automatically derived based on the sampling rate of the stream.
* @param timestamps Alternatively a buffer of timestamp values holding time stamps for each sample in the data buffer.
* @param data_elements The number of data values (of type T) in the data buffer. Must be a multiple of the channel count.
* @param pushthrough Whether to push the chunk through to the receivers instead of buffering it with subsequent samples.
*                    Note that the chunk_size, if specified at outlet construction, takes precedence over the pushthrough flag.
* @return Error code of the operation (usually attributed to the wrong data type).
*/
extern LIBLSL_C_API int lsl_push_chunk_f(lsl_outlet out, float *data, unsigned long data_elements);
extern LIBLSL_C_API int lsl_push_chunk_ft(lsl_outlet out, float *data, unsigned long data_elements, double timestamp);
extern LIBLSL_C_API int lsl_push_chunk_ftp(lsl_outlet out, float *data, unsigned long data_elements, double timestamp, int pushthrough);
extern LIBLSL_C_API int lsl_push_chunk_ftn(lsl_outlet out, float *data, unsigned long data_elements, double *timestamps);
extern LIBLSL_C_API int lsl_push_chunk_ftnp(lsl_outlet out, float *data, unsigned long data_elements, double *timestamps, int pushthrough);
extern LIBLSL_C_API int lsl_push_chunk_d(lsl_outlet out, double *data, unsigned long data_elements);
extern LIBLSL_C_API int lsl_push_chunk_dt(lsl_outlet out, double *data, unsigned long data_elements, double timestamp);
extern LIBLSL_C_API int lsl_push_chunk_dtp(lsl_outlet out, double *data, unsigned long data_elements, double timestamp, int pushthrough);
extern LIBLSL_C_API int lsl_push_chunk_dtn(lsl_outlet out, double *data, unsigned long data_elements, double *timestamps);
extern LIBLSL_C_API int lsl_push_chunk_dtnp(lsl_outlet out, double *data, unsigned long data_elements, double *timestamps, int pushthrough);
extern LIBLSL_C_API int lsl_push_chunk_l(lsl_outlet out, long *data, unsigned long data_elements);
extern LIBLSL_C_API int lsl_push_chunk_lt(lsl_outlet out, long *data, unsigned long data_elements, double timestamp);
extern LIBLSL_C_API int lsl_push_chunk_ltp(lsl_outlet out, long *data, unsigned long data_elements, double timestamp, int pushthrough);
extern LIBLSL_C_API int lsl_push_chunk_ltn(lsl_outlet out, long *data, unsigned long data_elements, double *timestamps);
extern LIBLSL_C_API int lsl_push_chunk_ltnp(lsl_outlet out, long *data, unsigned long data_elements, double *timestamps, int pushthrough);
extern LIBLSL_C_API int lsl_push_chunk_i(lsl_outlet out, int *data, unsigned long data_elements);
extern LIBLSL_C_API int lsl_push_chunk_it(lsl_outlet out, int *data, unsigned long data_elements, double timestamp);
extern LIBLSL_C_API int lsl_push_chunk_itp(lsl_outlet out, int *data, unsigned long data_elements, double timestamp, int pushthrough);
extern LIBLSL_C_API int lsl_push_chunk_itn(lsl_outlet out, int *data, unsigned long data_elements, double *timestamps);
extern LIBLSL_C_API int lsl_push_chunk_itnp(lsl_outlet out, int *data, unsigned long data_elements, double *timestamps, int pushthrough);
extern LIBLSL_C_API int lsl_push_chunk_s(lsl_outlet out, short *data, unsigned long data_elements);
extern LIBLSL_C_API int lsl_push_chunk_st(lsl_outlet out, short *data, unsigned long data_elements, double timestamp);
extern LIBLSL_C_API int lsl_push_chunk_stp(lsl_outlet out, short *data, unsigned long data_elements, double timestamp, int pushthrough);
extern LIBLSL_C_API int lsl_push_chunk_stn(lsl_outlet out, short *data, unsigned long data_elements, double *timestamps);
extern LIBLSL_C_API int lsl_push_chunk_stnp(lsl_outlet out, short *data, unsigned long data_elements, double *timestamps, int pushthrough);
extern LIBLSL_C_API int lsl_push_chunk_c(lsl_outlet out, char *data, unsigned long data_elements);
extern LIBLSL_C_API int lsl_push_chunk_ct(lsl_outlet out, char *data, unsigned long data_elements, double timestamp);
extern LIBLSL_C_API int lsl_push_chunk_ctp(lsl_outlet out, char *data, unsigned long data_elements, double timestamp, int pushthrough);
extern LIBLSL_C_API int lsl_push_chunk_ctn(lsl_outlet out, char *data, unsigned long data_elements, double *timestamps);
extern LIBLSL_C_API int lsl_push_chunk_ctnp(lsl_outlet out, char *data, unsigned long data_elements, double *timestamps, int pushthrough);
extern LIBLSL_C_API int lsl_push_chunk_str(lsl_outlet out, char **data, unsigned long data_elements);
extern LIBLSL_C_API int lsl_push_chunk_strt(lsl_outlet out, char **data, unsigned long data_elements, double timestamp);
extern LIBLSL_C_API int lsl_push_chunk_strtp(lsl_outlet out, char **data, unsigned long data_elements, double timestamp, int pushthrough);
extern LIBLSL_C_API int lsl_push_chunk_strtn(lsl_outlet out, char **data, unsigned long data_elements, double *timestamps);
extern LIBLSL_C_API int lsl_push_chunk_strtnp(lsl_outlet out, char **data, unsigned long data_elements, double *timestamps, int pushthrough);
extern LIBLSL_C_API int lsl_push_chunk_buf(lsl_outlet out, char **data, unsigned *lengths, unsigned long data_elements);
extern LIBLSL_C_API int lsl_push_chunk_buft(lsl_outlet out, char **data, unsigned *lengths, unsigned long data_elements, double timestamp);
extern LIBLSL_C_API int lsl_push_chunk_buftp(lsl_outlet out, char **data, unsigned *lengths, unsigned long data_elements, double timestamp, int pushthrough);
extern LIBLSL_C_API int lsl_push_chunk_buftn(lsl_outlet out, char **data, unsigned *lengths, unsigned long data_elements, double *timestamps);
extern LIBLSL_C_API int lsl_push_chunk_buftnp(lsl_outlet out, char **data, unsigned *lengths, unsigned long data_elements, double *timestamps, int pushthrough);


/**
* Check whether consumers are currently registered.
* While it does not hurt, there is technically no reason to push samples if there is no consumer.
*/
extern LIBLSL_C_API int lsl_have_consumers(lsl_outlet out);

/**
* Wait until some consumer shows up (without wasting resources).
* @return True if the wait was successful, false if the timeout expired.
*/
extern LIBLSL_C_API int lsl_wait_for_consumers(lsl_outlet out, double timeout);

/**
* Retrieve a handle to the stream info provided by this outlet.
* This is what was used to create the stream (and also has the Additional Network Information fields assigned).
* @return A copy of the streaminfo of the outlet or NULL in the event that an error occurred.
*         Note: it is the user's responsibility to destroy it when it is no longer needed.
*/ 
extern LIBLSL_C_API lsl_streaminfo lsl_get_info(lsl_outlet out);




/* ========================== */
/* ==== lsl_inlet object ==== */
/* ========================== */

/**
* Construct a new stream inlet from a resolved stream info.
* @param info A resolved stream info object (as coming from one of the resolver functions).
*             Note: the inlet makes a copy of the info object at its construction.
*             Note: the stream_inlet may also be constructed with a fully-specified stream_info, 
*                   if the desired channel format and count is already known up-front, but this is 
*                   strongly discouraged and should only ever be done if there is no time to resolve the 
*                   stream up-front (e.g., due to limitations in the client program).
* @param max_buflen Optionally the maximum amount of data to buffer (in seconds if there is a nominal 
*                   sampling rate, otherwise x100 in samples). Recording applications want to use a fairly 
*                   large buffer size here, while real-time applications would only buffer as much as 
*                   they need to perform their next calculation. A good default is 360, which corresponds to 6 minutes of data.
* @param max_chunklen Optionally the maximum size, in samples, at which chunks are transmitted.
*                     If specified as 0, the chunk sizes preferred by the sender are used.
*                     Recording applications can use a generous size here (leaving it to the network how 
*                     to pack things), while real-time applications may want a finer (perhaps 1-sample) granularity.
* @param recover Try to silently recover lost streams that are recoverable (=those that that have a source_id set).
*                It is generally a good idea to enable this, unless the application wants to act in a special way
*                when a data provider has temporarily crashed. If recover is 0 or the stream is not recoverable, 
*                most outlet functions will return an lsl_lost_error if the stream's source is lost.
* @return A newly created lsl_inlet handle or NULL in the event that an error occurred.
*/
extern LIBLSL_C_API lsl_inlet lsl_create_inlet(lsl_streaminfo info, int max_buflen, int max_chunklen, int recover);

/** 
* Destructor.
* The inlet will automatically disconnect if destroyed.
*/
extern LIBLSL_C_API void lsl_destroy_inlet(lsl_inlet in);

/**
* Retrieve the complete information of the given stream, including the extended description.
* Can be invoked at any time of the stream's lifetime.
* @param in The lsl_inlet object to act on.
* @param timeout Timeout of the operation. Use LSL_FOREVER to effectively disable it.
* @param ec Error code: if nonzero, can be either lsl_timeout_error (if the timeout has expired) or lsl_lost_error (if the stream source has been lost).
* @return A copy of the full streaminfo of the inlet or NULL in the event that an error happened.
*         Note: it is the user's responsibility to destroy it when it is no longer needed.
*/
extern LIBLSL_C_API lsl_streaminfo lsl_get_fullinfo(lsl_inlet in, double timeout, int *ec);

/**
* Subscribe to the data stream.
* All samples pushed in at the other end from this moment onwards will be queued and 
* eventually be delivered in response to pull_sample() calls.
* Pulling a sample without some preceding lsl_open_stream() is permitted (the stream will then be opened implicitly).
* @param in The lsl_inlet object to act on.
* @param timeout Optional timeout of the operation. Use LSL_FOREVER to effectively disable it.
* @param ec Error code: if nonzero, can be either lsl_timeout_error (if the timeout has expired) or lsl_lost_error (if the stream source has been lost).
*/
extern LIBLSL_C_API void lsl_open_stream(lsl_inlet in, double timeout, int *ec);

/**
* Drop the current data stream.
* All samples that are still buffered or in flight will be dropped and transmission 
* and buffering of data for this inlet will be stopped. If an application stops being 
* interested in data from a source (temporarily or not) but keeps the outlet alive, 
* it should call lsl_close_stream() to not waste unnecessary system and network 
* resources.
*/
extern LIBLSL_C_API void lsl_close_stream(lsl_inlet in);

/**
* Retrieve an estimated time correction offset for the given stream.
* The first call to this function takes several milliseconds until a reliable first estimate is obtained.
* Subsequent calls are instantaneous (and rely on periodic background updates).
* On a well-behaved network, the precision of these estimates should be below 1 ms (empirically it is within +/-0.2 ms).
* To get a measure of whether the network is well-behaved, use lsl_time_correction_ex and check uncertainty (which maps to round-trip-time).
* 0.2 ms is typical of wired networks. 2 ms is typical of wireless networks. The number can be much higher on poor networks.
*
* @param in The lsl_inlet object to act on.
* @param remote_time The current time of the remote computer that was used to generate this time_correction. 
*    If desired, the client can fit time_correction vs remote_time to improve the real-time time_correction further.
* @param uncertainty. The maximum uncertainty of the given time correction.
* @param timeout Timeout to acquire the first time-correction estimate. Use LSL_FOREVER to defuse the timeout.
* @param ec Error code: if nonzero, can be either lsl_timeout_error (if the timeout has expired) or lsl_lost_error (if the stream source has been lost).
* @return The time correction estimate. This is the number that needs to be added to a time stamp that was remotely generated via lsl_local_clock() 
*         to map it into the local clock domain of this machine.
*/
extern LIBLSL_C_API double lsl_time_correction(lsl_inlet in, double timeout, int *ec);
extern LIBLSL_C_API double lsl_time_correction_ex(lsl_inlet in, double *remote_time, double *uncertainty, double timeout, int *ec);


/**
* Set post-processing flags to use. By default, the inlet performs NO post-processing and returns the 
* ground-truth time stamps, which can then be manually synchronized using time_correction(), and then 
* smoothed/dejittered if desired. This function allows automating these two and possibly more operations.
* Warning: when you enable this, you will no longer receive or be able to recover the original time stamps.
* @param in The lsl_inlet object to act on.
* @param flags An integer that is the result of bitwise OR'ing one or more options from processing_options_t 
*        together (e.g., post_clocksync|post_dejitter); a good setting is to use post_ALL.
* @return The error code: if nonzero, can be lsl_argument_error if an unknown flag was passed in.
*/
extern LIBLSL_C_API int lsl_set_postprocessing(lsl_inlet in, unsigned flags);


/* === Pulling a sample from the inlet === */

/**
* Pull a sample from the inlet and read it into a pointer to values.
* Handles type checking & conversion.
* @param in The lsl_inlet object to act on.
* @param buffer A pointer to hold the resulting values. 
* @param buffer_elements The number of samples allocated in the buffer. Note: it is the responsibility of the user to allocate enough memory.
* @param timeout The timeout for this operation, if any. Use LSL_FOREVER to effectively disable it. It is also permitted to use
*                0.0 here; in this case a sample is only returned if one is currently buffered.
* @param ec Error code: can be either no error or lsl_lost_error (if the stream source has been lost).
*           Note: if the timeout expires before a new sample was received the function returns 0.0; 
*                 ec is *not* set to lsl_timeout_error (because this case is not considered an error condition).
* @return The capture time of the sample on the remote machine, or 0.0 if no new sample was available. 
*         To remap this time stamp to the local clock, add the value returned by lsl_time_correction() to it. 
*/
extern LIBLSL_C_API double lsl_pull_sample_f(lsl_inlet in, float *buffer, int buffer_elements, double timeout, int *ec);
extern LIBLSL_C_API double lsl_pull_sample_d(lsl_inlet in, double *buffer, int buffer_elements, double timeout, int *ec);
extern LIBLSL_C_API double lsl_pull_sample_l(lsl_inlet in, long *buffer, int buffer_elements, double timeout, int *ec);
extern LIBLSL_C_API double lsl_pull_sample_i(lsl_inlet in, int *buffer, int buffer_elements, double timeout, int *ec);
extern LIBLSL_C_API double lsl_pull_sample_s(lsl_inlet in, short *buffer, int buffer_elements, double timeout, int *ec);
extern LIBLSL_C_API double lsl_pull_sample_c(lsl_inlet in, char *buffer, int buffer_elements, double timeout, int *ec);
extern LIBLSL_C_API double lsl_pull_sample_str(lsl_inlet in, char **buffer, int buffer_elements, double timeout, int *ec);

/**
* Pull a sample from the inlet and read it into an array of binary strings. These strings may contains 0's, therefore the lengths are read into
* the buffer_lengths array. Handles type checking & conversion.
* @param in The lsl_inlet object to act on.
* @param buffer A pointer to hold the resulting data. 
* @param buffer_lengths A pointer to an array that holds the resulting lengths for each returned binary string.
* @param buffer_elements The number of samples allocated in the buffer and buffer_lengths variables. 
*                        Note: it is the responsibility of the user to allocate enough memory.
* @param timeout The timeout for this operation, if any. Use LSL_FOREVER to effectively disable it. It is also permitted to use
*                0.0 here; in this case a sample is only returned if one is currently buffered.
* @param ec Error code: can be either no error or lsl_lost_error (if the stream source has been lost).
*           Note: if the timeout expires before a new sample was received the function returns 0.0; 
*                 ec is *not* set to lsl_timeout_error (because this case is not considered an error condition).
* @return The capture time of the sample on the remote machine, or 0.0 if no new sample was available. 
*         To remap this time stamp to the local clock, add the value returned by lsl_time_correction() to it. 
*/
extern LIBLSL_C_API double lsl_pull_sample_buf(lsl_inlet in, char **buffer, unsigned *buffer_lengths, int buffer_elements, double timeout, int *ec);

/**
* Pull a sample from the inlet and read it into a custom struct or buffer. 
* Overall size checking but no type checking or conversion are done. Do not use for variable-size/string-formatted streams.
* @param in The lsl_inlet object to act on.
* @param sample Pointer to hold the sample data. Search for #pragma pack for information on how to pack structs appropriately.
* @param timeout The timeout for this operation, if any. Aside from LSL_FOREVER it is also permitted to use
*                0.0 here; in this case a sample is only returned if one is currently buffered.
* @param ec Error code: can be either no error or lsl_lost_error (if the stream source has been lost).
*           Note: if the timeout expires before a new sample was received the function returns 0.0; 
*                 ec is *not* set to lsl_timeout_error (because this case is not considered an error condition).
* @return The capture time of the sample on the remote machine, or 0.0 if no new sample was available. 
*          To remap this time stamp to the local clock, add the value returned by .time_correction() to it. 
*/
extern LIBLSL_C_API double lsl_pull_sample_v(lsl_inlet in, void *buffer, int buffer_bytes, double timeout, int *ec);

/**
* Pull a chunk of data from the inlet and read it into a buffer.
* Handles type checking & conversion.
* IMPORTANT: Note that the provided data buffer size is measured in channel values (e.g., floats) rather than in samples.
* @param in The lsl_inlet object to act on.
* @param data_buffer A pointer to a buffer of data values where the results shall be stored.
* @param timestamp_buffer A pointer to a buffer of timestamp values where time stamps shall be stored. 
*                         If this is NULL, no time stamps will be returned.
* @param data_buffer_elements The size of the data buffer, in channel data elements (of type T). 
*                             Must be a multiple of the stream's channel count.
* @param timestamp_buffer_elements The size of the timestamp buffer. If a timestamp buffer is provided then this 
*                                  must correspond to the same number of samples as data_buffer_elements.
* @param timeout The timeout for this operation, if any. When the timeout expires, the function may return
*                before the entire buffer is filled. The default value of 0.0 will retrieve only data 
*                available for immediate pickup.
* @param ec Error code: can be either no error or lsl_lost_error (if the stream source has been lost).
*           Note: if the timeout expires before a new sample was received the function returns 0.0; 
*                 ec is *not* set to lsl_timeout_error (because this case is not considered an error condition).
* @return data_elements_written Number of channel data elements written to the data buffer.
*/
extern LIBLSL_C_API unsigned long lsl_pull_chunk_f(lsl_inlet in, float *data_buffer, double *timestamp_buffer, unsigned long data_buffer_elements, unsigned long timestamp_buffer_elements, double timeout, int *ec);
extern LIBLSL_C_API unsigned long lsl_pull_chunk_d(lsl_inlet in, double *data_buffer, double *timestamp_buffer, unsigned long data_buffer_elements, unsigned long timestamp_buffer_elements, double timeout, int *ec);
extern LIBLSL_C_API unsigned long lsl_pull_chunk_l(lsl_inlet in, long *data_buffer, double *timestamp_buffer, unsigned long data_buffer_elements, unsigned long timestamp_buffer_elements, double timeout, int *ec);
extern LIBLSL_C_API unsigned long lsl_pull_chunk_i(lsl_inlet in, int *data_buffer, double *timestamp_buffer, unsigned long data_buffer_elements, unsigned long timestamp_buffer_elements, double timeout, int *ec);
extern LIBLSL_C_API unsigned long lsl_pull_chunk_s(lsl_inlet in, short *data_buffer, double *timestamp_buffer, unsigned long data_buffer_elements, unsigned long timestamp_buffer_elements, double timeout, int *ec);
extern LIBLSL_C_API unsigned long lsl_pull_chunk_c(lsl_inlet in, char *data_buffer, double *timestamp_buffer, unsigned long data_buffer_elements, unsigned long timestamp_buffer_elements, double timeout, int *ec);
extern LIBLSL_C_API unsigned long lsl_pull_chunk_str(lsl_inlet in, char **data_buffer, double *timestamp_buffer, unsigned long data_buffer_elements, unsigned long timestamp_buffer_elements, double timeout, int *ec);

/**
* Pull a chunk of data from the inlet and read it into an array of binary strings. 
* These strings may contains 0's, therefore the lengths are read into the lengths_buffer array. 
* Handles type checking & conversion.
* IMPORTANT: Note that the provided data buffer size is measured in channel values (e.g., floats) rather than in samples.
* @param in The lsl_inlet object to act on.
* @param data_buffer A pointer to a buffer of data values where the results shall be stored.
* @param lengths_buffer A pointer to an array that holds the resulting lengths for each returned binary string.
* @param timestamp_buffer A pointer to a buffer of timestamp values where time stamps shall be stored. 
*                         If this is NULL, no time stamps will be returned.
* @param data_buffer_elements The size of the data buffer, in channel data elements (of type T). 
*                             Must be a multiple of the stream's channel count.
* @param timestamp_buffer_elements The size of the timestamp buffer. If a timestamp buffer is provided then this 
*                                  must correspond to the same number of samples as data_buffer_elements.
* @param timeout The timeout for this operation, if any. When the timeout expires, the function may return
*                before the entire buffer is filled. The default value of 0.0 will retrieve only data 
*                available for immediate pickup.
* @param ec Error code: can be either no error or lsl_lost_error (if the stream source has been lost).
*           Note: if the timeout expires before a new sample was received the function returns 0.0; 
*                 ec is *not* set to lsl_timeout_error (because this case is not considered an error condition).
* @return data_elements_written Number of channel data elements written to the data buffer.
*/

extern LIBLSL_C_API unsigned long lsl_pull_chunk_buf(lsl_inlet in, char **data_buffer, unsigned *lengths_buffer, double *timestamp_buffer, unsigned long data_buffer_elements, unsigned long timestamp_buffer_elements, double timeout, int *ec);

/**
* Query whether samples are currently available for immediate pickup.
* Note that it is not a good idea to use samples_available() to determine whether 
* a pull_*() call would block: to be sure, set the pull timeout to 0.0 or an acceptably
* low value. If the underlying implementation supports it, the value will be the number of 
* samples available (otherwise it will be 1 or 0).
*/
extern LIBLSL_C_API unsigned lsl_samples_available(lsl_inlet in);

/**
* Query whether the clock was potentially reset since the last call to was_clock_reset().
* This is rarely-used function is only needed for applications that combine multiple time_correction 
* values to estimate precise clock drift if they should tolerate cases where the source machine was 
* hot-swapped or restarted.
*/
extern LIBLSL_C_API unsigned lsl_was_clock_reset(lsl_inlet in);

/**
* Override the half-time (forget factor) of the time-stamp smoothing.
* The default is 90 seconds unless a different value is set in the config file.
* Using a longer window will yield lower jitter in the time stamps, but longer 
* windows will have trouble tracking changes in the clock rate (usually due to 
* temperature changes); the default is able to track changes up to 10 
* degrees C per minute sufficiently well.
* @param in The lsl_inlet object to act on.
* @param value The new value, in seconds. This is the time after which a past sample 
*			   will be weighted by 1/2 in the exponential smoothing window.
* @return The error code: if nonzero, can be lsl_argument_error if an unknown flag was passed in.
*/
extern LIBLSL_C_API int lsl_smoothing_halftime(lsl_inlet in, float value);



/* ============================ */
/* ==== lsl_xml_ptr object ==== */
/* ============================ */

/* === XML Tree Navigation === */

/** Get the first child of the element. */
extern LIBLSL_C_API lsl_xml_ptr lsl_first_child(lsl_xml_ptr e);

/** Get the last child of the element. */
extern LIBLSL_C_API lsl_xml_ptr lsl_last_child(lsl_xml_ptr e);

/** Get the next sibling in the children list of the parent node. */
extern LIBLSL_C_API lsl_xml_ptr lsl_next_sibling(lsl_xml_ptr e);

/** Get the previous sibling in the children list of the parent node. */
extern LIBLSL_C_API lsl_xml_ptr lsl_previous_sibling(lsl_xml_ptr e);

/** Get the parent node. */
extern LIBLSL_C_API lsl_xml_ptr lsl_parent(lsl_xml_ptr e);


/* === XML Tree Navigation by Name === */

/** Get a child with a specified name. */
extern LIBLSL_C_API lsl_xml_ptr lsl_child(lsl_xml_ptr e, char *name);

/** Get the next sibling with the specified name. */
extern LIBLSL_C_API lsl_xml_ptr lsl_next_sibling_n(lsl_xml_ptr e, char *name);

/** Get the previous sibling with the specified name. */
extern LIBLSL_C_API lsl_xml_ptr lsl_previous_sibling_n(lsl_xml_ptr e, char *name);


/* === Content Queries === */

/** Whether this node is empty. */
extern LIBLSL_C_API int lsl_empty(lsl_xml_ptr e);

/** Whether this is a text body (instead of an XML element). True both for plain char data and CData. */
extern LIBLSL_C_API int  lsl_is_text(lsl_xml_ptr e);

/** Name of the element. */
extern LIBLSL_C_API char *lsl_name(lsl_xml_ptr e);

/** Value of the element. */
extern LIBLSL_C_API char *lsl_value(lsl_xml_ptr e);

/** Get child value (value of the first child that is text). */
extern LIBLSL_C_API char* lsl_child_value(lsl_xml_ptr e);

/** Get child value of a child with a specified name. */
extern LIBLSL_C_API char* lsl_child_value_n(lsl_xml_ptr e, char *name);


/* === Data Modification === */

/**
* Append a child node with a given name, which has a (nameless) plain-text child with the given text value.
*/
extern LIBLSL_C_API lsl_xml_ptr lsl_append_child_value(lsl_xml_ptr e, char *name, char *value);

/**
* Prepend a child node with a given name, which has a (nameless) plain-text child with the given text value.
*/
extern LIBLSL_C_API lsl_xml_ptr lsl_prepend_child_value(lsl_xml_ptr e, char *name, char *value);

/**
* Set the text value of the (nameless) plain-text child of a named child node.
*/
extern LIBLSL_C_API int lsl_set_child_value(lsl_xml_ptr e, char *name, char *value);

/**
* Set the element's name.
* @return 0 if the node is empty (or if out of memory).
*/
extern LIBLSL_C_API int lsl_set_name(lsl_xml_ptr e, char *rhs);

/**
* Set the element's value.
* @return 0 if the node is empty (or if out of memory).
*/
extern LIBLSL_C_API int lsl_set_value(lsl_xml_ptr e, char *rhs);

/** Append a child element with the specified name. */
extern LIBLSL_C_API lsl_xml_ptr lsl_append_child(lsl_xml_ptr e, char *name);

/** Prepend a child element with the specified name. */
extern LIBLSL_C_API lsl_xml_ptr lsl_prepend_child(lsl_xml_ptr e, char *name);

/** Append a copy of the specified element as a child. */
extern LIBLSL_C_API lsl_xml_ptr lsl_append_copy(lsl_xml_ptr e, lsl_xml_ptr e2);

/** Prepend a child element with the specified name. */
extern LIBLSL_C_API lsl_xml_ptr lsl_prepend_copy(lsl_xml_ptr e, lsl_xml_ptr e2);

/** Remove a child element with the specified name. */
extern LIBLSL_C_API void lsl_remove_child_n(lsl_xml_ptr e, char *name);

/** Remove a specified child element. */
extern LIBLSL_C_API void lsl_remove_child(lsl_xml_ptr e, lsl_xml_ptr e2);




/* ======================================== */
/* ==== lsl_continuous_resolver object ==== */
/* ======================================== */

/**
* Construct a new continuous_resolver that resolves all streams on the network. 
* This is analogous to the functionality offered by the free function resolve_streams().
* @param forget_after When a stream is no longer visible on the network (e.g., because it was shut down),
*                     this is the time in seconds after which it is no longer reported by the resolver.
*                     The recommended default value is 5.0.
*/
extern LIBLSL_C_API lsl_continuous_resolver lsl_create_continuous_resolver(double forget_after);

/**
* Construct a new continuous_resolver that resolves all streams with a specific value for a given property.
* This is analogous to the functionality provided by the free function resolve_stream(prop,value).
* @param prop The stream_info property that should have a specific value (e.g., "name", "type", "source_id", or "desc/manufaturer").
* @param value The string value that the property should have (e.g., "EEG" as the type property).
* @param forget_after When a stream is no longer visible on the network (e.g., because it was shut down),
*                     this is the time in seconds after which it is no longer reported by the resolver.
*                     The recommended default value is 5.0.
*/
extern LIBLSL_C_API lsl_continuous_resolver lsl_create_continuous_resolver_byprop(char *prop, char *value, double forget_after);

/**
* Construct a new continuous_resolver that resolves all streams that match a given XPath 1.0 predicate.
* This is analogous to the functionality provided by the free function resolve_stream(pred).
* @param pred The predicate string, e.g. "name='BioSemi'" or "type='EEG' and starts-with(name,'BioSemi') and count(info/desc/channel)=32"
* @param forget_after When a stream is no longer visible on the network (e.g., because it was shut down),
*                     this is the time in seconds after which it is no longer reported by the resolver.
*                     The recommended default value is 5.0.
*/
extern LIBLSL_C_API lsl_continuous_resolver lsl_create_continuous_resolver_bypred(char *pred, double forget_after);

/**
* Obtain the set of currently present streams on the network (i.e. resolve result).
* @param res A continuous resolver (previously created with one of the lsl_create_continuous_resolver functions).
* @param buffer A user-allocated buffer to hold the current resolve results.
*               Note: it is the user's responsibility to either destroy the resulting streaminfo 
*               objects or to pass them back to the LSL during during creation of an inlet.
*               Note 2: The stream_info's returned by the resolver are only short versions that do not
*               include the .desc() field (which can be arbitrarily big). To obtain the full
*               stream information you need to call .info() on the inlet after you have created one.
* @param buffer_elements The user-provided buffer length.
* @return The number of results written into the buffer (never more than the provided # of slots) 
*         or a negative number if an error has occurred (values corresponding to lsl_error_code_t).
*/
extern LIBLSL_C_API int lsl_resolver_results(lsl_continuous_resolver res, lsl_streaminfo *buffer, unsigned buffer_elements);

/** 
* Destructor for the continuous resolver.
*/
extern LIBLSL_C_API void lsl_destroy_continuous_resolver(lsl_continuous_resolver res);


#ifdef __cplusplus
} /* end extern "C" */
#endif


#ifndef _WIN32
    #pragma GCC visibility pop
#endif

#endif

