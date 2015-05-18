#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <gazeapi.h>
#include <lsl_cpp.h>
//#include <synchapi.h>
#include <iostream>

using namespace std;
using namespace lsl;



// --- GazeStream definition
class GazeStream : public gtl::IGazeListener
{
public:
	GazeStream();
	~GazeStream();

	int connected();
	void get_frame();
	void disconnect();

	gtl::GazeData data;

private:
	// IGazeListener
	void on_gaze_data(gtl::GazeData const & gaze_data);
	gtl::GazeApi m_api;
	int connection;
	
};

// --- GazeStream implementation
GazeStream::GazeStream()
{
	cout << "Stream created.\n Waiting for device. . ." << endl;
	(void)connected();
}

int GazeStream::connected() {

	// Connect to the server in push mode on the default TCP port (6555)
	if (m_api.connect(true))
	{
		// Enable GazeStream notifications
		m_api.add_listener(*this);
		connection = 1;
		return 1;
	}
	return 0;
}

GazeStream::~GazeStream()
{
	if (connection) {
		disconnect();
	}
	cout << "Disconnected." << endl;
}

void GazeStream::disconnect() {
	m_api.remove_listener(*this);
	m_api.disconnect();
}

void GazeStream::on_gaze_data(gtl::GazeData const & gaze_data)
{
	data = gaze_data;
	//if (gaze_data.state & gtl::GazeData::GD_STATE_TRACKING_GAZE) {
	//	gtl::Point2D const & smoothedCoordinates = gaze_data.avg;
	//	//std::cout << "data received. " << smoothedCoordinates.x << ", " << smoothedCoordinates.y << std::endl;
	//}
}

void GazeStream::get_frame() {
	cout << "testing entry" << endl;
	m_api.get_frame(data);
	if (data.state & gtl::GazeData::GD_STATE_TRACKING_GAZE) {
		gtl::Point2D const & smoothedCoordinates = data.avg;
		std::cout << "data received. " << smoothedCoordinates.x << ", " << smoothedCoordinates.y << std::endl;
	}
	//return false;
}


void stream_gaze_data() { //TODO smoothing data

	stream_info info("EyeTribe", "Gaze", 8, 30, cf_float32, "EyeTribe");
	//TODO add identifier to stream name (unique)
	xml_element et_channels = info.desc().append_child("channels");
	et_channels.append_child("channel")
		.append_child_value("label", "Screen_X")
		.append_child_value("eye", "both")
		.append_child_value("type", "ScreenX")
		.append_child_value("unit", "pixels");
	et_channels.append_child("channel")
		.append_child_value("label", "Screen_Y")
		.append_child_value("eye", "both")
		.append_child_value("type", "ScreenY")
		.append_child_value("unit", "pixels");

	et_channels.append_child("channel")
		.append_child_value("label", "Pupil_X_left")
		.append_child_value("eye", "left")
		.append_child_value("type", "PupilX");
	et_channels.append_child("channel")
		.append_child_value("label", "Pupil_Y_left")
		.append_child_value("eye", "left")
		.append_child_value("type", "PupilY");
	et_channels.append_child("channel")
		.append_child_value("label", "Diameter_left")
		.append_child_value("eye", "left")
		.append_child_value("type", "Diameter");

	et_channels.append_child("channel")
		.append_child_value("label", "Pupil_X_right")
		.append_child_value("eye", "right")
		.append_child_value("type", "PupilX");
	et_channels.append_child("channel")
		.append_child_value("label", "Pupil_Y_right")
		.append_child_value("eye", "right")
		.append_child_value("type", "PupilY");
	et_channels.append_child("channel")
		.append_child_value("label", "Diameter_right")
		.append_child_value("eye", "right")
		.append_child_value("type", "Diameter");

	info.desc().append_child("acquisition")
		.append_child_value("manufacturer", "EyeTribe_Tracker");


	cout << "starting EyeTribe tracking. . ." << endl;


	stream_outlet outlet(info);

	GazeStream streamobj;
	if (streamobj.connected()) {
		cout << "Connected." << endl;
	}

	while (1)
	{
		float sample[8] = { streamobj.data.avg.x, streamobj.data.avg.y, streamobj.data.lefteye.avg.x,
							streamobj.data.lefteye.avg.y, streamobj.data.lefteye.psize,
							streamobj.data.righteye.avg.x, streamobj.data.righteye.avg.y,
							streamobj.data.righteye.psize};
		
		
		cout << "x: " << streamobj.data.avg.x << "y: " << streamobj.data.avg.y << endl;
		//cout << "left pupil: " << streamobj.data.lefteye.psize << endl;
		//cout << "right pupil: " << streamobj.data.righteye.psize << endl;
		//cout << endl;  

		Sleep(100/3);
		outlet.push_sample(sample);
	}
	streamobj.disconnect();

}

int main()
{

	stream_gaze_data();


	return 0;

}