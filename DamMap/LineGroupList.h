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
	Frame *frame;
	vector<Vec4i> linesP;
	vector<vector<Vec4i>> groupsOfLines;

public:
	LineGroupList(Frame& fr, int rho = 1, int thetaMinutes = 60, int threshold = 30, int minLineLength = 5, int maxLineGap = 16);
	~LineGroupList();

	void showGroups();
	const vector<vector<Vec4i>> getGroupsOfLines();
};

