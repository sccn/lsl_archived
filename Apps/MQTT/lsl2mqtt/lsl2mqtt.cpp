#include "liblsl/lsl_cpp.h"
#include "libmosquitto/mosquitto.h"
#include "libmosquitto/mosquittopp.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;
using namespace lsl;
using namespace mosqpp;

/// This class translates data on a given LSL stream into MQTT messages
class lsl2mqtt: public mosquittopp {
	public:
		lsl2mqtt(const string &query, const string &mqtt_topic, int qos, const string &broker_host, int broker_port=1883, int keepalive=60): mosquittopp(NULL), inlet_(NULL), payload_(NULL), mqtt_topic_(mqtt_topic), qos_(qos), broker_host_(broker_host), broker_port_(broker_port), keepalive_(keepalive){ 
			cout << "Looking for a stream that has " << query << "..." << endl;
			// resolve at least one stream that matches the query
			vector<stream_info> results = resolve_stream(query);
			// create an inlet to read from it
			inlet_ = new lsl::stream_inlet(results[0]);
			cout << "Found a stream (name=" << inlet_->info().name() << ")." << endl;
			cout << "Connecting to broker at " << broker_host << ":" << broker_port << " ..." << endl;
			// reserve buffer space to hold a sample
			payloadlen_ = inlet_->info().sample_bytes();
			payload_ = new char[payloadlen_];
			// try to connect to broker
			connect(broker_host_.c_str(), broker_port_, keepalive_);
			// start the mqtt main loop
			loop_start();
		}

		~lsl2mqtt() {
			if (inlet_)
				delete inlet_;
			if (payload_)
				delete payload_;
			loop_stop(true);
		}

		void on_connect(int rc) {
			if (rc != MOSQ_ERR_SUCCESS) {
				// could not connect to broker
				stringstream err; err << "Error: connection to " << broker_host_ << ":" << broker_port_ << " failed with result code " << rc << ".\n" << endl;
				cout << err.rdbuf();
				throw std::exception(err.str().c_str());
			} else {
				cout << "connected successfully to broker." << endl;
			}
		}

		void tick() {
			// try to get a new sample from LSL
			inlet_->pull_numeric_raw(payload_,payloadlen_);
			// publish it to MQTT
			publish(NULL,mqtt_topic_.c_str(),payloadlen_,payload_,qos_);
		}

private:
	// lsl-specific information
	stream_inlet* inlet_;	// the outlet over which we send messages

	// mqtt-specific information
	string mqtt_topic_;		// the topic that we subscribe to
	int qos_;				// quality of service level
	int payloadlen_;		// length of the payload
	void *payload_;			// holds our payload
	string broker_host_;	// hostname/ip of the broker
	int broker_port_;		// port of the broker
	int keepalive_;			// interval between keep-alive messages (in seconds)
};


int main(int argc, char* argv[]) {
	if (argc != 6) {
		cout << "Usage: " << endl;
		cout << "  lsl2mqtt Query Topic QoS Host Port" << endl;
		cout << endl;
		cout << "  === LSL parameters ===" << endl;
		cout << "  Query = quoted query string to find a stream" << endl; 
		cout << "          (e.g., \"name='BioSemi'\")" << endl;
		cout << "  === MQTT parameters ===" << endl;
		cout << "  Topic = MQTT topic under which to publish the data" << endl;
		cout << "  QoS = quality of service level" << endl;
		cout << "        (0=once,1=at least once,2=exactly once)" << endl;
		cout << "  Host = hostname or IP address of the broker" << endl;
		cout << "  Port = port of the broker" << endl;
		cout << endl;
		cout << "Example:" << endl;
		cout << "  lsl2mqtt \"type='EEG'\" sccn.ucsd.edu/mobi/EEG 2 192.168.1.1 1883" << endl;
	} else {	 
		// mqtt initialization
		lib_init();
		try {
			// create a new connector			
			lsl2mqtt link(argv[1], argv[2], atoi(argv[3]), argv[4], atoi(argv[5]));
			// run the main loop
			while (true)
				link.tick();
		} catch(std::exception &e) {
			cout << "Error: " << e.what() << endl;
		}
		// mqtt teardown
		lib_cleanup();
		cout << "done." << endl;
	}
	return 0;
}
