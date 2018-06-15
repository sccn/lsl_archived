/*
NatNetTypes defines the public, common data structures and types
used when working with NatNetServer and NatNetClient objects.

version 2.8.0.0
*/

#pragma once

// storage class specifier
// - to link to NatNet dynamically, define NATNETLIB_IMPORTS and link to the NatNet dynamic lib.
// - to link to NatNet statically, link to the NatNet static lib
#ifdef NATNETLIB_EXPORTS
#define DLLCLASS __declspec( dllexport )
#else
#ifdef NATNETLIB_IMPORTS
#define DLLCLASS __declspec( dllimport )
#else
#define DLLCLASS 
#endif
#endif

// model limits
#define MAX_MODELS                  200     // maximum number of MarkerSets 
#define MAX_RIGIDBODIES             1000    // maximum number of RigidBodies
#define MAX_NAMELENGTH              256     // maximum length for strings
#define MAX_MARKERS                 200     // maximum number of markers per MarkerSet
#define MAX_RBMARKERS               20      // maximum number of markers per RigidBody
#define MAX_SKELETONS               100     // maximum number of skeletons
#define MAX_SKELRIGIDBODIES         200     // maximum number of RididBodies per Skeleton
#define MAX_LABELED_MARKERS         1000    // maximum number of labeled markers per frame
#define MAX_UNLABELED_MARKERS       1000    // maximum number of unlabeled (other) markers per frame

#define MAX_FORCEPLATES             8       // maximum number of force plates
#define MAX_ANALOG_CHANNELS         32      // maximum number of data channels (signals) per analog/force plate device
#define MAX_ANALOG_SUBFRAMES        30      // maximum number of analog/force plate frames per mocap frame

#define MAX_PACKETSIZE				100000	// max size of packet (actual packet size is dynamic)

// Client/server message ids
#define NAT_PING                    0 
#define NAT_PINGRESPONSE            1
#define NAT_REQUEST                 2
#define NAT_RESPONSE                3
#define NAT_REQUEST_MODELDEF        4
#define NAT_MODELDEF                5
#define NAT_REQUEST_FRAMEOFDATA     6
#define NAT_FRAMEOFDATA             7
#define NAT_MESSAGESTRING           8
#define NAT_DISCONNECT              9 
#define NAT_UNRECOGNIZED_REQUEST    100

#define UNDEFINED                   999999.9999

typedef enum NAT_EulerOrder
{
    NAT_XYZs, NAT_XYXs, NAT_XZYs, NAT_XZXs, NAT_YZXs, NAT_YZYs, NAT_YXZs, NAT_YXYs, NAT_ZXYs, NAT_ZXZs, NAT_ZYXs, NAT_ZYZs,
    NAT_ZYXr, NAT_XYXr, NAT_YZXr, NAT_XZXr, NAT_XZYr, NAT_YZYr, NAT_ZXYr, NAT_YXYr, NAT_YXZr, NAT_ZXZr, NAT_XYZr, NAT_ZYZr
} NAT_EulerOrder;


// NatNet uses to set reporting level of messages.
// Clients use to set level of messages to receive.
typedef enum Verbosity
{
    Verbosity_None=0,   
    Verbosity_Info,     
    Verbosity_Warning, 
    Verbosity_Error,
    Verbosity_Debug,
} Verbosity;

// NatNet error reporting codes
typedef enum ErrorCode
{
    ErrorCode_OK = 0,					
    ErrorCode_Internal,				
    ErrorCode_External,				
    ErrorCode_Network,			
    ErrorCode_Other				
} ErrorCode;

// NatNet connection types
typedef enum ConnectionType
{
    ConnectionType_Multicast = 0,
    ConnectionType_Unicast
} ConnectionType;

// NatNet data types
typedef enum DataDescriptors
{
    Descriptor_MarkerSet = 0,
    Descriptor_RigidBody,
    Descriptor_Skeleton,
    Descriptor_ForcePlate
} DataDescriptors;

