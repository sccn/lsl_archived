/*
NatNetTypes defines the public, common data structures and types
used when working with NatNetServer and NatNetClient objects.

version 2.2.0.0
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
#define MAX_RBMARKERS               10      // maximum number of markers per RigidBody
#define MAX_SKELETONS               100     // maximum number of skeletons
#define MAX_SKELRIGIDBODIES         200     // maximum number of RididBodies per Skeleton

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
#define NAT_UNRECOGNIZED_REQUEST    100

#define UNDEFINED                   999999.9999

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
    Descriptor_Skeleton
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
        unsigned char  cData[20000];
        char           szData[20000];
        unsigned long  lData[5000];
        float          fData[5000];
        sSender        Sender;
    } Data;                                 // payload

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
    sRigidBodyData()
    {
        Markers = 0; MarkerIDs = 0; MarkerSizes = 0;
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

// Tracked Object data description.  
// A Mocap Server application (e.g. Arena or TrackingTools) may contain multiple
// tracked "objects (e.g. RigidBody, MarkerSet).  Each object will have its
// own DataDescription.
typedef struct 
{
    int type;
    union
    {
        sMarkerSetDescription* MarkerSetDescription;
        sRigidBodyDescription* RigidBodyDescription;
        sSkeletonDescription*  SkeletonDescription;
    } Data;									
} sDataDescription;

// All data descriptions for current session (as defined by host app)
typedef struct
{
    int nDataDescriptions;
    sDataDescription arrDataDescriptions[MAX_MODELS];
} sDataDescriptions;

// Single frame of data (for all tracked objects)
typedef struct
{
    int iFrame;                                 // host defined frame number
    int nMarkerSets;                            // # of marker sets in this frame of data
    sMarkerSetData MocapData[MAX_MODELS];       // MarkerSet data
    int nOtherMarkers;                          // # of undefined markers
    MarkerData* OtherMarkers;                   // undefined marker data
    int nRigidBodies;                           // # of rigid bodies
    sRigidBodyData RigidBodies[MAX_RIGIDBODIES];// Rigid body data
    int nSkeletons;                             // # of Skeletons
    sSkeletonData Skeletons[MAX_SKELETONS];     // Skeleton data
    float fLatency;                             // host defined time delta between capture and send

} sFrameOfMocapData;
