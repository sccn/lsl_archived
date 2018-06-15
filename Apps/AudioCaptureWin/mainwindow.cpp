#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <Objbase.h>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>


// desired buffer duration in seconds
const float buffer_duration = 1.0f; 
// update interval in seconds
const float update_interval = 0.05f;
// retry interval after device failure
const float retry_interval = 0.25;

// time conversion factor
const int REFTIMES_PER_SEC = 10000000;

// convert wBitsPerSample in WAVEFORMATEX to lsl::channel_format_t
lsl::channel_format_t bits2fmt(int bits) {
	if (bits == 8)
		return lsl::cf_int8;
	if (bits == 16)
		return lsl::cf_int16;
	if (bits == 32)
		return lsl::cf_float32;
	if (bits == 64)
		return lsl::cf_double64;
	throw std::runtime_error("Unsupported bitrate.");
}


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);
	
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link_audio()));

}

void MainWindow::closeEvent(QCloseEvent *ev) {
	if (reader_thread_)
		ev->ignore();
}

void MainWindow::link_audio() {
	if (reader_thread_) {
		// === perform unlink action ===
		shutdown_ = true;

		try {
			reader_thread_->interrupt();
			reader_thread_->join();
			reader_thread_.reset();
		} catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not stop the background processing: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		ui->linkButton->setText("Link");
		ui->lineEdit->setEnabled(true);
		ui->lineEdit->setReadOnly(false);

	} else {
		// === perform link action ===
		shutdown_ = false;

		IMMDeviceEnumerator *pEnumerator = NULL;
		IMMDevice *pDevice = NULL;
		IAudioClient *pAudioClient = NULL;
		IAudioCaptureClient *pCaptureClient = NULL;
		WAVEFORMATEX *pwfx = NULL;
		LPWSTR deviceId = L"unidentified";

		try {
			// get the device enumerator
			if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator)))
				throw std::runtime_error("Could not create multimedia device enumerator.");

			// ask for the default capture endpoint (note: this is acting as a "multimedia" client)
			if (FAILED(pEnumerator->GetDefaultAudioEndpoint(eCapture, eMultimedia, &pDevice)))
				throw std::runtime_error("Could not get a default recording device -- is one selected?");

			// try to activate and IAudioClient
			if (FAILED(pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient))) {
				DWORD state;
				pDevice->GetState(&state);
				switch (state) {
				case DEVICE_STATE_DISABLED:
					throw std::runtime_error("Could not activate the default recording device -- is it enabled?.");
				case DEVICE_STATE_UNPLUGGED:
					throw std::runtime_error("Could not activate the default recording device -- is it plugged in?");
				case DEVICE_STATE_NOTPRESENT:
					throw std::runtime_error("Could not activate the default recording device -- is one selected?");
				default:
					throw std::runtime_error("Failed to activate the default recording device.");
				}
			}

			// get device ID
			pDevice->GetId(&deviceId);

			// get mixing format
			if (FAILED(pAudioClient->GetMixFormat(&pwfx)))
				throw std::runtime_error("Could not get the data format of the recording device -- make sure it is available for use.");

			// initialize a shared buffer
			if (FAILED(pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, REFTIMES_PER_SEC*buffer_duration, 0, pwfx, NULL)))
				throw std::runtime_error("Could not initialize a shared audio client.");

			// get the capture service
			if (FAILED(pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pCaptureClient)))
				throw std::runtime_error("Unexpected error: could not get the audio capture service.");

			// try to start the actual recording
			if (FAILED(pAudioClient->Start()))
				throw std::runtime_error("Could not start recording.");

			// start the reader thread (which is responsible for deallocation of everything)
			reader_thread_.reset(new boost::thread(&MainWindow::process_samples,this,pEnumerator,pDevice,pAudioClient,pCaptureClient,pwfx,deviceId));

			// now we are linked
			ui->linkButton->setText("Unlink");
			ui->lineEdit->setDisabled(true);
			ui->lineEdit->setReadOnly(true);
		} catch(std::runtime_error &e) {
			// got an exception during initialization: release resources...
			if (pCaptureClient)
				pCaptureClient->Release();
			if (pAudioClient)
				pAudioClient->Release();
			if (pDevice)
				pDevice->Release();
			if (pEnumerator)
				pEnumerator->Release();
			if (pwfx)
				CoTaskMemFree(pwfx);
			// and show message box
			QMessageBox::critical(this,"Initialization Error", (std::string("Could not start recording: ") += e.what()).c_str(),QMessageBox::Ok);
		}		
	}
}

MainWindow::~MainWindow() {
	delete ui;
}


