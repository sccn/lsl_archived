#include "../../include/lsl_cpp.h"
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/atomic.hpp>
#include <iostream>
#include <time.h>
#include <stdlib.h>
using namespace std;

// key stress constants
int max_outlets = 15; // was 20
int max_inlets = 20; // was 30
int min_chunk_len_ms = 1;
int max_chunk_len_ms = 100;
int max_inlet_poll_interval_ms = 100;
int inlet_max_failure_interval_ms = 2000;
int outlet_max_failure_interval_ms = 2000;
int inlet_min_failure_interval_ms = 1;
int outlet_min_failure_interval_ms = 1;
int max_outlet_duration = 10;
int max_inlet_duration = 10;
double spawn_inlet_interval = 0.5;
double spawn_outlet_interval = 0.5;
int max_srate = 1000;
int max_channels = 10;
int max_buffered = 6;

// misc parameters
int max_chunk_oversize_factor = 5;
int max_samples = 10000000;

boost::atomic<int> num_outlets(0);
boost::atomic<int> num_inlets(0);

// some random names, types and formats
lsl::channel_format_t fmts[] = {lsl::cf_int8,lsl::cf_int16,lsl::cf_int32,lsl::cf_float32,lsl::cf_double64,lsl::cf_string};
const char *names[] = {"Test1","Test2","Test3","Test4"};
const char *types[] = {"EEG","Audio","MoCap"};

// set to true by the main program when we're ready to quit
bool stop_inlet = false, stop_outlet = false, start_outlet = false; 


// initialize a sample with data
template<class T>
void init_sample(int numchan, vector<T> &sample) {
	sample.resize(numchan);
	for (int c=0;c<numchan;c++)
		sample[c] = (T)17.3;
}


// run an outlet for some time (optionally with sporadic interruptions in between)
void run_outlet(const double duration_=0.0, const string name_=string(), const string type_=string(), const int numchan_=0, const lsl::channel_format_t fmt_=lsl::cf_undefined, const double srate_=0.0, const double seconds_between_failures_=0.0, const int chunk_len_=0) {
	num_outlets.fetch_add(1);
	std::ostringstream s; s << boost::this_thread::get_id();
	srand((unsigned)boost::hash<string>()(s.str()));
	try {
		// choose random parameters if desired
		double duration = (duration_ == 0.0) ? 1.0+rand()%(max_outlet_duration-1) : duration_;
		string name = name_.empty() ? names[rand()%(sizeof(names)/sizeof(names[0]))] : name_;
		string type = type_.empty() ? types[rand()%(sizeof(types)/sizeof(types[0]))] : type_;
		int numchan = (numchan_ == 0) ? 1+(rand()%(max_channels-1)) : numchan_;
		double srate = (srate_ == 0.0) ? 1.0 + (rand()%(max_srate-1)) : srate_;
		lsl::channel_format_t fmt = (fmt_ == lsl::cf_undefined) ? fmts[rand()%6] : fmt_;
		double seconds_between_failures = (seconds_between_failures_ == 0.0) ? (inlet_min_failure_interval_ms+rand()%outlet_max_failure_interval_ms)/1000.0 : seconds_between_failures_;
		int chunk_len = (chunk_len_ == 0) ? std::max(min_chunk_len_ms,(rand()%max_chunk_len_ms)) : chunk_len_;

		// create a new streaminfo
		lsl::stream_info info(name,type,numchan,srate,fmt,boost::uuids::to_string(boost::uuids::random_generator()()));

		// initialize data to send
		vector<float> chunk;
		init_sample((int)(numchan*floor(chunk_len*srate/1000*max_chunk_oversize_factor)),chunk);

		// and run...
		for (double endtime = lsl::local_clock()+duration;lsl::local_clock()<endtime;) {
			// run a single execution of the outlet
			{
				cout << "new outlet(" << name << "," << type << "," << numchan << "," << fmt << "," << srate << ")...";
				lsl::stream_outlet outlet(info,0,max_buffered);
				cout << "done." << endl;
				// send in bursts
				double now, start_time = lsl::local_clock(), fail_at=start_time+seconds_between_failures;
				for (int target,diff,written=0;written<max_samples && !stop_outlet;written+=diff) {
					boost::this_thread::sleep(boost::posix_time::milliseconds(chunk_len));
					now = lsl::local_clock();
					if (now>fail_at)
						break;
					target = (int)floor((now-start_time)*srate);
					int num_elements = (int)std::min((std::size_t)((target-written)*numchan),chunk.size());
					if (num_elements)
						outlet.push_chunk_multiplexed(&chunk[0],num_elements);
					diff = num_elements/numchan;
				}
			}
			cout << "del outlet(" << name << "," << type << "," << numchan << "," << fmt << "," << srate << ")" << endl;
			// downtime
			boost::this_thread::sleep(boost::posix_time::millisec(100*(rand()%50)));
		}
	} catch(std::exception &e) {
		std::cerr << "ERROR during run_outlet() Stress-test function: " <<	e.what() << std::endl;
	}
	num_outlets.fetch_sub(1);
}

