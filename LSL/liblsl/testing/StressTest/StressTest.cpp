#include "../../include/lsl_cpp.h"
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <time.h>
#include <stdlib.h>
using namespace std;

// some random names, types and formats
lsl::channel_format_t fmts[] = {lsl::cf_int8,lsl::cf_int16,lsl::cf_int32,lsl::cf_float32,lsl::cf_double64};
const char *types[] = {"EEG","Audio","MoCap"};
const char *names[] = {"Test1","Test2"};

// get the current time
double now() { return ((double)clock())/CLOCKS_PER_SEC; }

// generate a random sample
vector<double> rand_sample(int numchan) {
	vector<double> sample(numchan);
	for (int c=0;c<numchan;c++)
		sample[c] = (double)((rand()%1500)/500.0-1.5);
	return sample;
}

// run an outlet for some time (optionally with sporadic interruptions in between)
void run_outlet(double duration=0.0, string name=string(), string type=string(), int numchan=0, lsl::channel_format_t fmt=lsl::cf_undefined, double srate=0.0, double seconds_between_failures=0.0, int chunk_len=0) {
	try {
		// choose random parameters if desired
		srand(time(NULL));
		if (duration == 0.0)
			duration = 1.0+rand()%20;
		if (name.empty())
			name = names[rand()%(sizeof(names)/sizeof(names[0]))];
		if (type.empty())
			type = types[rand()%(sizeof(types)/sizeof(types[0]))];
		if (numchan == 0)
			numchan = 1+(rand()%10);
		if (srate == 0.0)
			srate = 1.0 + (rand()%999);
		if (fmt == lsl::cf_undefined)
			fmt = fmts[rand()%5];
		if (seconds_between_failures == 0.0)
			seconds_between_failures = (rand()%100)/10.0;
		if (chunk_len == 0)
			chunk_len = (rand()%100)-50;
		// create a new streaminfo
		std::ostringstream uid; uid << name << type << numchan << fmt;
		lsl::stream_info info(name,type,numchan,srate,fmt,uid.str());
		// and run...
		for (double endtime = now()+duration;now()<endtime;) {
			// run a single execution of the outlet
			{
				cout << "new outlet(" << name << "," << type << "," << numchan << "," << fmt << "," << srate << ")" << endl;
				lsl::stream_outlet outlet(info);
				for (double failtime = now()+seconds_between_failures;now()<failtime;) {
					if (chunk_len<=0) {
						// send sample by sample
						outlet.push_sample(rand_sample(numchan));
						boost::this_thread::yield();
						boost::this_thread::sleep(boost::posix_time::millisec(1000/srate));
					} else {
						// send in chunks
						vector<vector<double> > tmp;
						for (unsigned s=0;s<chunk_len;s++)
							tmp.push_back(rand_sample(numchan));
						outlet.push_chunk(tmp);
						boost::this_thread::yield();
						boost::this_thread::sleep(boost::posix_time::millisec(chunk_len*1000/srate));
					}
				}
			}
			cout << "del outlet(" << name << "," << type << "," << numchan << "," << fmt << "," << srate << ")" << endl;
			// downtime
			boost::this_thread::sleep(boost::posix_time::millisec(100*(rand()%50)));
		}
	} catch(std::exception &e) {
		std::cerr << "ERROR during run_outlet() Stress-test function: " <<	e.what() << std::endl;
	}
}

void random_outlets(double spawn_every=2.0, double duration=0.0, string name=string(), string type=string(), int numchan=0, lsl::channel_format_t fmt=lsl::cf_undefined, double srate=0.0, double seconds_between_failures=0.0, int chunk_len=0) {
	while(true) {
        try {
            boost::thread tmp(boost::bind(&run_outlet,duration,name,type,numchan,fmt,srate,seconds_between_failures,chunk_len));
		} catch(std::exception &e) {
			std::cerr << "Could not spawn a new outlet thread: " << e.what() << std::endl;
        }
		boost::this_thread::sleep(boost::posix_time::millisec(1000*spawn_every));
	}
}


// run an inlet for some time (optionally with sporadic interruptions in between)
void run_inlet(double duration=0.0, string name=string(), string type=string(), int in_chunks=-1, int request_info=-1, int request_time=-1, double seconds_between_failures=0.0) {
	try {
		double t=0.0;
		// choose random parameters if desired
		srand(time(NULL));
		if (duration == 0.0)
			duration = 1.0+rand()%20;
		if (name.empty())
			name = names[rand()%(sizeof(names)/sizeof(names[0]))];
		if (type.empty())
			type = types[rand()%(sizeof(types)/sizeof(types[0]))];
		if (in_chunks==-1)
			in_chunks = rand()%2 == 0;
		if (request_info==-1)
			request_info = rand()%3 == 0;
		if (request_time==-1)
			request_time = rand()%3 == 0;
		if (seconds_between_failures == 0.0)
			seconds_between_failures = (rand()%100)/10.0;
		// resolve by type...
		vector<lsl::stream_info> results = lsl::resolve_stream("type",type);
		// and run...
		for (double endtime = now()+duration;now()<endtime;) {
			// run a single execution of the inlet
			{
				cout << "new inlet(" << name << "," << type << ")" << endl;
				lsl::stream_inlet inlet(results[0]);
				if (request_info)
					cout << "  info = " << inlet.info().name() << endl;
				for (double failtime = now()+seconds_between_failures;now()<failtime;) {
					if (in_chunks) {
						vector<vector<double> > chunk; 
						inlet.pull_chunk(chunk);
					} else {
						vector<double> sample; 
						inlet.pull_sample(sample);
					}
					if (request_time)
						t = inlet.time_correction();
				}
			}
			cout << "del inlet(" << name << "," << type << ")" << endl;
			cout << "  tcorr = " << t << endl;
			// downtime
			boost::this_thread::sleep(boost::posix_time::millisec(100*(rand()%50)));
		}
	} catch(std::exception &e) {
		std::cerr << "ERROR during run_inlet() Stress-test function: " <<	e.what() << std::endl;
	}
}

void random_inlets(double spawn_every=2.0, double duration=0.0, string name=string(), string type=string(), int in_chunks=-1, int request_info=-1, int request_time=-1, double seconds_between_failures=0.0) {
	while(true) {
        try {
            boost::thread tmp(boost::bind(&run_inlet,duration,name,type,in_chunks,request_info,request_time,seconds_between_failures));
		} catch(std::exception &e) {
			std::cerr << "Could not spawn a new inlet thread: " << e.what() << std::endl;
        }
        boost::this_thread::sleep(boost::posix_time::millisec(1000*spawn_every));
	}
}

int main(int argc, char* argv[]) {
	boost::thread outlets(boost::bind(&random_outlets,3.0,0.0,"test","",5,lsl::cf_float32,0.0,0.0,0));
	boost::thread inlets(boost::bind(&random_inlets,3.0,0.0,"test2","",-1,-1,-1,0.0));
	cout << "Press any key to exit. " << endl; cin.get();
	return 0;
}

