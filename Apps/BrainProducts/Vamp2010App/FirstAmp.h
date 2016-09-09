/*----------------------------------------------------------------------------*/
/*
  FirstAmp DLL header file
*/
/*----------------------------------------------------------------------------*/
#ifndef ModID_FIRST_AMP_H
#define ModID_FIRST_AMP_H
/*----------------------------------------------------------------------------*/
/* Defines */

#ifdef FIRSTAMP_EXPORTS
/*! FIRSTAMP_API functions as being exported from a DLL */
#define FIRSTAMP_API __declspec(dllexport)
#else
/*! FIRSTAMP_API functions as being imported from a DLL */
#define FIRSTAMP_API __declspec(dllimport)
#endif

/*! Error codes */
#define FA_ERR_OK (0) /*!< Success (no errors) */
#define FA_ERR_ID (-1) /*!< Invalid ID (such ID not present now) */
#define FA_ERR_FAIL (-2) /*!< Function fail (internal error) */
#define FA_ERR_HANDLE (-3) /*!< Invalid handle (driver already used) */
#define FA_ERR_PARAM (-4) /*!< Invalid function parameter(s) */

/*! Invalid device ID */
#define FA_ID_INVALID (0)

/*! FirstAmp models */
#define FA_MODEL_16 (1) /*!< 16+1 main and 2 auxiliary channels */
#define FA_MODEL_8 (0) /*!< 8+1 main and 2 auxiliary channels */

/*! FirstAmp model 16 channels count */
#define FA_MODEL_16_CHANNELS_MAIN (16 + 1) /*!< 16 (EEG) + 1 (REF) channels */
#define FA_MODEL_16_CHANNELS_AUX (2) /*!< 2 auxiliary channels */

/*! FirstAmp model 8 channels count */
#define FA_MODEL_8_CHANNELS_MAIN (8 + 1) /*!< 8 (EEG) + 1 (REF) channels */
#define FA_MODEL_8_CHANNELS_AUX (2) /*!< 2 auxiliary channels */

/*! FirstAmp model 19/8 channels count in 20 kHz mode */
#define FA_MODE_20_KHZ_CHANNELS_MAIN (4) /*!< 4 (EEG) channels */
#define FA_MODE_20_KHZ_CHANNELS_AUX (0) /*!< 0 auxiliary channels */

/*! FirstAmp status bits (t_faDataModelX.Status) in data type */
#define FA_STATUS_BIT_IN_COUNT (9) /*!< Count of digital inputs */
#define FA_STATUS_BIT_IN_SHIFT (0) /*!< Position of digital inputs bits from LSB */
#define FA_STATUS_BIT_OUT_COUNT (1) /*!< Count of digital outputs */
#define FA_STATUS_BIT_OUT_SHIFT (9) /*!< Position of digital outputs bits from LSB */
/*! Total status bits count */
#define FA_STATUS_BITS_COUNT (FA_STATUS_BIT_IN_COUNT + FA_STATUS_BIT_OUT_COUNT)

/*! Size of user data area, bytes */
#define FA_USER_DATA_SIZE (512) /* do not change this */
/*----------------------------------------------------------------------------*/
/* Data Types */

/*! 
  Device information structure 
  Note:
  If serial number = 0, it's means that device has not production information.
  In such case - production date also invalid (= 0) and can not be used.
  Model for this situation - always FA_MODEL_16.
*/
typedef struct {
  unsigned int Model; /*!< Device model ID */
  unsigned int SerialNumber; /*!< Device serial number */
  FILETIME ProductionDate; /*!< Device production date and time */
} t_faInformation;

/*! Device property structure */
typedef struct {
 float RateEeg; /*!< EEG sampling rate, Hz */
 float RateAux; /*!< AUX sampling rate, Hz */
 float ResolutionEeg; /*!< EEG amplitude scale coefficients, V/bit */
 float ResolutionAux; /*!< AUX amplitude scale coefficients, V/bit */
 float RangeEeg; /*!< EEG input range, V */
 float RangeAux; /*!< AUX input range, V */
} t_faProperty;

/*! Device digital input and output structure */
typedef struct {
  unsigned int Inputs; /*!< Bits according digital inputs number */
  unsigned int Outputs; /*!< Bits according digital outputs number */
} t_faIo;

#pragma pack(push, 1) /* override default packing for structures */

/*! Device data type for FirstAmp 16 model */
typedef struct {
  signed int Main[FA_MODEL_16_CHANNELS_MAIN]; /*!< main channels data, 16 channels + REF channel */
  signed int Aux[FA_MODEL_16_CHANNELS_AUX]; /*!< auxiliary channels data, 2 channels */
  unsigned int Status; /*!< Digital inputs (bits 0 - 8) + output (bit 9) state + 22 MSB reserved bits */
  unsigned int Counter; /*!< 32-bit data sequencing cyclic counter */
} t_faDataModel16;

