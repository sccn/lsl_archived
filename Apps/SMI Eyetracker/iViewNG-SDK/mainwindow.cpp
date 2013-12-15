// -----------------------------------------------------------------------
//
// Portions (c) Copyright 1997-2013, SensoMotoric Instruments GmbH
// Portions (c) Copyright 2013, SCCN, UCSD
// 
// Permission  is  hereby granted,  free  of  charge,  to any  person  or
// organization  obtaining  a  copy  of  the  software  and  accompanying
// documentation  covered  by  this  license  (the  "Software")  to  use,
// reproduce,  display, distribute, execute,  and transmit  the Software,
// and  to  prepare derivative  works  of  the  Software, and  to  permit
// third-parties to whom the Software  is furnished to do so, all subject
// to the following:
// 
// The  copyright notices  in  the Software  and  this entire  statement,
// including the above license  grant, this restriction and the following
// disclaimer, must be  included in all copies of  the Software, in whole
// or  in part, and  all derivative  works of  the Software,  unless such
// copies   or   derivative   works   are   solely   in   the   form   of
// machine-executable  object   code  generated  by   a  source  language
// processor.
// 
// THE  SOFTWARE IS  PROVIDED  "AS  IS", WITHOUT  WARRANTY  OF ANY  KIND,
// EXPRESS OR  IMPLIED, INCLUDING  BUT NOT LIMITED  TO THE  WARRANTIES OF
// MERCHANTABILITY,   FITNESS  FOR  A   PARTICULAR  PURPOSE,   TITLE  AND
// NON-INFRINGEMENT. IN  NO EVENT SHALL  THE COPYRIGHT HOLDERS  OR ANYONE
// DISTRIBUTING  THE  SOFTWARE  BE   LIABLE  FOR  ANY  DAMAGES  OR  OTHER
// LIABILITY, WHETHER  IN CONTRACT, TORT OR OTHERWISE,  ARISING FROM, OUT
// OF OR IN CONNECTION WITH THE  SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// -----------------------------------------------------------------------


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <cv.h>

// tickets used by the iViewNG callback functions for hand-shaking
iViewTicket * gTicketStartAcquisition = NULL;
iViewTicket * gTicketConnect = NULL;
iViewTicket * gTicketAddLicense = NULL;
iViewTicket * gTicketGetServerTime = NULL;
iViewTicket * gTicketGetDeviceInfo = NULL;
iViewTicket * gTicketDeviceParameters = NULL;
iViewTicket * gTicketSubscriptionGaze = NULL;
iViewTicket * gTicketSubscriptionLeftEye = NULL;
iViewTicket * gTicketSubscriptionRightEye = NULL;
iViewTicket * gTicketSubscriptionScene = NULL;
iViewTicket * gTicketUnsubscription = NULL;
iViewTicket * gTicketStopAcquisition = NULL;
iViewTicket * gTicketCalibration = NULL;

// camera image parameters
int scene_color_chans, res_x, res_y, eyeres_x, eyeres_y;

// device parameters
uint32_t firmwareVersion=0,sceneCamVersion=0;
std::string driverVersion;
std::string serialNumber;

// moving-window univariate linear regression to get time stamps for frame numbers
using namespace boost::accumulators;
class online_regressor {
	typedef accumulator_set<double,features<tag::count,tag::rolling_mean> > accum_t;
public:
	online_regressor(int len=50): X(tag::rolling_window::window_size=len),Y(X),XX(X),XY(X),a(0),b(0),winlen(len) {  }
	double map(double x) { return a*x+b; }
	void update(double x,double y) {
		X(x); Y(y); XX(x*x); XY(x*y);
		double mX = rolling_mean(X), mY = rolling_mean(Y), mXX=rolling_mean(XX), mXY=rolling_mean(XY);
		a = count(X)>1 ? (mXY-mX*mY)/(mXX-mX*mX) : 0;
		b = mY - a*mX;
	}
private:
	accum_t X,Y,XX,XY;
	double a,b;
	int winlen;
};

// variables to calculate time synchronization between tracking server and local machine
double time_offset;			// time offset to add to server-side time-stamps to remap into local time domain (note: these are only valid while lsl::local_clock()<offset_valid_until)
double offset_valid_until;	// timeout beyond which time-offsets are not considered valid any more (due to clock drift)
double sample_age;			// the average sample age from the tracker
double sample_age_accum;	// the accumulated sample age
double sample_age_count;	// the number of samples in the accumulator (divide by this, if >0)
boost::shared_ptr<online_regressor> sceneframe_to_timestamp, eyeframe_to_timestamp; // moving-window regression mapping from frame indices to local timestamps

// parameters for hand-shaking control
static const unsigned int IVIEW_TICKETS_MAX = 1024;
unsigned int * _iView__receivedTickets = NULL;
unsigned int * _iView__receivedStreamCounter = NULL;
int _iView__waitingTime = 0;
IVIEW_CALLBACK _iView_UserCallback = NULL;

// iViewNG error handler
void CALL_API(iViewRC rc) {
	if (RC_NO_ERROR != rc) {
		wchar_t wmessage[1024];
		size_t msglen = sizeof(wmessage);
		iView_iViewRcToString(wmessage,&msglen,rc);
		char message[1024];
		std::wcstombs(message,&wmessage[0],sizeof(message));
		throw std::exception(message);
	}
}

void MyCallback(iViewTicket * const ticket);

/**
* A callback function which is used as API callback. If you register this callback to the iViewNG API,
* all iViewNG API calls will run into this function. Use this only if you want your program to block until
* a ticket is returned / a data stream event is received.
*/
void _iView__callback (iViewTicket * const ticket) {
	// return if there is no ticket
	if (NULL == ticket) 
		return;
	// memorize that we received this ticket number
	if (IVIEW_TICKETS_MAX <= ticket->ticketNumber) {
		wprintf (L"ERROR: Too many tickets, please increase IVIEW_TICKETS_MAX!\n");
		return;
	}
	_iView__receivedTickets [ticket->ticketNumber]++;
	iViewResult const * const result = ticket->result;
	// return if there is no result
	if (NULL == result) {
		return;
	}
	// return if the result is no data stream
	if (IVIEWRESULT_SUBSCRIBE_DATASTREAM != result->type) {
		return;
	}
	iViewDataStream const * const stream = (iViewDataStream const *) result->data;
	// break if the stream type is higher than expected
	if (NUMBER_OF_IVIEWDATASTREAMS <= stream->type) {
		return;
	}
	// memorize that we received this ticket number
	_iView__receivedStreamCounter [stream->type]++;
	return;
}

