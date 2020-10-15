#define NO_STRICT
#include "VTSConverter.h"
#include <VNECT.hpp>

CWEBSOCKETS_STATIC_DEFINITIONS

using namespace cv;
using namespace std;
using namespace boost::filesystem;

int convert(std::string videoPath, VNECT *vnect);
int calculateJointPositions(std::string dataDir, VNECT *vnect);

/*
	Remove commented out parts and comment line 32 to attempt faster conversion ie only need to initialise net once
	Has an issue on saving model data out where will force into number of frames total run through the net not just this video

	Once fixed can be implemented, hopefully reducing conversion time down from approx 50min for 45 files
 */

std::string directoryConvert(std::string directoryPath) {
	std::vector<string> filenames;

	//VNECT *vnect = new VNECT();
	
	for (directory_entry& entry : directory_iterator(directoryPath)) {
		if (entry.path().extension() == (".mp4")) {

			filenames.push_back(entry.path().stem().string());
			cout << entry.path().stem().string() << endl;

			convert(entry.path().string());
			//convert(entry.path().string(), vnect);
		}
	}

	//delete vnect;

	return directoryPath;
}

//converts a single video from video to joint angles, vnect instantiated with each execution
std::string convert(std::string videoPath){

	string directoryPath = videoPath.substr(0, videoPath.find_last_of("."));
	videoToDir(videoPath);
	if (!boost::filesystem::exists(directoryPath + "\\jointPositions.mddd")) {
		VNECT *vnect = new VNECT;

		calculateJointPositions(directoryPath, vnect);

		delete vnect;
	}

	cout << "calculating joint angles" << endl;
	if (!boost::filesystem::exists(directoryPath+"\\jointAngles.csv"))
		estimationToTimeSeries(directoryPath);

	cout << "complete" << endl;

	return directoryPath;
}

//converts a single video from video to joint angles, vnect object passed in for speed on initialisation
int convert(std::string videoPath, VNECT *vnect){

	string directoryPath = videoPath.substr(0, videoPath.find_last_of("."));

	videoToDir(videoPath);

	if (!boost::filesystem::exists(directoryPath + "\\jointPositions.mddd"))
		calculateJointPositions(directoryPath, vnect);

	cout << "calculating joint angles" << endl;
	if (!boost::filesystem::exists(directoryPath + "\\jointAngles.csv"))
		estimationToTimeSeries(directoryPath);
	cout << "complete" << endl;

	return 0;
}

//converts a video into jointPositions using vnect object
int calculateJointPositions(std::string dataDir, VNECT *vnect){

	std::string rootFile = dataDir.substr(dataDir.find_last_of("\\") + 1);
	std::string images_to_load = dataDir + "\\imageCalibration.images";
	std::string baseFolder = dataDir.substr(0, dataDir.find_last_of("\\"));

	m_numFrames = 0;
	load_image_sequence(images_to_load.c_str());

	m_currentCamera = 0;
	m_currentFrame = 0;

	int maxF = m_numFrames; 
	bool rescaleDone = false;
	for (; m_currentFrame < maxF; m_currentFrame++)
	{
		cv::Mat currentImage = getTheImage(m_currentCamera, m_currentFrame);

		if (currentImage.empty() == true) continue;

		vnect->processImg(currentImage);

		if (rescaleDone == false && vnect->initDone == true)
		{
			vnect->rescaleSkeleton();
			rescaleDone = true;
		}
	}
	
	vnect->save_joint_positions(dataDir + "\\jointPositions.mddd");

	return 0;
}