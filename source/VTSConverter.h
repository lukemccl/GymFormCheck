#pragma once
#ifndef VTSConverter_H
#define VTSConverter_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include "videoToDir.h"
#include "TimeSeriesGenerator.h"

std::string convert(std::string videoPath);
std::string directoryConvert(std::string directoryPath);
#endif