
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
	String sourceReference = "record4.mp4";
	VideoCapture capture(sourceReference);
	namedWindow("Get ROI variables", WINDOW_AUTOSIZE);
	int leftPercent, rightPercent, topPercent, bottomPercent, maxPixDim, quadrant;
	leftPercent = 14;
	rightPercent = 40;
	topPercent = 25;
	bottomPercent = 25;
	quadrant = 0; //quadrant probably should not exist in final
	maxPixDim = 250;
	createTrackbar("Left margin", "Get ROI variables", &leftPercent, 100);
	createTrackbar("Right margin", "Get ROI variables", &rightPercent, 100);
	createTrackbar("Top margin", "Get ROI variables", &topPercent, 100);
	createTrackbar("Bottom margin", "Get ROI variables", &bottomPercent, 100);
	createTrackbar("Max dimension (px)", "Get ROI variables", &maxPixDim, 500);
	createTrackbar("Quadrant", "Get ROI variables", &quadrant, 4);
	if (!capture.isOpened())
	{
		cout << "Could not open reference " << sourceReference << endl;
		return -1;
	}

	
	//double contrast = 2;
	Mat image;
	capture >> image;
	image = getQuadrant(image, quadrant, leftPercent, topPercent, rightPercent, bottomPercent, maxPixDim);
	History hist(image.rows, image.cols);
	//cout << "rows: " << image.rows << " cols: " << image.cols << endl;
	//testing only
	//capture.set(CAP_PROP_POS_FRAMES, 2640);

	namedWindow("Frame (preprocessing) variables", WINDOW_AUTOSIZE);
	int erodeIterations, cannyThresh1, cannyThresh2Percent, cannyAperture, darkThreshPercent, redMinHue, redMaxHue, minInverseProportionOfImage/*, dilateKernel2Dim, imgEdgeProp*/;
	erodeIterations = 20;
	cannyThresh1 = 13;
	cannyThresh2Percent = 1000;
	cannyAperture = 3;
	darkThreshPercent = 90;
	redMinHue = 17;//redMinHue = 160;
	redMaxHue = 37;//redMaxHue = 200;
	minInverseProportionOfImage = 6;
	//dilateKernel2Dim = 3;
	//imgEdgeProp = 7;
	createTrackbar("Erode Iterations", "Frame (preprocessing) variables", &erodeIterations, 200);
	//createTrackbar("Canny Threshold 1", "Frame (preprocessing) variables", &cannyThresh1, 45);
	//createTrackbar("Canny Threshold 2 percent", "Frame (preprocessing) variables", &cannyThresh2Percent, 400);
	//createTrackbar("Canny Aperture", "Frame (preprocessing) variables", &cannyAperture, 7);
	createTrackbar("Dark Threshold Percent", "Frame (preprocessing) variables", &darkThreshPercent, 100);
	createTrackbar("Min Inv", "Frame (preprocessing) variables", &minInverseProportionOfImage, 25);
	createTrackbar("Red Min Hue", "Frame (preprocessing) variables", &redMinHue, 360);
	createTrackbar("Red Max Hue", "Frame (preprocessing) variables", &redMaxHue, 360);
	//createTrackbar("DilateKernel2Dim", "Frame (preprocessing) variables", &dilateKernel2Dim, 17);
	//createTrackbar("imgEdgeProp", "Frame (preprocessing) variables", &imgEdgeProp, 15);



	namedWindow("Line Extraction and Grouping Variables", WINDOW_AUTOSIZE);
	int rho, thetaMinutes, threshold, minLineLength, maxLineGap;
	rho = 1;
	thetaMinutes = 60;
	threshold = 50/*3*/;
	minLineLength = 35;
	maxLineGap = 52;

	namedWindow("History Variables", WINDOW_AUTOSIZE);
	int crossedThresh, vanishedThresh, distThreshForSimilarity;
	crossedThresh = 90;
	vanishedThresh = 7;
	distThreshForSimilarity = 30;

	//TEST
	int permilThresh = 1500;
	namedWindow("adaptiveThreshold", WINDOW_AUTOSIZE);
	createTrackbar("C", "adaptiveThreshold", &permilThresh, 5000);

	while (char(cv::waitKey(1)) != 'q') {
		createTrackbar("Rho (px)", "Line Extraction and Grouping Variables", &rho, 20);
		createTrackbar("Theta (minutes)", "Line Extraction and Grouping Variables", &thetaMinutes, 1200);
		createTrackbar("Threshold", "Line Extraction and Grouping Variables", &threshold, 120);
		createTrackbar("minLineLength", "Line Extraction and Grouping Variables", &minLineLength, image.rows);
		createTrackbar("maxLineGap", "Line Extraction and Grouping Variables", &maxLineGap, image.rows / 2);

		createTrackbar("Crossed Thresh (px)", "History Variables", &crossedThresh, min(image.rows, image.cols));
		createTrackbar("Vanished Thresh", "History Variables", &vanishedThresh, 10);
		createTrackbar("Dist Thresh for Similarity", "History Variables", &distThreshForSimilarity, min(image.rows, image.cols));

		const clock_t begin_t = clock();

		//Get image
		capture >> image;
		imshow("src", image);
		if (!capture.isOpened()) break;
		image = getQuadrant(image, quadrant, leftPercent, topPercent, rightPercent, bottomPercent, maxPixDim);   //get quadrant 1 of the image
		std::cout << "Before frame: " << float(clock() - begin_t) * 1000 / CLOCKS_PER_SEC << endl;

		//Preprocessing
		const clock_t begin_t1 = clock();
		Frame frame = Frame(image, erodeIterations, cannyThresh1, cannyThresh2Percent / 100.0, max(3, cannyAperture / 2 * 2 + 1), darkThreshPercent / 100.0, redMinHue, redMaxHue, permilThresh, minInverseProportionOfImage/*, max(3, dilateKernel2Dim / 2 * 2 + 1), imgEdgeProp*/);
		std::cout << "frame: " << float(clock() - begin_t1) * 1000 / CLOCKS_PER_SEC << endl;

		//Extract line groups
		const clock_t afterFrame = clock();
		const clock_t begin_t2 = clock();
		LineGroupList lineGroups = LineGroupList(frame, rho, thetaMinutes, threshold, minLineLength, maxLineGap);
		std::cout << "lineGroups: " << float(clock() - begin_t2) * 1000 / CLOCKS_PER_SEC << endl;

		lineGroups.showGroups();

		//update history and map
		const clock_t begin_t3 = clock();
		hist.update(lineGroups.getGroupsOfLines(), crossedThresh, vanishedThresh, distThreshForSimilarity);
		std::cout << "hist: " << float(clock() - begin_t3) * 1000 / CLOCKS_PER_SEC << endl;

		//show map
		hist.showCurrent(image);
		hist.show();
		std::cout << "After frame: " << float(clock() - afterFrame) * 1000 / CLOCKS_PER_SEC << endl;

		std::cout << "inside while loop: " << float(clock() - begin_t) * 1000 / CLOCKS_PER_SEC << endl;
	}
	cv::waitKey(500);
}