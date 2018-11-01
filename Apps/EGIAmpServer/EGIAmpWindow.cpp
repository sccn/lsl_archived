#include "EGIAmpWindow.h"
#include "ui_EGIAmpWindow.h"

// LSL API
#include <lsl_cpp.h>

#include <boost/endian/conversion.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <chrono>

#include <QComboBox>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QSpinBox>
#include <boost/endian/conversion.hpp>
#include <iostream>
#include <regex>
#include <sstream>


// hard-coded data about EGI packets
const int block_header_bytes = 16;          // number of header bytes per block
const int sample_format1_header_bytes = 32; // number of header bytes per sample

// number of samples per chunk sent into LSL (has nothing to do with EGI)
const int samples_per_chunk = 32;


using namespace std::chrono_literals;

#pragma pack(1)

typedef struct {
	uint32_t header[8]; // DINS (Digital Inputs) 1-8/9-16 at bytes 24/25; net type at byte 26.
	float eeg[256];     // EEG Data.
	float pib[7];       // PIB data.
	float unused1;      // N/A
	float ref;          // The reference channel.
	float com;          // The common channel.
	float unused2;      // N/A
	float padding[13];  // N/A
} PacketFormat1;

typedef struct {
	uint8_t digitalInputs;
	uint8_t status;
	uint8_t batteryLevel[3];
	uint8_t temperature[3];
	uint8_t sp02;
	uint8_t heartRate[2];
} PacketFormat2_PIB_AUX;

typedef struct {
	uint16_t digitalInputs;
	uint8_t tr;
	PacketFormat2_PIB_AUX pib1_aux;
	PacketFormat2_PIB_AUX pib2_aux;
	uint64_t packetCounter;
	uint64_t timeStamp;
	uint8_t netCode;
	uint8_t reserved[38];
	int32_t eegData[256];
	uint32_t auxData[3];
	uint32_t refMonitor;
	uint32_t comMonitor;
	uint32_t driveMonitor;
	uint32_t diagnosticsChannel;
	uint32_t currentSense;
	int32_t pib1_Data[16];
	int32_t pib2_Data[16];
} PacketFormat2;

#pragma pack()

// Net codes
typedef enum {
	GSN64_2_0,  //  GSN 64
	GSN128_2_0, //  GSN 128
	GSN256_2_0, //  GSN 256

	HCGSN32_1_0,  //  HGSN 32
	HCGSN64_1_0,  //  HGSN 64
	HCGSN128_1_0, //  HGSN 128
	HCGSN256_1_0, //  HGSN 256

	MCGSN32_1_0,  //  MGSN 32
	MCGSN64_1_0,  //  MGSN 64
	MCGSN128_1_0, //  MGSN 128
	MCGSN256_1_0, //  MGSN 256

	TestConnector = 14,
	NoNet = 15,    //  net not connected
	Unknown = 0xFF //  Unknown or net not connected
} NetCode;

/**
 *  AmpDataPacket Header.
 */
typedef struct {

	/**
	 *  The ampID associated with this data packet (if appropriate).
	 */
	int64_t ampID;

	/**
	 *  Specifies the length of the data field.
	 */
	uint64_t length;

} AmpDataPacketHeader;

