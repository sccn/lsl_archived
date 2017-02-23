#include "LiveAmpHelperFuncs.h"

// configure LiveAmp amplifier, sampling rate, mode, channels etc...
bool MainWindow::ConfigureLiveAmp(void)
{	
	try
	{
		// amplifier configuration
		float fVar = (mSamplingRate);
		int res = ampSetProperty(mDevice, PG_DEVICE, 0, DPROP_F32_BaseSampleRate, &fVar, sizeof(fVar));
		if(res != AMP_OK)					
			throw std::runtime_error(("Error setting sampling rate, error code:  " + boost::lexical_cast<std::string>(res)).c_str());

		int mode = RM_NORMAL;
		res = ampSetProperty(mDevice, PG_DEVICE, 0, DPROP_I32_RecordingMode, &mode, sizeof(mode));
		if(res != AMP_OK)					
			throw std::runtime_error(("Error setting acquisition mode, error code:  " + boost::lexical_cast<std::string>(res)).c_str());	

	}
	catch(std::exception &e) {
		// generate error message
		std::string msg = "ConfigureLiveAmp  error. ";

		QMessageBox::critical(this,"Error", (std::string("ConfigureLiveAmp error:  ") + e.what()).c_str() ,QMessageBox::Ok);
		return false;
	}

	return true;
}

bool MainWindow::InitializeLiveAmp()
{	
	HANDLE hlocDevice = NULL;
	int res;
	char HWI[20];
	strcpy_s(HWI, "ANY");  // use word SIM to simulate the LiveAmp

	mDevice = NULL;
	std::string serialN = ui->deviceSerialNumber->text().toStdString();

	QMessageBox *ms = new QMessageBox(this);
	ms->setWindowTitle("connecting to LiveAmp amplifier...it takes some seconds...");
	ms->setText("Connecting to LiveAmp amplifier(s)...");
	ms->show();

	res = ampEnumerateDevices(HWI, sizeof(HWI), "LiveAmp", 0);
			
	ms->close();
	delete ms;
			
	if (res <= 0)			
		throw std::runtime_error("No LiveAmp connected");
	else
	{				
		int numDevices = res;
		for (int i = 0; i < numDevices; i++)
		{
			hlocDevice = NULL;
			res = ampOpenDevice(i, &hlocDevice);
			if(res != AMP_OK)
			{
				std::string msg = "Cannot open device: ";
				msg.append (boost::lexical_cast<std::string>(i).c_str());
				msg.append("  error= " );
				msg.append(boost::lexical_cast<std::string>(res).c_str());
				throw std::runtime_error(msg);
			}
				
			char sVar[20]; 
			res = ampGetProperty(hlocDevice, PG_DEVICE, i, DPROP_CHR_SerialNumber, sVar, sizeof(sVar));
			if(res != AMP_OK)		
			{
				std::string msg = "Cannot read Serial number, ";
				msg.append (boost::lexical_cast<std::string>(i).c_str());
				msg.append("  error= " );
				msg.append(boost::lexical_cast<std::string>(res).c_str());
				throw std::runtime_error(msg);
			}

			//QMessageBox::information(this,"Information", sVar, QMessageBox::Ok);

			int check = strcmp(sVar, serialN.c_str());
			if(check == 0)
			{
				QMessageBox::information(this,"Information", std::string("LiveAmp SN: " + serialN + " successfully connected!").c_str(), QMessageBox::Ok);

				mDevice = hlocDevice; // save device handler
				return true;
			}
			else
			{		
				res = ampCloseDevice(hlocDevice);
				if(res != AMP_OK)
				{						
					std::string msg = "Cannot close device: ";
					msg.append (boost::lexical_cast<std::string>(i).c_str());
					msg.append("  error= " );
					msg.append(boost::lexical_cast<std::string>(res).c_str());
					throw std::runtime_error(msg);
				}
			}
		}
	}

	if(mDevice == NULL)
	{
		std::string msg = "There is no LiveAmp with serial number: " + serialN + " detected!";		
		throw std::runtime_error(msg);
	}

	return false;
}



