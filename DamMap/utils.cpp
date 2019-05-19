#include "utils.h"

unsigned int root(unsigned int x) {
	unsigned int a, b;
	b = x;
	a = x = 0x3f;
	x = b / x;
	a = x = (x + a) >> 1;
	x = b / x;
	a = x = (x + a) >> 1;
	x = b / x;
	x = (x + a) >> 1;
	return(x);
}

int distance(int x0, int y0, int x1, int y1) {
	return root((x1 - x0)*(x1 - x0) + (y1 - y0)*(y1 - y0));
}


double angleOfLine(Vec4i line) {
	if ((line[2] - line[0]) == 0) return CV_PI / 2;
	return atan((line[3] - line[1]) / (line[2] - line[0]));
}

bool compareLine(Vec4i line0, Vec4i line1) {
	return distance(line0[0], line0[1], line0[2], line0[3]) > distance(line1[0], line1[1], line1[2], line1[3]);
}


int getAvgChannelNearby(const Mat& roi, int channel, int iterations) {
	int sum = 0;
	for (int i = 0; i < iterations; i++) {
		int col = rand() % roi.cols;
		int row = rand() % roi.rows;
		sum += roi.at<Vec3b>(row, col)[channel];
	}
	return sum / iterations;
}

int getMaxChannelNearby(const Mat& roi, int channel, int iterations) {
	int max = 0;
	for (int i = 0; i < iterations; i++) {
		int col = (i * 17) % roi.cols;
		int row = (i * 41) % roi.rows;
		int currentChannel = roi.at<Vec3b>(row, col)[channel];
		if (currentChannel > max) {
			max = currentChannel;
		}
	}
	return max;
}


bool linesAreSimilar(Vec4i line0, Vec4i line1, double angleThresh, int distanceThresh) {
	double angle0 = angleOfLine(line0);
	double angle1 = angleOfLine(line1);
	if (angleThresh < abs(angle0 - angle1) && abs(angle0 - angle1) < CV_PI - angleThresh) return false;
	//cout << line1[0] << " " << line1[1] << " " << line1[2] << " " << line1[3] << angle1 << endl;
	//cout << line0[0] << " " << line0[1] << " " << line0[2] << " " << line0[3] << angle1 << endl;
	int distance; //vertical or horizontal distance from line1, if it were extended forever, to first endpoint of line2
	if (moreVerticalThanHorizontal(angle1)) { // if line1 is closer to vertical than horizontal
		distance = ((cos(angle1) / sin(angle1)) * (line0[1] - line1[1]) + line1[0]) - line0[0];		//inverse function using point-slope form
		//cout << "distance thresh: " << distanceThresh << " distance horizontal: " << distance << endl;
	}
	else {
		double slope1 = (line1[3] - line1[1]) / (line1[2] - line1[0]);
		//cout << "point " << (slope1 * (double)(line0[0] - line1[0]) - line1[1]) << endl;
		//cout << "y value" << line0[1] << endl;
		distance = (tan(angle1) * (line0[0] - line1[0]) + line1[1]) - line0[1]; //point-slope form where x0 is line1[0] and y0 is line1[1]
		//cout << "distance vertical: " << distance << endl;
	}
	//cout << angle0 << endl;
	return abs(distance) < distanceThresh;
}