iViewRC iView_StartCatchingTickets () {
	// if required, release array of fetched ticket numbers
	if (_iView__receivedTickets) {
		memset (_iView__receivedTickets, 0, IVIEW_TICKETS_MAX * sizeof(unsigned int));
		free (_iView__receivedTickets);
		_iView__receivedTickets = NULL;
	}
	// reallocate
	_iView__receivedTickets = (unsigned int *) calloc (IVIEW_TICKETS_MAX, sizeof(unsigned int));
	if (NULL == _iView__receivedTickets)
		return RC_OUT_OF_MEMORY;
	// if required, release array of fetched stream events
	if (_iView__receivedStreamCounter) {
		memset (_iView__receivedStreamCounter, 0, NUMBER_OF_IVIEWDATASTREAMS * sizeof(unsigned int));
		free (_iView__receivedStreamCounter);
		_iView__receivedStreamCounter = NULL;
	}
	// reallocate
	_iView__receivedStreamCounter = (unsigned int *) calloc (NUMBER_OF_IVIEWDATASTREAMS, sizeof(unsigned int));
	if (NULL == _iView__receivedStreamCounter)
		return RC_OUT_OF_MEMORY;
	// save user callback
	_iView_UserCallback = iView_GetCallback ();
	// and apply convenience callback
	return iView_SetCallback (_iView__callback);
}

/**
* Block the caller until either
*
* - the ticket number was called
* - the function blocked for waitMilliseconds.
*
* The function unlocks the mutex when starting to wait to enable the API to enter the callback. The mutex
* is NOT locked afterwards. If you want to prevent API module from entering the callback, use
* ApiTestCallback::lockCallback() explicitly!
*
* Don't forget to call reset() to wipe out older results!
*/
int iView_WaitForTicket (iViewTicket const * const ticket, const unsigned int waitMilliseconds) {
	const unsigned int WAIT_PER_ITERATION = 50;
	unsigned int waitingTime = 0;
	do {
		if (_iView__receivedTickets [ticket->ticketNumber]) {
			break;
		}
		// if not found, sleep a bit
		iView_Sleep (WAIT_PER_ITERATION);
		waitingTime += WAIT_PER_ITERATION;
	}
	while (waitingTime <= waitMilliseconds);
	// restore user callback
	iView_SetCallback (_iView_UserCallback);
	return _iView__receivedTickets [ticket->ticketNumber] != 0;
}

/**
* Block the caller until either
*
* - numberOfEvents events of the type IVIEWRESULT_SUBSCRIBE_DATASTREAM with the provided stream type were received or
* - the function blocked for waitMilliseconds.
*
* The function unlocks the mutex when starting to wait to enable the API to enter the callback. The mutex
* is NOT locked afterwards. If you want to prevent API module from entering the callback, use
* ApiTestCallback::lockCallback() explicitly!
*
* Don't forget to call reset() to wipe out older results!
*/
int iView_WaitForDataStream (const iViewDataStreamType streamType, const unsigned int waitMilliseconds, const unsigned int numberOfEvents) {
	const unsigned int WAIT_PER_ITERATION = 50;
	unsigned int waitingTime = 0;
	while (numberOfEvents > _iView__receivedStreamCounter [streamType] && waitingTime <= waitMilliseconds) {
		iView_Sleep (WAIT_PER_ITERATION);
		waitingTime += WAIT_PER_ITERATION;
	}
	// restore user callback
	iView_SetCallback (_iView_UserCallback);
	return numberOfEvents <= _iView__receivedStreamCounter [streamType];
}




// LSL stream outlets used by the callback function
boost::shared_ptr<lsl::stream_outlet> outletGaze_;				// stream outlet for gaze coordinates if linked (should be a member of MainWindow, but isn't due to the CBF)
boost::shared_ptr<lsl::stream_outlet> outletLeftImage_;			// stream outlet for left eye image
boost::shared_ptr<lsl::stream_outlet> outletRightImage_;		// stream outlet for right eye image
boost::shared_ptr<lsl::stream_outlet> outletSceneImage_;		// stream outlet for scene camera
boost::shared_ptr<lsl::stream_outlet> outletCompressedScene_;	// stream outlet for compressed scene camera stream

MainWindow::MainWindow(QWidget *parent, const std::string &config_file) :
QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// set logger (TODO)

	// parse startup config file
	load_config(config_file);

	// make GUI connections
	QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link()));
	QObject::connect(ui->actionLoad_Configuration, SIGNAL(triggered()), this, SLOT(load_config_dialog()));
	QObject::connect(ui->actionSave_Configuration, SIGNAL(triggered()), this, SLOT(save_config_dialog()));
}

void MainWindow::load_config_dialog() {
	QString sel = QFileDialog::getOpenFileName(this,"Load Configuration File","","Configuration Files (*.cfg)");
	if (!sel.isEmpty())
		load_config(sel.toStdString());
}

void MainWindow::save_config_dialog() {
	QString sel = QFileDialog::getSaveFileName(this,"Save Configuration File","","Configuration Files (*.cfg)");
	if (!sel.isEmpty())
		save_config(sel.toStdString());
}

void MainWindow::closeEvent(QCloseEvent *ev) {
	if (outletGaze_)
		ev->ignore();
}