EGIAmpWindow::EGIAmpWindow(QWidget* parent, const std::string& config_file)
    : QMainWindow(parent), ui(new Ui::EGIAmpWindow), stop_flag(false) {
	ui->setupUi(this);

	// parse startup config file
	load_config(config_file);


	// make GUI connections
	QObject::connect(ui->actionQuit, &QAction::triggered, this, &EGIAmpWindow::close);
	QObject::connect(ui->linkButton, &QPushButton::clicked, this, &EGIAmpWindow::link_ampserver);
	QObject::connect(ui->actionLoad_Configuration, &QAction::triggered, this,
	                 &EGIAmpWindow::load_config_dialog);
	QObject::connect(ui->actionSave_Configuration, &QAction::triggered, this,
	                 &EGIAmpWindow::save_config_dialog);
	QObject::connect(this, &EGIAmpWindow::appendStatusMessage, ui->statusBox,
	                 &QPlainTextEdit::appendPlainText);
	QObject::connect(this, &EGIAmpWindow::enableSampleRateSelection, ui->sampleRateComboBox,
	                 &QComboBox::setEnabled);
	QObject::connect(this, &EGIAmpWindow::error, this, &EGIAmpWindow::displayError);
	QObject::connect(this, &EGIAmpWindow::enableUI, this, &EGIAmpWindow::unlockUI);
	QObject::connect(this, &EGIAmpWindow::disableUI, this, &EGIAmpWindow::lockUI);
	QObject::connect(this, &EGIAmpWindow::fieldsEnabled, this->ui->sampleRateComboBox,
	                 &QComboBox::setEnabled);
	QObject::connect(this, &EGIAmpWindow::fieldsEnabled, this->ui->amplifierId,
	                 &QSpinBox::setEnabled);
	QObject::connect(this, &EGIAmpWindow::fieldsEnabled, this->ui->serverAddress,
	                 &QLineEdit::setEnabled);
	QObject::connect(this, &EGIAmpWindow::fieldsEnabled, this->ui->commandPort,
	                 &QSpinBox::setEnabled);
	QObject::connect(this, &EGIAmpWindow::fieldsEnabled, this->ui->notificationPort,
	                 &QSpinBox::setEnabled);
	QObject::connect(this, &EGIAmpWindow::fieldsEnabled, this->ui->dataPort, &QSpinBox::setEnabled);
	QObject::connect(this, &EGIAmpWindow::setLinkButtonText, this->ui->linkButton,
	                 &QPushButton::setText);
}

void EGIAmpWindow::load_config_dialog() {
	QString sel = QFileDialog::getOpenFileName(this, "Load Configuration File", "",
	                                           "Configuration Files (*.cfg)");
	if (!sel.isEmpty()) load_config(sel.toStdString());
}

void EGIAmpWindow::save_config_dialog() {
	QString sel = QFileDialog::getSaveFileName(this, "Save Configuration File", "",
	                                           "Configuration Files (*.cfg)");
	if (!sel.isEmpty()) save_config(sel.toStdString());
}

void EGIAmpWindow::closeEvent(QCloseEvent* ev) {
	if (reader_thread_) ev->ignore();
}

void EGIAmpWindow::load_config(const std::string& filename) {
	using boost::property_tree::ptree;
	ptree pt;

	// parse file
	try {
		read_xml(filename, pt);
	} catch (std::exception& e) {
		QMessageBox::information(
		    this, "Error", QStringLiteral("Cannot read config file: ") + e.what(), QMessageBox::Ok);
		return;
	}

	// get config values
	try {
		ui->serverAddress->setText(
		    pt.get<std::string>("ampserver.address", "172.16.2.249").c_str());
		ui->commandPort->setValue(pt.get<int>("ampserver.commandport", 9877));
		ui->notificationPort->setValue(pt.get<int>("ampserver.notificationport", 9878));
		ui->dataPort->setValue(pt.get<int>("ampserver.dataport", 9879));
		ui->amplifierId->setValue(pt.get<int>("settings.amplifierid", 0));
		ui->sampleRateComboBox->setCurrentText(
		    QString(std::to_string(pt.get<int>("settings.samplingrate", 1000)).c_str()));
	} catch (std::exception&) {
		QMessageBox::information(this, "Error in Config File",
		                         "Could not read out config parameters.", QMessageBox::Ok);
		return;
	}
}

