/*----------------------------------------------------------------------------*/
/*
  ActiChamp DLL header file
*/
/*----------------------------------------------------------------------------*/
#ifndef ModID_CHAMP_H
#define ModID_CHAMP_H
/*----------------------------------------------------------------------------*/
/* Includes */

#ifndef WINAPI
  #include <windows.h>
#endif
/*----------------------------------------------------------------------------*/
/* Defines */

#ifdef ACTICHAMP_EXPORTS
/*! CHAMP_API functions as being exported from a DLL */
#define CHAMP_API __declspec(dllexport)
#else
/*! CHAMP_API functions as being imported from a DLL */
#define CHAMP_API __declspec(dllimport)
#endif

/*! Error codes */
#define CHAMP_ERR_OK     (0)	/*!< Success (no errors) */
#define CHAMP_ERR_HANDLE (-1)	/*!< Invalid handle (such handle not present now) */
#define CHAMP_ERR_PARAM  (-2)   /*!< Invalid function parameter(s) */
#define CHAMP_ERR_FAIL   (-3)	/*!< Function fail (internal error) */

/*! Invalid (not connected) impedance value */
#define CHAMP_IMP_INVALID (INT_MAX)

/*! Electrodes leds definitions */
#define CHAMP_EL_LED_OFF (0) /* Off leds */
#define CHAMP_EL_LED_GREEN (1 << 0) /* Green led on */
#define CHAMP_EL_LED_RED (1 << 1) /* Red led on */
#define CHAMP_EL_LED_YELLOW (AC_EL_LED_GREEN | AC_EL_LED_RED) /* Yellow led on */

/*! Electrodes analog switch definition */
#define CHAMP_EL_SWITCH_ON (1 << 2) /* Input analog switch to GND */
#define CHAMP_EL_SWITCH_OFF (0 << 2) /* Input analog switch to input */

/*! Parameters for function StartExt */
#define CHAMP_START_WITH_DEFAULT_PARAMS (0)
#define CHAMP_START_WITH_CHECK_CALIBRATION_COEFS (1)
/*----------------------------------------------------------------------------*/
/* Data Types */

#pragma pack(push, 1) /* override default packing for structures */

/* Version info about ActiChamp */
typedef struct {
  DWORD Dll;
  DWORD Driver;
  DWORD Cypress;
  DWORD Fpga;
  DWORD Msp430;
} t_champVersion;

/*! Mode enum */
typedef enum {
  CHAMP_MODE_NORMAL = 0, /* normal data acquisition */
  CHAMP_MODE_ACTIVE_SHIELD = 1, /* data acquisition with ActiveShield */
  CHAMP_MODE_IMPEDANCE = 2, /* impedance measure */
  CHAMP_MODE_TEST = 3, /* test signal (square wave 200 uV, 1 Hz)  */
} t_champMode;

/*! Samples rate (physical) enum */
typedef enum {
  CHAMP_RATE_10KHZ = 0, /* 10 kHz, all channels (default mode) */
  CHAMP_RATE_50KHZ = 1, /* 50 kHz, all channels */
  CHAMP_RATE_100KHZ = 2, /* 100 kHz, max 64 channels */
} t_champRate;

/*!  ADC data filter */
typedef enum {
	CHAMP_ADC_NATIVE = 0, /* no ADC data filter */
	CHAMP_ADC_AVERAGING_2 = 1, /* ADC data moving avaraging filter by 2 samples */
} t_champAdcFilter;

/*!  ADC data decimation */
typedef enum {
	CHAMP_DECIMATION_0 = 0, /* no decimation */
	CHAMP_DECIMATION_2 = 2, /* decimation by 2 */
} t_champDecimation;

typedef struct {
  t_champMode Mode; /* mode of acquisition */
  t_champRate Rate; /* samples rate */
} t_champSettings;

typedef struct {
  t_champMode Mode; /* mode of acquisition */
  t_champRate Rate; /* samples rate */
  t_champAdcFilter AdcFilter; /* ADC data filter */
  t_champDecimation Decimation; /* ADC data decimation */
} t_champSettingsEx;