Vec4i getAvgHoughLine(vector<Vec4i> linesP) {
	int sumWeights = 0;
	int sumPositions = 0;
	int sumPositionsCrosswise = 0;
	double sumAngles = 0;

	int minPosition;
	int maxPosition;
	int meanPositionCrosswise; //This is the weighted mean of the x if vertical and weighted mean of the y if horizontal
	int angle;


	for (size_t i = 0; i < linesP.size(); i++) {
		int weight = distance(linesP[i][0], linesP[i][1], linesP[i][2], linesP[i][3]); //weight based on length of line segment
		sumWeights += weight;
		sumAngles += angleOfLine(linesP[i]) * weight;
	}
	angle = sumAngles / sumWeights; //get weighted mean

	sumWeights = 0; //reset weight

	bool vertical = moreVerticalThanHorizontal(angle);
	if (vertical) {
		minPosition = linesP[0][1];
		maxPosition = linesP[0][1];
		for (size_t i = 0; i < linesP.size(); i++) {						//get min and max positions approximately parallel to most of the lines
			if (linesP[i][1] < minPosition) minPosition = linesP[i][1];
			if (linesP[i][3] < minPosition) minPosition = linesP[i][3];
			if (linesP[i][1] > maxPosition) maxPosition = linesP[i][1];
			if (linesP[i][3] > maxPosition) maxPosition = linesP[i][3];
		}

		for (size_t i = 0; i < linesP.size(); i++) {		//get mean crosswise position
			int weight = distance(linesP[i][0], linesP[i][1], linesP[i][2], linesP[i][3]);
			sumWeights += weight;
			sumPositionsCrosswise += (linesP[i][0] + linesP[i][2]) / 2 * weight;
		}
		meanPositionCrosswise = sumPositionsCrosswise / sumWeights; //get weighted mean
	}
	else {
		minPosition = linesP[0][0];
		maxPosition = linesP[0][0];
		for (size_t i = 0; i < linesP.size(); i++) { 						//get min and max positions approximately parallel to most of the lines
			if (linesP[i][0] < minPosition) minPosition = linesP[i][0];
			if (linesP[i][2] < minPosition) minPosition = linesP[i][2];
			if (linesP[i][0] > maxPosition) maxPosition = linesP[i][0];
			if (linesP[i][2] > maxPosition) maxPosition = linesP[i][2];
		}

		for (size_t i = 0; i < linesP.size(); i++) {		//get mean crosswise position
			int weight = distance(linesP[i][0], linesP[i][1], linesP[i][2], linesP[i][3]);
			sumWeights += weight;
			sumPositionsCrosswise += (linesP[i][0] + linesP[i][2]) / 2 * weight;
		}
		meanPositionCrosswise = sumPositionsCrosswise / sumWeights; // get weighted mean
	}

	//This next part is the generation of an "average" line based on the weights

	Vec4i meanLine;
	if (vertical) {
		meanLine[1] = minPosition;
		meanLine[3] = maxPosition;
		int diff = maxPosition - minPosition;
		meanLine[0] = meanPositionCrosswise - (diff / 2) * tan(angle);
		meanLine[2] = meanPositionCrosswise + (diff / 2) * tan(angle);
	}
	else {
		meanLine[0] = minPosition;
		meanLine[2] = maxPosition;
		int diff = maxPosition - minPosition;
		double cot = cos(angle) / sin(angle);
		meanLine[1] = meanPositionCrosswise - (diff / 2) * cot;
		meanLine[3] = meanPositionCrosswise + (diff / 2) * cot;
	}
	return meanLine;
}


bool moreVerticalThanHorizontal(double angle) {
	return abs(abs(angle) - CV_PI / 2) < CV_PI / 4;
}

Mat getQuadrant(Mat& img, int quadrant) {
	Mat roi;
	if (quadrant == 1) {
		roi = img(Range(0, img.rows / 2), Range(img.cols / 2, img.cols - 1));
	}
	else if (quadrant == 2) {
		roi = img(Range(0, img.rows / 2), Range(0, img.cols / 2));
	}
	else if (quadrant == 3) {
		roi = img(Range(img.rows / 2, img.rows - 1), Range(0, img.cols / 2));
	}
	else {
		roi = img(Range(img.rows / 2, img.rows - 1), Range(img.cols / 2, img.cols - 1));
	}
	//roi = roi(Range(roi.rows / 8, roi.rows / 10 * 9), Range(0, roi.cols / 10 * 7));
	roi = roi(Range(roi.rows / 7, roi.rows * 6 / 7), Range(roi.cols / 7, roi.cols * 3 / 5));
	return roi;
}