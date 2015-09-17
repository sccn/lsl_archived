#include "lsl_cpp.h"
#include "recording.h"
#include <iostream>

using namespace std;
using namespace lsl;

int main(int argc, char* argv[]) {
	// resolve all streams
	vector<stream_info> results = resolve_streams();
	
	// create a new recording
	vector<string> watchfor;
	try {
		recording rec("C:\\Recordings\\test.xdf",results,watchfor);
		std::cin.get();
	} 
	catch(boost::thread_interrupted &) {
		std::cout << "The main thread was interrupted." << std::endl;
	}
	catch(std::exception &e) {
		std::cout << "Main thread caught error: " << e.what() << std::endl;
		throw;
	}
	// destroy the recording
	return 0;
}
