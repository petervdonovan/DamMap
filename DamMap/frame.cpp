#include "frame.h"
#include "utils.h"


void Frame::channelBinary(const Mat& src, Mat& out, double thresh, int skip) {
	Mat output;
	output.create(src.rows, src.cols, CV_8UC3);
	for (int c = 0; c < src.channels(); c++) {
		int maxChannel;
		for (int xMajor = 0; xMajor < src.cols; xMajor += skip) {
			for (int yMajor = 0; yMajor < src.rows; yMajor += skip) {
				Mat roi(src, Range(max(0, yMajor + skip / 2 - skip), min(yMajor + skip / 2 + skip, src.rows)), Range(max(0, xMajor + skip / 2 - skip), min(xMajor + skip / 2 + skip, src.cols)));
				maxChannel = getMaxChannelNearby(roi, c);
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

int Frame::getAvgSaturationBackground(Mat& hsvMat/*, vector<cv::Mat>& foregroundBinaries*/) {
	int avg;
	Mat_<Vec3b> _hsvMat = hsvMat;
	for (int i = 0; i < 7; i++) {

		int col = (i * 672) % hsvMat.cols; //get random column index
		int row = (i * 968) % hsvMat.rows; //get random row    index
		//for (int j = 0; j < foregroundBinaries.size(); j++) { //Pick another pixel if it's a foreground pixel. (THIS PROBABLY DOES TAKE A LONG TIME.)
		//	if (foregroundBinaries[j].at<uchar>(row, col) != 0) {  
		//		col = rand() % hsvMat.cols;
		//		row = rand() % hsvMat.rows;
		//		j = 0;
		//	}
		//}
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

Frame::Frame(Mat &image, int erodeIterations, int cannyThresh1, double cannyRatio, int cannyAperture, double darkThresh, int redMinHue, int redMaxHue)
{
	const clock_t begin_t = clock();
	this->image = image;
	std::cout << "1: " << float(clock() - begin_t) * 1000 / CLOCKS_PER_SEC << endl;

	cvtColor(image, hsv, COLOR_BGR2HSV);

	std::cout << "2: " << float(clock() - begin_t) * 1000 / CLOCKS_PER_SEC << endl;

	//vector<Mat> foregrounds(0);
	int avg = getAvgSaturationBackground(hsv/*, foregrounds*/);
	std::cout << "2.5: " << float(clock() - begin_t) * 1000 / CLOCKS_PER_SEC << endl;
	inRange(hsv, Scalar(0, 0, 0), Scalar(255, 255, avg * darkThresh), dark);		//detect all that is not white background
	//Mat red;
	//inRange(hsv, Scalar(redMinHue, 0, 0), Scalar(redMaxHue, 255, 255), red);
	//dark += red;

	std::cout << "3: " << float(clock() - begin_t) * 1000 / CLOCKS_PER_SEC << endl;

	eraseSmall(dark, dark);

	std::cout << "3.5: " << float(clock() - begin_t) * 1000 / CLOCKS_PER_SEC << endl;

	Canny(image, canny, cannyThresh1, cannyThresh1 * cannyRatio, cannyAperture);

	std::cout << "4: " << float(clock() - begin_t) * 1000 / CLOCKS_PER_SEC << endl;
	
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

	std::cout << "5: " << float(clock() - begin_t) * 1000 / CLOCKS_PER_SEC << endl;

	for (int i = 0; i < erodeIterations; i++) {
		Mat subtract;
		cv::morphologyEx(canny, subtract, MORPH_HITMISS, kernels[i % 4]);
		canny -= subtract;
	}

	std::cout << "6: " << float(clock() - begin_t) * 1000 / CLOCKS_PER_SEC << endl;

	canny -= dark;
	showFinalCanny();


	std::cout << "7: " << float(clock() - begin_t) * 1000 / CLOCKS_PER_SEC << endl;
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
	namedWindow("Canny: ");
	moveWindow("Canny: ", 0, 0);
	imshow("Canny: ", canny);
}