void EGIAmpWindow::save_config(const std::string& filename) {
	using boost::property_tree::ptree;
	ptree pt;

	// transfer UI content into property tree
	try {
		pt.put("ampserver.address", ui->serverAddress->text().toStdString());
		pt.put("ampserver.commandport", ui->commandPort->value());
		pt.put("ampserver.notificationport", ui->notificationPort->value());
		pt.put("ampserver.dataport", ui->dataPort->value());
		pt.put("settings.amplifierid", ui->amplifierId->value());
		pt.put("settings.samplingrate", ui->sampleRateComboBox->currentText().toInt());
	} catch (std::exception& e) {
		QMessageBox::critical(this, "Error",
		                      QStringLiteral("Could not prepare settings for saving: ") + e.what(),
		                      QMessageBox::Ok);
	}

	// write to disk
	try {
		write_xml(filename, pt);
	} catch (std::exception& e) {
		QMessageBox::critical(this, "Error",
		                      QStringLiteral("Could not write to config file: ") + e.what(),
		                      QMessageBox::Ok);
	}
}

void EGIAmpWindow::unlockUI() {
	emit this->setLinkButtonText("Link");
	emit this->fieldsEnabled(true);

	// If the stream was interupted, we need to kill the reader_thread.
	if (reader_thread_) {
		reader_thread_->join();
		reader_thread_ = nullptr;
	}

	// If the stream was interupted, we need to kill the notification_thread_.
	if (notification_thread_) {
		notification_thread_->join();
		notification_thread_ = nullptr;
	}
}

void EGIAmpWindow::lockUI() {
	emit this->setLinkButtonText("Unlink");
	emit this->fieldsEnabled(false);
}

// start/stop the ampserver connection
void EGIAmpWindow::link_ampserver() {
	if (reader_thread_) {
		// === perform unlink action ===
		try {
            
			stop_flag = true;
			reader_thread_->join();
			reader_thread_ = nullptr;

			if (notification_thread_) { 
				notification_thread_->join();
				notification_thread_ = nullptr;
			}
            
			// Reenable sample rate selection since we have stopped streaming.
			emit this->enableSampleRateSelection(true);

		} catch (std::exception& e) {
			emit this->error(QStringLiteral("Could not stop the background processing: ") +
			                 e.what());
			return;
		}

		// indicate that we are now successfully unlinked
		ui->linkButton->setText("Link");
	} else {
			// === perform link action ===

			// get the UI parameters...
			std::string address = ui->serverAddress->text().toStdString();
			int commandPort = ui->commandPort->value();
			int notificationPort = ui->notificationPort->value();
			int dataPort = ui->dataPort->value();
			int amplifierId = ui->amplifierId->value();
			QString sampleRate = ui->sampleRateComboBox->currentText();
			int nChannels = 0;

			bool connected = false;
			char response[4096];

			try {

				this->stop_flag = false;
				// create the connections
				commandStream_.clear();
				commandStream_.exceptions(std::ios::eofbit | std::ios::failbit | std::ios::badbit);
				commandStream_.expires_from_now(2s);
				commandStream_.connect(
				    ip::tcp::endpoint(ip::address::from_string(address), commandPort));

				commandStream_.rdbuf()->set_option(ip::tcp::no_delay(true));
				commandStream_.expires_from_now(8760h);

				notificationStream_.clear();
				notificationStream_.expires_from_now(2s);
				notificationStream_.connect(
				    ip::tcp::endpoint(ip::address::from_string(address), notificationPort));
				notificationStream_.rdbuf()->set_option(ip::tcp::no_delay(true));

                dataStream_.clear();
                dataStream_.expires_from_now(5s);
				dataStream_.connect(ip::tcp::endpoint(ip::address::from_string(address), dataPort));
				dataStream_.rdbuf()->set_option(ip::tcp::no_delay(true));

				if (this->getAmplifierDetails(amplifierId)) {

					// Disable sample rate selection since we are about to start streaming.
					emit this->enableSampleRateSelection(false);

					this->initAmplifier(false);

					// send the listen message
					this->sendDatastreamCommand("cmd_ListenToAmp", std::to_string(amplifierId), "0",
					                            "0");

					if (this->channelCount_ != 0) {
						nChannels = this->channelCount_;
						emit this->channelCountUpdated(nChannels);
					}

					notification_thread_.reset(
					    new std::thread([this]() { 
						try {
							this->getNotifications();
						} catch (...) {};
					}));

					    // start reading the data
						reader_thread_ = std::make_unique<std::thread>([this, address, amplifierId, sampleRate, nChannels]() {//.reset(new std::thread([this, address, amplifierId, sampleRate, nChannels]() {
						    emit this->appendStatusMessage("Stream Initialized...\n");
						    try {
							    if (packetType_ == packetType1) {
								    this->read_packet_format_1(nChannels, &this->packetsReceived_);
							    } else if (packetType_ == packetType2) {
								    this->read_packet_format_2(nChannels, &this->packetsReceived_);
							    }

						    } catch (std::exception& ex) {
							    emit this->error(QStringLiteral("Encountered the following error "
							                                    "while reading data stream: ") +
							                     ex.what());
						    }
						    halt_ampserver(amplifierId);
					    });
				}
			} catch (std::ios_base::failure& err) {
				emit this->error("Could not initialize the AmpServer interface due to a "
				                 "connection error. Please make sure that the AmpServer is turned "
				                 "on, your network settings are correct, and you have an SDK "
				                 "license for it.");
				if (connected) halt_ampserver(amplifierId);
				return;
			} catch (std::exception& e) {
				emit this->error(QStringLiteral("Could not initialize the AmpServer interface: ") +
				                 e.what());
				if (connected) halt_ampserver(amplifierId);
				return;
			}

			// done, all successful
			// ui->linkButton->setText("Unlink");
			emit this->disableUI();
	}
}

