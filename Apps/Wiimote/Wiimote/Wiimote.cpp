#include <lsl_cpp.h>
#include <boost/date_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <vector>
#include "wiiuse.h"

using namespace std;
using namespace lsl;

const int max_wiimotes = 4;
const bool enable_sensors = true;

const char* const expansion_names[] = {"None","Nunchuk","Classic","GuitarHero3","WiiBoard","Unknown","Unknown","Unknown"};

/*
TODO:
- reduce orient and accel thresholds to scientific precision
*/


int main(int argc, char* argv[]) {
	wiimote** wiimotes;
	int num_found=0, num_connected=0;
	try {
		// initialize an array of wiimote objects.
		wiimotes = wiiuse_init(max_wiimotes);
		wiiuse_set_bluetooth_stack(wiimotes, max_wiimotes,WIIUSE_STACK_MS);

		// retrieve wiimotes that are in discovery mode
		while (!(num_found = wiiuse_find(wiimotes, max_wiimotes,5))) {
			cout << endl << "No Wiimotes have been found." << endl << endl <<
				"To connect your Wiimote, right-click the bluetooth" << endl <<
				"icon in your Windows taskbar and click 'Add a Device'." << endl <<
				"Now press buttons 1 & 2 on your Wiimote to make it" << endl <<
				"discoverable for 20 seconds. It should show up in your" << endl <<
				"device list as Nintendo RVL-CNT-01 or similar." << endl <<
				"Add this device and select 'pair this device without" << endl <<
				"using a code'." << endl <<
				"Press [Enter] when you are ready or Ctrl-C " << endl <<
				"to terminate the program." << endl;
			cin.get();
		}
		cout << "Found " << num_found << " wiimotes." << endl;

		// connect one or more of the wiimotes
		if (!(num_connected = wiiuse_connect(wiimotes, max_wiimotes)))
			throw runtime_error("Failed to connect to any Wiimote.");

		// indicate that we got connected successfully by a brief rumble
		for (int k=0;k<num_connected;k++)
			wiiuse_rumble(wiimotes[k],1);		
		boost::this_thread::sleep(boost::posix_time::millisec(500));		
		for (int k=0;k<num_connected;k++)
			wiiuse_rumble(wiimotes[k],0);

		if (num_connected < num_found)
			cout << "Warning: could not connect to all Wiimotes that were found." << endl;
		else
			cout << "Successfully connected to all Wiimotes." << endl;

		// check battery status and warn if necessary
		for (int k=0;k<num_connected;k++) {
			wiiuse_status(wiimotes[k]);
			if (wiimotes[k]->battery_level && (wiimotes[k]->battery_level < 0.2))
				cout << "Warning: The battery level of Wii #" << k+1 << " is at " << (int)(100*wiimotes[k]->battery_level) << " percent." << endl;
		}

		// open an outlet for each wiimote's event stream
		vector<boost::shared_ptr<stream_outlet> > marker_outlets;
		vector<boost::shared_ptr<stream_outlet> > sensor_outlets;
		for (int k=0;k<num_connected;k++) {
			stream_info info(string("WiimoteMarkers") + boost::lexical_cast<string>(k),"Markers",1,IRREGULAR_RATE,cf_string,string("WiimoteMarkers") + boost::lexical_cast<string>(k) + boost::asio::ip::host_name());
			marker_outlets.push_back(boost::shared_ptr<stream_outlet>(new stream_outlet(info)));
		}

		// open an outlet for each Wiimote's sensor streams
		if (enable_sensors) {
			for (int k=0;k<num_connected;k++) {
				wiimote *wm = wiimotes[k];
				// enable motion sensing
				wiiuse_motion_sensing(wm,1);
				// enable infrared tracking
				wiiuse_set_ir(wm,1);
				// create outlet
				int num_channels = 3 + 5 + 3 + 3;		// (raw accel x/y/z, orient p/y/r/ap/ar, gforce x/y/z, deduced IR x/y/z)
				if (wm->exp.type == EXP_NUNCHUK)
					num_channels += 3 + 5 + 3 + 2;		// (raw accel x/y/z, orient p/y/r/ap/ar, gforce x/y/z, joystick ang/mag)
				if (wm->exp.type == EXP_CLASSIC)
					num_channels += 2 + 2 + 2;			// (joystick-left ang/mag, joystick-right ang/mag, left-shoulder-button-pressure, right-shoulder-button-pressure)
				if (wm->exp.type == EXP_GUITAR_HERO_3)
					num_channels += 2 + 1;				// (joystick ang/mag, whammy-bar)
				if (wm->exp.type == EXP_WII_BOARD)
					num_channels = 4;					// (weight-topleft,weight-topright,weight-bottomleft,weight-bottomright)
				stream_info info(string("WiimoteSensors")+=boost::lexical_cast<string>(k),"MoCap",num_channels,IRREGULAR_RATE,cf_float32,string("WiimoteSensors") + boost::lexical_cast<string>(k) + boost::asio::ip::host_name());
				

				// === physical setup meta-data ===
				xml_element setup = info.desc().append_child("setup");
				xml_element handset = setup.append_child("handset")
					.append_child_value("assigned_id",boost::lexical_cast<string>(wm->unid).c_str())
					.append_child_value("state",boost::lexical_cast<string>(wm->state).c_str())
					.append_child_value("battery_level",boost::lexical_cast<string>(wm->battery_level).c_str())
					.append_child_value("expansion",expansion_names[wm->exp.type]);
				xml_element ir_bar = setup.append_child("screen_tracking")
					.append_child_value("bar_placement",wm->ir.pos==WIIUSE_IR_ABOVE ? "AboveScreen" : "BelowScreen")
					.append_child_value("aspect",wm->ir.aspect==WIIUSE_ASPECT_4_3 ? "4:3" : "16:9")
					.append_child_value("state",boost::lexical_cast<string>(wm->ir.state).c_str());
				ir_bar.append_child("virtual_resolution")
					.append_child_value("X",boost::lexical_cast<string>(wm->ir.vres[0]).c_str())
					.append_child_value("Y",boost::lexical_cast<string>(wm->ir.vres[1]).c_str());
				ir_bar.append_child("correction_offset")
					.append_child_value("X",boost::lexical_cast<string>(wm->ir.offset[0]).c_str())
					.append_child_value("Y",boost::lexical_cast<string>(wm->ir.offset[1]).c_str());

				xml_element accel_calib = handset.append_child("accel_calib");
				accel_calib.append_child("cal_zero")
					.append_child_value("X",boost::lexical_cast<string>(wm->accel_calib.cal_zero.x).c_str())
					.append_child_value("Y",boost::lexical_cast<string>(wm->accel_calib.cal_zero.y).c_str())
					.append_child_value("Z",boost::lexical_cast<string>(wm->accel_calib.cal_zero.z).c_str());
				accel_calib.append_child("cal_g")
					.append_child_value("X",boost::lexical_cast<string>(wm->accel_calib.cal_g.x).c_str())
					.append_child_value("Y",boost::lexical_cast<string>(wm->accel_calib.cal_g.y).c_str())
					.append_child_value("Z",boost::lexical_cast<string>(wm->accel_calib.cal_g.z).c_str());
				accel_calib.append_child_value("smoothing_alpha",boost::lexical_cast<string>(wm->accel_calib.st_alpha).c_str());

				if (wm->exp.type == EXP_NUNCHUK) {
					xml_element nunchuk = setup.append_child("nunchuk");
					xml_element accel_calib = nunchuk.append_child("accel_calib");
					accel_calib.append_child("cal_zero")
						.append_child_value("X",boost::lexical_cast<string>(wm->exp.nunchuk.accel_calib.cal_zero.x).c_str())
						.append_child_value("Y",boost::lexical_cast<string>(wm->exp.nunchuk.accel_calib.cal_zero.y).c_str())
						.append_child_value("Z",boost::lexical_cast<string>(wm->exp.nunchuk.accel_calib.cal_zero.z).c_str());
					accel_calib.append_child("cal_g")
						.append_child_value("X",boost::lexical_cast<string>(wm->exp.nunchuk.accel_calib.cal_g.x).c_str())
						.append_child_value("Y",boost::lexical_cast<string>(wm->exp.nunchuk.accel_calib.cal_g.y).c_str())
						.append_child_value("Z",boost::lexical_cast<string>(wm->exp.nunchuk.accel_calib.cal_g.z).c_str());
					accel_calib.append_child_value("smoothing_alpha",boost::lexical_cast<string>(wm->exp.nunchuk.accel_calib.st_alpha).c_str());
					xml_element joystick_calib = nunchuk.append_child("joystick_calib");
					joystick_calib.append_child("min")
						.append_child_value("X",boost::lexical_cast<string>(wm->exp.nunchuk.js.min.x).c_str())
						.append_child_value("Y",boost::lexical_cast<string>(wm->exp.nunchuk.js.min.y).c_str());
					joystick_calib.append_child("max")
						.append_child_value("X",boost::lexical_cast<string>(wm->exp.nunchuk.js.max.x).c_str())
						.append_child_value("Y",boost::lexical_cast<string>(wm->exp.nunchuk.js.max.y).c_str());
					joystick_calib.append_child("center")
						.append_child_value("X",boost::lexical_cast<string>(wm->exp.nunchuk.js.center.x).c_str())
						.append_child_value("Y",boost::lexical_cast<string>(wm->exp.nunchuk.js.center.y).c_str());
					nunchuk.append_child_value("orient_threshold",boost::lexical_cast<string>(wm->exp.nunchuk.orient_threshold).c_str());
					nunchuk.append_child_value("accel_threshold",boost::lexical_cast<string>(wm->exp.nunchuk.accel_threshold).c_str());
				}

				if (wm->exp.type == EXP_CLASSIC) {
					xml_element classic = setup.append_child("classic_controller");
					xml_element joyleft = classic.append_child("joystick_calib_left");
					joyleft.append_child("min")
						.append_child_value("X",boost::lexical_cast<string>(wm->exp.classic.ljs.min.x).c_str())
						.append_child_value("Y",boost::lexical_cast<string>(wm->exp.classic.ljs.min.y).c_str());
					joyleft.append_child("max")
						.append_child_value("X",boost::lexical_cast<string>(wm->exp.classic.ljs.max.x).c_str())
						.append_child_value("Y",boost::lexical_cast<string>(wm->exp.classic.ljs.max.y).c_str());
					joyleft.append_child("center")
						.append_child_value("X",boost::lexical_cast<string>(wm->exp.classic.ljs.center.x).c_str())
						.append_child_value("Y",boost::lexical_cast<string>(wm->exp.classic.ljs.center.y).c_str());
					xml_element joyright = classic.append_child("joystick_calib_right");
					joyright.append_child("min")
						.append_child_value("X",boost::lexical_cast<string>(wm->exp.classic.rjs.min.x).c_str())
						.append_child_value("Y",boost::lexical_cast<string>(wm->exp.classic.rjs.min.y).c_str());
					joyright.append_child("max")
						.append_child_value("X",boost::lexical_cast<string>(wm->exp.classic.rjs.max.x).c_str())
						.append_child_value("Y",boost::lexical_cast<string>(wm->exp.classic.rjs.max.y).c_str());
					joyright.append_child("center")
						.append_child_value("X",boost::lexical_cast<string>(wm->exp.classic.rjs.center.x).c_str())
						.append_child_value("Y",boost::lexical_cast<string>(wm->exp.classic.rjs.center.y).c_str());
				}

				if (wm->exp.type == EXP_GUITAR_HERO_3) {
					xml_element joystick_calib = setup.append_child("gh3_controller").append_child("joystick_calib");
					joystick_calib.append_child("min")
						.append_child_value("X",boost::lexical_cast<string>(wm->exp.gh3.js.min.x).c_str())
						.append_child_value("Y",boost::lexical_cast<string>(wm->exp.gh3.js.min.y).c_str());
					joystick_calib.append_child("max")
						.append_child_value("X",boost::lexical_cast<string>(wm->exp.gh3.js.max.x).c_str())
						.append_child_value("Y",boost::lexical_cast<string>(wm->exp.gh3.js.max.y).c_str());
					joystick_calib.append_child("center")
						.append_child_value("X",boost::lexical_cast<string>(wm->exp.gh3.js.center.x).c_str())
						.append_child_value("Y",boost::lexical_cast<string>(wm->exp.gh3.js.center.y).c_str());
				}

				if (wm->exp.type == EXP_WII_BOARD) {
					xml_element board_calib = setup.append_child("wiiboard").append_child("calib");
					board_calib.append_child("topleft")
						.append_child_value("A",boost::lexical_cast<string>(wm->exp.wb.ctl[0]).c_str())
						.append_child_value("B",boost::lexical_cast<string>(wm->exp.wb.ctl[1]).c_str())
						.append_child_value("C",boost::lexical_cast<string>(wm->exp.wb.ctl[2]).c_str());
					board_calib.append_child("topright")
						.append_child_value("A",boost::lexical_cast<string>(wm->exp.wb.ctr[0]).c_str())
						.append_child_value("B",boost::lexical_cast<string>(wm->exp.wb.ctr[1]).c_str())
						.append_child_value("C",boost::lexical_cast<string>(wm->exp.wb.ctr[2]).c_str());
					board_calib.append_child("bottomleft")
						.append_child_value("A",boost::lexical_cast<string>(wm->exp.wb.cbl[0]).c_str())
						.append_child_value("B",boost::lexical_cast<string>(wm->exp.wb.cbl[1]).c_str())
						.append_child_value("C",boost::lexical_cast<string>(wm->exp.wb.cbl[2]).c_str());
					board_calib.append_child("bottomright")
						.append_child_value("A",boost::lexical_cast<string>(wm->exp.wb.cbr[0]).c_str())
						.append_child_value("B",boost::lexical_cast<string>(wm->exp.wb.cbr[1]).c_str())
						.append_child_value("C",boost::lexical_cast<string>(wm->exp.wb.cbr[2]).c_str());
				}

				// channel layout
				lsl::xml_element channels = info.desc().append_child("channels");
				if (wm->exp.type == EXP_WII_BOARD) {
					// balance board channels
					channels.append_child("channel")
						.append_child_value("label","Weight_TopLeft")
						.append_child_value("type","Force")
						.append_child_value("unit","kilograms");
					channels.append_child("channel")
						.append_child_value("label","Weight_TopRight")
						.append_child_value("type","Force")
						.append_child_value("unit","kilograms");
					channels.append_child("channel")
						.append_child_value("label","Weight_BottomLeft")
						.append_child_value("type","Force")
						.append_child_value("unit","kilograms");
					channels.append_child("channel")
						.append_child_value("label","Weight_BottomRight")
						.append_child_value("type","Force")
						.append_child_value("unit","kilograms");
				} else {
					// otherwise we have first and foremost the Wiimote channels...
					channels.append_child("channel")
						.append_child_value("label","Wiimote_RawAccel_X")
						.append_child_value("type","Acceleration")
						.append_child_value("device","Wiimote")
						.append_child_value("unit","raw");
					channels.append_child("channel")
						.append_child_value("label","Wiimote_RawAccel_Y")
						.append_child_value("type","Acceleration")
						.append_child_value("device","Wiimote")
						.append_child_value("unit","raw");
					channels.append_child("channel")
						.append_child_value("label","Wiimote_RawAccel_Z")
						.append_child_value("type","Acceleration")
						.append_child_value("device","Wiimote")
						.append_child_value("unit","raw");

					channels.append_child("channel")
						.append_child_value("label","Wiimote_Pitch")
						.append_child_value("type","Orientation")
						.append_child_value("device","Wiimote")
						.append_child_value("unit","degrees");
					channels.append_child("channel")
						.append_child_value("label","Wiimote_Yaw")
						.append_child_value("type","Orientation")
						.append_child_value("device","Wiimote")
						.append_child_value("unit","degrees");
					channels.append_child("channel")
						.append_child_value("label","Wiimote_Roll")
						.append_child_value("type","Orientation")
						.append_child_value("device","Wiimote")
						.append_child_value("unit","degrees");
					channels.append_child("channel")
						.append_child_value("label","Wiimote_AbsolutePitch")
						.append_child_value("type","Orientation")
						.append_child_value("device","Wiimote")
						.append_child_value("unit","degrees");
					channels.append_child("channel")
						.append_child_value("label","Wiimote_AbsoluteRoll")
						.append_child_value("type","Orientation")
						.append_child_value("device","Wiimote")
						.append_child_value("unit","degrees");

					channels.append_child("channel")
						.append_child_value("label","Wiimote_Gravity_X")
						.append_child_value("type","Acceleration")
						.append_child_value("device","Wiimote");
					channels.append_child("channel")
						.append_child_value("label","Wiimote_Gravity_Y")
						.append_child_value("type","Acceleration")
						.append_child_value("device","Wiimote");
					channels.append_child("channel")
						.append_child_value("label","Wiimote_Gravity_Z")
						.append_child_value("type","Acceleration")
						.append_child_value("device","Wiimote");

					channels.append_child("channel")
						.append_child_value("label","Wiimote_Pointing_X")
						.append_child_value("type","Direction")
						.append_child_value("device","Wiimote");
					channels.append_child("channel")
						.append_child_value("label","Wiimote_Pointing_Y")
						.append_child_value("type","Direction")
						.append_child_value("device","Wiimote");
					channels.append_child("channel")
						.append_child_value("label","Wiimote_Pointing_Z")
						.append_child_value("type","Direction")
						.append_child_value("device","Wiimote");

					if (wm->exp.type == EXP_NUNCHUK) {
						// nunchuk extra channels
						channels.append_child("channel")
							.append_child_value("label","Nunchuk_RawAccel_X")
							.append_child_value("type","Acceleration")
							.append_child_value("device","Nunchuk")
							.append_child_value("unit","raw");
						channels.append_child("channel")
							.append_child_value("label","Nunchuk_RawAccel_Y")
							.append_child_value("type","Acceleration")
							.append_child_value("device","Nunchuk")
							.append_child_value("unit","raw");
						channels.append_child("channel")
							.append_child_value("label","Nunchuk_RawAccel_Z")
							.append_child_value("type","Acceleration")
							.append_child_value("device","Nunchuk")
							.append_child_value("unit","raw");

						channels.append_child("channel")
							.append_child_value("label","Nunchuk_Pitch")
							.append_child_value("type","Orientation")
							.append_child_value("device","Nunchuk")
							.append_child_value("unit","degrees");
						channels.append_child("channel")
							.append_child_value("label","Nunchuk_Yaw")
							.append_child_value("type","Orientation")
							.append_child_value("device","Nunchuk")
							.append_child_value("unit","degrees");
						channels.append_child("channel")
							.append_child_value("label","Nunchuk_Roll")
							.append_child_value("type","Orientation")
							.append_child_value("device","Nunchuk")
							.append_child_value("unit","degrees");
						channels.append_child("channel")
							.append_child_value("label","Nunchuk_AbsolutePitch")
							.append_child_value("type","Orientation")
							.append_child_value("device","Nunchuk")
							.append_child_value("unit","degrees");
						channels.append_child("channel")
							.append_child_value("label","Nunchuk_AbsoluteRoll")
							.append_child_value("type","Orientation")
							.append_child_value("device","Nunchuk")
							.append_child_value("unit","degrees");

						channels.append_child("channel")
							.append_child_value("label","Nunchuk_Gravity_X")
							.append_child_value("type","Acceleration")
							.append_child_value("device","Nunchuk");
						channels.append_child("channel")
							.append_child_value("label","Nunchuk_Gravity_Y")
							.append_child_value("type","Acceleration")
							.append_child_value("device","Nunchuk");
						channels.append_child("channel")
							.append_child_value("label","Nunchuk_Gravity_Z")
							.append_child_value("type","Acceleration")
							.append_child_value("device","Nunchuk");

						channels.append_child("channel")
							.append_child_value("label","Nunchuk_Joystick_Ang")
							.append_child_value("type","Direction")
							.append_child_value("device","Nunchuk")
							.append_child_value("unit","degrees");
						channels.append_child("channel")
							.append_child_value("label","Nunchuk_Joystick_Mag")
							.append_child_value("type","Magnitude")
							.append_child_value("device","Nunchuk");
					}

					if (wm->exp.type == EXP_CLASSIC) {
						// classic controller extra channels
						channels.append_child("channel")
							.append_child_value("label","ClassicController_JoystickLeft_Ang")
							.append_child_value("type","Direction")
							.append_child_value("device","ClassicController")
							.append_child_value("unit","degrees");
						channels.append_child("channel")
							.append_child_value("label","ClassicController_JoystickLeft_Mag")
							.append_child_value("type","Magnitude")
							.append_child_value("device","ClassicController");
						channels.append_child("channel")
							.append_child_value("label","ClassicController_JoystickRight_Ang")
							.append_child_value("type","Direction")
							.append_child_value("device","ClassicController")
							.append_child_value("unit","degrees");
						channels.append_child("channel")
							.append_child_value("label","ClassicController_JoystickRight_Mag")
							.append_child_value("type","Magnitude")
							.append_child_value("device","ClassicController");

						channels.append_child("channel")
							.append_child_value("label","ClassicController_LeftShoulderButton")
							.append_child_value("type","Magnitude")
							.append_child_value("device","ClassicController");
						channels.append_child("channel")
							.append_child_value("label","ClassicController_RightShoulderButton")
							.append_child_value("type","Magnitude")
							.append_child_value("device","ClassicController");
					}

					if (wm->exp.type == EXP_GUITAR_HERO_3) {
						channels.append_child("channel")
							.append_child_value("label","GH3_Joystick_Ang")
							.append_child_value("type","Direction")
							.append_child_value("device","GH3")
							.append_child_value("unit","degrees");
						channels.append_child("channel")
							.append_child_value("label","GH3_Joystick_Mag")
							.append_child_value("type","Magnitude")
							.append_child_value("device","GH3");						
						channels.append_child("channel")
							.append_child_value("label","GH3_WhammyBar")
							.append_child_value("type","Magnitude")
							.append_child_value("device","GH3");
					}
				}

				// misc information
				info.desc().append_child("acquisition")
					.append_child_value("manufacturer","Nintendo")
					.append_child_value("model","Wiimote")
					.append_child_value("api","wiiuse")
					.append_child_value("api_version",wiiuse_version());

				sensor_outlets.push_back(boost::shared_ptr<stream_outlet>(new stream_outlet(info)));
			}
		}

		// enter main loop
		cout << "Now transmitting data..." << endl;
		while (true) {
			if (wiiuse_poll(wiimotes,max_wiimotes)) {
				double now = local_clock();
				for (int i=0; i<num_connected; ++i) {
					wiimote *wm = wiimotes[i];
					switch (wm->event) {
					case WIIUSE_EVENT:
						// === check for button events that go into the marker streams ===

						if (IS_JUST_PRESSED(wm,WIIMOTE_BUTTON_TWO))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonTwo pressed"),now);
						if (IS_RELEASED(wm,WIIMOTE_BUTTON_TWO))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonTwo released"),now);
						if (IS_JUST_PRESSED(wm,WIIMOTE_BUTTON_ONE))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonTwo pressed"),now);
						if (IS_RELEASED(wm,WIIMOTE_BUTTON_ONE))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonOne released"),now);
						if (IS_JUST_PRESSED(wm,WIIMOTE_BUTTON_B))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonB pressed"),now);
						if (IS_RELEASED(wm,WIIMOTE_BUTTON_B))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonB released"),now);
						if (IS_JUST_PRESSED(wm,WIIMOTE_BUTTON_A))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonA pressed"),now);
						if (IS_RELEASED(wm,WIIMOTE_BUTTON_A))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonA released"),now);
						if (IS_JUST_PRESSED(wm,WIIMOTE_BUTTON_MINUS))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonMinus pressed"),now);
						if (IS_RELEASED(wm,WIIMOTE_BUTTON_MINUS))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonMinus released"),now);
						if (IS_JUST_PRESSED(wm,WIIMOTE_BUTTON_HOME))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonHome pressed"),now);
						if (IS_RELEASED(wm,WIIMOTE_BUTTON_HOME))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonHome released"),now);
						if (IS_JUST_PRESSED(wm,WIIMOTE_BUTTON_LEFT))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonLeft pressed"),now);
						if (IS_RELEASED(wm,WIIMOTE_BUTTON_LEFT))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonLeft released"),now);
						if (IS_JUST_PRESSED(wm,WIIMOTE_BUTTON_RIGHT))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonRight pressed"),now);
						if (IS_RELEASED(wm,WIIMOTE_BUTTON_RIGHT))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonLight released"),now);
						if (IS_JUST_PRESSED(wm,WIIMOTE_BUTTON_UP))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonUp pressed"),now);
						if (IS_RELEASED(wm,WIIMOTE_BUTTON_UP))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonUp released"),now);
						if (IS_JUST_PRESSED(wm,WIIMOTE_BUTTON_DOWN))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonDown pressed"),now);
						if (IS_RELEASED(wm,WIIMOTE_BUTTON_DOWN))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonDown released"),now);
						if (IS_JUST_PRESSED(wm,WIIMOTE_BUTTON_PLUS))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonPlus pressed"),now);
						if (IS_RELEASED(wm,WIIMOTE_BUTTON_PLUS))
							marker_outlets[i]->push_sample(vector<string>(1,"WiimoteButtonPlus released"),now);

						if (wm->exp.type == EXP_NUNCHUK) {
							if (IS_JUST_PRESSED((&(wm->exp.nunchuk)),NUNCHUK_BUTTON_C))
								marker_outlets[i]->push_sample(vector<string>(1,"NunchukButtonC pressed"),now);
							if (IS_RELEASED((&(wm->exp.nunchuk)),NUNCHUK_BUTTON_C))
								marker_outlets[i]->push_sample(vector<string>(1,"NunchukButtonC released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.nunchuk)),NUNCHUK_BUTTON_Z))
								marker_outlets[i]->push_sample(vector<string>(1,"NunchukButtonZ pressed"),now);
							if (IS_RELEASED((&(wm->exp.nunchuk)),NUNCHUK_BUTTON_Z))
								marker_outlets[i]->push_sample(vector<string>(1,"NunchukButtonZ released"),now);
						}

						if (wm->exp.type == EXP_CLASSIC) {
							if (IS_JUST_PRESSED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_UP))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonUp pressed"),now);
							if (IS_RELEASED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_UP))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonUp released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_LEFT))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonLeft pressed"),now);
							if (IS_RELEASED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_LEFT))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonLeft released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_ZR))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonZR pressed"),now);
							if (IS_RELEASED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_ZR))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonZR released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_X))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonX pressed"),now);
							if (IS_RELEASED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_X))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonX released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_A))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonA pressed"),now);
							if (IS_RELEASED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_A))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonA released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_Y))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonY pressed"),now);
							if (IS_RELEASED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_Y))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonY released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_B))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonB pressed"),now);
							if (IS_RELEASED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_B))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonB released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_ZL))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonZL pressed"),now);
							if (IS_RELEASED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_ZL))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonZL released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_FULL_L))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonFullL pressed"),now);
							if (IS_RELEASED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_FULL_L))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonFullL released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_FULL_R))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonFullR pressed"),now);
							if (IS_RELEASED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_FULL_R))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonFullR released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_PLUS))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonPlus pressed"),now);
							if (IS_RELEASED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_PLUS))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonPlus released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_HOME))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonHome pressed"),now);
							if (IS_RELEASED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_HOME))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonHome released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_MINUS))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonMinus pressed"),now);
							if (IS_RELEASED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_MINUS))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonMinus released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_DOWN))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonDown pressed"),now);
							if (IS_RELEASED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_DOWN))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonDown released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_RIGHT))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonRight pressed"),now);
							if (IS_RELEASED((&(wm->exp.classic)),CLASSIC_CTRL_BUTTON_RIGHT))
								marker_outlets[i]->push_sample(vector<string>(1,"ClassicButtonRight released"),now);
						}

						if (wm->exp.type == EXP_GUITAR_HERO_3) {
							if (IS_JUST_PRESSED((&(wm->exp.gh3)),GUITAR_HERO_3_BUTTON_STRUM_UP))
								marker_outlets[i]->push_sample(vector<string>(1,"GH3ButtonStrumUp pressed"),now);
							if (IS_RELEASED((&(wm->exp.gh3)),GUITAR_HERO_3_BUTTON_STRUM_UP))
								marker_outlets[i]->push_sample(vector<string>(1,"GH3ButtonStrumUp released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.gh3)),GUITAR_HERO_3_BUTTON_YELLOW))
								marker_outlets[i]->push_sample(vector<string>(1,"GH3ButtonYellow pressed"),now);
							if (IS_RELEASED((&(wm->exp.gh3)),GUITAR_HERO_3_BUTTON_YELLOW))
								marker_outlets[i]->push_sample(vector<string>(1,"GH3ButtonYellow released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.gh3)),GUITAR_HERO_3_BUTTON_GREEN))
								marker_outlets[i]->push_sample(vector<string>(1,"GH3ButtonGreen pressed"),now);
							if (IS_RELEASED((&(wm->exp.gh3)),GUITAR_HERO_3_BUTTON_GREEN))
								marker_outlets[i]->push_sample(vector<string>(1,"GH3ButtonGreen released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.gh3)),GUITAR_HERO_3_BUTTON_BLUE))
								marker_outlets[i]->push_sample(vector<string>(1,"GH3ButtonBlue pressed"),now);
							if (IS_RELEASED((&(wm->exp.gh3)),GUITAR_HERO_3_BUTTON_BLUE))
								marker_outlets[i]->push_sample(vector<string>(1,"GH3ButtonBlue released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.gh3)),GUITAR_HERO_3_BUTTON_RED))
								marker_outlets[i]->push_sample(vector<string>(1,"GH3ButtonRed pressed"),now);
							if (IS_RELEASED((&(wm->exp.gh3)),GUITAR_HERO_3_BUTTON_RED))
								marker_outlets[i]->push_sample(vector<string>(1,"GH3ButtonRed released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.gh3)),GUITAR_HERO_3_BUTTON_ORANGE))
								marker_outlets[i]->push_sample(vector<string>(1,"GH3ButtonOrange pressed"),now);
							if (IS_RELEASED((&(wm->exp.gh3)),GUITAR_HERO_3_BUTTON_ORANGE))
								marker_outlets[i]->push_sample(vector<string>(1,"GH3ButtonOrange released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.gh3)),GUITAR_HERO_3_BUTTON_PLUS))
								marker_outlets[i]->push_sample(vector<string>(1,"GH3ButtonPlus pressed"),now);
							if (IS_RELEASED((&(wm->exp.gh3)),GUITAR_HERO_3_BUTTON_PLUS))
								marker_outlets[i]->push_sample(vector<string>(1,"GH3ButtonPlus released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.gh3)),GUITAR_HERO_3_BUTTON_MINUS))
								marker_outlets[i]->push_sample(vector<string>(1,"GH3ButtonMinus pressed"),now);
							if (IS_RELEASED((&(wm->exp.gh3)),GUITAR_HERO_3_BUTTON_MINUS))
								marker_outlets[i]->push_sample(vector<string>(1,"GH3ButtonMinus released"),now);
							if (IS_JUST_PRESSED((&(wm->exp.gh3)),GUITAR_HERO_3_BUTTON_STRUM_DOWN))
								marker_outlets[i]->push_sample(vector<string>(1,"GH3ButtonStrumDown pressed"),now);
							if (IS_RELEASED((&(wm->exp.gh3)),GUITAR_HERO_3_BUTTON_STRUM_DOWN))
								marker_outlets[i]->push_sample(vector<string>(1,"GH3ButtonStrumDown released"),now);
						}

						// === construct the samples that go into the sensor streams ===
						if (enable_sensors) {
							if (wm->exp.type == EXP_WII_BOARD) {
								// balance board
								float sample[4] = {wm->exp.wb.tl, wm->exp.wb.tr, wm->exp.wb.bl, wm->exp.wb.br};
								sensor_outlets[i]->push_sample(sample,now);
							}
							if (wm->exp.type == EXP_NONE) {
								// Wiimote without expansions
								float sample[14] = {wm->accel.x, wm->accel.y, wm->accel.z, 
									wm->orient.pitch,wm->orient.yaw,wm->orient.roll,wm->orient.a_pitch,wm->orient.a_roll,
									wm->gforce.x,wm->gforce.y,wm->gforce.z,
									wm->ir.x,wm->ir.y,wm->ir.z};
								sensor_outlets[i]->push_sample(sample,now);
							}
							if (wm->exp.type == EXP_NUNCHUK) {
								// Wiimote with Nunchuk
								float sample[27] = {wm->accel.x, wm->accel.y, wm->accel.z, 
									wm->orient.pitch,wm->orient.yaw,wm->orient.roll,wm->orient.a_pitch,wm->orient.a_roll,
									wm->gforce.x,wm->gforce.y,wm->gforce.z,
									wm->ir.x,wm->ir.y,wm->ir.z,
									wm->exp.nunchuk.accel.x,wm->exp.nunchuk.accel.y,wm->exp.nunchuk.accel.z,
									wm->exp.nunchuk.orient.pitch,wm->exp.nunchuk.orient.yaw,wm->exp.nunchuk.orient.roll,wm->exp.nunchuk.orient.a_pitch,wm->exp.nunchuk.orient.a_roll,
									wm->exp.nunchuk.gforce.x,wm->exp.nunchuk.gforce.y,wm->exp.nunchuk.gforce.z,
									wm->exp.nunchuk.js.ang,wm->exp.nunchuk.js.mag};
								sensor_outlets[i]->push_sample(sample,now);
							}
							if (wm->exp.type == EXP_CLASSIC) {
								// Wiimote with classic controller
								float sample[20] = {wm->accel.x, wm->accel.y, wm->accel.z, 
									wm->orient.pitch,wm->orient.yaw,wm->orient.roll,wm->orient.a_pitch,wm->orient.a_roll,
									wm->gforce.x,wm->gforce.y,wm->gforce.z,
									wm->ir.x,wm->ir.y,wm->ir.z,
									wm->exp.classic.ljs.ang,wm->exp.classic.ljs.mag,
									wm->exp.classic.rjs.ang,wm->exp.classic.rjs.mag,
									wm->exp.classic.l_shoulder,wm->exp.classic.r_shoulder};
								sensor_outlets[i]->push_sample(sample,now);
							}
							if (wm->exp.type == EXP_GUITAR_HERO_3) {
								// Wiimote with GH3 controller
								float sample[17] = {wm->accel.x, wm->accel.y, wm->accel.z, 
									wm->orient.pitch,wm->orient.yaw,wm->orient.roll,wm->orient.a_pitch,wm->orient.a_roll,
									wm->gforce.x,wm->gforce.y,wm->gforce.z,
									wm->ir.x,wm->ir.y,wm->ir.z,
									wm->exp.gh3.js.ang,wm->exp.gh3.js.mag,
									wm->exp.gh3.whammy_bar};
								sensor_outlets[i]->push_sample(sample,now);
							}
						}
						break;

					case WIIUSE_DISCONNECT:
					case WIIUSE_UNEXPECTED_DISCONNECT:
						cout << "Warning: One of the Wiimotes was disconnected." << endl;
						break;
					case WIIUSE_NUNCHUK_INSERTED:
					case WIIUSE_CLASSIC_CTRL_INSERTED:
					case WIIUSE_WII_BOARD_CTRL_INSERTED:
					case WIIUSE_GUITAR_HERO_3_CTRL_INSERTED:
						throw runtime_error("An expansion was inserted at runtime.");
					case WIIUSE_NUNCHUK_REMOVED:
					case WIIUSE_CLASSIC_CTRL_REMOVED:
					case WIIUSE_GUITAR_HERO_3_CTRL_REMOVED:
					case WIIUSE_WII_BOARD_CTRL_REMOVED:
						throw runtime_error("An expansion was removed at runtime.");
					default:
						break;
					}
				}
			}
			boost::this_thread::yield();
		}
	}
	catch (std::runtime_error &e) {
		cerr << "Encountered an error: " << e.what() << endl;
	}
	wiiuse_cleanup(wiimotes,num_connected);

	return 0;
}

