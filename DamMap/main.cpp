#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

#include "utils.h"
#include "frame.h"
#include "LineGroupList.h"
#include "History.h"


using namespace std;
using namespace cv;


int main()
{
	String sourceReference = "lineFollow.mp4";
	VideoCapture capture(sourceReference);
	if (!capture.isOpened())
	{
		cout << "Could not open reference " << sourceReference << endl;
		return -1;
	}

	Mat image;
	capture >> image;
	image = getQuadrant(image, 1);
	History hist(image.rows, image.cols);

	while (char(cv::waitKey(1)) != 'q') {
		capture >> image;
		if (!capture.isOpened()) break;
		image = getQuadrant(image, 1);   //get quadrant 1 of the image

		Frame frame = Frame(image);

		LineGroupList lineGroups = LineGroupList(frame);
		lineGroups.showGroups(); 	//Show groups of lines (for debugging)
		hist.update(lineGroups.getGroupsOfLines()); //Update map
		hist.showCurrent(image);	//Show simplified lines (for debugging)
		hist.show();	//Show map
	}
	cv::waitKey(0);
}