// run an inlet for some time (optionally with sporadic interruptions in between)
void run_inlet(const double duration_=0.0, const string name_=string(), const string type_=string(), const int in_chunks_=-1, const int request_info_=-1, const int request_time_=-1, const double seconds_between_failures_=0.0) {
	num_inlets.fetch_add(1);
	std::ostringstream s; s << boost::this_thread::get_id();
	srand((unsigned)boost::hash<string>()(s.str()));
	try {
		// choose random parameters if desired		
		double duration = (duration_ == 0.0) ? 1.0+rand()%(max_outlet_duration-1) : duration_;
		string name = name_.empty() ? names[rand()%(sizeof(names)/sizeof(names[0]))] : name_;
		string type = type_.empty() ? types[rand()%(sizeof(types)/sizeof(types[0]))] : type_;
		int request_info = (request_info_==-1) ? rand()%3 == 0 : request_info_;
		int request_time = (request_time_==-1) ? rand()%3 == 0 : request_time_;
		double seconds_between_failures = (seconds_between_failures_ == 0.0) ? (inlet_min_failure_interval_ms+rand()%outlet_max_failure_interval_ms)/1000.0 : seconds_between_failures_;

		// resolve by type...
		vector<lsl::stream_info> results = lsl::resolve_stream("type",type,1,5);
		if (results.empty())
			throw lsl::lost_error("No stream found.");
		lsl::stream_info result = results[rand()%results.size()];
		vector<float> chunk;		

		// and run...
		double t=0.0;
		for (double endtime = lsl::local_clock()+duration;lsl::local_clock()<endtime;) {
			// run a single execution of the inlet
			{
				cout << "new inlet(" << name << "," << type << ")...";
				lsl::stream_inlet inlet(result,max_buffered);
				cout << "done." << endl;
				int numchans = inlet.info().channel_count();
				init_sample(numchans*(int)ceil(max_chunk_len_ms*result.nominal_srate()/1000*max_chunk_oversize_factor),chunk);
				if (request_info) {
					cout << "  info = " << inlet.info(1.0).name() << endl;
				}
				for (double fail_at = lsl::local_clock()+seconds_between_failures;lsl::local_clock()<fail_at;) {
					boost::this_thread::sleep(boost::posix_time::milliseconds(1+rand()%max_inlet_poll_interval_ms));
					inlet.pull_chunk_multiplexed(&chunk[0],NULL,chunk.size(),0);
					if (request_time)
						t = inlet.time_correction(1.0);
				}
			}
			cout << "del inlet(" << name << "," << type << ")" << endl;
			if (request_time)
				cout << "  tcorr = " << t << endl;
			// downtime
			boost::this_thread::sleep(boost::posix_time::millisec(100*(rand()%50)));
		}
	} 
	catch(lsl::timeout_error &) { std::cerr << "Timeout exceeded; stopping inlet." << std::endl; }
	catch(lsl::lost_error &) { std::cerr << "Found no matching outlet; stopping inlet." << std::endl; }
	catch(std::exception &e) {
		std::cerr << "ERROR during run_inlet() Stress-test function: " <<	e.what() << std::endl;
	}
	num_inlets.fetch_sub(1);
}

void random_inlets(double spawn_every=0.0, double duration=0.0, string name=string(), string type=string(), int in_chunks=-1, int request_info=-1, int request_time=-1, double seconds_between_failures=0.0) {
	if (spawn_every == 0.0)
		spawn_every = spawn_inlet_interval;
	while(true) {
        try {
			if (num_inlets.load() < max_outlets)
				boost::thread tmp(&run_inlet,duration,name,type,in_chunks,request_info,request_time,seconds_between_failures);
		} catch(std::exception &e) {
			std::cerr << "Could not spawn a new inlet thread: " << e.what() << std::endl;
        }		
		boost::this_thread::sleep(boost::posix_time::millisec((boost::int64_t)(1000*spawn_every)));
	}
}

void random_outlets(double spawn_every=0.0, double duration=0.0, string name=string(), string type=string(), int numchan=0, lsl::channel_format_t fmt=lsl::cf_undefined, double srate=0.0, double seconds_between_failures=0.0, int chunk_len=0) {
	if (spawn_every == 0.0)
		spawn_every = spawn_outlet_interval;
	while(true) {
        try {
			if (num_outlets.load() < max_inlets)
				boost::thread tmp(&run_outlet,duration,name,type,numchan,fmt,srate,seconds_between_failures,chunk_len);
		} catch(std::exception &e) {
			std::cerr << "Could not spawn a new outlet thread: " << e.what() << std::endl;
        }		
		boost::this_thread::sleep(boost::posix_time::millisec((boost::int64_t)(1000*spawn_every)));
	}
}

int main(int argc, char* argv[]) {
	srand((unsigned)time(NULL));
	cout << "This stress test program puts heavy load on network equipment," << endl;
	cout << "particularly when multiple instances run on the same network." << endl;
	cout << "We recommend to not run this software on a corporate or campus" << endl;
	cout << "network since it generates erratic heavy traffic that can " << endl;
	cout << "alert network operators and/or may crash unreliable equipment." << endl << endl; 
	cout << "Are you sure you want to continue? [y/n]" << endl;
	if ((argc>1 && string(argv[1]) == "-f") || tolower(cin.get()) == 'y') {
		boost::thread outlets(&random_outlets,0.0,0.0,string(),string(),0,lsl::cf_undefined,0.0,0.0,0);
		boost::thread inlets(&random_inlets,0.0,0.0,string(),string(),-1,-1,-1,0.0);
		cout << "Press ENTER to exit. " << endl; cin.get();cin.get();
	}
	return 0;
}
