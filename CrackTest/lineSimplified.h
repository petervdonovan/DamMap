#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>

#include "utils.h"

using namespace std;
using namespace cv;


class LineSimplified
{
private:
	bool vertical;
	vector<int> distFromTopLeft;
	int distThreshForSimilarity;
	bool isSimilar(vector<Vec4i> lines);
	double LineSimplified::meanAngle(vector<Vec4i> lines);
	int LineSimplified::meanDist(vector<Vec4i> lines, bool vertical);
	int vanished;
	const int meanRecentDists();
public:
	LineSimplified(vector<Vec4i> lines, int distThreshForSimilarity = 100);
	~LineSimplified();
	const bool getVertical();
	const int getDistFromTopLeft();
	bool update(vector<Vec4i> lines); //postcondition: return false if the group of lines given is too dissimilar
	void incrementVanished();
	int getVanished();
	Vec4i getEndpoints(int rows, int cols);
	const bool crossed(int distThresh = 100);
	//distThresh = the number of pixels that the line must have crossed
};