/*! Device data type for FirstAmp 8 model */
typedef struct {
  signed int Main[FA_MODEL_8_CHANNELS_MAIN]; /*!< main channels data, 8 channels + REF channel */
  signed int Aux[FA_MODEL_8_CHANNELS_AUX]; /*!< auxiliary channels data, 2 channels */
  unsigned int Status; /*!< Digital inputs (bits 0 - 8) + output (bit 9) state + 22 MSB reserved bits */
  unsigned int Counter; /*!< 32-bit data sequencing cyclic counter */
} t_faDataModel8;

/*! Device data type for acquisition modes = 20 kHz */
typedef struct {
  signed int Main[FA_MODE_20_KHZ_CHANNELS_MAIN]; /*!< main channels data, 4 channels */
  unsigned int Status; /*!< Digital inputs (bits 0 - 8) + output (bit 9) state + 22 MSB reserved bits */
  unsigned int Counter; /*!< 32-bit data sequencing cyclic counter */
} t_faDataFormatMode20kHz;

/*! Device data state structure */
typedef struct {
  unsigned int Size; /*!< Size of this structure */
  /*! Buffers state */
  unsigned int DllBufferSize; /*!< Size of dll buffer, bytes */
  unsigned int DllBufferCount; /*!< Count of data in dll buffer, bytes */
  unsigned int DriverBufferSize; /*!< Size of driver buffer, bytes */
  unsigned int DriverBufferCount; /*!< Count of data in driver buffer, bytes */
  unsigned int DeviceBufferSize; /*!< Size of device buffer, bytes */
  unsigned int DeviceBufferCount; /*!< Count of data in device buffer, bytes */
  /*! Statistics, reset at call of faStart(Id) */
  unsigned int DataSamples; /*!< Total simultaneously reading data, samples */
  unsigned int DataCounterErrors; /*!< Total cyclic counter errors (loss of data) */
} t_faDataState;

/*! 
  Data acquisition modes settings.

  Note for 20 kHz mode:
    - device can acqusition 4 channels, monopolar or differential.
    - set channel's number for positive and negative input of differential pair.
    - device channels numbers starting from 0, i.e. at label channel 1 = internal 0.
    - for mopolar set positive's channel to wishful, 
      and negative to invalid (any number not present in model, for example -1).

  Examples:
   - example for differential pair 0 (channel 1 - channel 2)
     var.Mode20kHz4Channels.ChannelsPos[0] = 1 - 1;
     var.Mode20kHz4Channels.ChannelsNeg[0] = 2 - 1; 
   - example for monopolar pair 1 (channel REF)
     var.Mode20kHz4Channels.ChannelsPos[1] = FA_MODEL_16_CHANNELS_MAIN - 1;
     var.Mode20kHz4Channels.ChannelsNeg[1] = -1;
   - example for differential pair 2 (channel 1 - channel REF)
     var.Mode20kHz4Channels.ChannelsPos[2] = 1 - 1;
     var.Mode20kHz4Channels.ChannelsNeg[2] = FA_MODEL_16_CHANNELS_MAIN - 1;
   - example for inverted monopolar pair 3 (inverted channel 1)
     var.Mode20kHz4Channels.ChannelsPos[3] = -1;
     var.Mode20kHz4Channels.ChannelsNeg[3] = 1 - 1;
*/
typedef union {
  /* settings for 20 kHz mode */
  struct {
    int ChannelsPos[FA_MODE_20_KHZ_CHANNELS_MAIN]; /* positive input in pair */
    int ChannelsNeg[FA_MODE_20_KHZ_CHANNELS_MAIN]; /* negative input in pair */
  } Mode20kHz4Channels;
} t_faDataModeSettings;

#pragma pack(pop) /* restore default packing for structures */

/*! Data acquisition mode */
typedef enum {
  dmNormal = 0, /* normal - all channels at 2 kHz */
  dm20kHz4Channels, /* fast - 20 kHz, 4 Channels, no auxiliary */
} t_faDataMode;
/*----------------------------------------------------------------------------*/
/* Functions */

/*!
  Function return count of device currently connected to system
\return - device count
*/
FIRSTAMP_API unsigned int WINAPI faGetCount(void);

/*!
  Function return device ID on order number (from 0 to faGetCount() - 1)
\param Number - order number of device
\return - device ID
*/
FIRSTAMP_API int WINAPI faGetId(unsigned int Number);

/*----------------------------------------------------------------------------*/

/*!
  Function to open device for work
\param Id - device ID
\return - error code
*/
FIRSTAMP_API int WINAPI faOpen(int Id);

