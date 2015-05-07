//---------------------------------------------------------------------------


#pragma hdrstop

#include "StreamThread.h"
#include "stdio.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)



/*
 * Use this constructor when reading data manually.
 * Primary used to have asynchronous opening of inlet.
 */


TStreamThread::TStreamThread(char *typeNam):TThread(false)
{
	FreeOnTerminate=false;
	strcpy(typeName,typeNam);
	ProcessDataI32 = NULL;
	ProcessDataF32 = NULL;
	ProcessDataF64 = NULL;
	ProcessDataString = NULL;
	count = resampleCount = 0;
	resamplingRate = -1;
	connected = false;
}

TStreamThread::TStreamThread(char * typeNam, TProcessDataI32 PD):TThread(false)
{
		FreeOnTerminate=false;
		strcpy(typeName,typeNam);
		ProcessDataI32 = PD;
		ProcessDataF32 = NULL;
		ProcessDataF64 = NULL;
		ProcessDataString = NULL;
		count = resampleCount = 0;
		resamplingRate = -1.0;
		connected = false;

		//if(errcode != 0)
		//	THROW ERROR

};

TStreamThread::TStreamThread(char * typeNam, TProcessDataF32 PD):TThread(false)
{
		FreeOnTerminate=false;
		strcpy(typeName,typeNam);
		ProcessDataI32 = NULL;
		ProcessDataF32 = PD;
		ProcessDataF64 = NULL;
		ProcessDataString = NULL;
		count = resampleCount = 0;
		resamplingRate = -1.0;
		connected = false;

		//if(errcode != 0)
		//	THROW ERROR

};

TStreamThread::TStreamThread(char * typeNam, TProcessDataF64 PD):TThread(false)
{
		FreeOnTerminate=false;
		strcpy(typeName,typeNam);
		ProcessDataI32 = NULL;
		ProcessDataF32 = NULL;
		ProcessDataF64 = PD;
		ProcessDataString = NULL;
		count = resampleCount = 0;
		resamplingRate = -1.0;
		connected = false;

};

TStreamThread::TStreamThread(char * typeNam, TProcessDataString PD):TThread(false)
{
		FreeOnTerminate=false;
		strcpy(typeName,typeNam);
		ProcessDataI32 = NULL;
		ProcessDataF32 = NULL;
		ProcessDataF64 = NULL;
		ProcessDataString = PD;
		count = resampleCount = 0;
		resamplingRate = -1.0;
		connected = false;

};


__fastcall TStreamThread::~TStreamThread()
{
	lsl_destroy_inlet(inlet);
};

void TStreamThread::SetResamplingRate(double rsamplingRate)
{
	resamplingRate = rsamplingRate;
}

double TStreamThread::GetResamplingRate()
{
	if(resamplingRate < 0) return samplingRate;
	else return resamplingRate;
}

void __fastcall TStreamThread::Execute()
{



	while (!Terminated)
	{
		if(!connected) {
			lsl_streaminfo info;

			int streamsFound = lsl_resolve_bypred(&info,1, typeName, 1,1.0);

			if(streamsFound < 1) {
				if(CONSOLE) printf("Requested Stream not resolved by timeout.\n");
				Sleep(5);
				continue;
			}


			inlet = lsl_create_inlet(info, 300, LSL_NO_PREFERENCE,1);
			lsl_open_stream(inlet,1.0,&errcode);
			//fills out desc field in xml header
			info = lsl_get_fullinfo(inlet,1.0,&errcode);

			if(errcode) {
				if(CONSOLE) printf("error opening stream: %d\n", errcode);
				Sleep(5);

				continue;
			} else {
				connected = true;
				nChannels = lsl_get_channel_count(info);
				samplingRate = lsl_get_nominal_srate(info);
				xmlHeader = lsl_get_xml(info);
				if(CONSOLE) printf("%s\n", xmlHeader);
				if(nChannels > MAX_CHANNELS) {
					if(CONSOLE) printf("Too many channels\n");
					Sleep(5);
					continue;
					//THROW ERROR or replace with new1d
				}
			}
		}

		if(ProcessDataI32) {

			double timestamp = lsl_pull_sample_i(inlet,i32buf, nChannels, 1.0, &errcode);

			if(timestamp) {
				if(resamplingRate <= 0)
					ProcessDataI32(i32buf, nChannels, samplingRate);
				else if(resamplingRate > samplingRate) {
					while(resampleCount/resamplingRate < count/samplingRate) {
						ProcessDataI32(i32buf, nChannels, resamplingRate);
						resampleCount++;
					}
				} else { //if(resamplingRate < samplingRate)
				   if(count/samplingRate > resampleCount/resamplingRate) {

						ProcessDataI32(i32buf, nChannels, resamplingRate);
						resampleCount++;
				   }
				}
				count++;
			} else {
				Sleep(5);
				continue;
			}
		}

		else if(ProcessDataF32) {

			double timestamp = lsl_pull_sample_f(inlet,f32buf, nChannels, 1.0, &errcode);
			if(timestamp) {
				if(resamplingRate <= 0)
					ProcessDataF32(f32buf, nChannels, samplingRate);
				else if(resamplingRate > samplingRate) {
					while(resampleCount/resamplingRate < count/samplingRate) {
						ProcessDataF32(f32buf, nChannels, resamplingRate);
						resampleCount++;
					}
				} else { //if(resamplingRate < samplingRate)
				   if(count/samplingRate > resampleCount/resamplingRate) {

						ProcessDataF32(f32buf, nChannels, resamplingRate);
						resampleCount++;
				   }
				}
				count++;
			} else {
				Sleep(5);
				continue;
			}
		}

		else if(ProcessDataF64) {

			double timestamp = lsl_pull_sample_d(inlet,f64buf, nChannels, 1.0, &errcode);
			if(timestamp) {
				if(resamplingRate <= 0)
					ProcessDataF64(f64buf, nChannels, samplingRate);
				else if(resamplingRate > samplingRate) {
					while(resampleCount/resamplingRate < count/samplingRate) {
						ProcessDataF64(f64buf, nChannels, resamplingRate);
						resampleCount++;
					}
				} else { //if(resamplingRate < samplingRate)
				   if(count/samplingRate > resampleCount/resamplingRate) {

						ProcessDataF64(f64buf, nChannels, resamplingRate);
						resampleCount++;
				   }
				}
				count++;
			} else {
				Sleep(5);
				continue;
			}
		}

		else if(ProcessDataString) {
			double timestamp;
			if(nChannels == 1) {
					timestamp = lsl_pull_sample_str(inlet, &sbuf, 1, 1.0, &errcode);
				if(timestamp) {
					ProcessDataString(sbuf, 1, samplingRate);
					count++;
				} else {
					Sleep(5);
					continue;
				}
			}  else {

				timestamp = lsl_pull_sample_str(inlet, multi_sbuf, nChannels, 1.0, &errcode);
				if(timestamp) {

					ProcessDataString(multi_sbuf[0], 1, samplingRate);
					count++;
				} else {
					Sleep(5);
					continue;
				}
			}
		//data is not processed by call back. Must use
		//	while(lsl_pull_sample_f(streamThread->inlet,data, 6, 0.0, &errcode));
		// in caller
		} else {
			Sleep(100);
			continue;
		}
	}
}