/*! Device property structure */
typedef struct {
  unsigned int CountEeg;	/* numbers of Eeg channels */
  unsigned int CountAux;	/* numbers of Aux channels */
  unsigned int TriggersIn;  /* numbers of input triggers */
  unsigned int TriggersOut;	/* numbers of output triggers */
  float Rate;					/*!< Sampling rate, Hz */
  float ResolutionEeg;			/*!< EEG amplitude scale coefficients, V/bit */
  float ResolutionAux;			/*!< AUX amplitude scale coefficients, V/bit */
  float RangeEeg;				/*!< EEG input range peak-peak, V */
  float RangeAux;				/*!< AUX input range peak-peak, V */
} t_champProperty;

/*! Device data type for ActiChamp-AUX model */
typedef struct {
  signed int Aux[8]; /*!< auxiliary channels data */
  unsigned int Triggers; /*!< Digital inputs (bits 0 - 7) + output (bits 8 - 15) state + 
							  module error (bit 16: Aux module, bits 17 - 21: Eeg modules) + 
							  module present (bit 22: Aux module, bits 23 - 27: Eeg modules) + 
							  3 MSB reserved bits + MyButton bit (bit 31) */
  unsigned int Counter; /*!< 32-bit data sequencing cyclic counter for checking the data loss */
} t_champDataModelAux;

/*! Device data type for ActiChamp-32 model */
typedef struct {
  signed int Main[32]; /*!< main channels data */
  signed int Aux[8]; /*!< auxiliary channels data */
  unsigned int Triggers; /*!< Digital inputs (bits 0 - 7) + output (bits 8 - 15) state + 
							  module error (bit 16: Aux module, bits 17 - 21: Eeg modules) + 
							  module present (bit 22: Aux module, bits 23 - 27: Eeg modules) + 
							  3 MSB reserved bits + MyButton bit (bit 31) */
  unsigned int Counter; /*!< 32-bit data sequencing cyclic counter for checking the data loss */
} t_champDataModel32;

/*! Device data type for ActiChamp-64 model */
typedef struct {
  signed int Main[64]; /*!< main channels data */
  signed int Aux[8]; /*!< auxiliary channels data */
  unsigned int Triggers; /*!< Digital inputs (bits 0 - 7) + output (bits 8 - 15) state + 
							  module error (bit 16: Aux module, bits 17 - 21: Eeg modules) + 
							  module present (bit 22: Aux module, bits 23 - 27: Eeg modules) + 
							  3 MSB reserved bits + MyButton bit (bit 31) */
  unsigned int Counter; /*!< 32-bit data sequencing cyclic counter for checking the data loss */
} t_champDataModel64;

/*! Device data type for ActiChamp-96 model */
typedef struct {
  signed int Main[96]; /*!< main channels data */
  signed int Aux[8]; /*!< auxiliary channels data */
  unsigned int Triggers; /*!< Digital inputs (bits 0 - 7) + output (bits 8 - 15) state + 
							  module error (bit 16: Aux module, bits 17 - 21: Eeg modules) + 
							  module present (bit 22: Aux module, bits 23 - 27: Eeg modules) + 
							  3 MSB reserved bits + MyButton bit (bit 31) */
  unsigned int Counter; /*!< 32-bit data sequencing cyclic counter for checking the data loss */
} t_champDataModel96;

/*! Device data type for ActiChamp-128 model */
typedef struct {
  signed int Main[128]; /*!< main channels data */
  signed int Aux[8]; /*!< auxiliary channels data */
  unsigned int Triggers; /*!< Digital inputs (bits 0 - 7) + output (bits 8 - 15) state + 
							  module error (bit 16: Aux module, bits 17 - 21: Eeg modules) + 
							  module present (bit 22: Aux module, bits 23 - 27: Eeg modules) + 
							  3 MSB reserved bits + MyButton bit (bit 31) */
  unsigned int Counter; /*!< 32-bit data sequencing cyclic counter for checking the data loss */
} t_champDataModel128;

