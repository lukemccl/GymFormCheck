#include "videoToDir.h"

using namespace std;
using namespace cv;	
namespace fs = boost::filesystem;

void videoToDir(std::string videoFile = "") {
	//set up directory of files for input video
	std::string directory = videoFile.substr(0, videoFile.find_last_of("."));
	std::string rootDir = directory.substr(0, directory.find_last_of("\\"));

	VideoCapture capture(videoFile);
	if (!capture.isOpened()) {
		cout << "Error opening video file " + videoFile << endl;
		return;
	}
	int numFrames = capture.get(CAP_PROP_FRAME_COUNT);
	int vidWidth = capture.get(CAP_PROP_FRAME_WIDTH);
	int vidHeight = capture.get(CAP_PROP_FRAME_HEIGHT);
	capture.release();

	//writing video specific files if video dir not found
	if (!boost::filesystem::exists(directory)) {
		CreateDirectory(directory.c_str(), NULL);
	}
	if (!boost::filesystem::exists(directory + "/imageSequence")) {
		videoSplit(videoFile, directory);
	}
	if (!boost::filesystem::exists(directory + "/imageCalibration.images")) {
		dotImages(numFrames, directory);
	}
}

//splits video into each consecutive frame
void videoSplit(std::string videoFile = "", std::string directory = "") {
	VideoCapture capture(videoFile);

	if (!capture.isOpened()) {
		cout << "Error opening video file " + videoFile << endl;
		return;
	}

	directory += "\\imageSequence";
	CreateDirectory(directory.c_str(), NULL);

	int i = 0;
	int cap = capture.get(CAP_PROP_FRAME_COUNT);
	while (i < cap) {
		Mat frame;
		capture >> frame;

		std::string index = to_string(i);
		std::string num = std::string((6 - index.length()), '0') + index;
		imwrite(directory + "\\img_" + num + ".jpg", frame);

		i++;
	}
	capture.release();
}

//creates .images config file for a given video
void dotImages(int numFrames = 0, std::string directory = "") {
	ofstream outFile (directory + "\\imageCalibration.images");
	
 	outFile << "Skeletool Images file v1.0" << endl;
	outFile << "cameras 1" << endl;
	outFile << "frames " + to_string(numFrames) << endl;
	outFile << "basepath " + directory + "\\\\imageSequence" << endl;
	outFile << "images img_%06d.jpg" << endl;

	outFile.close();
}