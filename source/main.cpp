#define NO_STRICT
#include <chrono>
#include <iostream>
#include "boost/filesystem.hpp"
#include <Python.h>
#include "VTSConverter.h"

using namespace std; 
using namespace boost::filesystem;

int main(int argc, char *argv[]) {
	auto start = std::chrono::high_resolution_clock::now();

	//checks improper amount of arguments and first argument - second is checked by ML module
	if (argc != 3 || !exists(argv[1])) {
		cout << "Incorrect arguments found - format should be:" << endl;
		cout << "TRAINING       : VNECT.exe <Training Folder> <Model Name>" << endl;
		cout << "CLASSIFICATION : VNECT.exe <Video File> <Model Name>" << endl;
	} 
	else {
		char* pyArgv[3];

		//these arguments are independent of anything in VTSC - passed straight through to ML module
		pyArgv[0] = "TSAnalysis.py";
		pyArgv[2] = argv[2];

		if (is_regular_file(argv[1])) {
			//is single file -- perform analysis
			cout << "File Classification Mode" << endl;
			if (((path)argv[1]).extension().string() != (".mp4")) {
				cout << "Must input an mp4 file" << endl;
				return 0;
			}

			//convert file name to created directory
			string directoryName = convert(argv[1]);
			char *cstr = new char[directoryName.size() + 1];
			directoryName.copy(cstr, directoryName.size() + 1);
			cstr[directoryName.size()] = '\0';

			//classify using created directory 
			pyArgv[1] = cstr;

		}
		else {
			//is directory -- convert all to training & train
			cout << "Directory Training Mode" << endl;

			directoryConvert(argv[1]);

			//put directory of joint angles through python
			pyArgv[1] = argv[1];
		}

		//launch python ML module 
		string pyCommand = "python " + string(pyArgv[0]) + " " + string(pyArgv[1]) + " " + string(pyArgv[2]);
		std::system(pyCommand.c_str());
	}

	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	std::cout << "Elapsed time: " << elapsed.count() << " s\n";
	return 0;
}