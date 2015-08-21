#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

MainWindow::MainWindow(QWidget *parent, const std::string &config_file) :
QMainWindow(parent), ui(new Ui::MainWindow), pDI(NULL), pController(NULL)
{
	try {
		ui->setupUi(this);

		HRESULT hr;
		// Init DirectInput
		if (FAILED(hr=DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&pDI,NULL)))
			throw std::runtime_error("Could not create DirectInput instance. Please make sure that you have DirectX 9.0 installed.");

		// Enumerate game controllers and add them to the UI
		ui->deviceSelector->clear();
		if (FAILED(hr=pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, controller_enum_callback, this, DIEDFL_ATTACHEDONLY)))
			throw std::runtime_error("Could not enumerate game controllers. Please make sure that you have a controller plugged in and that you DirectX is properly installed.");
		if (indexToInstance_.empty())
			throw std::runtime_error("No game controllers are plugged in.");

		// parse startup config file
		load_config(config_file);

		// make GUI connections
		QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
		QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link()));
		QObject::connect(ui->actionLoad_Configuration, SIGNAL(triggered()), this, SLOT(load_config_dialog()));
		QObject::connect(ui->actionSave_Configuration, SIGNAL(triggered()), this, SLOT(save_config_dialog()));
	} catch(std::exception &e) {
		QMessageBox::critical(this,"Error",e.what(),QMessageBox::Ok);
		throw;
	}
}

// enumerates controller features and sets ranges for the axes
BOOL CALLBACK MainWindow::object_enum_callback(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID *pWindow) {
    if(pdidoi->dwType & DIDFT_AXIS) {
        DIPROPRANGE diprg; 
        diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
        diprg.diph.dwHow        = DIPH_BYID; 
        diprg.diph.dwObj        = pdidoi->dwType;
        diprg.lMin              = -1000; 
        diprg.lMax              = +1000; 
        // Set the range for the axis
        ((MainWindow*)pWindow)->pController->SetProperty(DIPROP_RANGE,&diprg.diph);
    }
    return DIENUM_CONTINUE;
}

// enumerates controllers and populates GUI combobox with them
BOOL CALLBACK MainWindow::controller_enum_callback(const DIDEVICEINSTANCE* pdidInstance, VOID *pWindow) {
	return ((MainWindow*)pWindow)->on_controller(pdidInstance);
}

BOOL CALLBACK MainWindow::on_controller(const DIDEVICEINSTANCE* pdidInstance) {
	wchar_t *bstrGuid = NULL;
	StringFromCLSID(pdidInstance->guidInstance,&bstrGuid);
	indexToInstance_.insert(boost::bimap<int,std::wstring>::value_type(ui->deviceSelector->count(),std::wstring(bstrGuid)));
	char instance_name[4096]; wcstombs(instance_name,pdidInstance->tszInstanceName,sizeof(instance_name));
	ui->deviceSelector->addItem(instance_name);
	::CoTaskMemFree(bstrGuid);
	return DIENUM_CONTINUE;
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
	if (reader_thread_)
		ev->ignore();
}

void MainWindow::load_config(const std::string &filename) {
	using boost::property_tree::wptree;
	wptree pt;

	// parse file
	try {
		read_xml(filename, pt);
	} catch(std::exception &e) {
		QMessageBox::information(this,"Error",(std::string("Cannot read config file: ")+= e.what()).c_str(),QMessageBox::Ok);
		return;
	}

	// get config values
	try {
		// pre-select the device in the UI
		std::wstring deviceguid = pt.get<std::wstring>(L"settings.deviceguid",L"");
		if (!deviceguid.empty()) {
			if (!indexToInstance_.right.count(deviceguid)) {
				QMessageBox::information(this,"Error","The previously configured device was not found. Is it plugged in?",QMessageBox::Ok);
			} else
				ui->deviceSelector->setCurrentIndex(indexToInstance_.right.at(deviceguid));
		}
	} catch(std::exception &) {
		QMessageBox::information(this,"Error in Config File","Could not read out config parameters.",QMessageBox::Ok);
		return;
	}
}

