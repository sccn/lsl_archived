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


// === parameters used by the callback function ===

// tickets used for hand-shaking
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

// parameters for hand-shaking control
static const unsigned int IVIEW_TICKETS_MAX = 1024;
unsigned int * _iView__receivedTickets = NULL;
unsigned int * _iView__receivedStreamCounter = NULL;
int _iView__waitingTime = 0;
IVIEW_CALLBACK _iView_UserCallback = NULL;

// device parameters
uint32_t firmwareVersion=0,sceneCamVersion=0;
std::string driverVersion;
std::string serialNumber;

// variables to calculate time synchronization between tracking server and local machine
double time_offset;			// time offset to add to server-side time-stamps to remap into local time domain (note: these are only valid while lsl::local_clock()<offset_valid_until)
double offset_valid_until;	// timeout beyond which time-offsets are not considered valid any more (due to clock drift)
double sample_age;			// the average sample age from the tracker
double sample_age_accum;	// the accumulated sample age
double sample_age_count;	// the number of samples in the accumulator (divide by this, if >0)
boost::shared_ptr<online_regressor> sceneframe_to_timestamp, eyeframe_to_timestamp; // moving-window regression mapping from frame indices to local timestamps

// output image parameters
int scene_color_chans=0,scene_w=0,scene_h=0,eye_w=0,eye_h=0;
// temporary images, used for image size/color conversions
IplImage *sceneImage=NULL,*eyeImage=NULL,*sceneImage2=NULL,*eyeImage2=NULL;

// LSL stream outlets accessed by the callback function
boost::shared_ptr<lsl::stream_outlet> outletGaze_,outletLeftImage_,outletRightImage_,outletSceneImage_,outletCompressedScene_;


// === callback function management ===

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


// translate sampling rate to iView format
iViewSamplingRate samplingRate2iView(int srate) {
	switch (srate) {
		case 0:
			return IVIEWSAMPLINGRATE_CURRENT;
		case 24:
			return IVIEWSAMPLERATE_ETG_24;
		case 30:
			return IVIEWSAMPLERATE_ETG_30;
		case 60:
			return IVIEWSAMPLERATE_ETG_60;
		case 120:
			return IVIEWSAMPLERATE_ETG_120;
		case 240:
			return IVIEWSAMPLERATE_ETG_240;
		default:
			throw std::runtime_error("Unsupported sampling rate.");
	}	
}

// translate resolution string to iView format
iViewCameraResolution resolution2iView(const std::string &res) {
	if (res == "320x240")
		return IVIEWRESOLUTION_ETG_320x240;
	if (res == "400x300")
		return IVIEWRESOLUTION_ETG_400x300;
	if (res == "640x480")
		return IVIEWRESOLUTION_ETG_640x480;
	if (res == "960x720")
		return IVIEWRESOLUTION_ETG_960x720;
	if (res == "1280x720")
		return IVIEWRESOLUTION_ETG_1280x720;
	if (res == "1280x960")
		return IVIEWRESOLUTION_ETG_1280x960;
	if (res == "current setting")
		return IVIEWRESOLUTION_CURRENT;
	throw std::runtime_error("Unsupported resolution.");
}

// translate resolution string to width/height parameters
void resolution2WH(const std::string &res, int &w, int &h) {
	std::vector<std::string> tmp;
	boost::algorithm::split(tmp,res,boost::algorithm::is_any_of("x"));
	w = boost::lexical_cast<int>(tmp[0]);
	h = boost::lexical_cast<int>(tmp[1]);
}

iViewTrackingMode trackingSides2iView(const std::string &tm) {
	if (tm == "both eyes")
		return IVIEWTRACKINGMODE_ETG_BINOCULAR;
	if (tm == "left eye")
		return IVIEWTRACKINGMODE_ETG_MONOCULAR_LEFT;
	if (tm == "right eye")
		return IVIEWTRACKINGMODE_ETG_MONOCULAR_RIGHT;
	if (tm == "current setting")
		return IVIEWTRACKINGMODE_CURRENT;
	throw std::runtime_error("Unsupported tracking mode.");
}

