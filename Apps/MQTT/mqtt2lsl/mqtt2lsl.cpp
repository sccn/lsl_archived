#include "liblsl/lsl_cpp.h"
#include "libmosquitto/mosquitto.h"
#include "libmosquitto/mosquittopp.h"
#include <iostream>
#include <string>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace lsl;
using namespace mosqpp;
// for timestamp parsing
using namespace boost;
using namespace boost::posix_time;


/// This class translates messages on an MQTT topic at some host into a local LSL stream.
class mqtt2lsl: public mosquittopp {
	public:
		mqtt2lsl(const stream_info &info, const string &mqtt_topic, const string &broker_host, int broker_port=1883, const string &msgformat="binary", int keepalive=60): mosquittopp(NULL), info_(info), outlet_(NULL), mqtt_topic_(mqtt_topic), broker_host_(broker_host), broker_port_(broker_port), msgformat_(msgformat), got_data_(false), keepalive_(keepalive) { 
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
			if (msgformat_ == "binary") {
				if (message->payloadlen != info_.sample_bytes()) {
					// incompatible data type
					stringstream err; err << "Error: mqtt message payload size (" << message->payloadlen << ") does not match the specified data size (" << info_.sample_bytes() << ").\n" << endl;
					cout << err.rdbuf() << endl;
					throw std::exception(err.str().c_str());
				} else {
					// forward binary sample to LSL
					outlet_->push_numeric_raw(message->payload);
				}
			}
			if (msgformat_ == "text") {
				// parse string
				vector<double> values;
				stringstream ss(string((const char*)message->payload,message->payloadlen));
				for (int k=0;k<info_.channel_count();k++) {
					double v; ss >> v;
					values.push_back(v);
				}
				// forward sample to LSL
				outlet_->push_sample(values);
			}
			if (msgformat_ == "textbatch") {
				// parse string
				vector<double> values;
				string msg = string((const char*)message->payload,message->payloadlen);
				stringstream ss(msg);
				// for each sample...
				while (!ss.eof()) {
					// read and ignore time stamp
					string timestamp; ss >> timestamp;
					if (timestamp.size() < 10)
						break;
					//timestamp = timestamp.substr(6);
					vector<string> parts; boost::split(parts,timestamp,boost::is_any_of(":"));
					if (parts.size() >= 4) {
						double time;
						// try {
						// 	time = lexical_cast<double>(parts[0])*3600 + lexical_cast<double>(parts[1])*60 + lexical_cast<double>(parts[2]) + lexical_cast<double>(string("0.")+=parts[3]);
						// } catch(...) { }
						time = lsl::local_clock();
						// read the remaining values
						values.clear();
						for (int k=0;k<info_.channel_count();k++) {
							double v; ss >> v;
							values.push_back(v);
						}
						// forward sample to LSL
						outlet_->push_sample(values,time);
					}
				}
			}
			if (!got_data_) {
				cout << "getting data..." << endl;
				got_data_ = true;
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
	string msgformat_;		// format of the MQTT messages (e.g., binary, text or textbatch)
	bool got_data_;			// whether we've seen data already
	int keepalive_;			// interval between keep-alive messages (in seconds)
};


int main(int argc, char* argv[]) {
	if (argc != 11) {
		cout << "Usage: " << endl;
		cout << "  mqtt2lsl Name Type Rate NumChans Format UID Topic Host Port MsgFormat" << endl;
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
		cout << "  MsgFormat = message format, can be binary, text, or textbatch" << endl;
		cout << endl;
		cout << "Example:" << endl;
		cout << "  mqtt2lsl test1 MoCap 0 6 1 xxx sensor/data 192.168.1.1 1883 binary" << endl;
	} else {	 
		// mqtt initialization
		lib_init();
		try {
			// create a new connector			
			mqtt2lsl link(stream_info(argv[1],argv[2],atoi(argv[4]),atof(argv[3]),(channel_format_t)atoi(argv[5]),argv[6]), argv[7], argv[8],atoi(argv[9]),string(argv[10]));

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
