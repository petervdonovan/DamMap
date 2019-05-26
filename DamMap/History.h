#pragma once
#include <opencv2/opencv.hpp>
#include "lineSimplified.h"


using namespace std;
using namespace cv;

class History
{
private:
	int rows, cols;
	double borderThresh;
	vector<LineSimplified> currentLines;
	vector<Point> points;
public:
	History(int rows, int cols, double borderThresh = 0.4);
	~History();

	void update(vector<vector<Vec4i>> lineGroups, int crossedThresh, int vanishedThresh, int distThreshForSimilarity);
	void showCurrent(Mat);
	void show();
};