iViewWhiteBalanceProgram whiteBalance2iView(const std::string &wb) {
	if (wb == "Automatic")
		return IVIEWWHITEBALANCE_ETG_AUTO;
	if (wb == "Outdoor Daylight")
		return IVIEWWHITEBALANCE_ETG_DAYLIGHT;
	if (wb == "Outdoor Cloudy")
		return IVIEWWHITEBALANCE_ETG_BASIC;
	if (wb == "Indoor Incandescent (Tungsten)")
		return IVIEWWHITEBALANCE_ETG_TUNGSTEN;
	if (wb == "Indoor Fluorescent (White)")
		return IVIEWWHITEBALANCE_ETG_FLUORESCENT;
	if (wb == "Indoor Fluorescent (Daylight)")
		return IVIEWWHITEBALANCE_ETG_FLUORESCENT_H;
	throw std::runtime_error("Unsupported white balance mode.");
}

// === main class ===

MainWindow::MainWindow(QWidget *parent, const std::string &config_file): QMainWindow(parent), ui(new Ui::MainWindow), linked_(false) {
	ui->setupUi(this);
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
		// connection setup
		ui->serverAddress->setText(pt.get<std::string>("server.address","(launch process)").c_str());
		ui->serverPort->setText(pt.get<std::string>("server.port","(use default)").c_str());
		ui->licenseKey->setText(pt.get<std::string>("server.licensekey","(use default)").c_str());
		ui->deviceType->setCurrentIndex(ui->deviceType->findText(pt.get<std::string>("device.type","ETG 1.x").c_str()));
		// eye tracking and eye video access
		ui->eyeResolution->setCurrentIndex(ui->eyeResolution->findText(pt.get<std::string>("eyecam.resolution","640x480").c_str()));
		ui->eyeSamplingRate->setCurrentIndex(ui->eyeSamplingRate->findText(pt.get<std::string>("eyecam.samplingrate","30").c_str()));
		ui->eyeTrackSides->setCurrentIndex(ui->eyeTrackSides->findText(pt.get<std::string>("eyecam.trackingsides","both eyes").c_str()));
		ui->addLeftEyeVideo->setCheckState(pt.get<bool>("eyecam.stream_lefteye",false) ? Qt::Checked : Qt::Unchecked);
		ui->addRightEyeVideo->setCheckState(pt.get<bool>("eyecam.stream_righteye",false) ? Qt::Checked : Qt::Unchecked);
		ui->addGazeStream->setCheckState(pt.get<bool>("eyecam.stream_gazecoords",false) ? Qt::Checked : Qt::Unchecked);
		// scene video access
		ui->sceneSamplingRate->setCurrentIndex(ui->sceneSamplingRate->findText(pt.get<std::string>("scenecam.samplingrate","30").c_str()));
		ui->sceneResolution->setCurrentIndex(ui->sceneResolution->findText(pt.get<std::string>("scenecam.resolution","640x480").c_str()));
		ui->sceneColorSpace->setCurrentIndex(ui->sceneColorSpace->findText(pt.get<std::string>("scenecam.colorspace","Grayscale").c_str()));
		ui->sceneWhiteBalance->setCurrentIndex(ui->sceneWhiteBalance->findText(pt.get<std::string>("scenecam.whitebalance","Automatic").c_str()));
		ui->addUncompressedSceneStream->setCheckState(pt.get<bool>("scenecam.stream_uncompressed",false) ? Qt::Checked : Qt::Unchecked);
		ui->addCompressedSceneStream->setCheckState(pt.get<bool>("scenecam.stream_compressed",false) ? Qt::Checked : Qt::Unchecked);
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
		// connection setup
		pt.put("server.address",ui->serverAddress->text().toStdString());
		pt.put("server.port",ui->serverPort->text().toStdString());
		pt.put("server.licensekey",ui->licenseKey->text().toStdString());
		pt.put("device.type",ui->deviceType->currentText().toStdString());
		/// eye tracking and eye video access
		pt.put("eyecam.resolution",ui->eyeResolution->currentText().toStdString());
		pt.put("eyecam.samplingrate",ui->eyeSamplingRate->currentText().toStdString());
		pt.put("eyecam.trackingsides",ui->eyeTrackSides->currentText().toStdString());
		pt.put("eyecam.stream_lefteye",ui->addLeftEyeVideo->checkState()==Qt::Checked);
		pt.put("eyecam.stream_righteye",ui->addRightEyeVideo->checkState()==Qt::Checked);
		pt.put("eyecam.stream_gazecoords",ui->addGazeStream->checkState()==Qt::Checked);
		// scene video access
		pt.put("scenecam.samplingrate",ui->sceneSamplingRate->currentText().toStdString());
		pt.put("scenecam.resolution",ui->sceneResolution->currentText().toStdString());
		pt.put("scenecam.colorspace",ui->sceneColorSpace->currentText().toStdString());
		pt.put("scenecam.whitebalance",ui->sceneWhiteBalance->currentText().toStdString());
		pt.put("scenecam.stream_uncompressed",ui->addUncompressedSceneStream->checkState()==Qt::Checked);
		pt.put("scenecam.stream_compressed",ui->addCompressedSceneStream->checkState()==Qt::Checked);
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
	if (linked_) {
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
		linked_ = false;
	} else {
		// === perform link action ===
		try {
			// get the UI parameters...
			std::string serverAddress = ui->serverAddress->text().toStdString();
			std::string serverPort = ui->serverPort->text().toStdString();
			std::string licenseKey = ui->licenseKey->text().toStdString();
			std::string deviceType = ui->deviceType->currentText().toStdString();
			std::string eyeResolution = ui->eyeResolution->currentText().toStdString();
			int eyeSamplingRate = boost::lexical_cast<int>(ui->eyeSamplingRate->currentText().toStdString());
			std::string eyeTrackingSides = ui->eyeTrackSides->currentText().toStdString();
			bool streamLeftEyeVideo = ui->addLeftEyeVideo->checkState()==Qt::Checked;
			bool streamRightEyeVideo = ui->addRightEyeVideo->checkState()==Qt::Checked;
			bool streamGazeCoords = ui->addGazeStream->checkState()==Qt::Checked;
			int sceneSamplingRate = boost::lexical_cast<int>(ui->sceneSamplingRate->currentText().toStdString());
			std::string sceneResolution = ui->sceneResolution->currentText().toStdString();
			std::string sceneColorSpace = ui->sceneColorSpace->currentText().toStdString();
			std::string sceneWhiteBalance = ui->sceneWhiteBalance->currentText().toStdString();
			bool streamSceneUncompressed = ui->addUncompressedSceneStream->checkState()==Qt::Checked;
			bool streamSceneCompressed = ui->addCompressedSceneStream->checkState()==Qt::Checked;

			// set up desired resolutions
			resolution2WH(sceneResolution,scene_w,scene_h);
			resolution2WH(eyeResolution,eye_w,eye_h);

			// dealloc the temproary images, if any
			if (sceneImage)
				cvReleaseImage(&sceneImage);
			if (eyeImage)
				cvReleaseImage(&eyeImage);
			if (sceneImage2)
				cvReleaseImage(&sceneImage2);
			if (eyeImage2)
				cvReleaseImage(&eyeImage2);
			sceneImage=eyeImage=sceneImage2=eyeImage2=NULL;

			// reset API parameters
			gTicketStartAcquisition=gTicketConnect=gTicketAddLicense=gTicketDeviceParameters=gTicketSubscriptionGaze=gTicketSubscriptionLeftEye=gTicketGetDeviceInfo = NULL;
			gTicketSubscriptionRightEye=gTicketSubscriptionScene=gTicketUnsubscription=gTicketStopAcquisition=gTicketCalibration=gTicketGetServerTime = NULL;

			// reset timing and device parameters
			time_offset=offset_valid_until=sample_age=sample_age_accum=sample_age_count = 0;
			firmwareVersion=0; sceneCamVersion=0; driverVersion.clear(); serialNumber.clear();
			sceneframe_to_timestamp.reset(new online_regressor());
			eyeframe_to_timestamp.reset(new online_regressor()); 

			// set up server connection settings
			iViewHost ivServer = {0};
			if (serverAddress.empty())
				serverAddress = "127.0.0.1";
			if (serverAddress == "(launch process)")
				ivServer.connectionType = IVIEW_SERVERADRRESS_SHAREDMEMORY;
			else {
				if (serverAddress.find(":") != std::string::npos) {
					ivServer.connectionType = IVIEW_SERVERADRRESS_IPV6;
					strncpy(ivServer.hostAddress.ipAddress.ipv6,&serverAddress[0],serverAddress.size());
				} else {
					ivServer.connectionType = IVIEW_SERVERADRRESS_IPV4;
					strncpy(ivServer.hostAddress.ipAddress.ipv4,&serverAddress[0],serverAddress.size());
				}
			}
			if (serverPort == "(use default)" || serverPort.empty())
				ivServer.hostAddress.port = 0;
			else
				ivServer.hostAddress.port = boost::lexical_cast<unsigned short>(serverPort);
			ivServer.device = IVIEWDEVICE_ETG_CAMERAPLAYBACK;

			// set up license key
			if (licenseKey == "(use default)")
				licenseKey = "";

			// initalize API & get license version
			CALL_API(iView_Init(IVIEWSDK_IVNG));
			iViewVersion lib_version;
			iView_GetLibraryVersion(&lib_version);

			// start server
			if (IVIEW_SERVERADRRESS_SHAREDMEMORY == ivServer.connectionType)
				CALL_API(iView_StartServer(NULL,L"--mode etg"));

			// set up callback function
			CALL_API(iView_SetCallback(MyCallback));

			// connect
			CALL_API(iView_CreateTicket(&gTicketConnect));
			CALL_API(iView_Connect(gTicketConnect,&ivServer,1000,4,NULL));

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

			// set up and submit device parameters to server
			iViewDeviceParameters deviceParameters;
			deviceParameters.deviceType = IVIEWDEVICE_ETG_CAMERAPLAYBACK;
			iViewDeviceParametersEtgCameraPlayback specificParameters;
			specificParameters.outputDirectory = L".";										// The directory for storing recorded files, NOT ending with the path separator.
			specificParameters.baseFilename = L"iViewNG";									// The filename to append to outputDirectory, NOT including any suffix, NOT including any path separator.
			specificParameters.operationMode = trackingSides2iView(eyeTrackingSides);		// The operation mode(s) of the eye tracker.
			specificParameters.cameraResolutionScene = resolution2iView(sceneResolution);	// The resolution of the submitted scene images [pixels]
			specificParameters.cameraResolutionEye = resolution2iView(eyeResolution);		// The resolution of the submitted eye images [pixels]
			specificParameters.samplingRateSceneCam = samplingRate2iView(sceneSamplingRate);// Average scene camera frame rate [frames per second]
			specificParameters.samplingRateEyeCam = samplingRate2iView(eyeSamplingRate);	// Average eye camera frame rate [frames per second]
			specificParameters.whiteBalanceProgram = whiteBalance2iView(sceneWhiteBalance);	// Scene camera autowhitebalance program.
			specificParameters.audioState = IVIEWAUDIOSTATE_CURRENT;						// Audio activation state (currently not accessible).
			specificParameters.overlaySpecList = NULL;
			deviceParameters.parameters = &specificParameters;

			CALL_API(iView_CreateTicket (&gTicketDeviceParameters));
			CALL_API(iView_StartCatchingTickets());
			CALL_API(iView_SetDeviceParameters(gTicketDeviceParameters, &deviceParameters));
			if (0 == iView_WaitForTicket (gTicketDeviceParameters,5000))
				throw std::exception("Failed to set device parameters.");

			// subscribe to eye-image streams
			if (streamLeftEyeVideo) {
				iViewStreamSubscription subscriptionLeftEye;
				subscriptionLeftEye.streamType = IVIEWDATASTREAM_EYEIMAGES_LEFT;
				subscriptionLeftEye.streamSpec = NULL;
				CALL_API(iView_CreateTicket(&gTicketSubscriptionLeftEye));
				CALL_API(iView_StartCatchingTickets());
				CALL_API(iView_SubscribeDataStream(gTicketSubscriptionLeftEye, &subscriptionLeftEye));
				if (0 == iView_WaitForTicket(gTicketSubscriptionLeftEye, 2000))
					throw std::exception("Failed to subscribe to left eye camera stream.");

				// create LSL streaminfo & outlet
				lsl::stream_info info("iViewNG-LeftEyeVideo-RAW","VideoRaw",eye_w*eye_h,eyeSamplingRate,lsl::cf_int8,"iViewNG-LeftEyeVideo-RAW_" + serverAddress + serverPort + "_" + serialNumber);
				info.desc().append_child("encoding")
					.append_child_value("width",boost::lexical_cast<std::string>(eye_w).c_str())
					.append_child_value("height",boost::lexical_cast<std::string>(eye_h).c_str())
					.append_child_value("color_channels","1")
					.append_child_value("color_format","GRAY")
					.append_child_value("codec","RAW");
				info.desc().append_child("display")
					.append_child_value("pixel_aspect","1.0")
					.append_child_value("origin","top-left");
				info.desc().append_child("acquisition")
					.append_child_value("view","left_eye");
				outletLeftImage_.reset(new lsl::stream_outlet(info));
			}
			if (streamRightEyeVideo) {
				iViewStreamSubscription subscriptionRightEye;
				subscriptionRightEye.streamType = IVIEWDATASTREAM_EYEIMAGES_RIGHT;
				subscriptionRightEye.streamSpec = NULL;
				CALL_API(iView_CreateTicket(&gTicketSubscriptionRightEye));
				CALL_API(iView_StartCatchingTickets());
				CALL_API(iView_SubscribeDataStream(gTicketSubscriptionRightEye, &subscriptionRightEye));
				if (0 == iView_WaitForTicket(gTicketSubscriptionRightEye, 2000))
					throw std::exception("Failed to subscribe to right eye camera stream.");

				// create LSL streaminfo & outlet
				lsl::stream_info info("iViewNG-RightEyeVideo-RAW","VideoRaw",eye_w*eye_h,eyeSamplingRate,lsl::cf_int8,"iViewNG-RightEyeVideo-RAW_" + serverAddress + serverPort + "_" + serialNumber);
				info.desc().append_child("encoding")
					.append_child_value("width",boost::lexical_cast<std::string>(eye_w).c_str())
					.append_child_value("height",boost::lexical_cast<std::string>(eye_h).c_str())
					.append_child_value("color_channels","1")
					.append_child_value("color_format","GRAY")
					.append_child_value("codec","RAW");
				info.desc().append_child("display")
					.append_child_value("pixel_aspect","1.0")
					.append_child_value("origin","top-left");
				info.desc().append_child("acquisition")
					.append_child_value("view","right_eye");
				outletRightImage_.reset(new lsl::stream_outlet(info));
			}

			// subscribe to uncompressed scene image?
			if (streamSceneUncompressed) {
				scene_color_chans = sceneColorSpace == "Grayscale" ? 1 : 3;
				if (ivServer.connectionType == IVIEW_SERVERADRRESS_SHAREDMEMORY || serverAddress == "127.0.0.1") {
					// on a local connection we transfer uncompressed video
					iViewStreamSubscription subscriptionScene;
					subscriptionScene.streamType = IVIEWDATASTREAM_SCENEIMAGES;
					subscriptionScene.streamSpec = NULL;
					CALL_API(iView_CreateTicket(&gTicketSubscriptionScene));
					CALL_API(iView_StartCatchingTickets());
					CALL_API(iView_SubscribeDataStream(gTicketSubscriptionScene, &subscriptionScene));
					if (0 == iView_WaitForTicket (gTicketSubscriptionScene,2000))
						throw std::exception("Failed to subscribe to scene camera stream.");
				} else {
					// on a network connection we transfer compressed video
					iViewStreamSubscription subscriptionScene;
					subscriptionScene.streamType = IVIEWDATASTREAM_SCENEIMAGES_H264_DECODED;
					// need to force lossless transport for H.264
					iViewDataStreamSpecQualityOfService communicationQuality;
					communicationQuality.type = IVIEWDATASTREAMSPEC_QUALITY_OF_SERVICE;
					communicationQuality.next = NULL;
					communicationQuality.quality = IVIEWQOS_LOSSLESS;
					subscriptionScene.streamSpec = (iViewDataStreamSpec*) &communicationQuality;
					CALL_API(iView_CreateTicket(&gTicketSubscriptionScene));
					CALL_API(iView_StartCatchingTickets());
					CALL_API(iView_SubscribeDataStream(gTicketSubscriptionScene, &subscriptionScene));
					if (0 == iView_WaitForTicket (gTicketSubscriptionScene,2000))
						throw std::exception("Failed to subscribe to scene camera stream.");
				}

				// create LSL streaminfo & outlet
				lsl::stream_info info("iViewNG-SceneVideo-RAW","VideoRaw",scene_w*scene_h*scene_color_chans,sceneSamplingRate,lsl::cf_int8,"iViewNG-SceneVideo-RAW_" + serverAddress + serverPort + "_" + serialNumber);
				info.desc().append_child("encoding")
					.append_child_value("width",boost::lexical_cast<std::string>(scene_w).c_str())
					.append_child_value("height",boost::lexical_cast<std::string>(scene_h).c_str())
					.append_child_value("color_channels",boost::lexical_cast<std::string>(scene_color_chans).c_str())
					.append_child_value("color_format",sceneColorSpace=="Grayscale"?"GRAY":"RGB")
					.append_child_value("codec","RAW");
				info.desc().append_child("display")
					.append_child_value("pixel_aspect","1.0")
					.append_child_value("origin","top-left");
				info.desc().append_child("acquisition")
					.append_child_value("view","scene_camera");
				outletSceneImage_.reset(new lsl::stream_outlet(info));
			}

			// subscribe to compressed scene video?
			if (streamSceneCompressed) {
				iViewStreamSubscription subscriptionScene;
				subscriptionScene.streamType = IVIEWDATASTREAM_SCENEIMAGES_H264;
				// need to force lossless transport for H.264
				iViewDataStreamSpecQualityOfService communicationQuality;
				communicationQuality.type = IVIEWDATASTREAMSPEC_QUALITY_OF_SERVICE;
				communicationQuality.next = NULL;
				communicationQuality.quality = IVIEWQOS_LOSSLESS;
				subscriptionScene.streamSpec = (iViewDataStreamSpec*) &communicationQuality;
				CALL_API(iView_CreateTicket(&gTicketSubscriptionScene));
				CALL_API(iView_StartCatchingTickets());
				CALL_API(iView_SubscribeDataStream(gTicketSubscriptionScene, &subscriptionScene));
				if (0 == iView_WaitForTicket (gTicketSubscriptionScene,2000))
					throw std::exception("Failed to subscribe to scene camera stream.");
				// create LSL streaminfo & outlet
				lsl::stream_info info("iViewNG-SceneVideo","VideoCompressed",1,0,lsl::cf_string,"iViewNG-SceneVideo_" + serverAddress + serverPort + "_" + serialNumber);
				info.desc().append_child("encoding").append_child_value("codec","DAVC");
				info.desc().append_child("acquisition")
					.append_child_value("view","scene_camera");
				outletCompressedScene_.reset(new lsl::stream_outlet(info));
			}

			// subscribe to gaze data stream
			if (streamGazeCoords) {
				iViewStreamSubscription subscriptionGaze;
				memset(&subscriptionGaze, 0, sizeof(iViewStreamSubscription));
				subscriptionGaze.streamType = IVIEWDATASTREAM_GAZE_INFORMATION;
				CALL_API(iView_CreateTicket(&gTicketSubscriptionGaze));
				CALL_API(iView_StartCatchingTickets());
				CALL_API(iView_SubscribeDataStream(gTicketSubscriptionGaze,&subscriptionGaze));
				if (0 == iView_WaitForTicket (gTicketSubscriptionGaze,2000))
					throw std::exception("Failed to subscribe to gaze data stream.");

				// create gaze streaminfo
				lsl::stream_info infoGaze("iViewNG-Gaze","Gaze",26,eyeSamplingRate,lsl::cf_float32,"iViewNG-Gaze_" + serverAddress + serverPort + "_" + serialNumber);
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
				channels.append_child("channel").append_child_value("label","PupilRadius_left").append_child_value("eye","left").append_child_value("type","Radius").append_child_value("unit","millimeters");
				channels.append_child("channel").append_child_value("label","PupilRadius_right").append_child_value("eye","right").append_child_value("type","Radius").append_child_value("unit","millimeters");
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
			}

			// start data acquisition
			CALL_API(iView_CreateTicket(&gTicketStartAcquisition));
			CALL_API(iView_StartCatchingTickets());
			CALL_API(iView_StartDataAcquisition(gTicketStartAcquisition));
			if (0 == iView_WaitForTicket(gTicketStartAcquisition, 4000))
				throw std::exception("Failed to Start data acquisition.");

			// TODO: consider implementing calibration support
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
		linked_ = true;
	}
}


MainWindow::~MainWindow() {
	delete ui;
}


// convert an iViewImage to a data sample (numeric vector)
void lsl_send_image(boost::shared_ptr<lsl::stream_outlet> &outlet, iViewImage* img, int w, int h, int channels, double timestamp, bool is_scene) {
	IplImage *outimage = (IplImage*)img;
	if ((w == img->width || h == img->height) && channels != img->numberOfChannels) {
		// perform color space conversion
		IplImage **tmp = is_scene ? &sceneImage : &eyeImage;
		if (*tmp == NULL)
			*tmp = cvCreateImage(cvSize(img->width,img->height),IPL_DEPTH_8U,channels);
		cvCvtColor((IplImage*)img, *tmp, CV_BGR2GRAY);
		outimage = *tmp;
	}
	if ((w != img->width || h != img->height) && channels == img->numberOfChannels) {
		// perform size conversion
		IplImage **tmp = is_scene ? &sceneImage : &eyeImage;
		if (*tmp == NULL)
			*tmp = cvCreateImage(cvSize(w,h),IPL_DEPTH_8U,channels);
		cvResize((IplImage*)img, *tmp, CV_INTER_LINEAR);
		outimage = *tmp;
	}
	if ((w != img->width || h != img->height) && channels != img->numberOfChannels) {
		// convert both size and scale
		IplImage **tmp = is_scene ? &sceneImage : &eyeImage;
		if (*tmp == NULL)
			*tmp = cvCreateImage(cvSize(img->width,img->height),IPL_DEPTH_8U,channels);
		cvCvtColor((IplImage*)img, *tmp, CV_BGR2GRAY);
		IplImage **tmp2 = is_scene ? &sceneImage2 : &eyeImage2;
		if (*tmp2 == NULL)
			*tmp2 = cvCreateImage(cvSize(w,h),IPL_DEPTH_8U,channels);
		cvResize(*tmp, *tmp2, CV_INTER_LINEAR);
		outimage = *tmp2;
	}

	// handle image pitch, convert unsigned to signed integers
	char *dst = (char*)outimage->imageData, *src = (char*)outimage->imageData;
	for (int y=0;y<outimage->height;y++,src+=(outimage->widthStep - outimage->width*channels))
		for (int x=0,e=outimage->width*channels;x<e;x++)
			*dst++ = *src++ - 128;
	// send sample
	if (outlet)
		outlet->push_sample((char*)outimage->imageData,timestamp);
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
				lsl_send_image(outletLeftImage_,image->imageData,eye_w,eye_h,1,eyeframe_to_timestamp->map(image->eyeFrameNumber),false);
			}
			break;
		case IVIEWDATASTREAM_EYEIMAGES_RIGHT:
			if (outletRightImage_) {
				iViewDataStreamEyeImage *image = (iViewDataStreamEyeImage*)stream->data;
				lsl_send_image(outletRightImage_,image->imageData,eye_w,eye_h,1,eyeframe_to_timestamp->map(image->eyeFrameNumber),false);
			}
			break;
		case IVIEWDATASTREAM_SCENEIMAGES:
		case IVIEWDATASTREAM_SCENEIMAGES_H264_DECODED:
			if (outletSceneImage_) {
				iViewDataStreamSceneImage *image = (iViewDataStreamSceneImage*)stream->data;
				lsl_send_image(outletSceneImage_,image->imageData,scene_w,scene_h,scene_color_chans,sceneframe_to_timestamp->map(image->sceneFrameNumber),true);
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