/*! Device data type for ActiChamp-160 model */
typedef struct {
  signed int Main[160]; /*!< main channels data */
  signed int Aux[8]; /*!< auxiliary channels data */
  unsigned int Triggers; /*!< Digital inputs (bits 0 - 7) + output (bits 8 - 15) state + 
							  module error (bit 16: Aux module, bits 17 - 21: Eeg modules) + 
							  module present (bit 22: Aux module, bits 23 - 27: Eeg modules) + 
							  3 MSB reserved bits + MyButton bit (bit 31) */
  unsigned int Counter; /*!< 32-bit data sequencing cyclic counter for checking the data loss */
} t_champDataModel160;

/*! Device data status */
typedef struct {
  unsigned int Samples; /* Total samples */
  unsigned int Errors; /* Total errors */
  float Rate; /*!< Data rate, Hz */
  float Speed; /*!< Data speed, MB/s */
} t_champDataStatus;

/*! Device error status */
typedef struct {
  unsigned int Samples; /* Total samples */
  unsigned int Crc; /* Crc errors on data samples */
  unsigned int Counter; /* Counter errors on data samples */
  unsigned int Modules[1 + 5]; /* Errors on modules, index = 0 - AUX, index = 1..5 - module 1..5  */
} t_champErrorStatus;

/*! Impedance settings structure */
typedef struct {
  /*!< Between Good and Bad level is indicate as both leds (yellow emulation)*/
  unsigned int Good; /*!< Good level (green led indication), Ohm */
  unsigned int Bad; /*!< Bad level (red led indication), Ohm */
  unsigned int LedsDisable; /*!< Disable electrode's leds, if not zero */
  unsigned int TimeOut; /*!< Impedance mode time-out (0 - 65535), sec */
} t_champImpedanceSetup;

/*! Impedance control structure */
typedef struct {
  /* READ-WRITE information */
  unsigned int Splitter; /*!< Current splitter for impedance measure, (0 .. Splitters - 1) */
						 /*!< if = Splitters, measure on all electrodes) */
  /* READ-ONLY information, ignored when write (set) */
  unsigned int Splitters; /*!< Count of splitters in actiCap device */
  unsigned int Electrodes; /*!< Electrodes channels count */
  unsigned int ElectrodeFrom; /*!< Electrode from which impedance measure */
  unsigned int ElectrodeTo; /*!< Electrode to which impedance measure */
  unsigned int Time; /*!< Time in impedance mode, sec */
} t_champImpedanceMode;

/*! Voltages structure */
typedef struct {
  float VDC; /*!< Power supply, V */
  float V3; /*!< Internal 3.3, V */
  float Temperature; /*!< Celsius degrees */
  // + voltages valid only during data acquisition
  float DVDD3; /*!< Digital 3.3, V */
  float AVDD3; /*!< Analog 3.3, V */
  float AVDD5; /*!< Analog 5.0, V */
  float REF; /*!< Reference 2.048, V */
} t_champVoltages;

/*! Module control structure */
typedef struct {
  /*
	Bits:
	0 - AUX module
	1 - 5 - Main EEG modules (1 - 5)
	6 - 31 - Reserved
  */
  unsigned int Present; /*!< Bits indicate that module present in hardware */
  unsigned int Enabled; /*!< Bits indicate that module enable for use */
} t_champModules;

/*! MyButton enum */
typedef enum {
  CHAMP_MY_BUTTON_NONE = 0, /* Button not pressed (default) */
  CHAMP_MY_BUTTON_HOLD = 1, /* Button pressed and hold by user */
  CHAMP_MY_BUTTON_PUSH = 2, /* Button change state from none to hold (press by user) */
  CHAMP_MY_BUTTON_PULL = 3  /* Button change state from hold to none (free by user) */
} t_champMyButton;

typedef struct {
	unsigned int Model;
	unsigned int SerialNumber;
	char padding[1024];
} t_champDeviceInfo;


