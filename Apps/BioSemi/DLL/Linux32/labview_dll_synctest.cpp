#include <stdio.h> 
#include <stdlib.h>
#include <time.h>

#ifdef WIN32
#include <windows.h> 
#include <conio.h>
#include <string.h>
#else
// linux
#include <unistd.h>	// usleep
#include <errno.h>
#include <signal.h>
//#include <bits/sigset.h>
#endif // WIN32

#include "labview_dll.h"
#include "bsif.h"

#ifndef WIN32
// linux
#define VOID	void
#define LPVOID	void*
#define MEM_COMMIT	0
#define MEM_RELEASE	0
#define PAGE_READWRITE	0

typedef void (*sighandler_t)(int);

static int	do_exit=0;

static int _kbhit(void)
	{
	return do_exit;
	}

static int _getch()
	{
	do_exit = 0;
	return (int)'q';
	}

static void sighandler(int signum)
	{
	printf("got signal %d\n", signum);
	do_exit = 1;
	}

static int GetLastError()
	{
	return errno;
	}

static char *VirtualAlloc(int *junk1, SIZE_T size, int junk2, int junk3)
	{
	return (char *)malloc(size);
	}

static int VirtualFree(PUCHAR junk1, SIZE_T junk2, int junk3)
	{
	free(junk1);
	return 1;
	}

static void Sleep(int msecs)
        {// sleep for msecs milliseconds
        struct timespec waitTime;

	if (msecs > 0)
		{
        	waitTime.tv_sec = msecs/1000;
        	waitTime.tv_nsec = (msecs % 1000)*1000*1000;

        	while (nanosleep(&waitTime, &waitTime)!=0)
			;
		}

        return;
        }

#endif

VOID 
PrintLastErrorAsText()
	{
#ifdef WIN32
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), 0,
    		(LPTSTR) &lpMsgBuf, 0, NULL);
	printf("%s\n", (LPSTR)lpMsgBuf);
#endif // WIN32
	return;
	}

