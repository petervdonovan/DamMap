#include "lineSimplified.h"
bool LineSimplified::isSimilar(vector<Vec4i> lines) {
	bool groupIsVertical = moreVerticalThanHorizontal(meanAngle(lines));
	int dist = meanDist(lines, vertical);
	return (groupIsVertical == vertical) && abs(dist - meanRecentDists()) < distThreshForSimilarity;
}

double LineSimplified::meanAngle(vector<Vec4i> lines) {
	int numSamples = 0;
	double sumAngles = 0, meanAngle;
	if (lines.size() < 6) {
		for (auto it = lines.begin(); it != lines.end(); ++it) {
			//Account for if the angle is close to -pi/2 but most lines are close to +pi/2
			if (numSamples > 1 && (abs(angleOfLine(*it) + CV_PI) - sumAngles / numSamples) < abs(angleOfLine(*it) - sumAngles / numSamples)) {
				sumAngles += angleOfLine(*it) + CV_PI;
			}
			//Account for if the angle is close to +pi/2 but most lines are close to -pi/2
			else if (numSamples > 1 && (abs(angleOfLine(*it) - CV_PI) - sumAngles / numSamples) < abs(angleOfLine(*it) - sumAngles / numSamples)) {
				sumAngles += angleOfLine(*it) - CV_PI;
			}
			else {
				sumAngles += angleOfLine(*it);
			}
			numSamples++;
		}
	}
	else {
		for (int i = 0; i < 5; ++i) {
			Vec4i sample = lines[rand() % lines.size()];
			//Account for if the angle is close to -pi/2 but most lines are close to +pi/2
			if (numSamples > 1 && (abs(angleOfLine(sample) + CV_PI) - sumAngles / numSamples) < abs(angleOfLine(sample) - sumAngles / numSamples)) {
				sumAngles += angleOfLine(sample) + CV_PI;
			}
			//Account for if the angle is close to +pi/2 but most lines are close to -pi/2
			else if (numSamples > 1 && (abs(angleOfLine(sample) - CV_PI) - sumAngles / numSamples) < abs(angleOfLine(sample) - sumAngles / numSamples)) {
				sumAngles += angleOfLine(sample) - CV_PI;
			}
			else {
				sumAngles += angleOfLine(sample);
			}
			numSamples++;
		}
	}
	meanAngle = sumAngles / numSamples;
	return meanAngle;
}
int LineSimplified::meanDist(vector<Vec4i> lines, bool vertical) {
	int numSamples = 0;
	double sumDists = 0, meanDist;
	if (vertical) {
		if (lines.size() < 6) {
			for (auto it = lines.begin(); it != lines.end(); ++it) {
				sumDists += (*it)[0] + (*it)[2];
				numSamples += 2;
			}
		}
		else {
			for (int i = 0; i < 5; ++i) {
				sumDists += lines[rand() % lines.size()][0] + lines[rand() % lines.size()][2];
				numSamples += 2;
			}
		}
	}
	else {
		if (lines.size() < 6) {
			for (auto it = lines.begin(); it != lines.end(); ++it) {
				sumDists += (*it)[1] + (*it)[3];
				numSamples += 2;
			}
		}
		else {
			for (int i = 0; i < 5; ++i) {
				sumDists += lines[rand() % lines.size()][1] + lines[rand() % lines.size()][3];
				numSamples += 2;
			}
		}
	}
	meanDist = sumDists / numSamples;
	return meanDist;
}

LineSimplified::LineSimplified(vector<Vec4i> lines, int distThreshForSimilarity)
{
	this->distThreshForSimilarity = distThreshForSimilarity;
	vertical = abs(meanAngle(lines)) > CV_PI / 4;
	distFromTopLeft.push_back(meanDist(lines, vertical));
	vanished = 0;
}

LineSimplified::~LineSimplified()
{
}

const bool LineSimplified::getVertical() {
	return vertical;
}

const int LineSimplified::getDistFromTopLeft() {
	return distFromTopLeft.back();
}

bool LineSimplified::update(vector<Vec4i> lines) {
	if (!isSimilar(lines)) {
		return false;
	}
	distFromTopLeft.push_back(meanDist(lines, vertical));
	vanished = 0;
	return true;
}

void LineSimplified::incrementVanished() {
	vanished++;
}

int LineSimplified::getVanished() {
	return vanished;
}

Vec4i LineSimplified::getEndpoints(int rows, int cols) {
	Vec4i endpointLine;
	if (vertical) {
		endpointLine[0] = distFromTopLeft.back();
		endpointLine[1] = 0;
		endpointLine[2] = distFromTopLeft.back();
		endpointLine[3] = rows - 1;
	}
	else {
		endpointLine[0] = 0;
		endpointLine[1] = getDistFromTopLeft();
		endpointLine[2] = cols - 1;
		endpointLine[3] = getDistFromTopLeft();
	}
	return endpointLine;
}

const bool LineSimplified::crossed(int distThresh) {
	return vanished && abs(distFromTopLeft.back() - distFromTopLeft.front()) > distThresh;
}

const int LineSimplified::meanRecentDists() {
	int sumRecentDists = 0;
	int numRecentDists = min((int)distFromTopLeft.size(), 3);
	for (int it = 1; it <= numRecentDists; it++) {
		sumRecentDists += distFromTopLeft[distFromTopLeft.size() - it];
	}
	return sumRecentDists / numRecentDists;
}
