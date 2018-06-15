#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace lsl;
using boost::lexical_cast;

// some hard-coded settings
//const bool use_multicast = true;						// we're not using multicast since it may be dysfunctional on some routers / OSs
const Verbosity verbosity_level = Verbosity_Debug;

MainWindow::MainWindow(QWidget *parent, const string &config_file) :
QMainWindow(parent), ui(new Ui::MainWindow), nnc(NULL)
{
	ui->setupUi(this);

	// parse startup config file
	load_config(config_file);

	// make GUI connections
	QObject::connect(ui->action_Quit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link_optitrack()));
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
	if (nnc)
		ev->ignore();
}

void MainWindow::load_config(const string &filename) {
	using boost::property_tree::ptree;
	ptree pt;
	// parse file
	try {
		read_xml(filename, pt);
	} catch(exception &e) {
		QMessageBox::information(this,"Error",(string("Cannot read config file: ")+= e.what()).c_str(),QMessageBox::Ok);
		return;
	}
	// get config values
	try {
		// initialize the list of camera types
		string camtypes = pt.get<string>("cameras.knowntypes","[S250e, Prime 13, Flex 13, V100:R2, V120:Duo, V120:Trio, V120:SLIM]");
		vector<string> typelist;
		boost::algorithm::split(typelist,camtypes.substr(1,camtypes.size()-2),boost::algorithm::is_any_of(","),boost::algorithm::token_compress_on);
		ui->cameraType->clear();
		for (int k=0;k<typelist.size();k++) {
			boost::algorithm::trim_if(typelist[k],boost::algorithm::is_any_of(" '\""));
			ui->cameraType->addItem(typelist[k].c_str());
		}
		// select the configured type
		string selectedtype = pt.get<string>("cameras.type","S250e");
		int idx = std::find(typelist.begin(),typelist.end(),selectedtype) - typelist.begin();
		if (idx)
			ui->cameraType->setCurrentIndex(idx);
		// deduce nominal sampling rate from camera type
		camtype = ui->cameraType->itemText(ui->cameraType->currentIndex()).toStdString();
		srate = 120;
		if (camtype == "S250e")
			srate = 250;
		if (camtype == "Prime 13")
			srate = 240;
		if (camtype == "V100:R2")
			srate = 100;
		
		// use the sample rate given by the config file (ditch the camera type field?)"
		if(srate!=std::atof(pt.get<string>("fps","").c_str()))
			srate = std::atof(pt.get<string>("fps","").c_str());	
		// cpp type conversion dance
		std::ostringstream ss;
		ss << srate;
		std::string s(ss.str());
		ui->fpsEdit->setText(s.c_str());	
		
		// and assign the configured server address
		ui->serverAddress->setText(pt.get<string>("server.address","").c_str());

		ui->clientAddress->setText(pt.get<string>("client.address","127.0.0.1").c_str());

		ui->multicastCheck->setChecked("false");
		if(pt.get<string>("multicastEnabled").compare("true")==0)
			ui->multicastCheck->setChecked("true");

		ui->multicastAddress->setText(pt.get<string>("server.multicast_address","(default)").c_str());
		ui->commandPort->setText(pt.get<string>("server.commandport","1510").c_str());
		ui->dataPort->setText(pt.get<string>("server.dataport","1511").c_str());
		
	} catch(exception &) {
		QMessageBox::information(this,"Error in Config File","Could not read out config parameters.",QMessageBox::Ok);
		return;
	}
}

void MainWindow::save_config(const string &filename) {
	using boost::property_tree::ptree;
	ptree pt;
	// transfer UI content into property tree
	try {
		pt.put("server.address",ui->serverAddress->text().toStdString());
		pt.put("server.multicast_address",ui->multicastAddress->text().toStdString());
		pt.put("client.address",ui->clientAddress->text().toStdString());
		pt.put("server.commandport",ui->commandPort->text().toStdString());
		pt.put("server.dataport",ui->dataPort->text().toStdString());
		ostringstream camtypes;
		camtypes << "[";
		for (int k=0;k<ui->cameraType->count();k++)
			camtypes << ui->cameraType->itemText(k).toStdString() << ", ";
		string tmp(camtypes.str());
		pt.put("cameras.knowntypes",tmp.substr(0,tmp.size()-2)+= "]");
		pt.put("cameras.type",ui->cameraType->itemText(ui->cameraType->currentIndex()).toStdString());
		pt.put("fps",ui->fpsEdit->text().toStdString());
		if(ui->multicastCheck->isChecked()==true)
		pt.put("multicastEnabled","true");

	} catch(exception &e) {
		QMessageBox::critical(this,"Error",(string("Could not prepare settings for saving: ")+=e.what()).c_str(),QMessageBox::Ok);
	}
	// write to disk
	try {
		write_xml(filename, pt);
	} catch(exception &e) {
		QMessageBox::critical(this,"Error",(string("Could not write to config file: ")+=e.what()).c_str(),QMessageBox::Ok);
	}
}


