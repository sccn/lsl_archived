#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


// maximum waiting time during setup
const float max_waiting_time = 3.0;

// Assumed lag of the PhaseSpace hardware/API, in seconds
// According to the product page, "All of these features can be enjoyed in real-time, with less than 10ms latency."
// Since there is intrinsic network jitter of at least 1-2ms involved, the mean lag must be below 10ms if this statement was true.
// Here we are assuming 8ms +/- 2ms lag per packet (jitter is being averaged out in offline processing)
const double compensated_lag = 0.008;


MainWindow::MainWindow(QWidget *parent, const std::string &config_file) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// parse startup config file
	load_config(config_file);

	// make GUI connections
	QObject::connect(ui->action_Quit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link_phasespace()));
	QObject::connect(ui->actionLoad_Configuration, SIGNAL(triggered()), this, SLOT(load_config_dialog()));
	QObject::connect(ui->action_Save_Configuration, SIGNAL(triggered()), this, SLOT(save_config_dialog()));
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
	if (reader_thread_) {
		ev->ignore();
		QMessageBox::warning(this,"Warning","Link still active.",QMessageBox::Ok);
	}
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
		ui->serverAddress->setText(pt.get<std::string>("server.address","192.168.1.230").c_str());
		ui->markerNames->clear();
		BOOST_FOREACH(ptree::value_type &v, pt.get_child("descriptions.markers"))
			ui->markerNames->appendPlainText(v.second.data().c_str());
		ui->rigidNames->clear();
		BOOST_FOREACH(ptree::value_type &v, pt.get_child("descriptions.rigids"))
			ui->rigidNames->appendPlainText(v.second.data().c_str());
		ui->setupName->clear();
		BOOST_FOREACH(ptree::value_type &v, pt.get_child("setups.predefined"))
			ui->setupName->addItem(v.second.data().c_str());
		if (!ui->setupName->count())
			ui->setupName->addItem("untitled");
		ui->setupName->setCurrentIndex(pt.get<int>("setups.selected",0));
		ui->rigidStreams->setCheckState(pt.get<bool>("streams.one_per_rigid",false) ? Qt::Checked : Qt::Unchecked);
	} catch(std::exception &) {
		QMessageBox::information(this,"Error in Config File","Could not read out config parameters.",QMessageBox::Ok);
		return;
	}
}