/*!
  Function to close device for work
\param Id - device ID
\return - error code
*/
FIRSTAMP_API int WINAPI faClose(int Id);
/*----------------------------------------------------------------------------*/
/*!  
  Function to start data acquisition
\param Id - device ID
\return - error code
*/
FIRSTAMP_API int WINAPI faStart(int Id);

/*!  
  Function to stop data acquisition
\param Id - device ID
\return - error code
*/
FIRSTAMP_API int WINAPI faStop(int Id);

/*!  
  Function return accusition data from internal buffer.
  If some channels is not connect, data values are equals to INT_MAX for its.
  Data return in format t_faDataModel16 or t_faDataModel8 or
  t_faDataFormatMode20kHz, depends from model and acquisition mode.
  Needs call this function until not return <= 0, 
  for prevent overflow of internal buffer.
\param Id - device ID
\param Buffer - pointer to buffer for device data.
\param Size - size of buffer for device data, bytes.
\return
  if > 0 - count of bytes copied to buffer
  if = 0 - no more data in internal buffer
  if < 0 - error code  
*/
FIRSTAMP_API int WINAPI faGetData(int Id, void *Buffer, unsigned int Size);

/*----------------------------------------------------------------------------*/
/*!  
  Function return device information
  Note:
  - if device has not this information in EEPROM (for example not programming or
    data corruption) than serial number in t_faInformation structure set to 0 
    and device information structure return as for unprogramming device.
    See note for t_faInformation structure.
\param Id - device ID
\param Information - address of t_faInformation structure for return data
\return - error code
*/
FIRSTAMP_API int WINAPI faGetInformation(int Id, t_faInformation *Information);

/*!  
  Function return device property
\param Id - device ID
\param Property - address of t_faProperty structure for return data
\return - error code
*/
FIRSTAMP_API int WINAPI faGetProperty(int Id, t_faProperty *Property);
/*----------------------------------------------------------------------------*/
/*!  
  Function to start impedance measure.
  Note:
    - for correct work of impedance measure (faGetImpedance()) needs to start 
      data acqusition (faStart()) before start of impedance measure (faStartImpedance()).
\param Id - device ID
\return - error code
*/
FIRSTAMP_API int WINAPI faStartImpedance(int Id);

/*!
  Function to stop impedance measure.
  Note:
    - this function only stop impedance measure, but not stop data acqusition.
\param Id - device ID
\return - error code
*/
FIRSTAMP_API int WINAPI faStopImpedance(int Id);

/*!
  Function return impedance values for all channels and ground electrode in Ohm.
  Remarks (notes):
  - ground electrode must be connected for impedance measure
  - for correct measure must be connected ground electrode and minimum 2 electodes
  - if connect only ground and 1 electrode, value of ground impedance equal zero
    and value of electrode equal sum of real ground and electrode impedance
  - if electrode not connect, value equal to INT_MAX (from <limits.h>)
\param Id - device ID
\param Buffer - pointer to destination buffer for N (channels) + 1 (ground) values in Ohm
\param Size - size of destination buffer, bytes
\return - error code  
*/
FIRSTAMP_API int WINAPI faGetImpedance(int Id, unsigned int *Buffer, unsigned int Size);
/*----------------------------------------------------------------------------*/
/*!  
  Function to start of calibration signal on EEG channles
  Note:
  - calibration signal is square signal with period 2 sec with amplitude 200 uV,
    (on-off time ratio 1 : 1)
  - amplitute correct only on unconnected channels.
  - on connected electrodes amplitude of calibration signal uncorrect (devided 
    between electrode impedance and channel output impedance )
\param Id - device ID
\return - error code
*/
FIRSTAMP_API int WINAPI faStartCalibration(int Id);

/*!
  Function to stop of calibration signal on EEG channles
\param Id - device ID
\return - error code
*/
FIRSTAMP_API int WINAPI faStopCalibration(int Id);
/*----------------------------------------------------------------------------*/
/*!
  Function to get state of outputs and inputs
\param Id - device ID
\param Io - pointer to buffer in format of t_faIo
\return - error code
*/
FIRSTAMP_API int WINAPI faGetIo(int Id, t_faIo *Io);

