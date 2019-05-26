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
		if (i->crossed(min(rows * 3 / 5, cols * 3 / 5))) {
			cout << "crossed into new square ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! " << endl;
			if (i->getVertical()) {
				if (i->getDistFromTopLeft() < cols / 2) {
					points.push_back(Point(points.back().x + 1, points.back().y));
				}
				else {
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
}