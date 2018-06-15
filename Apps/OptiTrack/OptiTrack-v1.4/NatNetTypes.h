/*
   NatNetTypes defines the public, common data structures and types
   used when working with NatNetServer and NatNetClient objects.

   version 1.4.0.0
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
#define MAX_MODELS					200
#define MAX_RIGIDBODIES				1000
#define MAX_NAMELENGTH				256

// Client/server message ids
#define NAT_PING					0 
#define NAT_PINGRESPONSE			1
#define NAT_REQUEST					2
#define NAT_RESPONSE				3
#define NAT_REQUEST_MODELDEF		4
#define NAT_MODELDEF				5
#define NAT_REQUEST_FRAMEOFDATA		6
#define NAT_FRAMEOFDATA				7
#define NAT_MESSAGESTRING			8
#define NAT_UNRECOGNIZED_REQUEST	100

#define UNDEFINED					999999.9999

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

typedef enum ErrorCode
{
    ErrorCode_OK = 0,					
    ErrorCode_Internal,				
    ErrorCode_External,				
    ErrorCode_Network,			
    ErrorCode_Other				
} ErrorCode;

typedef enum DataDescriptors
{
	Descriptor_MarkerSet = 0,
	Descriptor_RigidBody,
	Descriptor_Skeleton
} DataDescriptors;

// data types
typedef double SegmentData[7];				// posX, posY, posZ, rotX, rotY, rotZ, bonelength
typedef float  MarkerData[3];				// posX, posY, posZ

// sender
typedef struct
{
    char          szName[MAX_NAMELENGTH];	// sending app's name
    unsigned char Version[4];				// sending app's version [major.minor.build.revision]
    unsigned char NatNetVersion[4];			// sending app's NatNet version [major.minor.build.revision]

} sSender;

// packet
typedef struct
{
    unsigned short iMessage;				// message ID (e.g. NAT_FRAMEOFDATA)
    unsigned short nDataBytes;				// Num bytes in payload
    union
    {
        unsigned char  cData[20000];
        char           szData[20000];
        unsigned long  lData[5000];
        float          fData[5000];
        sSender        Sender;
    } Data;									// Payload

} sPacket;

// Server application description
typedef struct
{
    bool HostPresent;						// host is present and accounted for
    char szHostComputerName[MAX_NAMELENGTH];// host computer name
    unsigned char HostComputerAddress[4];	// host IP address
    char szHostApp[MAX_NAMELENGTH];			// Name of host app 
    unsigned char HostAppVersion[4];		// Version of host app
    unsigned char NatNetVersion[4];			// Host app's version of NatNet

} sServerDescription;

// Marker Set Definition
typedef struct
{
    char szName[MAX_NAMELENGTH];			// MarkerSet name
    int nMarkers;							// # of markers in MarkerSet
    char** szMarkerNames;					// Array of marker names

} sMarkerSetDescription;

// Marker Set Data (single frame of one MarkerSet )
typedef struct
{
    char szName[MAX_NAMELENGTH];			// MarkerSet name
    int nMarkers;							// # of markers in MarkerSet
    MarkerData* Markers;					// 2D Array of marker data ( [nMarkers][3] )

} sMarkerSetData;

// Rigid Body Definition
typedef struct
{
	int ID;									// Rigid body indentifier
  int parentID;           // ID of parent rigid body in case hierarchy exists
  float offsetx, offsety, offsetz; //offset position relative to parent
} sRigidBodyDescription;

// Rigid Body Data ( single frame of one rigid body )
typedef struct
{
    int ID;									// ID
    float x, y, z;							// Position
    float qx, qy, qz, qw;					// Orientation
    int nMarkers;							// number of markers associated with this rigid body
    MarkerData* Markers;					// marker data ( [nMarkers][3] )

} sRigidBodyData;

// Single data definition
typedef struct 
{
	int type;
    union
    {
		sMarkerSetDescription* MarkerSetDescription;
		sRigidBodyDescription* RigidBodyDescription;
    } Data;									
} sDataDescription;

// All data definitions for current session (as defined buy host app)
typedef struct
{
	int nDataDescriptions;
	sDataDescription arrDataDescriptions[MAX_MODELS];
} sDataDescriptions;

// Single frame of data (for n MocapModels)
typedef struct
{
    int iFrame;									// host defined frame number
    int nMarkerSets;							// # of marker sets in this frame of data
    sMarkerSetData MocapData[MAX_MODELS];		// MarkerSet data
    int nOtherMarkers;							// # of undefined markers
    MarkerData* OtherMarkers;					// undefined marker data
	int nRigidBodies;							// # of rigid bodies
	sRigidBodyData RigidBodies[MAX_RIGIDBODIES];// Rigid body data
    float fLatency;								// host defined time delta between capture and send

} sFrameOfMocapData;
