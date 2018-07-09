#include <iostream>
#include <lsl_cpp.h>

int main(int argc, char* argv[]) {
	try {
		// create a new stream_info and declare some meta-data (in accordance with XDF format)
		const char* name = argc > 1 ? argv[1] : "MetaTester";
		lsl::stream_info info(name, "EEG", 8, 100, lsl::cf_float32, "myuid323457");
		lsl::xml_element chns = info.desc().append_child("channels");
		const char* labels[] = {"C3", "C4", "Cz", "FPz", "POz", "CPz", "O1", "O2"};
		for (const char* label : labels)
			chns.append_child("channel")
			    .append_child_value("label", label)
			    .append_child_value("unit", "microvolts")
			    .append_child_value("type", "EEG");
		info.desc().append_child_value("manufacturer", "SCCN");
		info.desc()
		    .append_child("cap")
		    .append_child_value("name", "EasyCap")
		    .append_child_value("size", "54")
		    .append_child_value("labelscheme", "10-20");

		// create outlet for the stream
		lsl::stream_outlet outlet(info);

		// === the following could run on another computer ===

		// resolve the stream and open an inlet
		std::vector<lsl::stream_info> results = lsl::resolve_stream("name", name);
		lsl::stream_inlet inlet(results[0]);

		// get the full stream info (including custom meta-data) and dissect it
		lsl::stream_info inf = inlet.info();
		std::cout << "The stream's XML meta-data is: \n"
		          << inf.as_xml()
		          << "\nThe manufacturer is: " << inf.desc().child_value("manufacturer")
		          << "\nThe cap circumference is: " << inf.desc().child("cap").child_value("size")
		          << "\nThe channel labels are as follows:\n";
		lsl::xml_element ch = inf.desc().child("channels").child("channel");
		for (int k = 0; k < info.channel_count(); k++) {
			std::cout << "  " << ch.child_value("label") << std::endl;
			ch = ch.next_sibling();
		}

	} catch (std::exception& e) { std::cerr << "Got an exception: " << e.what() << std::endl; }
	std::cout << "Press any key to exit. " << std::endl;
	std::cin.get();
	return 0;
}
