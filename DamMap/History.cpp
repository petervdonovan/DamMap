#include "History.h"

History::History(int rows, int cols, double borderThresh)
{
	this->rows = rows;
	this->cols = cols;
	this->borderThresh = borderThresh;
	this->points.push_back(Point(0, 0));
}


History::~History()
{
}

void History::update(vector<vector<Vec4i>> groupsOfLines, int crossedThresh, int vanishedThresh, int distThreshForSimilarity) {
	//First look in the groups of lines for matches for each existing line
	for (auto i = currentLines.begin(); i != currentLines.end();) {
		bool found = false; //Start by assuming no match for the given current line is found. 
							//If none is found, that will be recorded for possible deletion of the line
		for (auto group = groupsOfLines.begin(); group != groupsOfLines.end(); ) {
			if (i->update(*group)) {
				found = true;
				group = groupsOfLines.erase(group);		//if this group of lines matches this current line, it is removed
			}
			else {
				++group;
			}
		}
		if (!found) i->incrementVanished();
		int distMustCross = i->getVertical() ? cols * 3 / 5 : rows * 3 / 5;
		if (i->crossed(distMustCross)) {
			cout << "crossed into new square ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! " << endl;
			if (i->getVertical()) {
				if (i->getDistFromTopLeft() < cols / 2) {
					points.push_back(Point(points.back().x + 1, points.back().y));
				}
				else {
					//cout << i->getDistFromTopLeft() << " and " << cols / 2;
					points.push_back(Point(points.back().x - 1, points.back().y));
				}
			}
			else {
				if (i->getDistFromTopLeft() < rows / 2) {
					points.push_back(Point(points.back().x, points.back().y + 1));
				}
				else {
					points.push_back(Point(points.back().x, points.back().y - 1));
				}
			}
			i = currentLines.erase(i);
		}
		else if (i->getVanished() > vanishedThresh) {
			i = currentLines.erase(i);
		}
		else {
			++i;
		}
	}
	//look at remaining groups of lines for groups that are near the edge of the camera's view
	//These are the groups that recently appeared and may be new lines that have entered the camera's view
	//(as opposed to having just appeared as random noise)
	for (auto group = groupsOfLines.begin(); group != groupsOfLines.end(); ++group) {
		int totalLength = 0;
		for (int i = 0; i < group->size(); i++) {
			totalLength += distance((*group)[i][0], (*group)[i][1], (*group)[i][2], (*group)[i][3]);
		}
		if (totalLength > rows) {
			LineSimplified newLine = LineSimplified(*group, distThreshForSimilarity);
			int dim = (newLine.getVertical() ? cols : rows);
			if (newLine.getDistFromTopLeft() < dim * borderThresh || newLine.getDistFromTopLeft() > dim * (1 - borderThresh)) {
				currentLines.push_back(newLine);
			}
		}
	}
}

void History::showCurrent(Mat img) {
	for (size_t i = 0; i < currentLines.size(); i++) {
		//generate arbitrary color that is always the same for a given index in the currentLines array
		Scalar color = Scalar((i * 92) % 255, (i * 193) % 255, (i * 912) % 255);
		Vec4i endPointL = currentLines[i].getEndpoints(img.rows, img.cols);
		line(img, Point(endPointL[0], endPointL[1]), Point(endPointL[2], endPointL[3]), color, 3, 1);
	}
	namedWindow("History");
	moveWindow("History", 1000, 0);
	imshow("History", img);
}

void History::show() {
	Mat map(600, 600, CV_8UC1, Scalar(0));
	for (size_t i = 0; i < points.size(); i++) {
		Rect rec(275 + points[i].x * 50, 275 + points[i].y * 50, 50, 50);
		putText(map, to_string(i), Point(290 + points[i].x * 50, 305 + points[i].y * 50), 0,
			1, Scalar(255), 3, 8);
		rectangle(map, rec, Scalar(255));
	}
	namedWindow("Map");
	moveWindow("Map", 0, 500);
	imshow("Map", map);
	guessMap();
}

void History::guessMap() {
	//Assume all to be true and find correct one by process of elimination
	bool grid1 = true, grid2 = true, grid3 = true, grid4 = true, grid5 = true, grid6 = true;
	if (points.size() > 1) {
		if (points[1].x > points[0].x) { //If it starts by moving right
			grid1 = false;
			grid2 = false;
		}
		else {
			grid3 = false;
			grid4 = false;
			grid5 = false;
			grid6 = false;
		}
	}
	if (points.size() > 2) {
		int verticalCount = 0;
		int horizontalCount = 0;
		//iterate through all squares except first and (last - 1)
		for (size_t i = 1; i < min(points.size(), (size_t) 13) - 1; i++) { 
			if (points[i + 1].y != points[i].y) verticalCount++;
			else horizontalCount++;
		}
		if (verticalCount > horizontalCount) {
			grid1 = false;
			grid3 = false;
			grid5 = false;
		}
		else if (horizontalCount > verticalCount) {
			grid2 = false;
			grid4 = false;
			grid6 = false;
		}
	}
	cout << "grid1: " << grid1 << endl;
	cout << "grid2: " << grid2 << endl;
	cout << "grid3: " << grid3 << endl;
	cout << "grid4: " << grid4 << endl;
	cout << "grid5: " << grid5 << endl;
	cout << "grid6: " << grid6 << endl;
	//TODO: Find a way to determine if it's reached the end, and use that to tell 
	//the difference between 4 and 6 or between 3 and 5.
	//Just counting the number of squares it has gone through is too unreliable
}