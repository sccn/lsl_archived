#include "../../../include/lsl_cpp.h"
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
	try {
		// create a new stream_info and declare some meta-data (in accordance with XDF format)
		lsl::stream_info info("MetaTester","EEG",8,100,lsl::cf_float32,"myuid323457");
		lsl::xml_element chns = info.desc().append_child("channels");
		const char *labels[] = {"C3","C4","Cz","FPz","POz","CPz","O1","O2"};
        for (int k=0;k<8;k++)
            chns.append_child("channel")
                .append_child_value("label", labels[k])
                .append_child_value("unit", "microvolts")
                .append_child_value("type","EEG");
        info.desc().append_child_value("manufacturer","SCCN");
        info.desc().append_child("cap")
            .append_child_value("name","EasyCap")
            .append_child_value("size","54")
            .append_child_value("labelscheme","10-20");

		// create outlet for the stream
		lsl::stream_outlet outlet(info);

		// === the following could run on another computer ===

		// resolve the stream and open an inlet
		std::vector<lsl::stream_info> results = lsl::resolve_stream("name","MetaTester");
		lsl::stream_inlet inlet(results[0]);

        // get the full stream info (including custom meta-data) and dissect it
		lsl::stream_info inf = inlet.info();
        cout << "The stream's XML meta-data is: " << endl;
        cout << inf.as_xml() << endl;
        cout << "The manufacturer is: " << inf.desc().child_value("manufacturer") << endl;
        cout << "The cap circumference is: " << inf.desc().child("cap").child_value("size") << endl;
        cout << "The channel labels are as follows:" << endl;
		lsl::xml_element ch = inf.desc().child("channels").child("channel");
        for (int k=0;k<info.channel_count();k++) {
            cout << "  " << ch.child_value("label") << endl;
            ch = ch.next_sibling();
        }

	} catch(std::exception &e) {
		cerr << "Got an exception: " << e.what() << endl;
	}
	cout << "Press any key to exit. " << endl; cin.get();
	return 0;
}
