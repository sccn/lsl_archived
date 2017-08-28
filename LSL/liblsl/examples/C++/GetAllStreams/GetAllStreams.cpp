#include "../../../include/lsl_cpp.h"
#include <iostream>
using namespace std;

/**
* This example program shows how all streams that are currently present on the lab network can be resolved and displayed.
* This is useful for browsing applications.
*/

int main(int argc, char* argv[]) {
	try {
		cout << "Here is a one-shot resolve of all current streams:" << endl;

		// discover all streams on the network
		vector<lsl::stream_info> results = lsl::resolve_streams();
		
		// display them
		for (unsigned k=0;k<results.size();k++)
			cout << results[k].as_xml() << endl << endl;

		cout << "Press any key to switch to the continuous resolver test: " << endl; cin.get();

		lsl::continuous_resolver r;
		while(true) {
			vector<lsl::stream_info> tmp = r.results();
			for (unsigned k=0;k<tmp.size();k++)
				cout << tmp[k].name() << " ";
			cout << endl;
		}


	} catch(std::exception &e) {
		cerr << "Got an exception: " << e.what() << endl;
	}
	cout << "Press any key to exit. " << endl; cin.get();
	return 0;
}