void MainWindow::save_config(const std::string &filename) {
	using boost::property_tree::ptree;
	ptree pt;

	// transfer UI content into property tree
	try {
		pt.put("server.address",ui->serverAddress->text().toStdString());
		std::vector<std::string> marker_names;
		boost::algorithm::split(marker_names,ui->markerNames->toPlainText().toStdString(),boost::algorithm::is_any_of("\n"));
		BOOST_FOREACH(std::string &v, marker_names)
			pt.add("descriptions.markers.marker", v);
		std::vector<std::string> rigid_names;
		boost::algorithm::split(rigid_names,ui->rigidNames->toPlainText().toStdString(),boost::algorithm::is_any_of("\n"));
		BOOST_FOREACH(std::string &v, rigid_names)
			pt.add("descriptions.rigids.rigid", v);
		for (int k=0;k<ui->setupName->count();k++)
			pt.add("setups.predefined.name", ui->setupName->itemText(k).toStdString());
		pt.put("setups.selected",ui->setupName->currentIndex());
		pt.put("streams.one_per_rigid",ui->rigidStreams->checkState()==Qt::Checked);
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


// start/stop the phasespace connection
void MainWindow::link_phasespace() {
	if (reader_thread_) {
		// === perform unlink action ===
		try {
			stop_ = true;
			reader_thread_->interrupt();
			reader_thread_->join();
			reader_thread_.reset();
		} catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not stop the background processing: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		try {
			owlDone();
		} catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not unlink orderly: ")+=e.what()).c_str(),QMessageBox::Ok);
		}

		// indicate that we are now successfully unlinked
		ui->linkButton->setText("Link");
	} else {
		// === perform link action ===
		
		// added by DEM 11/2/2015 -- needed for changing no camera exception into simple warning
		// check if the server name has a : in it, i.e. is from an owl emulator
		int f = 0;
		if(server_name.find(":")) f=1;
		
		// try to extract the marker & body descriptions from the UI
		try {
			boost::algorithm::split(marker_names,ui->markerNames->toPlainText().toStdString(),boost::algorithm::is_any_of("\n"));
			boost::algorithm::split(rigid_names,ui->rigidNames->toPlainText().toStdString(),boost::algorithm::is_any_of("\n"));
		}
		catch(std::exception &) {
			QMessageBox::information(this,"Error","Could not parse marker/body names from the UI.",QMessageBox::Ok);
			return;
		}

		// get the setup name...
		setup_name = ui->setupName->itemText(ui->setupName->currentIndex()).toStdString();

		// connect to the device
		server_name = ui->serverAddress->text().toStdString().c_str();
		try {
			if (owlInit(server_name.c_str(),OWL_SLAVE) < 0)
				throw std::runtime_error("Failed to connect (is the server box turned on and the server address correct?).");
			if(!owlGetStatus())
				throw std::runtime_error(std::string("Setup error: ") += boost::lexical_cast<std::string>(owlGetError()));
		} catch (std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not initialize the PhaseSpace interface: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// query setup parameters
		try {
			float srate = 0;
			if (owlGetFloatv(OWL_FREQUENCY,&srate) < 0)
				throw std::runtime_error("Could not query sampling rate.");

			// enable streaming
			owlSetInteger(OWL_STREAMING, OWL_ENABLE);

			// enable transmission of comm data
			owlSetInteger(OWL_COMMDATA, OWL_ENABLE);

			// get the camera setup
			std::vector<OWLCamera> cameras(64);
			boost::posix_time::ptime start = boost::posix_time::second_clock::universal_time();

			// get the camera setup
			
			while (true) {
				int num_cameras = owlGetCameras(&cameras[0],cameras.size());
				if (num_cameras < 0)
					throw std::runtime_error("Querying the camera setup produced an error.");
				if (num_cameras > 0) {
					cameras.resize(num_cameras);
					break;
				} else
					boost::this_thread::yield();
				if ((boost::posix_time::second_clock::universal_time() - start).total_seconds() > max_waiting_time)
					if(f==1) {
						QMessageBox::warning(this,"Warning","No Camera Information in Owl Emulator.",QMessageBox::Ok);
						break;
					}
					else	
						throw std::runtime_error("Could not query the camera setup -- note that the PhaseSpace master program should be running.");
			}

			// get the number and ID's of defined markers...
			std::vector<OWLMarker> markers(1024);
			start = boost::posix_time::second_clock::universal_time();
			while (true) {
				int num_markers = owlGetMarkers(&markers[0],markers.size());
				if (num_markers < 0)
					throw std::runtime_error("Querying the marker setup produced an error.");
				if (num_markers > 0) {
					markers.resize(num_markers);
					break;
				} else
					boost::this_thread::yield();
				if ((boost::posix_time::second_clock::universal_time() - start).total_seconds() > max_waiting_time)
					throw std::runtime_error("Could not query the marker setup; PhaseSpace reports 0 markers.");
			}

			// get the number and ID's of defined rigid bodies...
			std::vector<OWLRigid> rigids(256);
			start = boost::posix_time::second_clock::universal_time();
			while (true) {
				int num_rigids = owlGetRigids(&rigids[0],rigids.size());
				if (num_rigids < 0)
					throw std::runtime_error("Querying the rigid-body setup produced an error.");
				if (num_rigids > 0) {
					rigids.resize(num_rigids);
					break;
				} else
					boost::this_thread::yield();
				if ((boost::posix_time::second_clock::universal_time() - start).total_milliseconds() > 500) {
					// no rigid bodies found...
					rigids.clear();
					break;
				}
			}

			// get the interpolation value
			int interpolation;
			owlGetIntegerv(OWL_INTERPOLATION,&interpolation);

			if (int err = owlGetError())
				throw std::runtime_error(std::string("Cannot query setup parameters: ") += boost::lexical_cast<std::string>(err));

			// sanitize the marker names
			marker_names.resize(markers.size());
			rigid_names.resize(rigids.size());
			for (int k=0,e=markers.size();k<e;k++)
				if (marker_names[k].empty())
					marker_names[k] = std::string("Marker") += boost::lexical_cast<std::string>(markers[k].id);
			for (int k=0,e=rigids.size();k<e;k++)
				if (rigid_names[k].empty())
					rigid_names[k] = std::string("Rigid") += boost::lexical_cast<std::string>(rigids[k].id);

			// start the reader thread
			stream_per_rigid = ui->rigidStreams->checkState() == Qt::Checked;
			stop_ = false;
			reader_thread_.reset(new boost::thread(&MainWindow::read_thread,this,srate,interpolation,cameras,markers,rigids));

		} catch (std::exception &e) {
			owlDone();
			QMessageBox::critical(this,"Error",(std::string("Problem during setup: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}


// background data reader thread
void MainWindow::read_thread(float srate, int interpolation, std::vector<OWLCamera> cameras, std::vector<OWLMarker> markers, std::vector<OWLRigid> rigids) {

	// create main Mocap stream
	lsl::stream_info info("PhaseSpace","Mocap",markers.size()*4 + rigids.size()*8 + 1,srate,lsl::cf_float32,server_name);

	// physical setup
	lsl::xml_element setup = info.desc().append_child("setup");
	setup.append_child_value("name",setup_name.c_str());
	lsl::xml_element cams = setup.append_child("cameras");
	for (int k=0;k<cameras.size();k++) {
		lsl::xml_element cam = cams.append_child("camera");
		cam.append_child_value("label",boost::lexical_cast<std::string>(cameras[k].id).c_str());
		cam.append_child_value("confidence",boost::lexical_cast<std::string>(cameras[k].cond).c_str());
		cam.append_child("position")
			.append_child_value("X",boost::lexical_cast<std::string>(cameras[k].pose[0]/1000).c_str())
			.append_child_value("Y",boost::lexical_cast<std::string>(cameras[k].pose[1]/1000).c_str())
			.append_child_value("Z",boost::lexical_cast<std::string>(cameras[k].pose[2]/1000).c_str());
		cam.append_child("orientation")
			.append_child_value("A",boost::lexical_cast<std::string>(cameras[k].pose[3]).c_str())
			.append_child_value("B",boost::lexical_cast<std::string>(cameras[k].pose[4]).c_str())
			.append_child_value("C",boost::lexical_cast<std::string>(cameras[k].pose[5]).c_str())
			.append_child_value("D",boost::lexical_cast<std::string>(cameras[k].pose[6]).c_str());
		cam.append_child("settings")
			.append_child_value("flag",boost::lexical_cast<std::string>(cameras[k].flag).c_str());
	}

	lsl::xml_element mrks = setup.append_child("markers");
	for (int k=0;k<markers.size();k++) {
		mrks.append_child("marker")
			.append_child_value("label",marker_names[k].c_str())
			.append_child_value("id",boost::lexical_cast<std::string>(markers[k].id).c_str());
	}
	lsl::xml_element objs = setup.append_child("objects");
	for (int k=0;k<rigids.size();k++) {
		objs.append_child("object")
			.append_child_value("label",rigid_names[k].c_str())
			.append_child_value("id",boost::lexical_cast<std::string>(rigids[k].id).c_str())
			.append_child_value("type","Rigid");
	}

	// channel layout
	lsl::xml_element channels = info.desc().append_child("channels");
	for (int k=0;k<markers.size();k++) {
		channels.append_child("channel")
			.append_child_value("label",(std::string(marker_names[k])+="_X").c_str())
			.append_child_value("marker",marker_names[k].c_str())
			.append_child_value("type","PositionX")
			.append_child_value("unit","meters");
		channels.append_child("channel")
			.append_child_value("label",(std::string(marker_names[k])+="_Y").c_str())
			.append_child_value("marker",marker_names[k].c_str())
			.append_child_value("type","PositionY")
			.append_child_value("unit","meters");
		channels.append_child("channel")
			.append_child_value("label",(std::string(marker_names[k])+="_Z").c_str())
			.append_child_value("marker",marker_names[k].c_str())
			.append_child_value("type","PositionZ")
			.append_child_value("unit","meters");
		channels.append_child("channel")
			.append_child_value("label",(std::string(marker_names[k])+="_Conf").c_str())
			.append_child_value("marker",marker_names[k].c_str())
			.append_child_value("type","Confidence");
	}
	for (int k=0;k<rigids.size();k++) {
		channels.append_child("channel")
			.append_child_value("label",(std::string(rigid_names[k])+="_X").c_str())
			.append_child_value("object",rigid_names[k].c_str())
			.append_child_value("type","PositionX")
			.append_child_value("unit","meters");
		channels.append_child("channel")
			.append_child_value("label",(std::string(rigid_names[k])+="_Y").c_str())
			.append_child_value("object",rigid_names[k].c_str())
			.append_child_value("type","PositionY")
			.append_child_value("unit","meters");
		channels.append_child("channel")
			.append_child_value("label",(std::string(rigid_names[k])+="_Z").c_str())
			.append_child_value("object",rigid_names[k].c_str())
			.append_child_value("type","PositionZ")
			.append_child_value("unit","meters");
		channels.append_child("channel")
			.append_child_value("label",(std::string(rigid_names[k])+="_A").c_str())
			.append_child_value("object",rigid_names[k].c_str())
			.append_child_value("type","OrientationA")
			.append_child_value("unit","quaternion");
		channels.append_child("channel")
			.append_child_value("label",(std::string(rigid_names[k])+="_B").c_str())
			.append_child_value("object",rigid_names[k].c_str())
			.append_child_value("type","OrientationB")
			.append_child_value("unit","quaternion");
		channels.append_child("channel")
			.append_child_value("label",(std::string(rigid_names[k])+="_C").c_str())
			.append_child_value("object",rigid_names[k].c_str())
			.append_child_value("type","OrientationC")
			.append_child_value("unit","quaternion");
		channels.append_child("channel")
			.append_child_value("label",(std::string(rigid_names[k])+="_D").c_str())
			.append_child_value("object",rigid_names[k].c_str())
			.append_child_value("type","OrientationD")
			.append_child_value("unit","quaternion");
		channels.append_child("channel")
			.append_child_value("label",(std::string(rigid_names[k])+="_Conf").c_str())
			.append_child_value("object",rigid_names[k].c_str())
			.append_child_value("type","Confidence");
	}
	channels.append_child("channel")
		.append_child_value("label","TriggerTTL")
		.append_child_value("type","Trigger")
		.append_child_value("unit","boolean");

	// misc information
	info.desc().append_child("acquisition")
		.append_child_value("manufacturer","PhaseSpace")
		.append_child_value("model","Impulse")
		.append_child_value("compensated_lag",boost::lexical_cast<std::string>(compensated_lag).c_str());
	info.desc().append_child("filtering")
		.append_child_value("interpolation",boost::lexical_cast<std::string>(interpolation).c_str());

	// make a new outlet
	lsl::stream_outlet outlet(info);

	// also make outlets per rigid body, if enabled
	std::vector<boost::shared_ptr<lsl::stream_outlet> > rigid_outlets;
	if (stream_per_rigid) {
		for (int k=0;k<rigids.size();k++) {
			// make streaminfo
			lsl::stream_info info(std::string("PhaseSpace_")+=rigid_names[k],"Mocap",8,srate,lsl::cf_float32,(std::string(server_name)+=rigid_names[k])+=boost::lexical_cast<std::string>(k));
			// append minimal meta-data
			lsl::xml_element setup = info.desc().append_child("setup");
			setup.append_child_value("name",setup_name.c_str());
			setup.append_child("objects").append_child("object")
				.append_child_value("label",rigid_names[k].c_str())
				.append_child_value("id",boost::lexical_cast<std::string>(rigids[k].id).c_str())
				.append_child_value("type","Rigid");
			lsl::xml_element channels = info.desc().append_child("channels");
			channels.append_child("channel")
				.append_child_value("label",(std::string(rigid_names[k])+="_X").c_str())
				.append_child_value("object",rigid_names[k].c_str())
				.append_child_value("type","PositionX")
				.append_child_value("unit","meters");
			channels.append_child("channel")
				.append_child_value("label",(std::string(rigid_names[k])+="_Y").c_str())
				.append_child_value("object",rigid_names[k].c_str())
				.append_child_value("type","PositionY")
				.append_child_value("unit","meters");
			channels.append_child("channel")
				.append_child_value("label",(std::string(rigid_names[k])+="_Z").c_str())
				.append_child_value("object",rigid_names[k].c_str())
				.append_child_value("type","PositionZ")
				.append_child_value("unit","meters");
			channels.append_child("channel")
				.append_child_value("label",(std::string(rigid_names[k])+="_A").c_str())
				.append_child_value("object",rigid_names[k].c_str())
				.append_child_value("type","OrientationA")
				.append_child_value("unit","quaternion");
			channels.append_child("channel")
				.append_child_value("label",(std::string(rigid_names[k])+="_B").c_str())
				.append_child_value("object",rigid_names[k].c_str())
				.append_child_value("type","OrientationB")
				.append_child_value("unit","quaternion");
			channels.append_child("channel")
				.append_child_value("label",(std::string(rigid_names[k])+="_C").c_str())
				.append_child_value("object",rigid_names[k].c_str())
				.append_child_value("type","OrientationC")
				.append_child_value("unit","quaternion");
			channels.append_child("channel")
				.append_child_value("label",(std::string(rigid_names[k])+="_D").c_str())
				.append_child_value("object",rigid_names[k].c_str())
				.append_child_value("type","OrientationD")
				.append_child_value("unit","quaternion");
			channels.append_child("channel")
				.append_child_value("label",(std::string(rigid_names[k])+="_Conf").c_str())
				.append_child_value("object",rigid_names[k].c_str())
				.append_child_value("type","Confidence");
			info.desc().append_child("acquisition").append_child_value("compensated_lag",boost::lexical_cast<std::string>(compensated_lag).c_str());

			// <synchronization> meta-data
			// the mean and rms here are measured accurately
			//the other numbers are approximate based on histograms
			info.desc().append_child("synchronization")
				.append_child_value("offset_mean","-.00778")
				.append_child_value("offset_rms", "0.0003")
				.append_child_value("offset_median", "-0.00778")
				.append_child_value("offset_5_centile", "-0.00928")
				.append_child_value("offset_95_centile", "-0.00628");

			// make outlet
			rigid_outlets.push_back(boost::shared_ptr<lsl::stream_outlet>(new lsl::stream_outlet(info)));
		}
	}

	// send data...
	std::vector<OWLMarker> tmp_markers(markers.size());
	std::vector<OWLRigid> tmp_rigids(rigids.size());
	std::vector<float> big_sample(info.channel_count());
	std::vector<float> rigid_sample(8);
	unsigned char buffer[1024]; // for comm data

	while (!stop_) {
		// spin until we get the number of markers that we're looking for...
		while (owlGetMarkers(&tmp_markers[0],tmp_markers.size()) < markers.size())
			boost::this_thread::yield();

		if (!rigids.empty()) {
			// spin until we get the number of rigids that we're looking for...
			while (owlGetRigids(&tmp_rigids[0],tmp_rigids.size()) < rigids.size())
				boost::this_thread::yield();
		}

		// construct a sample for the main outlet
		for (int k=0;k<tmp_markers.size();k++) {
			big_sample[k*4+0] = tmp_markers[k].x/1000;
			big_sample[k*4+1] = tmp_markers[k].y/1000;
			big_sample[k*4+2] = tmp_markers[k].z/1000;
			big_sample[k*4+3] = tmp_markers[k].cond;
		}
		for (int k=0;k<tmp_rigids.size();k++) {
			// position
			big_sample[markers.size()*4 + k*8 + 0] = tmp_rigids[k].pose[0]/1000;
			big_sample[markers.size()*4 + k*8 + 1] = tmp_rigids[k].pose[1]/1000;
			big_sample[markers.size()*4 + k*8 + 2] = tmp_rigids[k].pose[2]/1000;
			// orientation
			big_sample[markers.size()*4 + k*8 + 3] = tmp_rigids[k].pose[3];
			big_sample[markers.size()*4 + k*8 + 4] = tmp_rigids[k].pose[4];
			big_sample[markers.size()*4 + k*8 + 5] = tmp_rigids[k].pose[5];
			big_sample[markers.size()*4 + k*8 + 6] = tmp_rigids[k].pose[6];
			// confidence
			big_sample[markers.size()*4 + k*8 + 7] = tmp_rigids[k].cond;
		}

		// append trigger channel
		int n = owlGetString(OWL_COMMDATA, (char*)buffer);
		big_sample[markers.size()*4 + tmp_rigids.size()*8] = (n>0) ? buffer[24] : 0;

		// according to the PhaseSpace spec, the obtained samples are up to 10ms old
		double acquisition_time = lsl::local_clock() - compensated_lag;

		// send sample over the main outlet
		outlet.push_sample(big_sample,acquisition_time);

		// send one sample per rigid body, if enabled
		if (!rigids.empty() && stream_per_rigid)
			for (int k=0;k<tmp_rigids.size();k++) {
				float rigid_sample[8] = {tmp_rigids[k].pose[0]/1000,tmp_rigids[k].pose[1]/1000,tmp_rigids[k].pose[2]/1000,
					tmp_rigids[k].pose[3],tmp_rigids[k].pose[4],tmp_rigids[k].pose[5],tmp_rigids[k].pose[6],tmp_rigids[k].cond};
				rigid_outlets[k]->push_sample(rigid_sample,acquisition_time);
			}
	}
}


MainWindow::~MainWindow()
{
	delete ui;
}
