#include "frame.h"
#include "utils.h"


void Frame::channelBinary(const Mat& src, Mat& out, double thresh, int skip) {
	Mat output;
	output.create(src.rows, src.cols, CV_8UC3);
	//const clock_t begin_time = clock();
	for (int c = 0; c < src.channels(); c++) {
		int maxChannel;
		for (int xMajor = 0; xMajor < src.cols; xMajor += skip) {
			for (int yMajor = 0; yMajor < src.rows; yMajor += skip) {
				Mat roi(src, Range(max(0, yMajor + skip / 2 - skip), min(yMajor + skip / 2 + skip, src.rows)), Range(max(0, xMajor + skip / 2 - skip), min(xMajor + skip / 2 + skip, src.cols)));
				//Mat roi(src, Range(max(0, yMajor + 3 - margin), min(yMajor + 3 + margin, src.rows - 1)), Range(max(0, xMajor + 3 - margin), min(xMajor + 3 + margin, src.cols - 1)));
				//const clock_t startMaxChannel = clock();
				maxChannel = getMaxChannelNearby(roi, c);
				//std::cout << "time for getMaxChannelNearby: " << float(clock() - startMaxChannel) * 1000 / CLOCKS_PER_SEC << endl;
				for (int x = xMajor; x < src.cols && (x % skip != 0 || x == xMajor); x++) {
					for (int y = yMajor; y < src.rows && (y % skip != 0 || y == yMajor); y++) {
						if (src.at<Vec3b>(y, x)[c] > maxChannel * thresh) {
							output.at<Vec3b>(y, x)[c] = 255;
						}
						else {
							output.at<Vec3b>(y, x)[c] = 0;
						}
					}
				}
			}
		}
	}
	//std::cout << "inside channelBinary: " << float(clock() - begin_time) * 1000 / CLOCKS_PER_SEC << endl;

	out = output.clone();
}

//Precondition: colSize and rowSize are proportions of the frame width
void Frame::eraseSmall(const Mat& srcBin, Mat& outBin, double erodeSize, double dilateSize) {
	//declare erode and dilate elements
	int i = image.cols;
	Mat erodeElement = getStructuringElement(MORPH_RECT,
		Size(i * erodeSize, i * erodeSize),
		Point(-1, -1));
	Mat dilateElement = getStructuringElement(MORPH_RECT,
		Size(i * dilateSize, i * dilateSize),
		Point(-1, -1));
	//Erode and dilate
	erode(srcBin, outBin, erodeElement);
	dilate(outBin, outBin, dilateElement);
}

void Frame::eraseSmallAbs(const Mat& srcBin, Mat& outBin, int erodeSize, int dilateSize) {
	//declare erode and dilate elements
	Mat erodeElement = getStructuringElement(MORPH_RECT,
		Size(erodeSize, erodeSize),
		Point(-1, -1));
	Mat dilateElement = getStructuringElement(MORPH_RECT,
		Size(dilateSize, dilateSize),
		Point(-1, -1));

	//Erode and dilate
	erode(srcBin, outBin, erodeElement);
	dilate(outBin, outBin, dilateElement);
}

int Frame::getAvgSaturationBackground(Mat& hsvMat, vector<cv::Mat>& foregroundBinaries) {
	int avg;
	Mat_<Vec3b> _hsvMat = hsvMat;
	for (int i = 0; i < 20; i++) {

		int col = rand() % hsvMat.cols; //get random column index
		int row = rand() % hsvMat.rows; //get random row    index
		for (int j = 0; j < foregroundBinaries.size(); j++) { //Pick another pixel if it's a foreground pixel. (THIS PROBABLY DOES TAKE A LONG TIME.)
			if (foregroundBinaries[j].at<uchar>(row, col) != 0) {  
				col = rand() % hsvMat.cols;
				row = rand() % hsvMat.rows;
				j = 0;
			}
		}
		int current = _hsvMat(row, col)[2];
		if (i == 0) {
			avg = i;
		}
		else {
			avg = (avg * i + current) / (i + 1);
		}
	}
	return avg;
}

Frame::Frame(Mat image)
{
	this->image = image;
	//channelBinary(image, imgChannelBinary, 0.97, 2);

	//inRange(imgChannelBinary, Scalar(0, 0, 0), Scalar(20, 20, 40), darkerThanLocal);
	//cvtColor(image, hsv, COLOR_BGR2HSV);

	//vector<Mat> foregrounds(1);
	//foregrounds[0] = darkerThanLocal.clone();
	//int avg = getAvgSaturationBackground(hsv, foregrounds);
	//inRange(hsv, Scalar(0, 0, 0), Scalar(255, 255, avg * 0.9), dark);		//detect all that is not white background
	//Mat red;
	//inRange(hsv, Scalar(160, 0, 0), Scalar(200, 255, 255), red);
	//dark += red;

	//eraseSmall(dark, dark);
	////imshow("darkness subtracted", dark);
	//darkerThanLocal -= dark;
	//eraseSmallAbs(darkerThanLocal, darkerThanLocal);
	//
	////imshow("darkerthanlocal", darkerThanLocal);
	//bitwise_not(darkerThanLocal, darkerThanLocal);
	Canny(image, canny, 15, 45, 3);
	
	Mat kernels[4] = { (Mat_<int>(3, 3) <<
		0, -1, -1,
		0, 1, -1,
		0, -1, -1),
	(Mat_<int>(3, 3) <<
		0, 0, 0,
		-1, 1, -1,
		-1, -1, -1),
	(Mat_<int>(3, 3) <<
		-1, -1, 0,
		-1, 1, 0,
		-1, -1, 0),
	(Mat_<int>(3, 3) <<
		-1, -1, -1,
		-1, 1, -1,
		0, 0, 0) };
	for (int i = 0; i < 48; i++) {
		Mat subtract;
		cv::morphologyEx(canny, subtract, MORPH_HITMISS, kernels[i % 4]);
		canny -= subtract;
	}
	imshow("canny", canny);
	//canny -= darkerThanLocal;
}


Frame::~Frame()
{
}

const Mat& Frame::getChannelBinary() {
	return imgChannelBinary;
}
const Mat& Frame::getFinalMask() {
	return darkerThanLocal;
}
const Mat& Frame::getFinalCanny() {
	return canny;
}
const Mat& Frame::getSource() {
	return image;
}

void Frame::showSource() {
	imshow("Source image: ", image);
}
void Frame::showFinalMask() {
	imshow("Mask: ", darkerThanLocal);
}
void Frame::showFinalCanny() {
	imshow("Canny: ", canny);
}