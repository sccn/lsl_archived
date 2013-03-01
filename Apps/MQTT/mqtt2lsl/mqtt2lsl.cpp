#include "liblsl/lsl_cpp.h"
#include "libmosquitto/mosquitto.h"
#include "libmosquitto/mosquittopp.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;
using namespace lsl;
using namespace mosqpp;

/// This class translates messages on an MQTT topic at some host into a local LSL stream.
class mqtt2lsl: public mosquittopp {
	public:
		mqtt2lsl(const stream_info &info, const string &mqtt_topic, const string &broker_host, int broker_port=1883, int keepalive=60): mosquittopp(NULL), info_(info), outlet_(NULL), mqtt_topic_(mqtt_topic), broker_host_(broker_host), broker_port_(broker_port), keepalive_(keepalive) { 
			// first try to connect to broker
			connect(broker_host_.c_str(), broker_port_, keepalive_);
			// start the mqtt main loop
			loop_start();
		}

		~mqtt2lsl() {
			if (outlet_)
				delete outlet_;
			loop_stop(true);
		}

		void on_connect(int rc) {
			if (rc == MOSQ_ERR_SUCCESS) {
				// connection to broker successful: subscribe to the topic
				subscribe(NULL, mqtt_topic_.c_str());
				cout << "Connected to " << broker_host_ << ":" << broker_port_ << " with result code " << rc << ".\n" << endl;
			} else {
				// could not connect to broker
				stringstream err; err << "Error: connection to " << broker_host_ << ":" << broker_port_ << " failed with result code " << rc << ".\n" << endl;
				cout << err.rdbuf() << endl;
				throw std::exception(err.str().c_str());
			}
		}

		void on_subscribe(int mid, int qos_count, const int *granted_qos) {
			// we're subscribed to the topic: create a new outlet to publish the data to LSL
			outlet_ = new stream_outlet(info_);
			cout << "Subscribed to topic " << mqtt_topic_ << ", seen in LSL as name=" << info_.name() << ", type=" << info_.type() << "\n" << endl;
		}

		void on_message(const struct mosquitto_message *message) {
			if (message->payloadlen != info_.sample_bytes()) {
				// incompatible data type
				stringstream err; err << "Error: mqtt message payload size (" << message->payloadlen << ") does not match the specified data size (" << info_.sample_bytes() << ").\n" << endl;
				cout << err.rdbuf() << endl;
				throw std::exception(err.str().c_str());
			} else {
				// success: forward sample to LSL
				outlet_->push_numeric_raw(message->payload);
			}
		}

private:
	// lsl specific information
	stream_info info_;		// stream information object
	stream_outlet* outlet_;	// the outlet over which we send messages

	// mqtt specific information
	string mqtt_topic_;		// the topic that we subscribe to
	string broker_host_;	// hostname/ip of the broker
	int broker_port_;		// port of the broker
	int keepalive_;			// interval between keep-alive messages (in seconds)
};


int main(int argc, char* argv[]) {
	if (argc != 10) {
		cout << "Usage: " << endl;
		cout << "  mqtt2lsl Name Type Rate NumChans Format UID Topic Host Port" << endl;
		cout << endl;
		cout << "  === LSL parameters ===" << endl;
		cout << "  Name = human-readable name of the stream in LSL" << endl;
		cout << "  Type = content type of the stream (e.g., MoCap or EEG)" << endl;
		cout << "  Rate = nominal samplingrate in Hz or 0 if irregular" << endl;
		cout << "  NumChans = number of channels per sample" << endl;
		cout << "  Format = numeric value format, integer" << endl;
		cout << "           (float32=1, double64=2, int32=4, int16=5, int8=6)" << endl;
		cout << "  UID = unique identifier of the data source, if available" << endl;
		cout << "        (or arbitrary, e.g., test123)" << endl;
		cout << "  === MQTT parameters ===" << endl;
		cout << "  Topic = MQTT topic to subscribe to" << endl;
		cout << "  Host = hostname or IP address of the broker" << endl;
		cout << "  Port = port of the broker" << endl;
		cout << endl;
		cout << "Example:" << endl;
		cout << "  mqtt2lsl test1 MoCap 0 6 1 xxx sensor/data 192.168.1.1 1883" << endl;
	} else {	 
		// mqtt initialization
		lib_init();
		try {
			// create a new connector			
			mqtt2lsl link(stream_info(argv[1],argv[2],atoi(argv[4]),atof(argv[3]),(channel_format_t)atoi(argv[5]),argv[6]), argv[7], argv[8],atoi(argv[9]));

			// let it run until the user wants to quit
			cout << "Press [Enter] to stop the program." << endl;
			cin.get();
		} catch(std::exception &e) {
			cout << "Error: " << e.what() << endl;
		}
		// mqtt teardown
		lib_cleanup();
		cout << "done." << endl;
	}
	return 0;
}