#pragma pack(pop) /* restore default packing for structures */
/*----------------------------------------------------------------------------*/
/* Functions */
/*----------------------------------------------------------------------------*/
/*!
  Function to get version info about ActiChamp
  Note:
  - if hDevice = NULL, only Dll version is valid.
\param hDevice - device handle
\param Version - pointer to structure with version
\return - error code
*/
CHAMP_API int WINAPI champGetVersion(HANDLE hDevice, t_champVersion *Version);
/*----------------------------------------------------------------------------*/
/*!
  Function return count of device currently connected to system
\return - device count
*/
CHAMP_API unsigned int WINAPI champGetCount(void);

/*!
  Function to open device for work
\param Number - device number from 0 to champGetCount()
\return - device handle or NULL (if fail)
*/
CHAMP_API HANDLE WINAPI champOpen(unsigned int Number);

/*!
  Function to close device for work
\param hDevice - device handle
\return - error code
*/
CHAMP_API int WINAPI champClose(HANDLE hDevice);
/*----------------------------------------------------------------------------*/
/*!
  Function to get device settings for acquisition
\param hDevice - device handle
\param Settings - pointer to structure with settings
\return - error code
*/
CHAMP_API int WINAPI champGetSettingsEx(HANDLE hDevice, t_champSettingsEx *SettingsEx);

/*!
  Function to set device settings for acquisition
\param hDevice - device handle
\param Settings - pointer to structure with settings
\return - error code
*/
CHAMP_API int WINAPI champSetSettingsEx(HANDLE hDevice, t_champSettingsEx *SettingsEx);

/*!
  Function to get device settings for acquisition
\param hDevice - device handle
\param Settings - pointer to structure with settings
\return - error code
*/
CHAMP_API int WINAPI champGetSettings(HANDLE hDevice, t_champSettings *Settings);

/*!
  Function to set device settings for acquisition
\param hDevice - device handle
\param Settings - pointer to structure with settings
\return - error code
*/
CHAMP_API int WINAPI champSetSettings(HANDLE hDevice, t_champSettings *Settings);

/*!
  Function to get device property of acquisition
\param hDevice - device handle
\param Settings - pointer to structure with settings
\return - error code
*/
CHAMP_API int WINAPI champGetProperty(HANDLE hDevice, t_champProperty *Property);

/*!  
  Function to start data acquisition
\param hDevice - device handle
\return - error code
*/
CHAMP_API int WINAPI champStart(HANDLE hDevice);

/*!  
  Function to start data acquisition with additional options
\param hDevice - device handle
\param Params - additional options
\return - error code
*/
CHAMP_API int WINAPI champStartExt(HANDLE hDevice, unsigned int Params);

/*!  
  Function to stop data acquisition
\param hDevice - device handle
\return - error code
*/
CHAMP_API int WINAPI champStop(HANDLE hDevice);

/*!  
  Function return accusition data from internal buffer.
  Data return in format t_champDataModelxxx, depends from device model.
  Needs call this function until not return <= 0, for prevent overflow of internal buffer.
\param hDevice - device handle
\param Buffer - pointer to buffer for device data.
\param Size - size of buffer for device data, bytes.
\return
  if > 0 - count of bytes copied to buffer
  if = 0 - no more data in internal buffer
  if < 0 - error code  
*/
CHAMP_API int WINAPI champGetData(HANDLE hDevice, void *Buffer, unsigned int Size);


/*!  
  Function to read data in blocking mode.
  Data is returned in t_champDataModelxxx format, depending on the device model.
  This function returns after the requested amount of data is available from device.
  This function is NOT reentrant and it's up to the application to ensure that the device is not closed
  as long as GetDataBlocking is in progress.
  The amount of data bytes for a defined time interval can be calculated as:
  Size = (CountEeg + CountAux + 2) * sizof(int) * Rate[Hz] * Interval[s]
\param hDevice - device handle
\param Buffer - pointer to buffer for device data.
\param Size - size of buffer for device data, bytes.
\return
  if > 0 - count of bytes copied to buffer
  if = 0 - no more data in internal buffer
  if < 0 - error code  
*/
CHAMP_API int WINAPI champGetDataBlocking(HANDLE hDevice, void *Buffer, unsigned int Size);