#ifdef WIN32
int __cdecl
#else
int 
#endif
main (int nargs, char *argv[]) 

	{
	int	nsecs;
	HANDLE	usbHandle;
	BOOL	nStatus;
	int	sec;
	char	infoBuffer[120];
	INT_PTR	ringBufferSize;
	int	plusBytes;
#ifndef WIN32
        struct sigaction sigact;
 
        sigact.sa_handler = sighandler;
        sigemptyset(&sigact.sa_mask);
        sigact.sa_flags = 0;
        sigaction(SIGINT, &sigact, NULL);
        sigaction(SIGTERM, &sigact, NULL);
        sigaction(SIGQUIT, &sigact, NULL);
#endif

	printf("****************************************\n");
	printf("Parameters can be, in any order:\n");
	printf("    Time to run, in seconds:\n");
	printf("        default is 600 seconds (10 minutes)\n");
	printf("        0=unlimited\n\n");
	printf("    Ring buffer size, in 512 byte units,\n");
	printf("        must have a 'b' or 'B' prefix:\n");
	printf("        default is B65536 for 32 MBytes\n\n");
#ifdef WIN32
	printf("    Ring buffer extra bytes beyond the 512 multiple,\n");
	printf("        must have a '+' prefix and be a multiple of 4:\n");
	printf("        default is +0 for no extra bytes\n\n");
#endif	// WIN32
#ifdef WIN32
	printf("\nHit any key to quit\n");
#else
	printf("\nHit ctrl-c to quit\n");
#endif
	printf("****************************************\n");

	printf("sizeof long = %d\n", sizeof(long));

	nsecs = 600;
	ringBufferSize = 65536;
	plusBytes = 0;

	for (int i=1; i<nargs; i++)
		if ((argv[i][0] == 'b') || (argv[i][0] == 'B'))
			ringBufferSize = atoi(argv[i]+1);
		else if (argv[i][0] == '+')
			plusBytes = atoi(argv[i]+1);
		else
			nsecs = atoi(argv[i]);

	if (plusBytes/4*4 != plusBytes)
		{
		printf("\n*** +bytes not a multiple of 4! ***\n\n");
		return 1;
		}
	sec = 0;

	if ((usbHandle=OPEN_DRIVER_ASYNC()) == (HANDLE)NULL)
		{
		printf("Can't open device driver!\n");
		return 1;
		}	

	/********************************
	 * allocate the large ring buffer
	 * *****************************/

	PUCHAR	samBuffer;

	INT_PTR bytes2Use = ((INT_PTR)ringBufferSize)*512 + (INT_PTR)plusBytes;
	
	printf("ring buffer size = %d\n", bytes2Use);

	if ((samBuffer=(PUCHAR)VirtualAlloc(NULL, (SIZE_T)bytes2Use, MEM_COMMIT, 
		PAGE_READWRITE)) == 0)
		{
		printf("VirtualAlloc returns 0, GetLastError = %d\n",
			GetLastError());
		PrintLastErrorAsText();
		return 0;
		}

	/**********************************************
	 * connect the ring buffer and the usb driver
	 * *******************************************/

	READ_MULTIPLE_SWEEPS(usbHandle, (PCHAR)samBuffer, bytes2Use);
	
	/************************************************
	 * start the flow of samples to the ring buffer
	 * *********************************************/

	CHAR	controlBuffer[64];
//	ULONG	bytesWritten;

	for (int i=0; i<64; i++)
		controlBuffer[i] = 0;
	controlBuffer[0] = (CHAR)(-1);

	Sleep(1000);		// msec sleep

	nStatus = USB_WRITE(usbHandle, &controlBuffer[0]);	// enable

	if (nStatus == FALSE)
		{
		// ERROR
		printf ("usb_write for enable handshake trouble %d\n",
			GetLastError()); 
		PrintLastErrorAsText();
		return 7;
		}

//	DWORD	seam;
	INT_PTR	seam=0;
	INT_PTR	lastSeam=0;
	double	dsec;

	PUCHAR	curBuffer = samBuffer;

	INT_PTR	bytesRead;
	BOOL	first;
	int	nextSync;
	int	lastSync;
	time_t	startTime;
	time_t	curTime;
	time_t	lastRateTime;
	INT_PTR	lastRateSeam;
	int	nch;
	int	stride;
	BOOL	pointerSync;
	int	printFreq;
	BOOL	read_p_ret;

	first=TRUE;
	nextSync=0;
	lastSync=0;
	startTime=time(NULL);
	lastRateTime = startTime;
	lastRateSeam = 0;
	stride = 0;		// until known
	pointerSync = FALSE;	// until known
	printFreq = 10;		// every 10 buffers, if stride is 64 msec; adjusted when stride known
	infoBuffer[0] = 0;

	int rate = 0;
	INT_PTR seamSpan = 0;
	int nStatusLines = 0;
	for (int nbuffers=0;;nbuffers++)
		{
		if (pointerSync == FALSE)
#ifdef WIN32
			if (stride > 20)
#endif	// WIN32
				Sleep((stride==0)?125:(stride>1)?(stride/2):1);

		if (nsecs > 0)
			{// time limited run
   			curTime = time(NULL);			// Get system time
   			sec = (int)difftime(curTime, startTime);
			if (sec > nsecs)
				{// run time limit reached
				printf("*** Run time limit (%d seconds) reached\n", nsecs);
				break;				// time to quit
				}
			}

		/********************************
		 * wait for a buffer
		 * *****************************/

		for (;;)
			{
			bytesRead = 0;

			// quit if a key is hit
			if (_kbhit())
				{
				_getch();
				break;
				}

			if ((read_p_ret=READ_POINTER(usbHandle, &seam)) == FALSE)
				{
				printf("READ_POINTER returns FALSE -> acquisition not running!\n");
				break;		// assume acquisition stopped
				}
			if (seam != lastSeam)
				{
				bytesRead = seam - lastSeam;
				if (bytesRead < 0)
					bytesRead += bytes2Use;
				if (bytesRead > 0)
					break;
				}

			if (pointerSync == FALSE)
#ifdef WIN32
				if (stride > 20)
#endif	// WIN32
					Sleep((stride==0)?125:(stride>1)?(stride/2):1);
			}

		if (bytesRead == 0)
			break;		// key hit
		if (read_p_ret == FALSE)
			break;		// assume acquisition stopped

   		curTime = time(NULL); 		// Get system time

		// every 5 seconds calculate sample rate per channel
   		double dsecRate = difftime(curTime, lastRateTime);
		if (dsecRate > 4.9)
			{
			seamSpan = seam - lastRateSeam;
			if (seamSpan < 0)
				seamSpan += bytes2Use;
			rate = (int)((double)seamSpan/((double)nch*dsecRate*4.));
			lastRateTime = curTime;
			lastRateSeam = seam;
			}

		// every 'printFreq' buffers print status line
		if (nbuffers%printFreq == 0)
			{
			if ((infoBuffer[0] == 0) && (nbuffers > 1))
				{
				// get info delayed because actual stride
				// isn't determined until end of first buffer
				GET_DRIVER_INFO(infoBuffer, sizeof(infoBuffer));
				printf("%s\n", infoBuffer);

				// determine stride and sync
				if (stride == 0)
					{
					stride = BSIF_GET_STRIDE();
					if ((stride < 0) || (stride > 30000))
						stride = 0;
					else if (stride == 0)
						stride = 1;
					// print every 10th buffer when stride is 64 msec
					printFreq = (stride>640)?1:(640/stride);
					pointerSync = BSIF_GET_SYNC();
					}
				}
			
   			dsec = difftime(curTime, startTime);
			printf("at second %d, buffer %d, #bytes %d, seam %d, rate/channel %d\n",
				(int)dsec, nbuffers, (int)bytesRead, seam, rate);
			nStatusLines++;		

			if (nStatusLines >= 20)
				{
				printf("****************************************\n");
#ifdef WIN32
				printf("Hit any key to quit\n");
#else
				printf("Hit ctrl-c to quit\n");
#endif
				printf("****************************************\n");
				nStatusLines = 0;
				}
	
			}

		lastSeam = seam;

		INT_PTR longs2Check = samBuffer+seam - curBuffer;
		if (longs2Check < 0)
			longs2Check += bytes2Use;
		longs2Check /= 4;

		if (first == TRUE)
			{// determine number of channels
			nch = 0;
			for (int fstSync=0; fstSync<longs2Check; fstSync++)
				if (((int *)samBuffer)[fstSync] == 0xffffff00)
					{
					if (fstSync != 0)
						printf("1st buffer, 1st sync at byte %d !!!\n", fstSync*4);

					for (int nxtSync=fstSync+1; nxtSync<longs2Check; nxtSync++)
						if (((int *)samBuffer)[nxtSync] == 0xffffff00)
							{
							nch = nxtSync - fstSync;
							lastSync -= nch;
							break;
							}
					break;
					}
			if (nch == 0)
				printf("Can't find 2 sync codes to determine number of channels\n");
			else
				printf("number of channels = %d\n", nch);
			first = FALSE;
			}
		
		// check for sync words at the right places and replace them for next pass 		
		int ii;
		int nmissing=0;
		for (ii=0; ii<longs2Check; ii+=nch,nextSync+=nch)
			{
			if (nextSync*4 >= bytes2Use)
				 nextSync -= (INT)bytes2Use/4;
			if (((int *)samBuffer)[nextSync] != 0xffffff00)
				{
				nmissing++;
				printf("missing sync %d: buffer %d, [%d] = %8lx\n",
					nmissing, nbuffers, nextSync,
					((int *)samBuffer)[nextSync]);
				if (nmissing > 5)
					{
					printf("more than 5 missing syncs in 1 buffer\n");
					break;
					}

				int j;
				for (j=ii; j<longs2Check; j++,nextSync++)
					{
					if (nextSync*4 >= bytes2Use)
						nextSync -= (INT)bytes2Use/4;
					if (((int *)samBuffer)[nextSync] == 0xffffff00)
						{
						printf("sync found again at [%d] = %8lx\n",
							nextSync, ((int *)samBuffer)[nextSync]);
						break;
						}
					}

				if (j < longs2Check)
					{
					ii = j;
					((int *)samBuffer)[nextSync] = 0x01234567;
					lastSync = nextSync;
					continue;
					}
				else
					{
					ii = j;
					printf("no sync found in rest of buffer\n");
					break;
					}
				}

			((int *)samBuffer)[nextSync] = 0x01234567;
			lastSync = nextSync;
			}

		if ((nmissing > 0) || (ii < longs2Check))
			break;		// missing sync

		// update buffer address
		curBuffer = samBuffer + nextSync*4;
		if (curBuffer >= samBuffer+bytes2Use)
			curBuffer -= bytes2Use;

		}  // end of sampling loop

	printf("stopping ...\n");

	/***************************************
	 * stop sample flow into ring buffer
	 * ************************************/
	controlBuffer[0] = 0;

	nStatus = USB_WRITE(usbHandle, (PCHAR)&controlBuffer[0]);
		
	if (nStatus == FALSE)
		{
		// ERROR
		printf ("USB_WRITE for disable handshake trouble %d\n",
			GetLastError()); 
		PrintLastErrorAsText();
		return 7;
		}
	
	CLOSE_DRIVER(usbHandle);

	if (infoBuffer[0] != 0)
		printf("%s\n", infoBuffer);

	if (VirtualFree(samBuffer, (SIZE_T)0, MEM_RELEASE) == 0)
		{
		printf("VirtualFree returns 0, GetLastError = %d\n",
			GetLastError());
		PrintLastErrorAsText();
		return 0;
		}

	if ((nsecs > 0) && (sec < nsecs))
		{
#ifdef WIN32
		printf("Aborted, hit a key to exit\n");
#else
		printf("Aborted, hit ctrl-c to exit\n");
#endif
		for (;;)
			if (_kbhit())
				{
				_getch();
				break;
				}
			else
				Sleep(1000);	// 1 second
		}
	else
		{
#ifdef WIN32
		printf("Done, hit a key to exit\n");
#else
		printf("Done, hit ctrl-c to exit\n");
#endif
		for (;;)
			if (_kbhit())
				{
				_getch();
				break;
				}
			else
				Sleep(1000);	// 1 second
		}
printf("Done!!!\n");
	return 0; 
	}