void EGIAmpWindow::displayError(QString description) {
	QMessageBox::critical(this, "Error", description, QMessageBox::Ok);
}

bool EGIAmpWindow::initAmplifier(bool verbose) {

	emit this->appendStatusMessage("Initializing Stream...\n");

	bool connected = false;

	int amplifierId = ui->amplifierId->value();
	QString sampleRate = ui->sampleRateComboBox->currentText();

	// Because it is possible that the amplifier was not properly disconnected from,
	// disconnect and shut down before starting. This will ensure that the
	// packetCounter is reset.
	std::string stopResponse = this->sendCommand("cmd_Stop", std::to_string(amplifierId), "0", "0");

	std::string setPowerOffResponse =
	    this->sendCommand("cmd_SetPower", std::to_string(amplifierId), "0", "0");

	// Set the sample rate
	std::string setSampleRateResponse = this->sendCommand(
	    "cmd_SetDecimatedRate", std::to_string(amplifierId), "0", sampleRate.toStdString());

	// turn on the amp
	std::string setPowerOnResponse =
	    this->sendCommand("cmd_SetPower", std::to_string(amplifierId), "0", "1");

	// start the desired amplifier
	std::string startResponse =
	    this->sendCommand("cmd_Start", std::to_string(amplifierId), "0", "0");
	connected = true;

	// set to default acquisition mode (note: this should almost surely come before
	// the start call...)
	std::string defaultAcquisitionResponse =
	    this->sendCommand("cmd_DefaultAcquisitionState", std::to_string(amplifierId), "0", "0");

	if (verbose) {
		emit this->appendStatusMessage("Stop: " + QString::fromStdString(stopResponse.c_str()));
		emit this->appendStatusMessage("SetPower: " +
		                               QString::fromStdString(setPowerOffResponse.c_str()));
		emit this->appendStatusMessage("SetDecimatedRate: " +
		                               QString::fromStdString(setSampleRateResponse.c_str()));
		emit this->appendStatusMessage("SetPower: " +
		                               QString::fromStdString(setPowerOnResponse.c_str()));
		emit this->appendStatusMessage("Start: " + QString::fromStdString(startResponse.c_str()));
		emit this->appendStatusMessage("DefaultAcquisitionState: " +
		                               QString::fromStdString(defaultAcquisitionResponse.c_str()));
	}

	return connected;
}