/*!
  Function to get data status of acquisition
\param hDevice - device handle
\param DataStatus - pointer to structure with data status
\return - error code
*/
CHAMP_API int WINAPI champGetDataStatus(HANDLE hDevice, t_champDataStatus *DataStatus);

/*!
  Function to get error status of acquisition
\param hDevice - device handle
\param ErrorStatus - pointer to structure with error status
\return - error code
*/
CHAMP_API int WINAPI champGetErrorStatus(HANDLE hDevice, t_champErrorStatus *ErrorStatus);
/*----------------------------------------------------------------------------*/
/*!
  Function to get state of triggers (input and output).
  8-bit inputs (bits 0 - 7) + 8-bit outputs (bits 8 - 15) + 16 MSB reserved bits.
\param hDevice - device handle
\param Triggers - pointer to 32-bit variable for get state of triggers
\return - error code
*/
CHAMP_API int WINAPI champGetTriggers(HANDLE hDevice, unsigned int *Triggers);

/*!
  Function to set state of triggers (output only).
  8-bit inputs (bits 0 - 7) + 8-bit outputs (bits 8 - 15) + 16 MSB reserved bits.
\param hDevice - device handle
\param Triggers - 32-bit variable for set state of triggers
\return - error code
*/
CHAMP_API int WINAPI champSetTriggers(HANDLE hDevice, unsigned int Triggers);
/*----------------------------------------------------------------------------*/
/*!
  Function to get impedance values for all EEG channels and ground in Ohm.

  Remarks (notes):
  - ~750 ms is required for measure impedance per 32 electrodes.
  - max impedance value ~ 300-500 kOhm
  - impedance measure from 0 Ohm to 120 kOhm with accuracity +/- 15%
  - work only in impedance mode.
  - ground electrode must be connected for impedance measure.
  - REF electrode (1-st electrode on 1-st module) must be connected for impedance measure.
  - if electrode not connect, value equal to CHAMP_IMP_INVALID(INT_MAX from <limits.h>)

\param hDevice - device handle
\param Buffer - pointer to destination buffer values in Ohm 
  for N channels (equal CountEeg field in t_champProperty) + 1 GND (ground).
\param Size - size of destination buffer, bytes
\return - error code
*/
CHAMP_API int WINAPI champImpedanceGetData(HANDLE hDevice, unsigned int *Buffer, unsigned int Size);

/*!
  Function to get settings for impedance mode.
\param hDevice - device handle
\param Setup - settings for impedance mode
\return - error code
*/
CHAMP_API int WINAPI champImpedanceGetSetup(HANDLE hDevice, t_champImpedanceSetup *Setup);

/*!
  Function to set settings for impedance mode.
\param hDevice - device handle
\param Setup - settings for impedance mode
\return - error code
*/
CHAMP_API int WINAPI champImpedanceSetSetup(HANDLE hDevice, t_champImpedanceSetup *Setup);

/*!
  Function to get current impedance mode
  Note:
    - see t_champImpedanceMode structure decription,
      fields Splitter, ElectrodeFrom, ElectrodeTo
	  is valid only in impedance mode      
\param hDevice - device handle
\param Control - pointer to t_champImpedanceMode structure
\return - error code
*/
CHAMP_API int WINAPI champImpedanceGetMode(HANDLE hDevice, t_champImpedanceMode *Mode);

