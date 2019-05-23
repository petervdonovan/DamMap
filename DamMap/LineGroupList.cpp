#include "LineGroupList.h"



LineGroupList::LineGroupList(Frame& fr)
{
	this->frame = &fr;

	HoughLinesP(frame->getFinalCanny(), linesP, 1, CV_PI / 180, 30, 5, 16); // runs the actual detection

	//sorting linesP from longest to shortest line
	sort(linesP.begin(), linesP.end(), compareLine);

	//iterate through lines detected by probabilistic Hough detector
	while (linesP.size() > 0) {
		vector<Vec4i> lineGroup;
		Vec4i lineOfInterest;
		//Move line from linesP to lineGroup
		for (size_t i = 0; i < 4; i++) {
			lineOfInterest[i] = linesP[0][i];
		}
		lineGroup.push_back(lineOfInterest);
		linesP.erase(linesP.begin());
		//Search for similar lines in linesP to try to group them together with lineOfInterest
		for (auto it = linesP.begin(); it != linesP.end(); ) {
			if (linesAreSimilar(*it, lineOfInterest)) {
				Vec4i newLine;
				for (size_t j = 0; j < 4; j++) {  //copy current line in linesP to a new line
					newLine[j] = (*it)[j];
				}
				lineGroup.push_back(newLine);
				it = linesP.erase(it);
			}
			else {
				++it;
			}
		}
		//Store a group of lines that are similar to each other
		groupsOfLines.push_back(lineGroup);
	}
}

LineGroupList::~LineGroupList()
{
}

//Show groups of lines (for debugging)
void LineGroupList::showGroups() {
	Mat img;
	frame->getSource().copyTo(img); //Separate copy to avoid changing the source image
	for (auto it = groupsOfLines.begin(); it != groupsOfLines.end(); it++) {
		//Give each line group a random color, so that it can be seen that they are correctly grouped together
		Scalar color = Scalar(rand() % 255, rand() % 255, rand() % 255);
		for (auto j = (*it).begin(); j != (*it).end(); j++) {
			//Draw each line in the line group
			line(img, Point((*j)[0], (*j)[1]), Point((*j)[2], (*j)[3]), color, 1, 1);
		}
	}
	imshow("image with hough lines p", img);
}

const vector<vector<Vec4i>> LineGroupList::getGroupsOfLines() {
	return groupsOfLines;
}
