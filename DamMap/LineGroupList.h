#pragma once
#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

#include "utils.h"
#include "frame.h"
#include "lineSimplified.h"

class LineGroupList
{
private:
	Frame *frame; //Used to process the source image
	vector<Vec4i> linesP; //Unprocessed lines produced by OpenCV Hough line detector
	vector<vector<Vec4i>> groupsOfLines; //lines extracted from linesP and grouped together by angle and proximity

public:
	LineGroupList(Frame& frame);
	~LineGroupList();

	void showGroups(); //Draw line groups (debugging)
	const vector<vector<Vec4i>> getGroupsOfLines(); //Get line groups
};