typedef float MarkerData[3];                // posX, posY, posZ

// sender
typedef struct
{
    char szName[MAX_NAMELENGTH];            // host app's name
    unsigned char Version[4];               // host app's version [major.minor.build.revision]
    unsigned char NatNetVersion[4];         // host app's NatNet version [major.minor.build.revision]

} sSender;

// packet
// note : only used by clients who are depacketizing NatNet packets directly
typedef struct
{
    unsigned short iMessage;                // message ID (e.g. NAT_FRAMEOFDATA)
    unsigned short nDataBytes;              // Num bytes in payload
    union
    {
        unsigned char  cData[MAX_PACKETSIZE];
        char           szData[MAX_PACKETSIZE];
        unsigned long  lData[MAX_PACKETSIZE/4];
        float          fData[MAX_PACKETSIZE/4];
        sSender        Sender;
    } Data;                                 // payload - statically allocated for convenience.  Actual packet size is determined by  nDataBytes

} sPacket;

// Mocap server application description
typedef struct
{
    bool HostPresent;                       // host is present and accounted for
    char szHostComputerName[MAX_NAMELENGTH];// host computer name
    unsigned char HostComputerAddress[4];   // host IP address
    char szHostApp[MAX_NAMELENGTH];         // name of host app 
    unsigned char HostAppVersion[4];        // version of host app
    unsigned char NatNetVersion[4];         // host app's version of NatNet

} sServerDescription;

// Marker
typedef struct
{
    int ID;                                 // Unique identifier
    float x;                                // x position
    float y;                                // y position
    float z;                                // z position
    float size;                             // marker size
    short params;                            // host defined parameters
} sMarker;

// MarkerSet Definition
typedef struct
{
    char szName[MAX_NAMELENGTH];            // MarkerSet name
    int nMarkers;                           // # of markers in MarkerSet
    char** szMarkerNames;                   // array of marker names

} sMarkerSetDescription;

// MarkerSet Data (single frame of one MarkerSet)
typedef struct
{
    char szName[MAX_NAMELENGTH];            // MarkerSet name
    int nMarkers;                           // # of markers in MarkerSet
    MarkerData* Markers;                    // Array of marker data ( [nMarkers][3] )

} sMarkerSetData;

// Rigid Body Definition
typedef struct
{
    char szName[MAX_NAMELENGTH];            // RigidBody name
    int ID;                                 // RigidBody identifier
    int parentID;                           // ID of parent Rigid Body (in case hierarchy exists)
    float offsetx, offsety, offsetz;        // offset position relative to parent
} sRigidBodyDescription;

// Rigid Body Data (single frame of one rigid body)
typedef struct sRigidBodyData
{
    int ID;                                 // RigidBody identifier
    float x, y, z;                          // Position
    float qx, qy, qz, qw;                   // Orientation
    int nMarkers;                           // Number of markers associated with this rigid body
    MarkerData* Markers;                    // Array of marker data ( [nMarkers][3] )
    int* MarkerIDs;                         // Array of marker IDs
    float* MarkerSizes;                     // Array of marker sizes
    float MeanError;                        // Mean measure-to-solve deviation
    short params;                           // Host defined tracking flags
    sRigidBodyData()
    {
        Markers = 0; MarkerIDs = 0; MarkerSizes = 0; params=0;
    }
} sRigidBodyData;

// Skeleton Description
typedef struct sSkeletonDescription
{
    char szName[MAX_NAMELENGTH];                             // Skeleton name
    int skeletonID;                                          // Skeleton identifier
    int nRigidBodies;                                        // # of rigid bodies (bones) in skeleton
    sRigidBodyDescription RigidBodies[MAX_SKELRIGIDBODIES];  // array of rigid body (bone) descriptions 
} sSkeletonDescription;


