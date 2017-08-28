#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ole2.h>
#include <NuiApi.h>
#include <lsl_cpp.h>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <stdlib.h>

using namespace std;
using namespace lsl;

const int NUM_CHANNELS_PER_JOINT = 4;															// (x, y, z, confidence)
const int NUM_CHANNELS_PER_SKELETON = (NUI_SKELETON_POSITION_COUNT*NUM_CHANNELS_PER_JOINT) + 2; // (joint, joint, ..., joint, tracking id, quality flags)
const int NUM_CHANNELS_PER_STREAM = NUI_SKELETON_MAX_TRACKED_COUNT*NUM_CHANNELS_PER_SKELETON;	// (skeleton, skeleton, ..., skeleton)

const char *const joint_names[] = {"HipCenter","Spine","ShoulderCenter","Head","ShoulderLeft","ElbowLeft","WristLeft","HandLeft","ShoulderRight",
	"ElbowRight","WristRight","HandRight","HipLeft","KneeLeft","AnkleLeft","FootLeft","HipRight","KneeRight","AnkleRight","FootRight"};

// connect to a Kinect sensor and stream its data into LSL
void stream_from_sensor(int sensornum, bool smoothdata) {
	HRESULT hr=0;
	try {
		// instantiate sensor
		cout << "initializing sensor " << sensornum << "..." << endl;
		INuiSensor *sensor;
		if (FAILED(hr=NuiCreateSensorByIndex(sensornum,&sensor)))
			throw runtime_error("Could not instantiate sensor.");

		// initialize and enable skeletal tracking
		if (FAILED(hr = sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON)))
			throw runtime_error("Could not initialize NUI functionality.");
		if (FAILED(hr = sensor->NuiSkeletonTrackingEnable(NULL,NUI_SKELETON_TRACKING_FLAG_SUPPRESS_NO_FRAME_DATA)))
			throw runtime_error("Could not enable skeletal tracking.");

		cout << "opening outlet..." << endl;
		char tmp[1024]; ;
		stream_info info(string("KinectMocap")+=boost::lexical_cast<string>(sensornum),"Mocap",NUM_CHANNELS_PER_STREAM,30,cf_float32,string(tmp,tmp+wcstombs(tmp,sensor->NuiUniqueId(),sizeof(tmp))));

		// physical setup
		xml_element setup = info.desc().append_child("setup");
		xml_element cam = setup.append_child("cameras").append_child("camera");
		cam.append_child_value("label","Kinect");
		cam.append_child("position")
			.append_child_value("X","0.0")
			.append_child_value("Y","0.0")
			.append_child_value("Z","0.0");
		cam.append_child_value("diagonal_fov",boost::lexical_cast<string>(NUI_CAMERA_DEPTH_NOMINAL_DIAGONAL_FOV).c_str());
		cam.append_child_value("horizontal_fov",boost::lexical_cast<string>(NUI_CAMERA_DEPTH_NOMINAL_HORIZONTAL_FOV).c_str());
		cam.append_child_value("vertical_fov",boost::lexical_cast<string>(NUI_CAMERA_DEPTH_NOMINAL_VERTICAL_FOV).c_str());

		// markers
		xml_element mrks = setup.append_child("markers");
		for (int k=0;k<NUI_SKELETON_POSITION_COUNT;k++) {
			mrks.append_child("marker")
				.append_child_value("label",joint_names[k])
				.append_child_value("id",boost::lexical_cast<std::string>(k).c_str());
		}

		// channel layout
		xml_element channels = info.desc().append_child("channels");
		for (int s=0;s<NUI_SKELETON_MAX_TRACKED_COUNT;s++) {
			for (int k=0;k<NUI_SKELETON_POSITION_COUNT;k++) {
				channels.append_child("channel")
					.append_child_value("label",(string(joint_names[k])+="_X").c_str())
					.append_child_value("marker",joint_names[k])
					.append_child_value("type","PositionX")
					.append_child_value("unit","meters");
				channels.append_child("channel")
					.append_child_value("label",(string(joint_names[k])+="_Y").c_str())
					.append_child_value("marker",joint_names[k])
					.append_child_value("type","PositionY")
					.append_child_value("unit","meters");
				channels.append_child("channel")
					.append_child_value("label",(string(joint_names[k])+="_Z").c_str())
					.append_child_value("marker",joint_names[k])
					.append_child_value("type","PositionZ")
					.append_child_value("unit","meters");
				channels.append_child("channel")
					.append_child_value("label",(string(joint_names[k])+="_Conf").c_str())
					.append_child_value("marker",joint_names[k])
					.append_child_value("type","Confidence")
					.append_child_value("unit","normalized");
			}
			channels.append_child("channel")
				.append_child_value("label",(string("SkeletonTrackingId")+=boost::lexical_cast<string>(s)).c_str())
				.append_child_value("type","TrackingId");
			channels.append_child("channel")
				.append_child_value("label",(string("SkeletonQualityFlags")+=boost::lexical_cast<string>(s)).c_str());
		}

		// misc meta-data
		info.desc().append_child("acquisition")
			.append_child_value("manufacturer","Microsoft")
			.append_child_value("model","Kinect 1.0");
		if (smoothdata)
			info.desc().append_child("filtering")
			.append_child("holt_double_exponential")
			.append_child_value("smoothing","0.5")
			.append_child_value("correction","0.5")
			.append_child_value("jitter_radius","0.05")
			.append_child_value("max_deviation_radius","0.04");

		stream_outlet outlet(info);

		// acquisition loop		
		cout << "starting to track on sensor " << sensornum << endl;
		while (true) {
			// get next frame
			NUI_SKELETON_FRAME frame = {0};
			if (FAILED(sensor->NuiSkeletonGetNextFrame(100,&frame)))
				continue;

			// for each skeleton index s
			vector<float> sample(NUM_CHANNELS_PER_STREAM);
			for (int s=0,i=0; s<NUI_SKELETON_COUNT; s++) {
				if (frame.SkeletonData[s].eTrackingState == NUI_SKELETON_TRACKED) {
					// optionally smooth the data
					if (smoothdata)
						sensor->NuiTransformSmooth(&frame,NULL);
					// assign the sample data (at slot i)
					for (int k=0;k<NUI_SKELETON_POSITION_COUNT;k++) {
						sample[i*NUM_CHANNELS_PER_SKELETON + k*NUM_CHANNELS_PER_JOINT + 0] = frame.SkeletonData[s].SkeletonPositions[k].x;
						sample[i*NUM_CHANNELS_PER_SKELETON + k*NUM_CHANNELS_PER_JOINT + 1] = frame.SkeletonData[s].SkeletonPositions[k].y;
						sample[i*NUM_CHANNELS_PER_SKELETON + k*NUM_CHANNELS_PER_JOINT + 2] = frame.SkeletonData[s].SkeletonPositions[k].z;
						sample[i*NUM_CHANNELS_PER_SKELETON + k*NUM_CHANNELS_PER_JOINT + 3] = frame.SkeletonData[s].eSkeletonPositionTrackingState[k] / 2.0f; // 0.0, 0.5, or 1.0
					}
					sample[i*NUM_CHANNELS_PER_SKELETON + NUM_CHANNELS_PER_JOINT*NUI_SKELETON_POSITION_COUNT] = frame.SkeletonData[s].dwTrackingID;
					sample[i*NUM_CHANNELS_PER_SKELETON + NUM_CHANNELS_PER_JOINT*NUI_SKELETON_POSITION_COUNT] = frame.SkeletonData[s].dwQualityFlags;
					i++;
				}
			}
			// push the sample into LSL
			outlet.push_sample(sample);
		}

		// shut down
		sensor->NuiShutdown();
		sensor->Release();
	}
	catch(exception &e) {
		cerr << "Error trying to stream from sensor " << sensornum << ": " << e.what() << endl;
	}
}

int main(int argc, char* argv[]) {
	HRESULT hr=0;
	try {
		cout << "Enumerating Kinect devices... ";
		int numsensors; 
		if (FAILED(hr = NuiGetSensorCount(&numsensors)))
			throw runtime_error("could not get sensor count.");
		cout << numsensors << " sensors found." << endl;
		if (!numsensors)
			return 0;

		// for now stream from the 0'th sensor
		stream_from_sensor(0,false);
		return 0;
	}
	catch(exception &e) {
		cerr << "Error in KinectMocap: " << e.what() << endl;
		cerr << "NUI error code: " << hr << endl;
		return 1;
	}
}