// start/stop OptiTrack connection
void MainWindow::link_optitrack() {	
	if (nnc) {
		// === perform unlink action ===

		nnc->Uninitialize();
		delete nnc;
		nnc = NULL;

		if (outlet)
			outlet.reset();

		// indicate that we are now successfully unlinked
		ui->linkButton->setText("Link");
	} else {
		// === perform link action ===
		bool is_initialized = false;
		try {
			// instantiate client object
			use_multicast = ui->multicastCheck->isChecked();
			nnc = new NatNetClient(use_multicast ? ConnectionType_Multicast : ConnectionType_Unicast);
			if (!nnc)
				throw runtime_error("Could not instantiate OptiTrack client service. Please make sure that you are using a supported operating system.");

			// get NatNet lib version
			unsigned char natnet_version[4];
			nnc->NatNetVersion(natnet_version);

			// set a bunch of properties
			nnc->SetMessageCallback(error_callback);
			nnc->SetVerbosityLevel(verbosity_level);
			nnc->SetDataCallback(data_callback,this);
			string multicast_addr = ui->multicastAddress->text().toStdString();
			char mc_addr[128]={0};strcpy(mc_addr,multicast_addr.c_str()); 
			if (!multicast_addr.empty() && multicast_addr != "(default)")
				nnc->SetMulticastAddress(mc_addr);

			// connect to NatNet server
			string server_addr = ui->serverAddress->text().toStdString();
			string client_addr = ui->clientAddress->text().toStdString();
			char my_ip[128]={0}, server_ip[128]={0}; strcpy(server_ip,server_addr.c_str()); strcpy(my_ip,client_addr.c_str());
			int errcode = nnc->Initialize(my_ip, server_ip, ui->commandPort->text().toInt(), ui->dataPort->text().toInt());
			if (errcode != ErrorCode_OK)
				throw runtime_error(string("Could not connect to OptiTrack server. Is the server running and the IP address correct? ErrorCode: ") += lexical_cast<string>(errcode));
			is_initialized = true;

			// get server info
			sServerDescription ServerDescription;
			memset(&ServerDescription, 0, sizeof(ServerDescription));
			nnc->GetServerDescription(&ServerDescription);
			if (!ServerDescription.HostPresent)
				throw runtime_error("Unable to connect to OptiTrack server. Is the application running (e.g., Arena)?");

			// get scene descriptions
			sDataDescriptions* pDataDefs = NULL; 
			nnc->GetDataDescriptions(&pDataDefs);
			if(!pDataDefs)
				throw runtime_error("Unable to retrieve Scene Descriptions.");



			// deduce number of channels and remapping tables
			ms_label2ofs.clear();
			mrk_labels.clear();
			rig_id2slot.clear();
			rig_id2label.clear();
			for (int i=0; i<pDataDefs->nDataDescriptions; i++) {
				if (pDataDefs->arrDataDescriptions[i].type == Descriptor_MarkerSet) {
					sMarkerSetDescription *pMS = pDataDefs->arrDataDescriptions[i].Data.MarkerSetDescription;
					ms_label2ofs[pMS->szName] = mrk_labels.size();
					for(int i=0;i<pMS->nMarkers;i++)
						mrk_labels.push_back(pMS->szMarkerNames[i]);
				}
				if (pDataDefs->arrDataDescriptions[i].type == Descriptor_RigidBody) {
					sRigidBodyDescription *pRB = pDataDefs->arrDataDescriptions[i].Data.RigidBodyDescription;
					if (!rig_id2slot.left.count(pRB->ID)) {
						rig_id2slot.insert(boost::bimap<int,int>::value_type(pRB->ID,rig_id2slot.size()));
						rig_id2label[pRB->ID] = pRB->szName;
					}
				}
				if (pDataDefs->arrDataDescriptions[i].type == Descriptor_Skeleton) {
					sSkeletonDescription *pSK = pDataDefs->arrDataDescriptions[i].Data.SkeletonDescription;
					for (int j=0; j<pSK->nRigidBodies; j++) {
						sRigidBodyDescription *pRB = &pSK->RigidBodies[j];
						if (!rig_id2slot.left.count(pRB->ID)) {
							rig_id2slot.insert(boost::bimap<int,int>::value_type(pRB->ID,rig_id2slot.size()));
							rig_id2label[pRB->ID] = pRB->szName;
						}
					}
				}
			}

			// 3 channels per marker (x,y,z), 8 channels per rigid (x,y,z, a,b,c,d, err)
			num_channels = 3*mrk_labels.size() + 8*rig_id2slot.size();

			// create stream info 
			stream_info info("OptiTrack","Mocap",num_channels,srate,cf_float32,server_addr + "_OptiTrack");

			// camera setup information
			xml_element setup = info.desc().append_child("setup");
			setup.append_child("cameras").append_child_value("model",camtype.c_str());

			// for each data description...
			xml_element allmrks = setup.append_child("markers");
			xml_element objs = setup.append_child("objects");
			for (int i=0; i<pDataDefs->nDataDescriptions; i++) {
				if (pDataDefs->arrDataDescriptions[i].type == Descriptor_MarkerSet) {
					// MarkerSet: this is a named collection of markers (each referenced by its label)
					// the associated data is the position (x,y,z) and size (w)
					sMarkerSetDescription* pMS = pDataDefs->arrDataDescriptions[i].Data.MarkerSetDescription;
					xml_element obj = objs.append_child("object")
						.append_child_value("label",pMS->szName)
						.append_child_value("class","MarkerSet");
					xml_element mrks = obj.append_child("markers");
					for(int i=0;i<pMS->nMarkers;i++) {
						mrks.append_child("marker").append_child_value("label",pMS->szMarkerNames[i]);
						allmrks.append_child("marker").append_child_value("label",pMS->szMarkerNames[i]);
					}
				}
				if (pDataDefs->arrDataDescriptions[i].type == Descriptor_RigidBody) {
					// RigidBody: this is a named object with an ID that may be embedded in a transform hierarchy
					// the associated data is the center of mass (x,y,z). orientation quaternion (a,b,c,d), and fit error (e)
					sRigidBodyDescription* pRB = pDataDefs->arrDataDescriptions[i].Data.RigidBodyDescription;
					xml_element obj = objs.append_child("object")
						.append_child_value("label",pRB->szName)
						.append_child_value("class","Rigid")
						.append_child_value("id",lexical_cast<string>(pRB->ID).c_str())
						.append_child_value("parent_id",lexical_cast<string>(pRB->parentID).c_str());
					obj.append_child("translation")
						.append_child_value("X",lexical_cast<string>(pRB->offsetx).c_str())
						.append_child_value("Y",lexical_cast<string>(pRB->offsety).c_str())
						.append_child_value("Z",lexical_cast<string>(pRB->offsetz).c_str());
				}
				if (pDataDefs->arrDataDescriptions[i].type == Descriptor_Skeleton) {
					// Skeleton: this is a named articulated hierarchy of rigids (note: not clear whether these are also listed redundantly as separate rigids)
					sSkeletonDescription* pSK = pDataDefs->arrDataDescriptions[i].Data.SkeletonDescription;
					xml_element obj = objs.append_child("object")
						.append_child_value("label",pSK->szName)
						.append_child_value("class","Skeleton")
						.append_child_value("id",lexical_cast<string>(pSK->skeletonID).c_str());
					xml_element rigids = obj.append_child("rigids");
					for (int j=0; j<pSK->nRigidBodies; j++) {
						sRigidBodyDescription* pRB = &pSK->RigidBodies[j];
						xml_element rig = rigids.append_child("rigid")
							.append_child_value("label",pRB->szName)
							.append_child_value("id",lexical_cast<string>(pRB->ID).c_str())
							.append_child_value("parent_id",lexical_cast<string>(pRB->parentID).c_str());
						rig.append_child("translation")
							.append_child_value("X",lexical_cast<string>(pRB->offsetx).c_str())
							.append_child_value("Y",lexical_cast<string>(pRB->offsety).c_str())
							.append_child_value("Z",lexical_cast<string>(pRB->offsetz).c_str());
					}
				}
			}

			// channel layout
			xml_element channels = info.desc().append_child("channels");
			// for each known marker we transmit 3 values (x,y,z)
			for (int k=0;k<mrk_labels.size();k++) {
				channels.append_child("channel")
					.append_child_value("label",(mrk_labels[k] + "_X").c_str())
					.append_child_value("marker",mrk_labels[k].c_str())
					.append_child_value("type","PositionX")
					.append_child_value("unit","meters");
				channels.append_child("channel")
					.append_child_value("label",(mrk_labels[k] + "_Y").c_str())
					.append_child_value("marker",mrk_labels[k].c_str())
					.append_child_value("type","PositionY")
					.append_child_value("unit","meters");
				channels.append_child("channel")
					.append_child_value("label",(mrk_labels[k] + "_Z").c_str())
					.append_child_value("marker",mrk_labels[k].c_str())
					.append_child_value("type","PositionZ")
					.append_child_value("unit","meters");
			}

			// for each known rigid body we transmit 8 values (x,y,z, a,b,c,d, 1.0/err)
			for (int k=0;k<rig_id2slot.size();k++) {
				string label = rig_id2label[rig_id2slot.right.at(k)];
				channels.append_child("channel")
					.append_child_value("label",(label + "_X").c_str())
					.append_child_value("object",label.c_str())
					.append_child_value("type","PositionX")
					.append_child_value("unit","meters");
				channels.append_child("channel")
					.append_child_value("label",(label + "_Y").c_str())
					.append_child_value("object",label.c_str())
					.append_child_value("type","PositionY")
					.append_child_value("unit","meters");
				channels.append_child("channel")
					.append_child_value("label",(label + "_Z").c_str())
					.append_child_value("object",label.c_str())
					.append_child_value("type","PositionZ")
					.append_child_value("unit","meters");

				channels.append_child("channel")
					.append_child_value("label",(label + "_A").c_str())
					.append_child_value("object",label.c_str())
					.append_child_value("type","OrientationA")
					.append_child_value("unit","quaternion");
				channels.append_child("channel")
					.append_child_value("label",(label + "_B").c_str())
					.append_child_value("object",label.c_str())
					.append_child_value("type","OrientationB")
					.append_child_value("unit","quaternion");
				channels.append_child("channel")
					.append_child_value("label",(label + "_C").c_str())
					.append_child_value("object",label.c_str())
					.append_child_value("type","OrientationC")
					.append_child_value("unit","quaternion");
				channels.append_child("channel")
					.append_child_value("label",(label + "_D").c_str())
					.append_child_value("object",label.c_str())
					.append_child_value("type","OrientationD")
					.append_child_value("unit","quaternion");

				channels.append_child("channel")
					.append_child_value("label",(label + "_Conf").c_str())
					.append_child_value("object",label.c_str())
					.append_child_value("type","Confidence");
			}

			// misc information
			xml_element acq = info.desc().append_child("acquisition");
			acq.append_child_value("manufacturer","NaturalPoint");
			acq.append_child_value("model","OptiTrack");
			acq.append_child("natnet_server")
				.append_child_value("address",server_addr.c_str())
				.append_child_value("hostname",ServerDescription.szHostComputerName)
				.append_child_value("app_name",ServerDescription.szHostApp)
				.append_child_value("app_version",(lexical_cast<string>((int)ServerDescription.HostAppVersion[0]) + "." + lexical_cast<string>((int)ServerDescription.HostAppVersion[1]) + "." + lexical_cast<string>((int)ServerDescription.HostAppVersion[2]) + "." + lexical_cast<string>((int)ServerDescription.HostAppVersion[3])).c_str())
				.append_child_value("natnet_version",(lexical_cast<string>((int)ServerDescription.NatNetVersion[0]) + "." + lexical_cast<string>((int)ServerDescription.NatNetVersion[1]) + "." + lexical_cast<string>((int)ServerDescription.NatNetVersion[2]) + "." + lexical_cast<string>((int)ServerDescription.NatNetVersion[3])).c_str());
			acq.append_child("natnet_client")
				.append_child_value("natnet_version",(lexical_cast<string>((int)natnet_version[0]) + "." + lexical_cast<string>((int)natnet_version[1]) + "." + lexical_cast<string>((int)natnet_version[2]) + "." + lexical_cast<string>((int)natnet_version[3])).c_str());

			// create outlet
			outlet.reset(new stream_outlet(info));
			sFrameOfMocapData* pData = nnc->GetLastFrameOfData();

		}
		catch (exception &e) {
			QMessageBox::critical(this,"Error",(string("Could not initialize the OptiTrack interface: ")+=e.what()).c_str(),QMessageBox::Ok);
			// shut down after error
			if (is_initialized)
				nnc->Uninitialize();
			if (nnc) {
				delete nnc;
				nnc = NULL;
			}
			if (outlet)
				outlet.reset();
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}

void MainWindow::on_frame(sFrameOfMocapData *data) {
	//QMessageBox::information(this,"Error","Got packet!",QMessageBox::Ok);
	double now = local_clock();
	if (!outlet)
		return;

	// construct sample
	vector<float> sample(num_channels);

	// assign all marker sets
	for (int k=0;k<data->nMarkerSets;k++) {
		int chn_offset = 3*ms_label2ofs[data->MocapData[k].szName];
		for (int j=0;j<data->MocapData[k].nMarkers;j++) {
			sample[chn_offset + j*3 + 0] = data->MocapData[k].Markers[j][0];
			sample[chn_offset + j*3 + 1] = data->MocapData[k].Markers[j][1];
			sample[chn_offset + j*3 + 2] = data->MocapData[k].Markers[j][2];
		}
	}

	// assign the rigid body data
	int rigid_offset = 3*mrk_labels.size();
	for (int i=0; i<data->nRigidBodies; i++) {
		if (rig_id2slot.left.count(data->RigidBodies[i].ID)) {
			int slot = rig_id2slot.left.at(data->RigidBodies[i].ID);
			sample[rigid_offset + slot*8 + 0] = data->RigidBodies[i].x;
			sample[rigid_offset + slot*8 + 1] = data->RigidBodies[i].y;
			sample[rigid_offset + slot*8 + 2] = data->RigidBodies[i].z;
			sample[rigid_offset + slot*8 + 3] = data->RigidBodies[i].qx;
			sample[rigid_offset + slot*8 + 4] = data->RigidBodies[i].qy;
			sample[rigid_offset + slot*8 + 5] = data->RigidBodies[i].qz;
			sample[rigid_offset + slot*8 + 6] = data->RigidBodies[i].qw;
			sample[rigid_offset + slot*8 + 7] = data->RigidBodies[i].MeanError;
		}
	}

	// assign the skeleton data
	for(int i=0; i<data->nSkeletons; i++) {
		sSkeletonData skData = data->Skeletons[i];
		for(int j=0; j< skData.nRigidBodies; j++) {
			sRigidBodyData rbData = skData.RigidBodyData[j];
			if (rig_id2slot.left.count(rbData.ID)) {
				int slot = rig_id2slot.left.at(rbData.ID);
				sample[rigid_offset + slot*8 + 0] = rbData.x;
				sample[rigid_offset + slot*8 + 1] = rbData.y;
				sample[rigid_offset + slot*8 + 2] = rbData.z;
				sample[rigid_offset + slot*8 + 3] = rbData.qx;
				sample[rigid_offset + slot*8 + 4] = rbData.qy;
				sample[rigid_offset + slot*8 + 5] = rbData.qz;
				sample[rigid_offset + slot*8 + 6] = rbData.qw;
				sample[rigid_offset + slot*8 + 7] = rbData.MeanError;
			}
		}
	}

	// send it off
	outlet->push_sample(sample,now);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::error_callback(int msgType, char *msg) {
	//cout << msg << endl;
}

void MainWindow::data_callback(sFrameOfMocapData *data, void *pUserData) {
	((MainWindow*)pUserData)->on_frame(data);
}
