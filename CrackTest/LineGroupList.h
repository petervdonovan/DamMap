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
	LineGroupList(Frame& frame);
	~LineGroupList();

	void showGroups();
	const vector<vector<Vec4i>> getGroupsOfLines();
};

