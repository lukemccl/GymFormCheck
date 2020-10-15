#pragma once
#ifndef TimeSeriesGenerator_H    // To make sure you don't declare the function more than once by including the header multiple times.
#define TimeSeriesGenerator_H
#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <math.h>
#include "opencv2/opencv.hpp"

int estimationToTimeSeries(std::string dataDir);
void parsePartValues(std::string line, double partValues[][3], int parts, int dimensions);
std::string anglesAroundConnectedParts(int part, std::vector<int> partConnections, double partValues[][3]);
double calcBetweenParts(double rootPart[], double partValue1[], double partValue2[]);
double calcAngle(cv::Vec3d connectionA, cv::Vec3d connectionB);
std::vector<std::string> split(std::string s, std::string delimiter);
void addEdge(std::vector<int> adj[], int u, int v);
void buildConnections(std::vector<int> connections[]);
#endif