std::string EGIAmpWindow::sendCommand(std::string command, std::string ampId, std::string channel,
                                      std::string value) {

	char response[4096];
	commandStream_ << "(sendCommand " << command << ' ' << ampId << ' ' << channel << ' ' << value
	               << ")\n"
	               << std::flush;
	commandStream_.getline(response, sizeof(response));
	return std::string(response);
}

void EGIAmpWindow::sendDatastreamCommand(std::string command, std::string ampId,
                                         std::string channel, std::string value) {

	char response[4096];
	dataStream_ << "(sendCommand " << command << ' ' << ampId << ' ' << channel << ' ' << value
	            << ")\n"
	            << std::flush;
}

bool EGIAmpWindow::getAmplifierDetails(int amplifierId) {
	try {
		std::string details =
		    this->sendCommand("cmd_GetAmpDetails", std::to_string(amplifierId), "0", "0");

		/**
		 *	Returns an expression similar to the following:
		 *	(sendCommand_return (status complete) (amp_details (serial_number A14100120) (amp_type
		 *NA400) (legacy_board false) (packet_format 2) (system_version 2.0.14) (number_of_channels
		 *32)))
		 *
		 *	The following regex is used to tokenize this string, so that we can get key value pairs,
		 *for example (in the preceding response): status				: complete
		 *	serial_number		: A14100120
		 *	amp_type			: NA400
		 *	legacy_board		: false
		 *	packet_format		: 2
		 *	system_version		: 2.0.14
		 *	number_of_channels	: 32
		 **/

		emit this->appendStatusMessage(
		    "__________________________\n  Amplifier Details\n__________________________");
		std::regex token("\\([\\w\\.\\s]+\\)");
		std::smatch match;
		std::vector<std::string> tokens;
		while (std::regex_search(details, match, token)) {
			for (std::string x : match) {
				if (x.find("packet_format") != std::string::npos) {

					// Set the packet format
					std::string packetTypeString =
					    x.substr(x.find("packet_format") + strlen("packet_format") + 1);
					packetTypeString = packetTypeString.substr(0, packetTypeString.length() - 1);
					this->packetType_ = (PacketType)std::stoi(packetTypeString);
					emit this->appendStatusMessage("    Packet Format: " +
					                               QString(packetTypeString.c_str()));

				} else if (x.find("number_of_channels") != std::string::npos) {

					// Set the number of channels available with this amplifier. Note: until we
					// start streaming and read the net code from the incoming packets we don't know
					// how many channels are actually being used to collect EEG.
					std::string channelCountString =
					    x.substr(x.find("number_of_channels") + strlen("number_of_channels") + 1);
					channelCountString =
					    channelCountString.substr(0, channelCountString.length() - 1);
					this->channelCount_ = (uint16_t)std::stoi(channelCountString);
					emit this->appendStatusMessage("    Channel Count: " +
					                               QString(channelCountString.c_str()));

				} else if (x.find("amp_type") != std::string::npos) {

					// Set the amplifier type. Note: This field only yields the family name. As the
					// NA410 and NA400 have different scaling factors that need to be applied, we
					// need to determine which specific amplifier is attached.
					std::string amplifierTypeString =
					    x.substr(x.find("amp_type") + strlen("amp_type") + 1);
					if (amplifierTypeString.find("NA300") != std::string::npos) {
						this->amplifierType_ = NA300;
					} else if (amplifierTypeString.find("NA400") != std::string::npos) {
						this->amplifierType_ = NA400;
					}

				} else if (x.find("legacy_board") != std::string::npos) {

					// The NA410 is differentiated from the NA400 by the presence or absence of
					// legacy_boards.
					if (x.find("true") != std::string::npos && this->amplifierType_ == NAunknown) {
						this->amplifierType_ = NA410;
					}
				} else if (x.find("serial_number") != std::string::npos) {

					// Print the serial number
					std::string serialNumberString =
					    x.substr(x.find("serial_number") + strlen("serial_number") + 1);
					serialNumberString =
					    serialNumberString.substr(0, serialNumberString.length() - 1);
					emit this->appendStatusMessage("    Serial Number: " +
					                               QString(serialNumberString.c_str()));
				} else if (x.find("system_version") != std::string::npos) {

					// Print the firmware version.
					std::string firmwareVersionString =
					    x.substr(x.find("system_version") + strlen("system_version") + 1);
					firmwareVersionString =
					    firmwareVersionString.substr(0, firmwareVersionString.length() - 1);
					emit this->appendStatusMessage("    Firmware Version: " +
					                               QString(firmwareVersionString.c_str()));
				}
			}
			details = match.suffix().str();
		}
		if (this->amplifierType_ == NA300) {
			emit this->appendStatusMessage("    Amplifier Type: Net Amps 300");
		} else if (this->amplifierType_ == NA400) {
			emit this->appendStatusMessage("    Amplifier Type: Net Amps 400");
		} else if (this->amplifierType_ == NA410) {
			emit this->appendStatusMessage("    Amplifier Type: Net Amps 410");
		}
		emit this->appendStatusMessage("__________________________\n__________________________\n");
		if (this->amplifierType_ == NA300) {
			this->scalingFactor_ = 0.0244140625f;
		} else if (this->amplifierType_ == NA400) {
			this->scalingFactor_ = 0.00015522042f;
		} else if (this->amplifierType_ == NA410) {
			this->scalingFactor_ = 0.00009636188f;
		}

		return true;
	} catch (...) { return false; }
}

