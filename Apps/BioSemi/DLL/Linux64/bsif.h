/*
 * Public bsif header file
 *
 * Copyright (C) 2003-2010 Biosemi <www.biosemi.com>
 */

#ifndef __BSIF_H__
#define __BSIF_H__

#ifdef WIN32
#define LINKAGE __cdecl
#else
// linux
#define LINKAGE
#endif

#ifndef __cplusplus
#define BOOL    int
#define FALSE   0
#define TRUE    1
#endif  // __cplusplus

#ifndef WIN32
// linux
#include <stdint.h>
#include <stddef.h>
#define HANDLE  void*
#ifdef __cplusplus
#ifndef BOOL
#define BOOL    bool
#endif	// BOOL
#ifndef FALSE
#define FALSE   false
#endif	// FALSE
#ifndef TRUE
#define TRUE    true
#endif	// TRUE
#endif	// __cplusplus
#define CHAR    char
#define PCHAR   char*
#define PUCHAR  unsigned char*
#define DWORD   int
#define SIZE_T  size_t
#define ULONG   unsigned long
#define PULONG  unsigned long*
#define INT_PTR intptr_t
#define PINT_PTR        intptr_t*
#define INT     int
#define VOID	void
#define LPVOID	void*
#endif

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
BOOL LINKAGE BSIF_READ_POINTER(HANDLE hdevice, PINT_PTR pointer);
VOID LINKAGE BSIF_SET_DEBUG(BOOL onoff);
VOID LINKAGE BSIF_SET_LOG(BOOL onoff);
VOID LINKAGE BSIF_SET_SYNC(BOOL onoff);
BOOL LINKAGE BSIF_GET_SYNC();
VOID LINKAGE BSIF_SET_STRIDE(int msec);
INT LINKAGE BSIF_GET_STRIDE();
HANDLE LINKAGE BSIF_OPEN_DRIVER();
BOOL LINKAGE BSIF_READ_MULTIPLE_SWEEPS(HANDLE WinUsbHandle, PCHAR data, INT_PTR nBytesToRead);
BOOL LINKAGE BSIF_USB_WRITE(HANDLE hdevice, PCHAR data);
BOOL LINKAGE BSIF_CLOSE_DRIVER(HANDLE hdevice);
PCHAR LINKAGE BSIF_GET_DRIVER_INFO(PCHAR infoBuffer, SIZE_T infoSize);
BOOL LINKAGE BSIF_DOWNLOAD_FX2_IIC_FIRMWARE();
BOOL LINKAGE BSIF_DOWNLOAD_FX2_EEPROM();
VOID BSIF_OPEN_KEYBOARD(VOID);
VOID BSIF_CLOSE_KEYBOARD(VOID);
BOOL BSIF_READ_KEYBOARD(int *keycode);
VOID BSIF_MY_LOAD();
VOID BSIF_MY_UNLOAD();
#ifdef __cplusplus
}
#endif  // __cplusplus


#endif	// __BSIF_H__
