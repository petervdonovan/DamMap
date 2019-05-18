
//try.cpp : This file contains the 'main' function. Program execution begins and ends there.

//#include "pch.h"
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

	
	//double contrast = 2;
	Mat image;
	capture >> image;
	image = getQuadrant(image, 1);
	History hist(image.rows, image.cols);

	////PERFORMANCE TEST
	//const clock_t startMaxChannel = clock();
	//for (int i = 0; i < 1000; i++) {
	//	getMaxChannelNearby(image, c);
	//}
	//std::cout << "time for getMaxChannelNearby: " << float(clock() - startMaxChannel) * 1000 / CLOCKS_PER_SEC << endl;


	while (char(cv::waitKey(1)) != 'q') {
		//const clock_t begin_time = clock();
		for (int i = 0; i < 1; i++) {   //skip through six images
			capture >> image;
		}
		if (!capture.isOpened()) break;
		image = getQuadrant(image, 1);   //get quadrant 1 of the image

		Frame frame = Frame(image);

		LineGroupList lineGroups = LineGroupList(frame);
		lineGroups.showGroups();
		hist.update(lineGroups.getGroupsOfLines());
		hist.showCurrent(image);
		hist.show();
		//std::cout << "inside while loop: " << float(clock() - begin_time) * 1000 / CLOCKS_PER_SEC << endl;
	}
	cv::waitKey(0);
}