void EGIAmpWindow::getNotifications() {
	// enter transmission loop
	while (notificationStream_.good() && !stop_flag) {
		notificationStream_.expires_from_now(1s);
		char response[4096];
		notificationStream_.getline(response, sizeof(response));
		if (std::string(response).length() > 0) {

			emit this->appendStatusMessage(
			    "__________________________\n  Notification Received\n    " + QString(response) +
			    "\n__________________________\n");
		}
	}
}

void EGIAmpWindow::read_packet_format_2(int numChannels, int* packetsReceived) {
	// enter transmission loop
	unsigned bytes = 0;
	uint64_t lastPacketCounter = 0;
	std::unique_ptr<lsl::stream_outlet> outlet;
	int nChannels = numChannels;
    bool firstPacketReceived = false;
    dataStream_.clear();
	while (dataStream_.good() && !stop_flag) {
        AmpDataPacketHeader header;
        dataStream_.clear();
        dataStream_.expires_from_now(1s);
		dataStream_.read((char*)&header, sizeof(header));

		header.ampID = boost::endian::big_to_native(header.ampID);
		header.length = boost::endian::big_to_native(header.length);

		// read and transmit the chunk sample-by-sample
		int nSamples = header.length / sizeof(PacketFormat2);
		int firstPacketCounter = 0;
		int uniquePackets = 0;
		for (int s = 0; s < nSamples && dataStream_.good(); s++) {
			std::vector<float> samples;
			PacketFormat2 packet;
			dataStream_.read((char*)&packet, sizeof(PacketFormat2));
			if (!outlet) {

				if ((NetCode)(packet.netCode) == HCGSN32_1_0 ||
				    (NetCode)(packet.netCode) == MCGSN32_1_0) {
					nChannels = 32;
				} else if ((NetCode)(packet.netCode) == GSN64_2_0 ||
				           (NetCode)(packet.netCode) == HCGSN64_1_0 ||
				           (NetCode)(packet.netCode) == MCGSN64_1_0) {
					nChannels = 64;
				} else if ((NetCode)(packet.netCode) == GSN128_2_0 ||
				           (NetCode)(packet.netCode) == HCGSN128_1_0 ||
				           (NetCode)(packet.netCode) == MCGSN128_1_0) {
					nChannels = 128;
				} else if ((NetCode)(packet.netCode) == GSN256_2_0 ||
				           (NetCode)(packet.netCode) == HCGSN256_1_0 ||
				           (NetCode)(packet.netCode) == MCGSN256_1_0) {
					nChannels = 256;
				}

				emit this->channelCountUpdated(nChannels);
				emit this->channelCountEnabled(false);

				// create streaminfo
				std::string streamname = "EGI NetAmp " + std::to_string(header.ampID);
				lsl::stream_info info(streamname, "EEG", nChannels,
				                      this->ui->sampleRateComboBox->currentText().toInt(),
				                      lsl::cf_float32,
				                      streamname + "at_" + ui->serverAddress->text().toStdString());

				// append some meta-data
				info.desc()
				    .append_child("acquisition")
				    .append_child_value("manufacturer", "Philips Neuro")
				    .append_child_value("model", "NetAmp")
				    .append_child_value("application", "AmpServer")
				    .append_child_value("precision", "24");

				// make a new outlet (we transmit at least every samples_per_chunk
				// samples)
				outlet = std::make_unique<lsl::stream_outlet>(info, samples_per_chunk);
			}
			if (packet.packetCounter != 0 && packet.packetCounter != lastPacketCounter + 1 &&
			    packet.packetCounter != lastPacketCounter && lastPacketCounter != 0) {
				emit this->appendStatusMessage(
				    "Packet(s) Dropped: " +
				    QString(std::to_string(packet.packetCounter - lastPacketCounter).c_str()));
			} else if (firstPacketReceived && packet.packetCounter == lastPacketCounter) {

				// For sample rates < 1000 samples per second, the amplifier duplicates packets
				// such that the _packet_ rate is always 1000. For example, at 250 s/s, each
				// packet is sent 4 time, while at 500 s/s each packet is sent twice. This is
				// done for legacy compatibility reasons. If this packets sampleCounter is the
				// same as the most recently received sampleCounter, we ignore the packet, as it
				// is a duplicate.
				continue;
			}
            
            
            if (lastPacketCounter == 0 && !firstPacketReceived) {
                emit this->appendStatusMessage("Stream Started...\n");
            }
            
            if (lastPacketCounter == 0) {
                firstPacketReceived = true;
            }

			// Track the last packetCounter. This allows us to determine if packets have been lost
			// or if a duplicate is received.
			lastPacketCounter = packet.packetCounter;

			// Because duplicate packets might have been received, we update the unique packet
			// counter here, after first checking for duplicates.
			uniquePackets++;

			if (lastTimeStamp_ != 0 &&
			    (packet.packetCounter %
			     (this->ui->sampleRateComboBox->currentText().toInt() / 2)) == 0) {
				lastTimeStamp_ = packet.timeStamp;
				lastPacketCounterWithTimeStamp_ = packet.packetCounter;

			} else if (lastTimeStamp_ == 0) {

				// For the first packet received, we output the timestamp.
				emit this->appendStatusMessage("Time Stamp: " +
				                               QString(std::to_string(packet.timeStamp).c_str()));
				lastTimeStamp_ = packet.timeStamp;
				lastPacketCounterWithTimeStamp_ = packet.packetCounter;
			}

			// Not necessary to convert endianness, as PacketFormat2 is
			// little endian natively.
			for (int channelNumber = 0; channelNumber < nChannels; channelNumber++) {
				samples.push_back(static_cast<float>(packet.eegData[channelNumber]) *
				                  this->scalingFactor_);
			}
			// push it into LSL
			outlet->push_sample(samples);
		}
		if (packetsReceived != nullptr) { (*packetsReceived) += uniquePackets; }
	}
	if (!dataStream_.good() && !stop_flag) { emit this->error("The stream was lost."); }
	emit this->enableUI();
}

