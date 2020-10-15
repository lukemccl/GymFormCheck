#pragma once
#ifndef videoToDir_H    // To make sure you don't declare the function more than once by including the header multiple times.
#define videoToDir_H

#include "opencv2/opencv.hpp"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>

void videoToDir(std::string videoFile);
void videoSplit(std::string videoFile, std::string directory);
void dotImages(int numFrames, std::string directory);

#endif