#include "TimeSeriesGenerator.h"

using namespace std;
using namespace cv;

/*
	0  - head
	1  - neck
	2  - Rsho
	3  - Relb
	4  - Rwri
	5  - Lsho
	6  - Lelb
	7  - Lwri
	8  - Rhip
	9 - Rkne
	10 - Rank
	11 - Lhip
	12 - Lkne
	13 - Lank
	14 - Spine
	15 - Rfoot
	16 - Lfoot
*/
const int dimensions = 3;
const int numParts = 17;

int estimationToTimeSeries(std::string dataDir) {
	//creating mapping between parts and connection model
	string trackedParts[numParts] = { "head", "neck", "Rsho", "Relb", "Rwri", "Lsho", "Lelb", "Lwri", "Rhip", "Rkne", "Rank", "Lhip", "Lkne", "Lank", "Spine", "Rfoot", "Lfoot" };
	vector<int> connections[numParts];
	buildConnections(connections);
	
	//opening I/O for both reading and output
	ifstream inFile;
	inFile.open(dataDir + "\\jointPositions.mddd");
	if (inFile.fail()) throw Exception();

	ofstream outFile(dataDir + "\\jointAngles.csv");
	outFile << "neckV,neckH,Rsho,Relb,Lsho,Lelb,Rhip,Rkne,Rank,Lhip,Lkne,Lank,LSpine,RSpine" << endl;
	
	string line;
	int i = 0;
	while (getline(inFile, line)) {
		if (i == 0) {
			i++;
			continue;
		}

		double partValues[numParts][dimensions];
		parsePartValues(line, partValues, numParts, dimensions);
		string line = "";

		//loop through every part logged and if viable calculate angles
		for (int part = 0; part < numParts; part++) {
			line += anglesAroundConnectedParts(part, connections[part], partValues);
		}
		//print to outfile
		line.pop_back();
		outFile << line << endl;
	}
	outFile.close();
	return 0;
}

//pull the 3d coord for every part into array
void parsePartValues(std::string line, double partValues[][dimensions], int parts, int dimensions) {
	vector<string> splitString = split(line, ",");
	if (splitString.size() != (1 + (parts * dimensions))) throw Exception();
	for (int p = 0; p < parts; p++) {
		for (int i = 0; i < dimensions; i++) {
			partValues[p][i] = std::stod(splitString.at(1+ (p * dimensions) + i));
		}
	}
}

//given a part and it's connections, returns the angle around that part
std::string anglesAroundConnectedParts(int part, vector<int> partConnections, double partValues[][dimensions]) {
	string line = "";
	//skip if unviable
	if (partConnections.size() < 2) return "";
	//if simple send to calcBetweenParts
	if (partConnections.size() == 2) {
		int partA = partConnections.at(0);
		int partB = partConnections.at(1);
		double angle = calcBetweenParts(partValues[part], partValues[partA], partValues[partB]);
		line += to_string(angle) + ",";
		//cout << trackedParts[partA] << " -> " << trackedParts[part] << " -> " << trackedParts[partB] << " = " << angle << endl;
	}
	//cases for neck to hips - create two simple angles
	if (partConnections.size() == 3) {
		int partA = partConnections.at(0);
		int partB = partConnections.at(1);
		int partC = partConnections.at(2);

		double angle1 = calcBetweenParts(partValues[part], partValues[partA], partValues[partB]);
		line += to_string(angle1) + ",";
		//cout << trackedParts[partA] << " -> " << trackedParts[part] << " -> " << trackedParts[partB] << " = " << angle1 << endl;

		double angle2 = calcBetweenParts(partValues[part], partValues[partA], partValues[partC]);
		line += to_string(angle2) + ",";
		//cout << trackedParts[partA] << " -> " << trackedParts[part] << " -> " << trackedParts[partC] << " = " << angle2 << endl;
	}
	//case across neck - create vertical and horizontal angles
	if (partConnections.size() == 4) {
		int partA = partConnections.at(0);
		int partB = partConnections.at(1);
		int partC = partConnections.at(2);
		int partD = partConnections.at(3);

		double angle1 = calcBetweenParts(partValues[part], partValues[partA], partValues[partB]);
		line += to_string(angle1) + ",";
		//cout << trackedParts[partA] << " -> " << trackedParts[part] << " -> " << trackedParts[partB] << " = " << angle1 << endl;

		double angle2 = calcBetweenParts(partValues[part], partValues[partC], partValues[partD]);
		line += to_string(angle2) + ",";
		//cout << trackedParts[partC] << " -> " << trackedParts[part] << " -> " << trackedParts[partD] << " = " << angle2 << endl;
	}
	return line;
}

double calcBetweenParts(double rootPart[3], double partValue1[3], double partValue2[3]) {
	Vec3d rootPartVec(rootPart[0], rootPart[1], rootPart[2]);
	Vec3d connectionA(partValue1[0], partValue1[1], partValue1[2]);
	Vec3d connectionB(partValue2[0], partValue2[1], partValue2[2]);
	connectionA -= rootPartVec;
	connectionB -= rootPartVec;

	return calcAngle(connectionA, connectionB);
}

double calcAngle(Vec3d connectionA, Vec3d connectionB) {
	double magA = 0;
	for (int i = 0; i < connectionA.channels; i++) {
		magA += (connectionA[i] * connectionA[i]);
	}
	magA = sqrt(magA);
	double magB = 0;
	for (int i = 0; i < connectionB.channels; i++) {
		magB += connectionB[i] * connectionB[i];
	}
	magB = sqrt(magB);
	double magnitude = (magA*magB);
	if (magnitude == 0) return 0;
	double angle = acos(connectionA.dot(connectionB) / (magA*magB));
	return angle * 180 / M_PI;
}

vector<string> split(string s, string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	string token;
	vector<string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

void addEdge(vector<int> adj[], int u, int v)
{
	adj[u].push_back(v);
	adj[v].push_back(u);
}

void buildConnections(vector<int> connections[]) {
	//neck
	addEdge(connections, 1, 0);
	addEdge(connections, 1, 14);
	addEdge(connections, 1, 2);
	addEdge(connections, 1, 5);

	//Rarm
	addEdge(connections, 2, 3);
	addEdge(connections, 3, 4);

	//Larm
	addEdge(connections, 5, 6);
	addEdge(connections, 6, 7);

	//Spine
	addEdge(connections, 14, 11);
	addEdge(connections, 14, 8);

	//RLeg
	addEdge(connections, 9, 8);
	addEdge(connections, 9, 10);
	addEdge(connections, 10, 15);

	//LLeg
	addEdge(connections, 12, 11);
	addEdge(connections, 12, 13);
	addEdge(connections, 13, 16);
}