void MainWindow::load_config(const std::string &filename) {
	using boost::property_tree::ptree;
	ptree pt;

	// parse file
	try {
		read_xml(filename, pt);
	} catch(std::exception &e) {
		QMessageBox::information(this,"Error",(std::string("Cannot read config file: ")+= e.what()).c_str(),QMessageBox::Ok);
		return;
	}

	// get config values
	try {
		ui->serverAddress->setText(pt.get<std::string>("server.address","(launch process)").c_str());
		ui->serverPort->setText(pt.get<std::string>("server.port","(use default)").c_str());
		ui->licenseKey->setText(pt.get<std::string>("server.licensekey","(use default)").c_str());
		ui->deviceType->setCurrentIndex(ui->deviceType->findText(pt.get<std::string>("device.type","ETG 1.x").c_str()));
		ui->samplingRate->setCurrentIndex(ui->samplingRate->findText(pt.get<std::string>("device.samplingrate","30").c_str()));
		ui->sceneResolution->setCurrentIndex(ui->sceneResolution->findText(pt.get<std::string>("scenecam.resolution","(do not stream)").c_str()));
		ui->sceneColorSpace->setCurrentIndex(ui->sceneColorSpace->findText(pt.get<std::string>("scenecam.colorspace","Grayscale").c_str()));
		ui->addCompressedSceneStream->setCheckState(pt.get<bool>("scenecam.include_compressed",false) ? Qt::Checked : Qt::Unchecked);
		ui->eyeResolution->setCurrentIndex(ui->eyeResolution->findText(pt.get<std::string>("eyecam.resolution","(do not stream)").c_str()));
		ui->eyeSides->setCurrentIndex(ui->eyeSides->findText(pt.get<std::string>("eyecam.sides","left eye").c_str()));
		ui->eyeSamplingRate->setValue(pt.get<int>("eyecam.samplingrate",30));
	} catch(std::exception &e) {
		QMessageBox::information(this,"Error in Config File",(std::string("Could not read config parameters: ")+=e.what()).c_str(),QMessageBox::Ok);
		return;
	}
}

void MainWindow::save_config(const std::string &filename) {
	using boost::property_tree::ptree;
	ptree pt;

	// transfer UI content into property tree
	try {
		pt.put("server.address",ui->serverAddress->text().toStdString());
		pt.put("server.port",ui->serverPort->text().toStdString());
		pt.put("server.licensekey",ui->licenseKey->text().toStdString());
		pt.put("device.type",ui->deviceType->currentText().toStdString());
		pt.put("device.samplingRate",ui->samplingRate->currentText().toStdString());
		pt.put("scenecam.resolution",ui->sceneResolution->currentText().toStdString());
		pt.put("scenecam.colorspace",ui->sceneColorSpace->currentText().toStdString());
		pt.put("scenecam.include_compressed",ui->addCompressedSceneStream->checkState()==Qt::Checked);
		pt.put("eyecam.resolution",ui->eyeResolution->currentText().toStdString());
		pt.put("eyecam.sides",ui->eyeSides->currentText().toStdString());
		pt.put("eyecam.samplingrate",ui->eyeSamplingRate->value());
	} catch(std::exception &e) {
		QMessageBox::critical(this,"Error",(std::string("Could not prepare settings for saving: ")+=e.what()).c_str(),QMessageBox::Ok);
	}

	// write to disk
	try {
		write_xml(filename, pt);
	} catch(std::exception &e) {
		QMessageBox::critical(this,"Error",(std::string("Could not write to config file: ")+=e.what()).c_str(),QMessageBox::Ok);
	}
}