// === the actual sample processing thread ===
void MainWindow::process_samples(IMMDeviceEnumerator *pEnumerator,IMMDevice *pDevice,IAudioClient *pAudioClient,IAudioCaptureClient *pCaptureClient,WAVEFORMATEX *pwfx,LPWSTR devID) {
	// set up the stream info
	std::wstring tmp(devID);
	source_id = QHostInfo::localHostName().toStdString() += std::string(tmp.begin(),tmp.end());
	lsl::stream_info info(ui->lineEdit->text().toStdString(),"Audio",pwfx->nChannels,pwfx->nSamplesPerSec,lsl::cf_float32,source_id);
	info.desc().append_child("provider").append_child_value("api","WASAPI");
	lsl::xml_element channels = info.desc().append_child("channels");
	if (pwfx->nChannels == 1) {
		channels.append_child("channel").append_child_value("label","FrontCenter");
	} else {
		channels.append_child("channel").append_child_value("label","FrontLeft");
		channels.append_child("channel").append_child_value("label","FrontRight");
	}

	// <synchronization> meta-data
	info.desc().append_child("synchronization")
		.append_child_value("offset_mean", "0.01047")
		.append_child_value("offset_rms", "0.000033")
		.append_child_value("offset_median", "0.01046")
		.append_child_value("offset_5_centile", "0.01003")
		.append_child_value("offset_95_centile", "0.01095");

	// create the outlet
	lsl::stream_outlet out(info);

	// repeat until interrupted (can re-connect if device was lost)
	while (!shutdown_) {
		try {
			try {
				// for each chunk update...
				while (!shutdown_) {
					// sleep for the update interval
					boost::this_thread::sleep(boost::posix_time::milliseconds(1000.0*update_interval));

					// get the length of the first packet
					UINT32 packetLength = 0;
					if (FAILED(pCaptureClient->GetNextPacketSize(&packetLength)))
						throw std::runtime_error("Device got invalidated.");

					LARGE_INTEGER now,freq;				// current time, timer frequency
					QueryPerformanceFrequency(&freq);

					int channels = pwfx->nChannels;

					// consume each chunk...
					while (packetLength) {
						BYTE *pData;					// pointer to the data...
						DWORD flags;					// flags of the recording device
						UINT64 captureTime;				// capture time of the first sample (in 100-ns units)
						UINT32 numFramesAvailable;		// number of frames available in the buffer

						// get the available data in the shared buffer
						if (FAILED(pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, &captureTime)))
							throw std::runtime_error("Device got invalidated.");

						// determine the age of the last sample of the chunk
						QueryPerformanceCounter(&now);
						double firstSampleAge = (double)(now.QuadPart)/freq.QuadPart - (double)(captureTime)/REFTIMES_PER_SEC;	// time passed since the sample was captured
						double lastSampleAge = (firstSampleAge - ((double)(numFramesAvailable-1))/pwfx->nSamplesPerSec);		// the last sample is younger than the first one...
						double lastSampleTime = lsl::local_clock() - lastSampleAge;

						if (!(flags&AUDCLNT_BUFFERFLAGS_SILENT)) {
							// submit the data to LSL
							float *audioValues = (float*)pData;
							std::vector<std::vector<float> > chunk(numFramesAvailable,std::vector<float>(channels));
							for (unsigned k=0;k<numFramesAvailable;k++)
								chunk[k].assign(&audioValues[k*channels],&audioValues[(k+1)*channels]);
							out.push_chunk(chunk, lastSampleTime);
						}

						// move on to next chunk
						if (FAILED(pCaptureClient->ReleaseBuffer(numFramesAvailable)))
							throw std::runtime_error("Device got invalidated.");
						if (FAILED(pCaptureClient->GetNextPacketSize(&packetLength)))
							throw std::runtime_error("Device got invalidated.");
					}
				}
			}
			catch(std::runtime_error &) {
				// device invalidated -- release...
				pCaptureClient->Release(); 
				pCaptureClient = 0;
				pAudioClient->Release(); 
				pAudioClient = 0;
				while (true) {
					// sleep for a bit
					boost::this_thread::sleep(boost::posix_time::milliseconds(1000.0*retry_interval));
					// try to reacquire with same parameters
					if (FAILED(pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient)))
						continue;
					if (FAILED(pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, REFTIMES_PER_SEC*buffer_duration, 0, pwfx, NULL)))
						continue;
					if (FAILED(pAudioClient->GetService(__uuidof(IAudioCaptureClient),(void**)&pCaptureClient)))
						continue;
					if (FAILED(pAudioClient->Start()))
						continue;
					// now back in business again...
					break;
				}
			}
		} catch(boost::thread_interrupted &) {
			// thread interrupted -- exit!
			break;
		}
	}

	// stop recording
	if (pAudioClient)
		pAudioClient->Stop();

	// finally release all allocated resources
	if (pCaptureClient)
		pCaptureClient->Release();
	if (pAudioClient)
		pAudioClient->Release();
	if (pDevice)
		pDevice->Release();
	if (pEnumerator)
		pEnumerator->Release();
	if (pwfx)
		CoTaskMemFree(pwfx);
}