// enables physical channels on LiveAmp 
bool EnableLiveAmpChannels(bool enableAUX, bool enableACC)
{
	int avlbchannels;	
	bool useBipolar = ui->useBipolar->checkState()==Qt::Checked;

	try
	{
		int res = ampGetProperty(mDevice, PG_DEVICE, 0, DPROP_I32_AvailableChannels, &avlbchannels, sizeof(avlbchannels));
		//QMessageBox::information(this, "Information", std::string("avlbchannels= " +   boost::lexical_cast<std::string>(avlbchannels)).c_str(), QMessageBox::Ok);

		for (int c = 0; c < avlbchannels; c++)
		{
			int type;

			res = ampGetProperty(mDevice, PG_CHANNEL, c, CPROP_I32_Type, &type, sizeof(type));
			if(res != AMP_OK)					
				throw std::runtime_error(("Error getting property for channel type: error code:  " + boost::lexical_cast<std::string>(res)).c_str());
		
			//QMessageBox::information(this, "Information", std::string("channel #' " +   boost::lexical_cast<std::string>(c)).c_str(), QMessageBox::Ok);
			if (type == CT_AUX)
			{		
				char cValue[20];
				int enabled;

				res = ampGetProperty(mDevice, PG_CHANNEL, c, CPROP_CHR_Function, &cValue, sizeof(cValue));
				if (res != AMP_OK)			
					throw std::runtime_error(("Error GetProperty CPROP_CHR_Function error: "  + boost::lexical_cast<std::string>(res)).c_str());
			
				res = ampGetProperty(mDevice, PG_CHANNEL, c, CPROP_B32_RecordingEnabled, &enabled, sizeof(enabled));
				if (res != AMP_OK)	
				{
					//QMessageBox::information(this, "Information",   ("Error GetProperty enable for ACC channels, error: " + boost::lexical_cast<std::string>(res)).c_str()  , QMessageBox::Ok);
					throw std::runtime_error(("Error SetProperty enable for ACC channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());
				}
			
				if(cValue == NULL || cValue[0] == NULL || !(cValue[0] == 'X' || cValue[0] == 'Y' ||cValue[0] == 'Z' || cValue[0] == 'x' ||cValue[0] == 'y' ||cValue[0] == 'z'))
				{
					if(enabled != enableAUX)
					{
						enabled = enableAUX;

						// Enables/disables AUX channels
						res = ampSetProperty(mDevice, PG_CHANNEL, c, CPROP_B32_RecordingEnabled, &enabled, sizeof(enabled));
						if (res != AMP_OK)	
							throw std::runtime_error(("Error SetProperty enable for ACC channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());
					}
				}
				else	
				{				
					if(enabled != enableACC)
					{
						enabled = enableACC;

						// Enables/disables ACC channels
						res = ampSetProperty(mDevice, PG_CHANNEL, c, CPROP_B32_RecordingEnabled, &enabled, sizeof(enabled));
						if (res != AMP_OK)			
							throw std::runtime_error(("Error SetProperty enable for ACC channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());
					}
				}
			
			}


			else if (type == CT_EEG || type == CT_BIP)
			{
				int enableEEG = true;
				// match indexes of input channel list (from UI List control)
				if(c >= mChannelCount)
				{
					enableEEG = false;
					res = ampSetProperty(mDevice, PG_CHANNEL, c, CPROP_B32_RecordingEnabled, &enableEEG, sizeof(enableEEG));
					if (res != AMP_OK)								
						throw std::runtime_error(("Error SetProperty disable for EEG channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());
				}
				else
				{
					res = ampSetProperty(mDevice, PG_CHANNEL, c, CPROP_B32_RecordingEnabled, &enableEEG, sizeof(enableEEG));
					if (res != AMP_OK)								
						throw std::runtime_error(("Error SetProperty enable for EEG channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());	


					if(useBipolar &&  c > 23 && c < 32 ) // set last 8 channels to bipolar
					{
						int bipType = CT_BIP;
						res = ampSetProperty(mDevice, PG_CHANNEL, c, CPROP_I32_Type, &bipType, sizeof(bipType));
						if (res != AMP_OK)								
							throw std::runtime_error(("Error SetProperty enable BIPOLAR EEG channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());	

					}
					
				}
			}

			// Trigger channel cannot be enabled disabled
			else if (type == CT_TRG || type == CT_DIG)		
				continue;					
		}
	}
	catch(std::exception &e) {
		QMessageBox::critical(this,"Error", (std::string("ConfigureLiveAmp error. ") + e.what()).c_str() ,QMessageBox::Ok);
		return false;
	}

	return true;
}

bool GenerateUsedPhysicalChannelIndexes(void)
{
	int enableCh;
	int type;
	int allEnabledChannel;  // only used==enabled channel

	eegIndexes.clear();
	auxIndexes.clear();
	accIndexes.clear();
	mTrigInIndexes.clear();
	mChannelIndexes.clear();

	// after enabling and disabling physical channels by LibveAmp, the order (indexing) changes... it must be checked again the channel types and indexing.
	int avlbchannels;

	try
	{
		int res = ampGetProperty(mDevice, PG_DEVICE, 0, DPROP_I32_AvailableChannels, &avlbchannels, sizeof(avlbchannels));
		for (int c = 0; c < avlbchannels; c++)
		{		
			res = ampGetProperty(mDevice, PG_CHANNEL, c, CPROP_B32_RecordingEnabled, &enableCh, sizeof(enableCh));
			if(res != AMP_OK)					
				throw std::runtime_error(("Error getting property for channel CPROP_B32_RecordingEnabled: error code:  " + boost::lexical_cast<std::string>(res)).c_str());

			if(!enableCh)
				continue;

			res = ampGetProperty(mDevice, PG_CHANNEL, c, CPROP_I32_Type, &type, sizeof(type));
			if(res != AMP_OK)					
				throw std::runtime_error(("Error getting property for channel type: error code:  " + boost::lexical_cast<std::string>(res)).c_str());
		
			if (type == CT_AUX)
			{		
				char cValue[20];	
				res = ampGetProperty(mDevice, PG_CHANNEL, c, CPROP_CHR_Function, &cValue, sizeof(cValue));
				if (res != AMP_OK)			
					throw std::runtime_error(("Error GetProperty CPROP_CHR_Function error: "  + boost::lexical_cast<std::string>(res)).c_str());
				
				if(cValue[0] == 'X' || cValue[0] == 'Y' ||cValue[0] == 'Z' || cValue[0] == 'x' ||cValue[0] == 'y' ||cValue[0] == 'z')	
					accIndexes.push_back(c);				
			
				else
					auxIndexes.push_back(c);
			}
		
			else if (type == CT_EEG || type == CT_BIP)
				eegIndexes.push_back(c);
		
				
			else if (type == CT_TRG || type == CT_DIG)
			{
				char cValue[20];	
				res = ampGetProperty(mDevice, PG_CHANNEL, c, CPROP_CHR_Function, &cValue, sizeof(cValue));
				if (res != AMP_OK)								
						throw std::runtime_error(("Error GetProperty CPROP_CHR_Function for CT_TRG, error: "  + boost::lexical_cast<std::string>(res)).c_str());		
								
				// we are interested only on Trigger Inputs... 
				if(strcmp("Trigger Input", cValue) == 0 )
					mTrigInIndexes.push_back(c); 	
			}

			allEnabledChannel++;

			if (type == CT_EEG || type == CT_BIP || type == CT_AUX)
			{
				float gain = 2;
				res = ampSetProperty(mDevice, PG_CHANNEL, c, CPROP_F32_Gain, &gain, sizeof(gain));
				if (res != AMP_OK)
					throw std::runtime_error(("Error SetProperty CPROP_F32_Gain, error: "  + boost::lexical_cast<std::string>(res)).c_str());		
			}
		}		
	}		

	catch(std::exception &e) {
		QMessageBox::critical(this,"Error", (std::string("GenerateUsedPhysicalChannelIndexes error.  ") + e.what()).c_str() ,QMessageBox::Ok);
		return false;
	}

	//QMessageBox::information(this, "Information", std::string("eegIndexes # " +   boost::lexical_cast<std::string>(eegIndexes.size())).c_str(), QMessageBox::Ok);

	// copy indexes of eeg, aux and acc physical channels 
	for (int i=0; i < eegIndexes.size(); i++)
		mChannelIndexes.push_back(eegIndexes[i]);

	//QMessageBox::information(this, "Information", std::string("auxIndexes # " +   boost::lexical_cast<std::string>(auxIndexes.size())).c_str(), QMessageBox::Ok);
	for (int i=0; i < auxIndexes.size(); i++)
		mChannelIndexes.push_back(auxIndexes[i]);

	//QMessageBox::information(this, "Information", std::string("accIndexes # " +   boost::lexical_cast<std::string>(accIndexes.size())).c_str(), QMessageBox::Ok);
	for (int i=0; i < accIndexes.size(); i++)
		mChannelIndexes.push_back(accIndexes[i]);

	return true;

}


// adjust channel labels, in case that there are more channels send to LSL (like aux or acc)
void AdjustChannelLabels(std::vector<std::string>& inpuChannelLabels, std::vector<std::string> &adjustedChannelLabels)
{
	// there is order of channels by LiveAmp : EEG + AUX + ACC
	if(inpuChannelLabels.size() != eegIndexes.size())
		QMessageBox::critical(this,"Error", "AdjustChannelLabels error, number of elements in input channel labels doesn't match!" ,QMessageBox::Ok);

	adjustedChannelLabels.clear();
	adjustedChannelLabels.resize(inpuChannelLabels.size());
	
	for (int i = 0; i < inpuChannelLabels.size(); i++)
		adjustedChannelLabels[i] = inpuChannelLabels[i];


	for (int i = 0; i < auxIndexes.size(); i++)
	{
		std::string aux = "AUX";
		aux += boost::lexical_cast<std::string>(i);
		adjustedChannelLabels.push_back (aux);
	}

	for (int i = 0; i < accIndexes.size(); i++)
	{
		std::string acc;
		std::string ix = boost::lexical_cast<std::string>(i/3 + 1);
		
		if (i%3 == 0)
			acc = "X" + ix;
		else if (i%3 == 1)
			acc = "Y" + ix;
		else if (i%3 == 2)
			acc = "Z" + ix;

		//QMessageBox::information(this, "Information", (std::string("ACC= ") +   acc).c_str(), QMessageBox::Ok);

		adjustedChannelLabels.push_back (acc);
	}

}



// extracts recorded samples form LiveAMp structure and copies to LSL buffer structure, ready to send over LSL
void PrepareDataToSendOverLSL(std::vector<std::vector<double>> &LiveAmptData, std::vector<std::vector<double>> &LSLData, std::vector<uint16_t> &trigger_buffer)
{
	int numSamples = LiveAmptData.size();

	if(numSamples <= 0)
		return;
	
	int lslChannleCount = mChannelIndexes.size();
	

	for (int s = 0; s < numSamples; s++)
	{
		std::vector<double> locBuffer (lslChannleCount); 
		for (int i=0; i < lslChannleCount; i++)	
			locBuffer[i] = LiveAmptData[s][mChannelIndexes[i]];  
				
		LSLData.push_back(locBuffer);
		trigger_buffer.push_back(LiveAmptData[s][mTrigInIndexes[0]]);  // for now we use only the first Trigger In channel!
	}
}

void ExtractLiveAmpData(BYTE* buffer, int seamples_read, int samplesize, int *dataTypes, int usedChannelsCnt, std::vector<std::vector<double>> &extractData)
{
	uint64_t sc;
	int numSamples = seamples_read / samplesize;
	int offset = 0;
	double sample = 0;
		
	extractData.clear();
	extractData.resize(numSamples);

	for (int s = 0; s < numSamples; s++)
	{
		offset = 0;
		sc = *(uint64_t*)&buffer[s*samplesize + offset];				
		offset += 8; // sample counter offset 

		extractData[s].resize(usedChannelsCnt);

		for (int i=0; i < usedChannelsCnt; i++)
		{
			switch (dataTypes[i])
			{
				case DT_INT16:
					{
						int16_t tmp = *(int16_t*)&buffer[s*samplesize + offset];
						sample = (double) tmp;
						offset += 2;
						break;
					}
				case DT_UINT16:
					{
						uint16_t tmp = *(uint16_t*)&buffer[s*samplesize + offset];
						sample = (double) tmp;
						offset += 2;
						break;
					}					
				case DT_INT32:
					{
						int32_t tmp = *(int32_t*)&buffer[s*samplesize + offset];
						sample = (double) tmp;
						offset += 4;
						break;
					}
				case DT_UINT32:
					{
						uint32_t tmp = *(uint32_t*)&buffer[s*samplesize + offset];
						sample = (double) tmp;
						offset += 4;
						break;
					}
				case DT_FLOAT32:
					{
						float tmp = *(float*)&buffer[s*samplesize + offset];
						sample = (double) tmp;
						offset += 4;
						break;
					}
				case DT_INT64:
					{
						int64_t tmp = *(int64_t*)&buffer[s*samplesize + offset];
						sample = (double) tmp;
						offset += 8;
						break;
					}
				case DT_UINT64:
					{
						uint64_t tmp = *(uint64_t*)&buffer[s*samplesize + offset];
						sample = (double) tmp;
						offset += 8;
						break;
					}
				case DT_FLOAT64:
					{
						
						double tmp = *(double*)&buffer[s*samplesize + offset];
						sample = (double) tmp;
						offset += 8;
						break;					
					}					
				default:
					break;
			}

			extractData[s][i] = sample;
		}
	}
}

// Get the current sample size in byte
int LiveAmp_SampleSize(HANDLE hDevice, int *typeArray, int* usedChannelsCnt)
{
	int res;
	int channels, datatype;
	BOOL enabled;
	int bytesize = 0;
	int cntUsedCh = 0;

	// iterate through all enabled channels
	res = ampGetProperty(hDevice, PG_DEVICE, 0, DPROP_I32_AvailableChannels, &channels, sizeof(channels));
	for (int c = 0; c < channels; c++)
	{
		res = ampGetProperty(hDevice, PG_CHANNEL, c, CPROP_B32_RecordingEnabled, &enabled, sizeof(enabled));
		if (enabled)
		{
			res = ampGetProperty(hDevice, PG_CHANNEL, c, CPROP_I32_DataType, &datatype, sizeof(datatype));
			
			typeArray[cntUsedCh] = datatype;
			cntUsedCh++;

			switch (datatype)
			{
			case DT_INT16:
			case DT_UINT16:
				{
					bytesize += 2;
				}
				break;
			case DT_INT32:
			case DT_UINT32:
			case DT_FLOAT32:
				{
					bytesize += 4;				
					
				}
				break;
			case DT_INT64:
			case DT_UINT64:
			case DT_FLOAT64:
				{
					bytesize += 8;
				}
				break;
			default:
				break;
			}
		}
	}
	// add the sample counter size
	bytesize += 8;
	*usedChannelsCnt = cntUsedCh;
	return bytesize;
}