/*!
  Function to set state of outputs
\param Id - device ID
\param Io - pointer to buffer in format of t_faIo (values for inputs ignored)
\return - error code
*/
FIRSTAMP_API int WINAPI faSetIo(int Id, t_faIo *Io);
/*----------------------------------------------------------------------------*/
/*!
  Function to show Windows bitmap on device LCD display
  Performance (for 24-bpp Windows bitmaps):  
  ~ 220 - 230 ms - for bitmaps with real colors count > 256
  ~ 115 - 120 ms - for bitmaps with real colors count <= 256
  - For max performance needs to use 24-bpp Windows bitmaps    
  - With 16-bit or 32-bit bitmaps time is increasing on ~50 ms
    due to Windows GDI function GetDIBits() overhead
  - Test Release version of the DLL on Windows XP, AMD Athlon 1.3 GHz, 512 MB
  Note:
  - colors count calculate in 320 x 240 pixels area, if input bitmap has
    smaller size - must be < 256 (<= 255) colors for max performance
  - device LCD display have 320 pixels x 240 pixels x 16 bit color
  - for Windows bitmap with any color depth function provide internal
    convertion and output to device
  - original bitmap is not modified
  - if bitmap smaller then device LCD diplay remaning area is black
  - if bitmap bigger then bitmap remainng area is ignored
  RLE compression:
  - RLE compression is using for decrease size of data for transfer to device
  - function automatic use RLE if it will produce better result than normal
\param Id - device ID
\param Bitmap - handle to Windows bitmap with any color depth
\return - error code
*/
FIRSTAMP_API int WINAPI faSetBitmap(int Id, HBITMAP Bitmap);
/*----------------------------------------------------------------------------*/
/*!
  Function to get contrast of LCD display
\param Id - device ID
\param Contrast - contrast, % (0-100)
\return - error code
*/
FIRSTAMP_API int WINAPI faGetContrast(int Id, unsigned int *Contrast);

/*!
  Function to set contrast of LCD display
\param Id - device ID
\param Contrast - contrast, % (0-100)
\return - error code
*/
FIRSTAMP_API int WINAPI faSetContrast(int Id, unsigned int Contrast);

/*!
  Function to get brightness of LCD display (see note to faSetBrightness)
\param Id - device ID
\param Brightness - brightness, % (0-100)
\return - error code
*/
FIRSTAMP_API int WINAPI faGetBrightness(int Id, unsigned int *Brightness);

/*!
  Function to set brightness of LCD display
  Note:
  - device hardware real support 4 levels of brightness
  - software convert percents to hardware levels with linear scale
    (0 % - min brightness, 100 % - max brightness)
\param Id - device ID
\param Brightness - brightness, % (0-100)
\return - error code
*/
FIRSTAMP_API int WINAPI faSetBrightness(int Id, unsigned int Brightness);
/*----------------------------------------------------------------------------*/
/*!
  Function to read user (application software) data from device nonvolatile memory.
  Note:
  - max size of user data equal to FA_USER_DATA_SIZE
  - device internal protect user data integrity with CRC code
\param Id - device ID
\param Buffer - pointer to buffer for data
\param Size - size of buffer for data, bytes
\param Count - pointer to count of reading data, bytes
\return - error code
*/
FIRSTAMP_API int WINAPI faGetUserData(int Id, void *Buffer, unsigned int Size, 
  unsigned int *Count);

/*!
  Function to write user (application software) data to device nonvolatile memory.
  Note:
  - max size of user data equal to FA_USER_DATA_SIZE
  - device internal protect user data integrity with CRC code
\param Id - device ID
\param Buffer - pointer to buffer for data
\param Size - size of buffer for data, bytes
\param Count - pointer to count of writing data, bytes
\return - error code
*/
FIRSTAMP_API int WINAPI faSetUserData(int Id, void *Buffer, unsigned int Size, 
  unsigned int *Count);
/*----------------------------------------------------------------------------*/
/*!
  Function to get data state information
  Note:
  - for future modification (increasing) of t_faDataState structure needs to
    set t_faDataState.Size field to size of actual t_faDataState format 
    before call this function.
\param Id - device ID
\param  DataState - pointer to buffer with t_faDataState structure format
\return - error code
*/
FIRSTAMP_API int WINAPI faGetDataState(int Id, t_faDataState *DataState);
/*----------------------------------------------------------------------------*/
/*!
  Function to get data acquisition mode
\param Id - device ID
\param Mode - pointer to data acquisition mode variable
\param Settings - pointer to optinal data acquisition mode settings buffer
\return - error code
*/
FIRSTAMP_API int WINAPI faGetDataMode(int Id, t_faDataMode *Mode, 
  t_faDataModeSettings *Settings);

/*!
  Function to set data acquisition mode.
  Warning:
    - do not change mode during acqusition, previous stop.
    - impedance measure in 20 kHz mode not working, needs set normal mode
  Note:    
    - for set normal mode call faSetDataMode(Id, dmNormal, NULL)
    - for set 20 kHz mode call faSetDataMode(Id, dm20kHz4Channels, &settings)
\param Id - device ID
\param Mode - data acquisition mode
\param Settings - optinal data acquisition mode settings
\return - error code
*/
FIRSTAMP_API int WINAPI faSetDataMode(int Id, t_faDataMode Mode, 
  t_faDataModeSettings *Settings);
/*----------------------------------------------------------------------------*/
#endif /* ModID_XXX_H */
