#pragma once
#include <opencv2/opencv.hpp>
#include "lineSimplified.h"


using namespace std;
using namespace cv;

class History
{
private:
	int rows, cols;   //Rows and columns of the image -- 
			//  needed to determine if a line has crossed a significant part of the ROV's view
	double borderThresh; //Size of the border in which new line groups can be recognized as lines.
			//Should be between 0 and 0.5. Smaller values are more likely to filter out noise
	vector<LineSimplified> currentLines; //Used to track lines that could be in the black grid
	vector<Point> points;  //Squares the ROV has been in
public:
	History(int rows, int cols, double borderThresh = 0.4);
	~History();

	void update(vector<vector<Vec4i>> lineGroups);
	void showCurrent(Mat); //Show simplified lines
	void show();  //Show the map
};
