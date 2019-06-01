#pragma once
#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

#include "utils.h"


using namespace std;
using namespace cv;


class Frame
{
private:
	Mat image, imgChannelBinary, hsv, blue, red, darkerThanLocal, dark, mask, canny;

	static Mat old;

	void channelBinary(const Mat& src, Mat& out, double thresh, int skip = 50);
	int getAvgSaturationBackground(Mat& hsvMat/*, vector<cv::Mat>& foregroundBinaries*/);
	void eraseSmall(const Mat& srcBin, Mat& outBin, double erodeSize = 0.03, double dilateSize = 0.08);
	void eraseSmallAbs(const Mat& srcBin, Mat& outBin, int erodeSize = 2, int dilateSize = 5);
	
public:
	Frame(Mat &image, int erodeIterations = 48, int cannyThresh1 = 15, double cannyRatio = 45, int cannyAperture = 3, double darkThresh = 0.9, int redMinHue = 160, int redMaxHue = 200, int permilThresh = 2000/*, int dilateKernel2Dim = 9, int imgEdgeProp = 7*/);
	~Frame();

	const Mat& getChannelBinary();
	const Mat& getFinalMask();
	const Mat& getFinalCanny();
	const Mat& getSource();

	void showSource();
	void showFinalMask();
	void showFinalCanny();
};