// start/stop the iViewNG connection
void MainWindow::link() {
	if (outletGaze_) {
		// === perform unlink action ===
		try {
			// shut down the iView connections
			CALL_API(iView_CreateTicket(&gTicketUnsubscription));
			CALL_API(iView_UnsubscribeDataStream(gTicketUnsubscription,IVIEWDATASTREAM_GAZE_INFORMATION));
			CALL_API(iView_CreateTicket(&gTicketStopAcquisition));
			CALL_API(iView_StopDataAcquisition(gTicketStopAcquisition));
			iView_Sleep(2000);
			if (gTicketConnect) CALL_API(iView_ReleaseTicket(&gTicketConnect));
			if (gTicketAddLicense) CALL_API(iView_ReleaseTicket(&gTicketAddLicense));
			if (gTicketGetServerTime) CALL_API(iView_ReleaseTicket(&gTicketGetServerTime));
			if (gTicketGetDeviceInfo) CALL_API(iView_ReleaseTicket(&gTicketGetDeviceInfo));			
			if (gTicketDeviceParameters) CALL_API(iView_ReleaseTicket(&gTicketDeviceParameters));
			if (gTicketStartAcquisition) CALL_API(iView_ReleaseTicket(&gTicketStartAcquisition));
			if (gTicketSubscriptionGaze) CALL_API(iView_ReleaseTicket(&gTicketSubscriptionGaze));
			if (gTicketUnsubscription) CALL_API(iView_ReleaseTicket(&gTicketUnsubscription));
			if (gTicketStopAcquisition) CALL_API(iView_ReleaseTicket(&gTicketStopAcquisition));
			CALL_API(iView_ShutdownServer(15000));
			CALL_API(iView_Shutdown());
			// reset the LSL outlets
			outletGaze_.reset();
			outletLeftImage_.reset();
			outletRightImage_.reset();
			outletSceneImage_.reset();
			outletCompressedScene_.reset();
		} catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not stop the background processing: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}
		// indicate that we are now successfully unlinked
		ui->linkButton->setText("Link");
	} else {
		// === perform link action ===
		try {
			// get the UI parameters...
			std::string serverAddress = ui->serverAddress->text().toStdString();
			std::string serverPort = ui->serverPort->text().toStdString();
			std::string licenseKey = ui->licenseKey->text().toStdString();
			std::string deviceType = ui->deviceType->currentText().toStdString();
			int samplingRate = boost::lexical_cast<int>(ui->samplingRate->currentText().toStdString());
			std::string sceneResolution = ui->sceneResolution->currentText().toStdString();
			std::string sceneColorSpace = ui->sceneColorSpace->currentText().toStdString();
			bool addCompressedSceneStream = ui->addCompressedSceneStream->checkState()==Qt::Checked;
			std::string eyeResolution = ui->eyeResolution->currentText().toStdString();
			std::string eyeSides = ui->eyeSides->currentText().toStdString();
			int eyeSamplingRate = ui->eyeSamplingRate->value();

			// reset API parameters
			gTicketStartAcquisition=gTicketConnect=gTicketAddLicense=gTicketDeviceParameters=gTicketSubscriptionGaze=gTicketSubscriptionLeftEye=gTicketGetDeviceInfo = NULL;
			gTicketSubscriptionRightEye=gTicketSubscriptionScene=gTicketUnsubscription=gTicketStopAcquisition=gTicketCalibration=gTicketGetServerTime = NULL;
			gSamplingRate = IVIEWSAMPLINGRATE_CURRENT;
			gCalibrating = 0;

			// reset timing and device parameters
			time_offset=offset_valid_until=sample_age=sample_age_accum=sample_age_count = 0;
			firmwareVersion=0; sceneCamVersion=0; driverVersion.clear(); serialNumber.clear();
			sceneframe_to_timestamp.reset(new online_regressor());
			eyeframe_to_timestamp.reset(new online_regressor()); 

			// set up server connection settings
			memset (&gServer,0,sizeof(iViewHost));
			if (serverAddress.empty())
				serverAddress = "127.0.0.1";
			if (serverAddress == "(launch process)")
				gServer.connectionType = IVIEW_SERVERADRRESS_SHAREDMEMORY;
			else {
				if (serverAddress.find(":") != std::string::npos) {
					gServer.connectionType = IVIEW_SERVERADRRESS_IPV6;
					strncpy(gServer.hostAddress.ipAddress.ipv6,&serverAddress[0],serverAddress.size());
				} else {
					gServer.connectionType = IVIEW_SERVERADRRESS_IPV4;
					strncpy(gServer.hostAddress.ipAddress.ipv4,&serverAddress[0],serverAddress.size());
				}
			}
			if (serverPort == "(use default)" || serverPort.empty())
				gServer.hostAddress.port = 0;
			else
				gServer.hostAddress.port = boost::lexical_cast<unsigned short>(serverPort);
			gServer.device = IVIEWDEVICE_ETG_CAMERAPLAYBACK;

			// set up sampling rate
			switch(samplingRate) {
				case 24:
					gSamplingRate = IVIEWSAMPLERATE_ETG_24;
					break;
				case 30:
					gSamplingRate = IVIEWSAMPLERATE_ETG_30;
					break;
				case 60:
					gSamplingRate = IVIEWSAMPLERATE_ETG_60;
					break;
				case 120:
					gSamplingRate = IVIEWSAMPLERATE_ETG_120;
					break;
				case 240:
					gSamplingRate = IVIEWSAMPLERATE_ETG_240;
					break;
				default:
					QMessageBox::critical(this,"Error","This sampling rate is not currently supported.",QMessageBox::Ok);
			}

			// set up license key
			if (licenseKey == "(use default)")
				licenseKey = "";

			// initalize API
			CALL_API(iView_Init(IVIEWSDK_IVNG));

			iViewVersion lib_version;
			iView_GetLibraryVersion(&lib_version);

			// start server
			if (IVIEW_SERVERADRRESS_SHAREDMEMORY == gServer.connectionType)
				CALL_API(iView_StartServer(NULL,L"--mode etg"));

			// set up callback function
			CALL_API(iView_SetCallback(MyCallback));

			// connect
			CALL_API(iView_CreateTicket(&gTicketConnect));
			CALL_API(iView_Connect(gTicketConnect,&gServer,1000,4,NULL));

			// transmit license
			CALL_API(iView_CreateTicket(&gTicketAddLicense));
			CALL_API(iView_StartCatchingTickets());
			CALL_API(iView_AddLicenseKey(gTicketAddLicense, ""));
			if (0 == iView_WaitForTicket(gTicketAddLicense, 2000))
				throw std::exception("Failed to set license key.");

			// query server time to estimate lag
			CALL_API(iView_CreateTicket(&gTicketGetServerTime));
			CALL_API(iView_GetServerTime(gTicketGetServerTime));
			for (unsigned waitingTime=0; !time_offset && waitingTime<=5000; waitingTime += 50)
				iView_Sleep(50);
			if (!time_offset)
				throw std::exception("Failed to get server time.");

			// query serial number and other device parameters
			CALL_API(iView_CreateTicket(&gTicketGetDeviceInfo));
			CALL_API(iView_GetDeviceInfo(gTicketGetDeviceInfo));
			for (unsigned waitingTime=0; serialNumber.empty() && waitingTime<=5000; waitingTime += 50)
				iView_Sleep(50);

			// submit device parameters to server
			iViewDeviceParametersEtgCameraPlayback specificParameters;
			specificParameters.outputDirectory = L".";
			specificParameters.baseFilename = L"iViewNG";
			specificParameters.operationMode = IVIEWTRACKINGMODE_CURRENT;
			specificParameters.cameraResolutionScene = IVIEWRESOLUTION_CURRENT;
			specificParameters.samplingRateSceneCam = IVIEWSAMPLINGRATE_CURRENT;
			specificParameters.whiteBalanceProgram = IVIEWWHITEBALANCE_ETG_AUTO;
			specificParameters.audioState = IVIEWTRACKINGMODE_CURRENT;
			specificParameters.overlaySpecList = NULL;
			if (gSamplingRate == IVIEWSAMPLERATE_ETG_60) {
				specificParameters.samplingRateEyeCam = IVIEWSAMPLERATE_ETG_60;
				specificParameters.cameraResolutionEye = IVIEWRESOLUTION_ETG_320x240;
			}
			else {
				specificParameters.samplingRateEyeCam = IVIEWSAMPLERATE_ETG_30;
				specificParameters.cameraResolutionEye = IVIEWRESOLUTION_ETG_640x480;
			}
			iViewDeviceParameters deviceParameters;
			deviceParameters.deviceType = IVIEWDEVICE_ETG_CAMERAPLAYBACK;
			deviceParameters.parameters = &specificParameters;
			CALL_API(iView_CreateTicket (&gTicketDeviceParameters));
			CALL_API(iView_StartCatchingTickets());
			CALL_API(iView_SetDeviceParameters(gTicketDeviceParameters, &deviceParameters));
			if (0 == iView_WaitForTicket (gTicketDeviceParameters,5000))
				throw std::exception("Failed to set device parameters.");

			// subscribe to scene image stream
			if (sceneResolution != "(do not stream)") {
				if (gServer.connectionType == IVIEW_SERVERADRRESS_SHAREDMEMORY || serverAddress == "127.0.0.1") {
					// on a local connection we transfer uncompressed video
					iViewDataStreamSpecSampleRate sampleRateScene;
					sampleRateScene.type = IVIEWDATASTREAMSPEC_SAMPLE_RATE;
					sampleRateScene.next = NULL;
					sampleRateScene.sampleRate = gSamplingRate;
					iViewStreamSubscription subscriptionScene;
					subscriptionScene.streamType = IVIEWDATASTREAM_SCENEIMAGES;
					subscriptionScene.streamSpec = (iViewDataStreamSpec*) &sampleRateScene;
					CALL_API(iView_CreateTicket(&gTicketSubscriptionScene));
					CALL_API(iView_StartCatchingTickets());
					CALL_API(iView_SubscribeDataStream(gTicketSubscriptionScene, &subscriptionScene));
					if (0 == iView_WaitForTicket (gTicketSubscriptionScene,2000))
						throw std::exception("Failed to subscribe to scene camera stream.");
				} else {
					// on a network connection we transfer compressed video
					iViewDataStreamSpecQualityOfService communicationQuality;
					communicationQuality.type = IVIEWDATASTREAMSPEC_QUALITY_OF_SERVICE;
					communicationQuality.next = NULL;
					communicationQuality.quality = IVIEWQOS_LOSSLESS;
					iViewStreamSubscription subscriptionScene;
					subscriptionScene.streamType = IVIEWDATASTREAM_SCENEIMAGES_H264_DECODED;
					subscriptionScene.streamSpec = (iViewDataStreamSpec*) &communicationQuality;
					CALL_API(iView_CreateTicket(&gTicketSubscriptionScene));
					CALL_API(iView_StartCatchingTickets());
					CALL_API(iView_SubscribeDataStream(gTicketSubscriptionScene, &subscriptionScene));
					if (0 == iView_WaitForTicket (gTicketSubscriptionScene,2000))
						throw std::exception("Failed to subscribe to scene camera stream.");
				}

				std::vector<std::string> scene_res;
				boost::algorithm::split(scene_res,sceneResolution,boost::algorithm::is_any_of("x"));
				res_x = boost::lexical_cast<int>(scene_res[0]);
				res_y = boost::lexical_cast<int>(scene_res[1]);
				bool grayscale = sceneColorSpace == "Grayscale";
				scene_color_chans = grayscale?1:3;

				// create LSL streaminfo & outlet
				lsl::stream_info info("iViewNG-SceneVideo-RAW","VideoRaw",res_x*res_y*scene_color_chans,samplingRate,lsl::cf_int8,"iViewNG-SceneVideo-RAW_" + serverAddress + serverPort + "_" + serialNumber);
				info.desc().append_child("format")
					.append_child_value("x_resolution",scene_res[0].c_str())
					.append_child_value("y_resolution",scene_res[1].c_str())
					.append_child_value("color_channels",boost::lexical_cast<std::string>(scene_color_chans).c_str())
					.append_child_value("color_format",grayscale?"GRAY":"RGB");
				info.desc().append_child("acquisition")
					.append_child_value("view","scene_camera");
				outletSceneImage_.reset(new lsl::stream_outlet(info));
			}


			// subscribe to eye-image streams
			if (eyeResolution != "(do not stream)") {
				std::vector<std::string> eye_res;
				boost::algorithm::split(eye_res,eyeResolution,boost::algorithm::is_any_of("x"));
				eyeres_x = boost::lexical_cast<int>(eye_res[0]);
				eyeres_y = boost::lexical_cast<int>(eye_res[1]);

				iViewDataStreamSpecSampleRate sampleRateEyes;
				sampleRateEyes.type = IVIEWDATASTREAMSPEC_SAMPLE_RATE;
				sampleRateEyes.next = NULL;
				sampleRateEyes.sampleRate = eyeSamplingRate;

				if (eyeSides == "left eye" || eyeSides == "both eyes") {
					iViewStreamSubscription subscriptionLeftEye;
					subscriptionLeftEye.streamType = IVIEWDATASTREAM_EYEIMAGES_LEFT;
					subscriptionLeftEye.streamSpec = (iViewDataStreamSpec*) &sampleRateEyes;
					CALL_API(iView_CreateTicket(&gTicketSubscriptionLeftEye));
					CALL_API(iView_StartCatchingTickets());
					CALL_API(iView_SubscribeDataStream(gTicketSubscriptionLeftEye, &subscriptionLeftEye));
					if (0 == iView_WaitForTicket(gTicketSubscriptionLeftEye, 2000))
						throw std::exception("Failed to subscribe to left eye camera stream.");

					// create LSL streaminfo & outlet
					lsl::stream_info info("iViewNG-LeftEyeVideo-RAW","VideoRaw",eyeres_x*eyeres_y,eyeSamplingRate,lsl::cf_int8,"iViewNG-LeftEyeVideo-RAW_" + serverAddress + serverPort + "_" + serialNumber);
					info.desc().append_child("format")
						.append_child_value("x_resolution",eye_res[0].c_str())
						.append_child_value("y_resolution",eye_res[1].c_str())
						.append_child_value("color_channels","1")
						.append_child_value("color_format","GRAY");
					info.desc().append_child("acquisition")
						.append_child_value("view","left_eye");
					outletLeftImage_.reset(new lsl::stream_outlet(info));
				}

				if (eyeSides == "right eye" || eyeSides == "both eyes") {
					iViewStreamSubscription subscriptionRightEye;
					subscriptionRightEye.streamType = IVIEWDATASTREAM_EYEIMAGES_RIGHT;
					subscriptionRightEye.streamSpec = (iViewDataStreamSpec*) &sampleRateEyes;
					CALL_API(iView_CreateTicket(&gTicketSubscriptionRightEye));
					CALL_API(iView_StartCatchingTickets());
					CALL_API(iView_SubscribeDataStream(gTicketSubscriptionRightEye, &subscriptionRightEye));
					if (0 == iView_WaitForTicket(gTicketSubscriptionRightEye, 2000))
						throw std::exception("Failed to subscribe to right eye camera stream.");

					// create LSL streaminfo & outlet
					lsl::stream_info info("iViewNG-RightEyeVideo-RAW","VideoRaw",res_x*res_y,eyeSamplingRate,lsl::cf_int8,"iViewNG-RightEyeVideo-RAW_" + serverAddress + serverPort + "_" + serialNumber);
					info.desc().append_child("format")
						.append_child_value("x_resolution",eye_res[0].c_str())
						.append_child_value("y_resolution",eye_res[1].c_str())
						.append_child_value("color_channels","1")
						.append_child_value("color_format","GRAY");
					info.desc().append_child("acquisition")
						.append_child_value("view","right_eye");
					outletRightImage_.reset(new lsl::stream_outlet(info));
				}
			}

			// include compressed scene stream
			if (addCompressedSceneStream) {
				iViewDataStreamSpecQualityOfService communicationQuality;
				communicationQuality.type = IVIEWDATASTREAMSPEC_QUALITY_OF_SERVICE;
				communicationQuality.next = NULL;
				communicationQuality.quality = IVIEWQOS_LOSSLESS;
				iViewStreamSubscription subscriptionScene;
				subscriptionScene.streamType = IVIEWDATASTREAM_SCENEIMAGES_H264;
				subscriptionScene.streamSpec = (iViewDataStreamSpec*) &communicationQuality;
				CALL_API(iView_CreateTicket(&gTicketSubscriptionScene));
				CALL_API(iView_StartCatchingTickets());
				CALL_API(iView_SubscribeDataStream(gTicketSubscriptionScene, &subscriptionScene));
				if (0 == iView_WaitForTicket (gTicketSubscriptionScene,2000))
					throw std::exception("Failed to subscribe to scene camera stream.");
				// create LSL streaminfo & outlet
				lsl::stream_info info("iViewNG-SceneVideo","Video",1,0,lsl::cf_string,"iViewNG-SceneVideo_" + serverAddress + serverPort + "_" + serialNumber);
				info.desc().append_child("format").append_child_value("codec","H.264");
				info.desc().append_child("acquisition")
					.append_child_value("view","scene_camera");
				outletCompressedScene_.reset(new lsl::stream_outlet(info));
			}

			// subscribe to gaze data stream
			iViewStreamSubscription subscriptionGaze;
			memset(&subscriptionGaze, 0, sizeof(iViewStreamSubscription));
			subscriptionGaze.streamType = IVIEWDATASTREAM_GAZE_INFORMATION;
			CALL_API(iView_CreateTicket(&gTicketSubscriptionGaze));
			CALL_API(iView_StartCatchingTickets());
			CALL_API(iView_SubscribeDataStream(gTicketSubscriptionGaze,&subscriptionGaze));
			if (0 == iView_WaitForTicket (gTicketSubscriptionGaze,2000))
				throw std::exception("Failed to subscribe to gaze data stream.");

			// create gaze streaminfo
			lsl::stream_info infoGaze("iViewNG-Gaze","Gaze",26,samplingRate,lsl::cf_float32,"iViewNG-Gaze_" + serverAddress + serverPort + "_" + serialNumber);
			// append meta-data -- for good measure we're describing all channels in excruciating detail here...
			lsl::xml_element channels = infoGaze.desc().append_child("channels");
			// combined scene image coordinates
			channels.append_child("channel").append_child_value("label","Screen_X_both").append_child_value("eye","both").append_child_value("type","ScreenX").append_child_value("unit","pixels");
			channels.append_child("channel").append_child_value("label","Screen_Y_both").append_child_value("eye","both").append_child_value("type","ScreenY").append_child_value("unit","pixels");
			// per-eye scene image coordinates
			channels.append_child("channel").append_child_value("label","Screen_X_left").append_child_value("eye","left").append_child_value("type","ScreenX").append_child_value("unit","pixels");
			channels.append_child("channel").append_child_value("label","Screen_Y_left").append_child_value("eye","left").append_child_value("type","ScreenY").append_child_value("unit","pixels");
			channels.append_child("channel").append_child_value("label","Screen_X_right").append_child_value("eye","right").append_child_value("type","ScreenX").append_child_value("unit","pixels");
			channels.append_child("channel").append_child_value("label","Screen_Y_right").append_child_value("eye","right").append_child_value("type","ScreenY").append_child_value("unit","pixels");
			// pupil radii
			channels.append_child("channel").append_child_value("label","Radius_left").append_child_value("eye","left").append_child_value("type","Radius").append_child_value("unit","millimeters");
			channels.append_child("channel").append_child_value("label","Radius_right").append_child_value("eye","right").append_child_value("type","Radius").append_child_value("unit","millimeters");
			// 3d positions
			channels.append_child("channel").append_child_value("label","EyePosition_X_left").append_child_value("eye","left").append_child_value("type","PositionX").append_child_value("unit","millimeters");
			channels.append_child("channel").append_child_value("label","EyePosition_Y_left").append_child_value("eye","left").append_child_value("type","PositionY").append_child_value("unit","millimeters");
			channels.append_child("channel").append_child_value("label","EyePosition_Z_left").append_child_value("eye","left").append_child_value("type","PositionZ").append_child_value("unit","millimeters");
			channels.append_child("channel").append_child_value("label","EyePosition_X_right").append_child_value("eye","right").append_child_value("type","PositionX").append_child_value("unit","millimeters");
			channels.append_child("channel").append_child_value("label","EyePosition_Y_right").append_child_value("eye","right").append_child_value("type","PositionY").append_child_value("unit","millimeters");
			channels.append_child("channel").append_child_value("label","EyePosition_Z_right").append_child_value("eye","right").append_child_value("type","PositionZ").append_child_value("unit","millimeters");
			// 3d directions
			channels.append_child("channel").append_child_value("label","EyeDirection_X_left").append_child_value("eye","left").append_child_value("type","DirectionX").append_child_value("unit","normalized");
			channels.append_child("channel").append_child_value("label","EyeDirection_Y_left").append_child_value("eye","left").append_child_value("type","DirectionY").append_child_value("unit","normalized");
			channels.append_child("channel").append_child_value("label","EyeDirection_Z_left").append_child_value("eye","left").append_child_value("type","DirectionZ").append_child_value("unit","normalized");
			channels.append_child("channel").append_child_value("label","EyeDirection_X_right").append_child_value("eye","right").append_child_value("type","DirectionX").append_child_value("unit","normalized");
			channels.append_child("channel").append_child_value("label","EyeDirection_Y_right").append_child_value("eye","right").append_child_value("type","DirectionY").append_child_value("unit","normalized");
			channels.append_child("channel").append_child_value("label","EyeDirection_Z_right").append_child_value("eye","right").append_child_value("type","DirectionZ").append_child_value("unit","normalized");
			// confidence values
			channels.append_child("channel").append_child_value("label","PupilConfidence_left").append_child_value("eye","left").append_child_value("type","Confidence").append_child_value("unit","normalized");
			channels.append_child("channel").append_child_value("label","PupilConfidence_right").append_child_value("eye","right").append_child_value("type","Confidence").append_child_value("unit","normalized");
			// uncertainties
			channels.append_child("channel").append_child_value("label","EyeballUncertainty_left").append_child_value("eye","left").append_child_value("type","Uncertainty").append_child_value("unit","custom")
				.append_child_value("description","Measure of uncertainty of eyeball estimator. Lower is better. For ETG: -1.5 is very good, 1.0 is mediore, 4.0 is bad.");				
			channels.append_child("channel").append_child_value("label","EyeballUncertainty_right").append_child_value("eye","right").append_child_value("type","Uncertainty").append_child_value("unit","custom")
				.append_child_value("description","Measure of uncertainty of eyeball estimator. Lower is better. For ETG: -1.5 is very good, 1.0 is mediore, 4.0 is bad.");				
			// frame numbers
			channels.append_child("channel").append_child_value("label","SceneFrameNumber").append_child_value("eye","both").append_child_value("type","FrameNumber").append_child_value("unit","integer");
			channels.append_child("channel").append_child_value("label","EyeFrameNumber").append_child_value("eye","both").append_child_value("type","FrameNumber").append_child_value("unit","integer");			
			// misc information
			infoGaze.desc().append_child("acquisition")
				.append_child_value("manufacturer","SMI")
				.append_child_value("model",deviceType.c_str())
				.append_child_value("serial_number",serialNumber.c_str())
				.append_child_value("firmware_version",boost::lexical_cast<std::string>(firmwareVersion).c_str())
				.append_child_value("scenecam_version",boost::lexical_cast<std::string>(sceneCamVersion).c_str())
				.append_child_value("driver_version",driverVersion.c_str())
				.append_child_value("API_version",(boost::lexical_cast<std::string>(lib_version.major) + "." + boost::lexical_cast<std::string>(lib_version.minor) + "." + boost::lexical_cast<std::string>(lib_version.patch) + "." + boost::lexical_cast<std::string>(lib_version.build)).c_str());

			// instantiate gaze data outlet
			outletGaze_.reset(new lsl::stream_outlet(infoGaze));

			// start data acquisition
			CALL_API(iView_CreateTicket(&gTicketStartAcquisition));
			CALL_API(iView_StartCatchingTickets());
			CALL_API(iView_StartDataAcquisition(gTicketStartAcquisition));
			if (0 == iView_WaitForTicket(gTicketStartAcquisition, 4000))
				throw std::exception("Failed to Start data acquisition.");

			// TODO: implement calibration support
		}
		catch(std::exception &e) {
			// reset the outlets again...
			outletGaze_.reset();
			outletLeftImage_.reset();
			outletRightImage_.reset();
			outletSceneImage_.reset();
			outletCompressedScene_.reset();
			QMessageBox::critical(this,"Error",(std::string("Could not initialize the iViewNG interface: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}


MainWindow::~MainWindow() {
	delete ui;
}


// convert an iViewImage to a data sample (numeric vector)
void image2sample(iViewImage* img, int xres, int yres, int channels, std::vector<char> &output) {
	// allocate space
	output.resize(xres*yres*channels);
	// convert color
	IplImage *tmp = cvCreateImage(cvSize(img->width,img->height),IPL_DEPTH_8U,channels);
	if (channels != img->numberOfChannels)
		cvCvtColor(img, tmp, CV_BGR2GRAY);
	else
		cvCopy(img,tmp);
	// rescale
	IplImage *result = cvCreateImage(cvSize(xres,yres),IPL_DEPTH_8U,channels);
	cvResize(tmp, result, CV_INTER_LINEAR);
	// copy
	char *dst = (char*)&output[0], *src = (char*)result->imageData;
	for (int y=0;y<yres;y++,src+=(result->widthStep - result->width*result->nChannels))
		for (int x=0;x<xres*channels;x++)
			*dst++ = *src++ - 128;
	// free memory
	cvReleaseImage(&result);
	cvReleaseImage(&tmp);
}


// main callback function
void MyCallback (iViewTicket * const ticket) {
	// this should never happen
	if (NULL == ticket)
		return;
	// extract the result
	iViewResult const * const result = ticket->result;
	// handle some known tickets
	if (gTicketCalibration == ticket)
		return;
	if (ticket == gTicketGetServerTime) {
		if (!(NULL == ticket->result || NULL == ticket->result->data)) {
			// calculate the time offset between server and this machine
			uint64_t time = ((iViewTime *) ticket->result->data)->time;
			double now = lsl::local_clock();
			time_offset = now - (double)time/1000000000.0;
			offset_valid_until = now + 30;
		}
		return;
	}
	if (ticket == gTicketGetDeviceInfo) {
		// query device info
		if (!(NULL == ticket->result || NULL == ticket->result->data)) {
			iViewDeviceInfo *device_info = ((iViewDeviceInfo*)ticket->result->data);
			if (device_info->type == IVIEWDEVICE_ETG_CAMERAPLAYBACK || device_info->type == IVIEWDEVICE_ETG) {
				if (device_info->version) {
					iViewDeviceVersionEtg *version = (iViewDeviceVersionEtg*)device_info->version;
					sceneCamVersion = version->softwareVersionSceneCam;
					firmwareVersion = version->firmwareVersionEyeCam;
					char buffer[1024]; 
					driverVersion = std::string(&buffer[0],std::wcstombs(buffer,&(version->driverVersionEyeCam[0]),sizeof(buffer)));
				}
				if (device_info->serialNumber) {
					iViewSerialNumberEtg *serial = (iViewSerialNumberEtg*)device_info->serialNumber;
					char buffer[1024]; 
					serialNumber = std::string(&buffer[0],std::wcstombs(buffer,&(serial->chassis[0]),sizeof(buffer)));
				}
			}
		}
		return;
	}

	// if the ticket carries no result or if it's not a data stream ticket, we're not interested
	if (NULL == result || IVIEWRESULT_SUBSCRIBE_DATASTREAM != result->type)
		return;
	// cast to the proper result type
	iViewDataStream const * const stream = (iViewDataStream const *) result->data;
	if(stream->lastStreamEntity == IVIEWDATASTREAM_END)
		return;
	// dispatch
	switch (stream->type) {
		case IVIEWDATASTREAM_GAZE_INFORMATION:
			if (outletGaze_) {
				iViewDataStreamGazeSample s = *((iViewDataStreamGazeSample const*)stream->data);
				// assemble gaze sample
				float sample[] = {
					s.pointOfRegard.x,s.pointOfRegard.y,
					s.leftEye.pointOfRegard.x,s.leftEye.pointOfRegard.y,
					s.rightEye.pointOfRegard.x,s.rightEye.pointOfRegard.y,
					s.leftEye.pupilRadius,s.rightEye.pupilRadius,
					s.leftEye.gazeBasePoint.x,s.leftEye.gazeBasePoint.y,s.leftEye.gazeBasePoint.z,
					s.rightEye.gazeBasePoint.x,s.rightEye.gazeBasePoint.y,s.rightEye.gazeBasePoint.z,
					s.leftEye.gazeDirection.x,s.leftEye.gazeDirection.y,s.leftEye.gazeDirection.z,
					s.rightEye.gazeDirection.x,s.rightEye.gazeDirection.y,s.rightEye.gazeDirection.z,
					s.leftEye.pupilConfidence,s.rightEye.pupilConfidence,
					s.leftEye.eyeballUncertainty,s.rightEye.eyeballUncertainty,
					s.sceneFrameNumber,s.eyeFrameNumber};
					double now = lsl::local_clock();
					if (now < offset_valid_until) {
						// update sample age estimate
						double sample_time = ((double)s.timestamp)/1000000000.0 + time_offset;
						double age = (now - sample_time);
						sample_age_accum += age;
						sample_age_count += 1;
						sample_age = sample_age_accum/sample_age_count;
					}
					// update the frame number to timestamp mappings
					double stamp = now-sample_age;
					sceneframe_to_timestamp->update(s.sceneFrameNumber,stamp);
					eyeframe_to_timestamp->update(s.eyeFrameNumber,stamp);
					// push into LSL
					outletGaze_->push_sample(sample,stamp);
			}
			break;
		case IVIEWDATASTREAM_EYEIMAGES_LEFT:
			if (outletLeftImage_) {
				iViewDataStreamEyeImage *image = (iViewDataStreamEyeImage*)stream->data;
				std::vector<char> sample; image2sample(image->imageData,eyeres_x,eyeres_y,1,sample);
				outletLeftImage_->push_sample(sample,eyeframe_to_timestamp->map(image->eyeFrameNumber));
			}
			break;
		case IVIEWDATASTREAM_EYEIMAGES_RIGHT:
			if (outletRightImage_) {
				iViewDataStreamEyeImage *image = (iViewDataStreamEyeImage*)stream->data;
				std::vector<char> sample; image2sample(image->imageData,eyeres_x,eyeres_y,1,sample);
				outletRightImage_->push_sample(sample,eyeframe_to_timestamp->map(image->eyeFrameNumber));
			}
			break;
		case IVIEWDATASTREAM_SCENEIMAGES:
		case IVIEWDATASTREAM_SCENEIMAGES_H264_DECODED:
			if (outletSceneImage_) {
				iViewDataStreamSceneImage *image = (iViewDataStreamSceneImage*)stream->data;
				std::vector<char> sample; image2sample(image->imageData,res_x,res_y,scene_color_chans,sample);
				outletSceneImage_->push_sample(sample,sceneframe_to_timestamp->map(image->sceneFrameNumber));
			}
			break;
		case IVIEWDATASTREAM_SCENEIMAGES_H264:
			if (outletCompressedScene_) {
				iViewDataStreamH264SceneImage *image = (iViewDataStreamH264SceneImage*)stream->data;
				std::string value((char*)image->data,image->dataSize);
				outletCompressedScene_->push_sample(&value,sceneframe_to_timestamp->map(image->sceneFrameNumber));
			}
			break;
	}
	iView_ReleaseResult(ticket);
	return;
}
