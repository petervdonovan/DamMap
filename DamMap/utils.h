#pragma once
#include <iostream>
#include<string>

#include<opencv2/opencv.hpp>


using namespace std;
using namespace cv; 



unsigned int root(unsigned int x);
int distance(int x0, int y0, int x1, int y1);
double angleOfLine(Vec4i line);
bool compareLine(Vec4i line0, Vec4i line1);
int getAvgChannelNearby(const Mat& roi, int channel, int iterations = 7);
int getMaxChannelNearby(const Mat& roi, int channel, int iterations = 7);
bool linesAreSimilar(Vec4i line0, Vec4i line1, double angleThresh = 0.5, int distanceThresh = 20);
Vec4i getAvgHoughLine(vector<Vec4i> linesP);
bool moreVerticalThanHorizontal(double angle);
Mat getQuadrant(Mat& img, int quadrant);