/*!
  Function to set current impedance mode
  Note:
    - must call in impedance mode 
    - see t_champImpedanceMode structure decription
    - only Splitter field is apply, all other's are ignored      
\param hDevice - device handle
\param Control - pointer to t_champImpedanceMode structure
\return - error code
*/
CHAMP_API int WINAPI champImpedanceSetMode(HANDLE hDevice, t_champImpedanceMode *Mode);
/*----------------------------------------------------------------------------*/
/*!
  Function to direct set (control) electrode's state (leds and analog switch) in all modes.

  Remarks (notes):
    - direct control of electrodes is start automatic with call of this function
    - for disable direct control of electrodes call champSetElectrodes(Id, NULL, 0)
    - to simplify control is needs to set states of all electrodes at one time
    - for set electrodes colors recomend use CHAMP_EL_LED_XXX (see defines) values.

  Warning (limitations) of using:
    - During data acquisition due to switching supply current (i.e. leds), 
      analog power supply voltage ripple is appear. Which follows to crosstalk 
      to low level input signal and to distortion (add some pulsation)
      after switching supply current (i.e. leds).
    - in Impedance mode input analog switch is not accessable.
    - in Impedance mode it is not recomend to call this function very frequently 
      (> few times per 1 sec) or to change (from previous state) too many electrodes, 
      because this will significantly increase impedance measure time (cycle) - see below.
    - After success call this function device will execute this command:
	  ~ 50 ms per 32 electrodes states changes from previous state

\param hDevice - device handle
\param Buffer - pointer to source buffer values for electrodes states
  for N channels (equal CountEeg field in t_champProperty) + 1 GND (ground).
\param Size - size of source buffer, bytes
\return - error code
*/
CHAMP_API int WINAPI champSetElectrodes(HANDLE hDevice, unsigned int *Buffer, unsigned int Size);
/*----------------------------------------------------------------------------*/
/*!
  Function to get device settings for acquisition
\param hDevice - device handle
\param Settings - pointer to buffer for results
\return - error code
*/
CHAMP_API int WINAPI champGetVoltages(HANDLE hDevice, t_champVoltages *Voltages);
/*----------------------------------------------------------------------------*/
/*!
  Function to get modules configuration for acquisition
\param hDevice - device handle
\param Modules - pointer to buffer for results
\return - error code
*/
CHAMP_API int WINAPI champGetModules(HANDLE hDevice, t_champModules *Modules);
/*----------------------------------------------------------------------------*/
/*!
  Function to set modules configuration for acquisition
\param hDevice - device handle
\param Modules - pointer to buffer for results
\return - error code
*/
CHAMP_API int WINAPI champSetModules(HANDLE hDevice, t_champModules *Modules);
/*----------------------------------------------------------------------------*/
/*!
  Function to get status of MyButton
\param hDevice - device handle
\param MyButton - pointer to to buffer for results
\return - error code
*/
CHAMP_API int WINAPI champGetMyButton(HANDLE hDevice, t_champMyButton *MyButton);


/*----------------------------------------------------------------------------*/
/*!
  Function to control LED of MyButton via pulse-width modulation
  Note:
  - if DutyCycle = 100 %, allways on.
  - if DutyCycle = 0 %, allways off.
\param hDevice - device handle
\param Period - Period, ms
\param DutyCycle - Duty cycle, %
\return - error code
*/
CHAMP_API int WINAPI champSetMyButtonLed(HANDLE hDevice, unsigned int Period, unsigned int DutyCycle);
/*----------------------------------------------------------------------------*/
/*!
  Function to set gain in ActiveShield mode
  Note: 
	- at default gain = 100.
	- gain value must be from 1 to 100
\param hDevice - device handle
\param Gain - gain value (1 - 100)
\return - error code
*/
CHAMP_API int WINAPI champSetActiveShieldGain(HANDLE hDevice, unsigned int Gain);
/*----------------------------------------------------------------------------*/
/*!
  Function to get additional error code and string
  This function recomend use if some other function return CHAMP_ERR_FAIL and
  needs additional information.
  Note:
	- error string on English only
\param hDevice - device handle
\param Code - pointer to error code variable
\param Buffer - pointer to error string buffer
\param Size - size of error string buffer
\return - error code
*/
CHAMP_API int WINAPI champGetError(HANDLE hDevice, int *Code, char *Buffer, unsigned int Size);
/*----------------------------------------------------------------------------*/
#endif /* ModID_XXX_H */
