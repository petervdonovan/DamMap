#include "LineGroupList.h"

LineGroupList::LineGroupList(Frame& fr, int rho, int thetaMinutes, int threshold, int minLineLength, int maxLineGap)
{
	this->frame = &fr;

	HoughLinesP(frame->getFinalCanny(), linesP, rho, CV_PI / 180 * thetaMinutes / 60, threshold, minLineLength, maxLineGap); // runs the actual detection

	//sorting linesP from longest to shortest line
	sort(linesP.begin(), linesP.end(), compareLine);

	while (linesP.size() > 0) {
		vector<Vec4i> lineGroup;
		Vec4i lineOfInterest;
		for (size_t i = 0; i < 4; i++) {
			lineOfInterest[i] = linesP[0][i];
		}
		lineGroup.push_back(lineOfInterest);
		linesP.erase(linesP.begin());
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
		groupsOfLines.push_back(lineGroup);
	}
}

LineGroupList::~LineGroupList()
{
}


void LineGroupList::showGroups() {
	Mat img;
	frame->getSource().copyTo(img);
	for (auto it = groupsOfLines.begin(); it != groupsOfLines.end(); it++) {
		Scalar color = Scalar(rand() % 255, rand() % 255, rand() % 255);
		for (auto j = (*it).begin(); j != (*it).end(); j++) {
			line(img, Point((*j)[0], (*j)[1]), Point((*j)[2], (*j)[3]), color, 1, 1);
		}
		//LineSimplified l = LineSimplified(*it);
		//cout << "Vertical: " << l.getVertical() << " DistFromTopLeft: " << l.getDistFromTopLeft() << endl;
		//Vec4i endPointL = l.getEndpoints(img.rows, img.cols);
		//line(img, Point(endPointL[0], endPointL[1]), Point(endPointL[2], endPointL[3]), color, 3, 1);
	}
	namedWindow("Image with Hough Lines");
	moveWindow("Image with Hough Lines", 500, 0);
	imshow("Image with Hough Lines", img);
}

const vector<vector<Vec4i>> LineGroupList::getGroupsOfLines() {
	return groupsOfLines;
}