// Skeleton Data
typedef struct
{
    int skeletonID;                                          // Skeleton identifier
    int nRigidBodies;                                        // # of rigid bodies
    sRigidBodyData* RigidBodyData;                           // Array of RigidBody data
} sSkeletonData;

typedef struct
{
    int ID;                                         // used for order, and for identification in the data stream
    char strSerialNo[128];                          // for unique plate identification
    float fWidth;                                   // plate physical width (manufacturer supplied)
    float fLength;                                  // plate physical length (manufacturer supplied)
    float fOriginX, fOriginY, fOriginZ;             // electrical center offset (from electrical center to geometric center-top of force plate) (manufacturer supplied)
    float fCalMat[12][12];                          // force plate calibration matrix (for raw analog voltage channel type only)
    float fCorners[4][3];                           // plate corners, in plate coordinates, clockwise from plate +x,+y (refer to C3D spec for details)
    int iPlateType;                                 // force plate 'type' (refer to C3D spec for details) 
    int iChannelDataType;                           // 0=Calibrated force data, 1=Raw analog voltages
    int nChannels;                                  // # of channels (signals)
    char szChannelNames[MAX_ANALOG_CHANNELS][MAX_NAMELENGTH];   // channel names
} sForcePlateDescription;

// Tracked Object data description.  
// A Mocap Server application (e.g. Arena or TrackingTools) may contain multiple
// tracked "objects (e.g. RigidBody, MarkerSet).  Each object will have its
// own DataDescription.
typedef struct 
{
    int type;
    union
    {
        sMarkerSetDescription*  MarkerSetDescription;
        sRigidBodyDescription*  RigidBodyDescription;
        sSkeletonDescription*   SkeletonDescription;
        sForcePlateDescription* ForcePlateDescription;
    } Data;
} sDataDescription;

// All data descriptions for current session (as defined by host app)
typedef struct
{
    int nDataDescriptions;
    sDataDescription arrDataDescriptions[MAX_MODELS];
} sDataDescriptions;


typedef struct
{
    int nFrames;                                    // # of analog frames of data in this channel data packet (typically # of subframes per mocap frame)
    float Values[MAX_ANALOG_SUBFRAMES];             // values
} sAnalogChannelData;

typedef struct
{
    int ID;                                         // ForcePlate ID (from data description)
    int nChannels;                                  // # of channels (signals) for this force plate
    sAnalogChannelData ChannelData[MAX_ANALOG_CHANNELS];// Channel (signal) data (e.g. Fx[], Fy[], Fz[])
    short params;                                   // Host defined flags
} sForcePlateData;

// Single frame of data (for all tracked objects)
typedef struct
{
    int iFrame;                                     // host defined frame number
    int nMarkerSets;                                // # of marker sets in this frame of data
    sMarkerSetData MocapData[MAX_MODELS];           // MarkerSet data
    int nOtherMarkers;                              // # of undefined markers
    MarkerData* OtherMarkers;                       // undefined marker data
    int nRigidBodies;                               // # of rigid bodies
    sRigidBodyData RigidBodies[MAX_RIGIDBODIES];    // Rigid body data
    int nSkeletons;                                 // # of Skeletons
    sSkeletonData Skeletons[MAX_SKELETONS];         // Skeleton data
    int nLabeledMarkers;                            // # of Labeled Markers
    sMarker LabeledMarkers[MAX_LABELED_MARKERS];    // Labeled Marker data (labeled markers not associated with a "MarkerSet")
    int nForcePlates;                               // # of force plates
    sForcePlateData ForcePlates[MAX_FORCEPLATES];   // Force plate data
    float fLatency;                                 // host defined time delta between capture and send
    unsigned int Timecode;                          // SMPTE timecode (if available)
    unsigned int TimecodeSubframe;                  // timecode sub-frame data
    double fTimestamp;                              // FrameGroup timestamp
    short params;                                   // host defined parameters
} sFrameOfMocapData;


