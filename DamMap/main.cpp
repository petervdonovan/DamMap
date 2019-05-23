
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
	String sourceReference = "record2.mp4";
	VideoCapture capture(sourceReference);
	int quadrant = 0; //Remove in final; we won't have quadrants in final
	if (!capture.isOpened())
	{
		cout << "Could not open reference " << sourceReference << endl;
		return -1;
	}

	
	//double contrast = 2;
	Mat image;
	capture >> image;
	image = getQuadrant(image, quadrant);
	History hist(image.rows, image.cols);

	////PERFORMANCE TEST
	//const clock_t startMaxChannel = clock();
	//for (int i = 0; i < 1000; i++) {
	//	getMaxChannelNearby(image, c);
	//}
	//std::cout << "time for getMaxChannelNearby: " << float(clock() - startMaxChannel) * 1000 / CLOCKS_PER_SEC << endl;

	//testing only
	capture.set(CAP_PROP_POS_MSEC, 43000);

	while (char(cv::waitKey(1)) != 'q') {
		const clock_t begin_t = clock();

		capture >> image;
		if (!capture.isOpened()) break;
		image = getQuadrant(image, quadrant);   //get quadrant 1 of the image
		std::cout << "Before frame: " << float(clock() - begin_t) * 1000 / CLOCKS_PER_SEC << endl;

		const clock_t begin_t1 = clock();
		Frame frame = Frame(image);
		std::cout << "frame: " << float(clock() - begin_t1) * 1000 / CLOCKS_PER_SEC << endl;

		const clock_t afterFrame = clock();
		const clock_t begin_t2 = clock();
		LineGroupList lineGroups = LineGroupList(frame);
		std::cout << "lineGroups: " << float(clock() - begin_t2) * 1000 / CLOCKS_PER_SEC << endl;

		lineGroups.showGroups();

		const clock_t begin_t3 = clock();
		hist.update(lineGroups.getGroupsOfLines());
		std::cout << "hist: " << float(clock() - begin_t3) * 1000 / CLOCKS_PER_SEC << endl;

		hist.showCurrent(image);
		hist.show();
		std::cout << "After frame: " << float(clock() - afterFrame) * 1000 / CLOCKS_PER_SEC << endl;

		std::cout << "inside while loop: " << float(clock() - begin_t) * 1000 / CLOCKS_PER_SEC << endl;
	}
	cv::waitKey(0);
}