#include "../../include/lsl_cpp.h"
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <map>
#include <time.h>
#include <stdlib.h>
using namespace std;

// set to true by the main program when we're ready to quit
bool stop_inlet = false, stop_outlet = false, start_outlet = false; 

// get the current time
double now() { return lsl::local_clock(); }

// initialize a sample with data
template<class T>
vector<T> init_sample(int numchan) {
	vector<T> sample(numchan);
	for (int c=0;c<numchan;c++)
		sample[c] = 17.3;
	return sample;
}

// initialize a sample with data
template<class T>
void init_sample(int numchan, vector<T> &sample) {
	sample.resize(numchan);
	for (int c=0;c<numchan;c++)
		sample[c] = 17.3f;
}

// run an outlet
template<class T>
void run_outlet(string name, string type, int numchan, lsl::channel_format_t fmt, double srate, int chunk_len, int max_samples) {
	try {
		// create a new streaminfo and outlet
		std::ostringstream uid; uid << name << type << numchan << fmt;
		lsl::stream_info info(name,type,numchan,srate,fmt,uid.str());
		lsl::stream_outlet outlet(info);
		cout << "outlet started." << endl;

		while (!start_outlet)
			boost::this_thread::sleep(boost::posix_time::milliseconds(1));

		// initialize data to send
		vector<T> sample,chunk;
		init_sample(numchan,sample);
		init_sample((int)(numchan*(chunk_len*srate/1000*5)),chunk);

		// send in bursts
		double start_time = lsl::local_clock();
		for (int target,diff,written=0;written<max_samples && !stop_outlet;written+=diff) {
			boost::this_thread::sleep(boost::posix_time::milliseconds(chunk_len));
			target = (int)floor((lsl::local_clock()-start_time)*srate);
			int num_elements = std::min((std::size_t)((target-written)*numchan),chunk.size());
			outlet.push_chunk_multiplexed(&chunk[0],num_elements);
			diff = num_elements/numchan;
		}
		cout << "outlet finished." << endl;
	}
	catch(boost::thread_interrupted &) {}
	catch(std::exception &e) {
		std::cerr << "ERROR during run_outlet() Stress-test function: " <<	e.what() << std::endl;
	}
}


// run an inlet for some time (optionally with sporadic interruptions in between)
template<class T>
void run_inlet(string type, bool in_chunks, int buffer_len) {
	try {
		vector<vector<T> > chunk; 
		vector<T> sample; 

		// resolve by type and create inlet
		vector<lsl::stream_info> results = lsl::resolve_stream("type",type);
		lsl::stream_inlet inlet(results[0],buffer_len);
		cout << "inlet started." << endl;
		start_outlet = true;

		// variables for data rate testing
		double starttime = lsl::local_clock(), next_display = starttime+1;
		double ts, last_ts;

		// run
		for (int k=0,num_samples=0;!stop_inlet;k++) {
			boost::this_thread::sleep(boost::posix_time::milliseconds(10));
			if (in_chunks) {
				inlet.pull_chunk(chunk);
				num_samples += chunk.size();
			} else {
				while (ts = inlet.pull_sample(sample,0.0)) {
					last_ts = ts;
					num_samples++;
				}
			}
			// display code
			if (k % 50 == 0) {
				double now = lsl::local_clock();
				if (now>next_display) {
					cout << num_samples/(now-starttime) << " samples/sec" << endl; // " samples/sec; latency = " << 1000*(now-last_ts) << " ms." << endl; // NOTE: testing latency requires that data is sent with a burst length of 0
					next_display = now+1;
				}
			}
		}
		cout << "inlet finished." << endl;
	} 
	catch(boost::thread_interrupted &) {}
	catch(std::exception &e) {
		std::cerr << "ERROR during run_inlet() Stress-test function: " <<	e.what() << std::endl;
	}
}

int main(int argc, char* argv[]) {
	double srate = 500000;
	int numchans = 1;
	lsl::channel_format_t format = lsl::cf_int8;
	int burstlen = 10;
	int maxsamples = 10000000;
	int bufferlen = 10;
	if (argc > 1)
		srate = boost::lexical_cast<double>(argv[1]);
	if (argc > 2)
		numchans = boost::lexical_cast<int>(argv[2]);
	if (argc > 3) {
		std::map<std::string,lsl::channel_format_t> m; m["int8"]=lsl::cf_int8;m["int16"]=lsl::cf_int16;m["int32"]=lsl::cf_int32;m["int64"]=lsl::cf_int64;m["float"]=lsl::cf_float32;m["double"]=lsl::cf_double64;m["float32"]=lsl::cf_float32;m["double64"]=lsl::cf_double64; m["string"]=lsl::cf_string;
		format = m[argv[3]];
	}
	if (argc > 4)
		burstlen = boost::lexical_cast<int>(argv[4]);
	if (argc > 5)
		maxsamples = boost::lexical_cast<int>(argv[5]);
	if (argc > 6)
		bufferlen = boost::lexical_cast<int>(argv[6]);

	{
		// fast serial transmission (1-channel char)
		boost::thread outlet(boost::bind(&run_outlet<float>,"FastSerial","Serial",numchans,format,srate,burstlen,maxsamples));
		boost::thread inlet(boost::bind(&run_inlet<float>,"Serial",false,bufferlen));

		// typical audio transmission (44KHz, 2-ch, 16-bit, 512-sample chunks)
		//boost::thread a(boost::bind(&run_outlet<char>,"TypicalAudio","Audio",2,lsl::cf_int16,44100,512,1000000));
		//boost::thread b(boost::bind(&run_inlet<char>,"Audio",true,10));

		// HD video transmission (30Hz, 2-ch, 16-bit, 512-sample chunks)
		//boost::thread outlet(boost::bind(&run_outlet<char>,"HDVideo","Video",640*480*3,lsl::cf_int8,10,0,1000000));
		//boost::thread inlet(boost::bind(&run_inlet<char>,"Video",false,1));

		//boost::thread highchan_outlet_thread(boost::bind(&run_outlet,1000000,"HighChanStream","Video",1920*1080*3,lsl::cf_int8,60,1000000,-1));
		//boost::thread highchan_inlet_thread(boost::bind(&run_inlet,1000000,"HighChanStream","Video",false,false,false,1000000));
		//cout << "Press [Enter] to exit: " << endl; cin.get();

		// take'em down
		outlet.join();
		stop_inlet = true;
		inlet.join();
	}
	cout << "exiting..." << endl;
	return 0;
}