void MainWindow::save_config(const std::string &filename) {
	using boost::property_tree::wptree;
	wptree pt;

	// transfer UI content into property tree
	try {
		pt.put(L"settings.deviceguid",indexToInstance_.left.at(ui->deviceSelector->currentIndex()));
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


// start/stop the GameController connection
void MainWindow::link() {
	HRESULT hr;
	if (reader_thread_) {
		// === perform unlink action ===
		try {
			stop_ = true;
			reader_thread_->interrupt();
			reader_thread_->join();
			reader_thread_.reset();
			// unacquire, release and delete everything...
			pController->Unacquire();
			SAFE_RELEASE(pController);
			SAFE_DELETE(pController);
		} catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not stop the background processing: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// indicate that we are now successfully unlinked
		ui->linkButton->setText("Link");
	} else {
		// === perform link action ===
		try {
			// get the UI parameters (selected GUID)
			std::wstring guidstr = indexToInstance_.left.at(ui->deviceSelector->currentIndex());
			std::string name = ui->deviceSelector->itemText(ui->deviceSelector->currentIndex()).toStdString();
			GUID guid;
			if (FAILED(hr=CLSIDFromString((LPOLESTR)guidstr.c_str(),&guid)))
				throw std::runtime_error("Did not find the selected device. Is it plugged in?");

		    // Obtain an interface to the selected joystick
			if (FAILED(hr=pDI->CreateDevice(guid,&pController,NULL)))
				throw std::runtime_error("Could not instantiate the selected device. Is it plugged in?");

			// and select the data format
			if (FAILED(hr=pController->SetDataFormat(&c_dfDIJoystick2)))
				throw std::runtime_error("Could not select data format for the controller. The controller might not be fully compatible with DirectInput.");

			// set cooperative level
		    if (FAILED(hr=pController->SetCooperativeLevel(winId(), DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
				throw std::runtime_error("Could not set cooperative level for the device. There might be another application running that demands exclusive access to the device.");

			// try to enumerate the device features; we'll do this to set the ranges for the axes
			if (FAILED(hr=pController->EnumObjects(object_enum_callback,this,DIDFT_ALL)))
				throw std::runtime_error("Could not enumerate device properties. The device might not be fully compatible with DirectInput.");

			// acquire it
			if (FAILED(hr=pController->Acquire()))
				throw std::runtime_error("Could not acquire access to the selected controller. Is another program using it in exclusive mode?");

			// start reading
			stop_ = false;
			reader_thread_.reset(new boost::thread(&MainWindow::read_thread,this,name));
		}
		catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not initialize the GameController interface: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}

// background data reader thread
void MainWindow::read_thread(std::string name) {
	HRESULT hr;

	// create streaminfo and outlet for the button events
	lsl::stream_info infoButtons(name + "Buttons","Markers",1,lsl::IRREGULAR_RATE,lsl::cf_string,name + "_Buttons_" + boost::asio::ip::host_name());
	lsl::stream_outlet outletButtons(infoButtons);

	// create streaminfo and outlet for the axes
	lsl::stream_info infoAxes(name + "Axes","Position",36,60,lsl::cf_float32,name + "_Axes_" + boost::asio::ip::host_name());
	// append some meta-data...
	lsl::xml_element channels = infoAxes.desc().append_child("channels");
	channels.append_child("channel")
		.append_child_value("label","X")
		.append_child_value("type","PositionX")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","Y")
		.append_child_value("type","PositionY")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","Z")
		.append_child_value("type","PositionZ")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","RX")
		.append_child_value("type","RotationX")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","RY")
		.append_child_value("type","RotationY")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","RZ")
		.append_child_value("type","RotationZ")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","U")
		.append_child_value("type","PositionU")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","V")
		.append_child_value("type","PositionV")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","PovN")
		.append_child_value("type","Position")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","PovE")
		.append_child_value("type","Position")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","PovS")
		.append_child_value("type","Position")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","PovW")
		.append_child_value("type","Position")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","VX")
		.append_child_value("type","VelocityX")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","VY")
		.append_child_value("type","VelocityY")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","VZ")
		.append_child_value("type","VelocityZ")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","VRX")
		.append_child_value("type","AngularVelocityX")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","VRY")
		.append_child_value("type","AngularVelocityY")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","VRZ")
		.append_child_value("type","AngularVelocityZ")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","VU")
		.append_child_value("type","VelocityU")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","VV")
		.append_child_value("type","VelocityV")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","AX")
		.append_child_value("type","AccelerationX")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","AY")
		.append_child_value("type","AccelerationY")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","AZ")
		.append_child_value("type","AccelerationZ")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","ARX")
		.append_child_value("type","AngularAccelerationX")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","ARY")
		.append_child_value("type","AngularAccelerationY")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","ARZ")
		.append_child_value("type","AngularAccelerationZ")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","AU")
		.append_child_value("type","AccelerationU")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","AV")
		.append_child_value("type","AccelerationV")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","FX")
		.append_child_value("type","ForceX")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","FY")
		.append_child_value("type","ForceY")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","FZ")
		.append_child_value("type","ForceZ")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","FRX")
		.append_child_value("type","TorqueX")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","FRY")
		.append_child_value("type","TorqueY")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","FRZ")
		.append_child_value("type","TorqueZ")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","FU")
		.append_child_value("type","ForceU")
		.append_child_value("unit","normalized_signed");
	channels.append_child("channel")
		.append_child_value("label","FV")
		.append_child_value("type","ForceV")
		.append_child_value("unit","normalized_signed");
	infoAxes.desc().append_child("acquisition")
		.append_child_value("model",name.c_str());
	lsl::stream_outlet outletAxes(infoAxes);

	// enter transmission loop
	bool waspressed[128] = {false};
	DIJOYSTATE2 js;	
	boost::posix_time::ptime t_start = boost::posix_time::microsec_clock::local_time();
	boost::int64_t t=0;
	while (!stop_) {
		// poll the device
		if (FAILED(hr=pController->Poll()))
			while (!stop_ && (hr = pController->Acquire()) == DIERR_INPUTLOST) ;

		// obtain its input state
		if (FAILED(hr=pController->GetDeviceState(sizeof(DIJOYSTATE2),&js)))
			QMessageBox::critical(this,"Error","Cannot obtain device state.",QMessageBox::Ok);

		double now = lsl::local_clock();

		// construct the axes sample and send it off
		float sample[36] = {js.lX,js.lY,js.lZ,js.lRx,js.lRy,js.lRz,js.rglSlider[0],js.rglSlider[1],
			js.rgdwPOV[0],js.rgdwPOV[1],js.rgdwPOV[2],js.rgdwPOV[3],js.lVX,js.lVY,js.lVZ,js.lVRx,js.lVRy,js.lVRz,
			js.rglVSlider[0],js.rglVSlider[1],js.lAX,js.lAY,js.lAZ,js.lARx,js.lARy,js.lARz,js.rglASlider[0],
			js.rglASlider[1],js.lFX,js.lFY,js.lFZ,js.lFRx,js.lFRy,js.lFRz,js.rglFSlider[0],js.rglFSlider[1]};
		// scale the numbers
		for (int k=0;k<36;k++)
			sample[k] /= 1000.0;
		outletAxes.push_sample(sample,now);

		// generate the button-event samples...
		for (int i=0; i<128; i++) {
			if ((js.rgbButtons[i]&0x80)) {
				if (!waspressed[i]) {
					waspressed[i] = true;
					std::string text("Button" + boost::lexical_cast<std::string>(i) + " pressed");
					outletButtons.push_sample(&text,now);
				}
			} else {
				if (waspressed[i]) {
					waspressed[i] = false;
					std::string text("Button" + boost::lexical_cast<std::string>(i) + " released");
					outletButtons.push_sample(&text,now);
				}
			}
		}		
		boost::this_thread::sleep(t_start + boost::posix_time::millisec((++t)*16));
	}
}

MainWindow::~MainWindow() {
	delete ui;
	SAFE_RELEASE(pDI);
	SAFE_DELETE(pDI);
}