void EGIAmpWindow::read_packet_format_1(int numChannels, int* packetsReceived) {

	// PacketFormat1 does not include a packetCounter, and so it is not possible to detect
	// dropped packets or to eliminate duplicates. We will compute an estimated sample rate
	// by taking the number of packets received over a ~0.5s window, dividing by a duplication
	// factor (the amplifier duplicates packets when sampling < 1k s/s, so the sample rate will
	// never be less than 1000 if we don't account for this), then dividing by the duration. This is
	// less accurate than the method implemented for PacketFormat2, but should give a rough estimate
	// of the sample rate.

	// enter transmission loop
	unsigned bytes = 0;
	clock_t startTime = std::clock();
	bool firstPacketReceived = false;
	std::shared_ptr<lsl::stream_outlet> outlet;
	int nChannels = numChannels;
    dataStream_.clear();
	while (dataStream_.good() && !stop_flag) {

		AmpDataPacketHeader header;
		dataStream_.read((char*)&header, sizeof(header));

		header.ampID = boost::endian::big_to_native(header.ampID);
		header.length = boost::endian::big_to_native(header.length);

		// read and transmit the chunk sample-by-sample
		int nSamples = header.length / sizeof(PacketFormat1);

		for (int s = 0; s < nSamples && dataStream_.good(); s++) {
			// reserve memory
			std::vector<float> sample;
			PacketFormat1 packet;

            dataStream_.clear();
            dataStream_.expires_from_now(1s);
            
			// read the data into the sample
			dataStream_.read((char*)&packet, sizeof(PacketFormat1));
			if (!firstPacketReceived) {
                firstPacketReceived = true;
                emit this->appendStatusMessage("Stream Started...\n");

				uint8_t* headerAsByteArray = reinterpret_cast<uint8_t*>(packet.header);
				uint8_t netCode = ((uint8_t)headerAsByteArray[26] & 0x78) >> 3;

				if (netCode == HCGSN32_1_0 || netCode == MCGSN32_1_0) {
					nChannels = 32;
				} else if (netCode == GSN64_2_0 || netCode == HCGSN64_1_0 ||
				           netCode == MCGSN64_1_0) {
					nChannels = 64;
				} else if (netCode == GSN128_2_0 || netCode == HCGSN128_1_0 ||
				           netCode == MCGSN128_1_0) {
					nChannels = 128;
				} else if (netCode == GSN256_2_0 || netCode == HCGSN256_1_0 ||
				           netCode == MCGSN256_1_0) {
					nChannels = 256;
				}

				emit this->channelCountUpdated(nChannels);
				emit this->channelCountEnabled(false);

				// create streaminfo
				std::string streamname = "EGI NetAmp " + std::to_string(header.ampID);
				lsl::stream_info info(streamname, "EEG", nChannels,
				                      this->ui->sampleRateComboBox->currentText().toInt(),
				                      lsl::cf_float32,
				                      streamname + "at_" + ui->serverAddress->text().toStdString());

				// append some meta-data
				info.desc()
				    .append_child("acquisition")
				    .append_child_value("manufacturer", "Philips Neuro")
				    .append_child_value("model", "NetAmp")
				    .append_child_value("application", "AmpServer")
				    .append_child_value("precision", "24");

				// make a new outlet (we transmit at least every samples_per_chunk
				// samples)
				outlet = std::shared_ptr<lsl::stream_outlet>(
				    new lsl::stream_outlet(info, samples_per_chunk));
			}

			// do endianness conversion
			for (int i = 0; i < nChannels; i++) {
				sample.push_back(packet.eeg[i]);
                boost::endian::big_to_native_inplace(*((uint32_t*)&sample[i]));
			}
			// push it into LSL
			outlet->push_sample(sample);
		}
		if (packetsReceived != nullptr) {
			(*packetsReceived) += nSamples;
		}
	}
	if (!dataStream_.good() && !stop_flag) { emit this->error("The stream was lost."); }
	emit this->enableUI();
}
void EGIAmpWindow::halt_ampserver(int id) {
	emit this->appendStatusMessage("Stream Stopping...\n");
	try {
		this->sendDatastreamCommand("cmd_StopListeningToAmp", std::to_string(id), "0", "0");
		this->sendCommand("cmd_Stop", std::to_string(id), "0", "0");
		this->sendCommand("cmd_SetPower", std::to_string(id), "0", "0");
	} catch (...) {}
	stop_flag = false;
}

EGIAmpWindow::~EGIAmpWindow() { delete ui; }
