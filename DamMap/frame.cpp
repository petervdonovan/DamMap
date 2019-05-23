#include "frame.h"
#include "utils.h"


void Frame::channelBinary(const Mat& src, Mat& out, double thresh, int skip) {
	Mat output;
	output.create(src.rows, src.cols, CV_8UC3);
	for (int c = 0; c < src.channels(); c++) {
		int maxChannel;
		//The image is split into squares of side length determined by skip. Each of these is processed individually to save time
		for (int xMajor = 0; xMajor < src.cols; xMajor += skip) {
			for (int yMajor = 0; yMajor < src.rows; yMajor += skip) {
				//Get region of interest
				Mat roi(src, Range(max(0, yMajor + skip / 2 - skip), min(yMajor + skip / 2 + skip, src.rows)), Range(max(0, xMajor + skip / 2 - skip), min(xMajor + skip / 2 + skip, src.cols)));
				//Get maximum value of nearby channels. This takes time, so it only happens once in each square region of interest
				maxChannel = getMaxChannelNearby(roi, c);
				for (int x = xMajor; x < src.cols && (x % skip != 0 || x == xMajor); x++) {
					for (int y = yMajor; y < src.rows && (y % skip != 0 || y == yMajor); y++) {
						//Set channels that are close to maximum value to 255 and all others to 0
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

int Frame::getAvgSaturationBackground(Mat& hsvMat, vector<cv::Mat>& foregroundBinaries) {
	int avg;
	Mat_<Vec3b> _hsvMat = hsvMat;
	for (int i = 0; i < 20; i++) {

		int col = rand() % hsvMat.cols; //get random column index
		int row = rand() % hsvMat.rows; //get random row index
		for (int j = 0; j < foregroundBinaries.size(); j++) { //Pick another pixel if it's a foreground pixel.
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
			avg = (avg * i + current) / (i + 1); //Average is weighted so that each sample matters equally even if taken at the end
		}
	}
	return avg;
}

Frame::Frame(Mat image)
{
	this->image = image;
	//Set channels all to 0 or 255
	channelBinary(image, imgChannelBinary, 0.97, 2);
	
	//Get all pixels whose channels were all darker than surrounding channels
	inRange(imgChannelBinary, Scalar(0, 0, 0), Scalar(20, 20, 20), darkerThanLocal);
	
	cvtColor(image, hsv, COLOR_BGR2HSV);
	
	vector<Mat> foregrounds(1);
	foregrounds[0] = darkerThanLocal.clone();
	int avg = getAvgSaturationBackground(hsv, foregrounds);
	inRange(hsv, Scalar(0, 0, 0), Scalar(255, 255, avg * 0.9), dark);//detect black (cracks/lines). This also includes shadows, however.
	
	eraseSmall(dark, dark); //Erase black gridlines, which are narrower than the red line
	darkerThanLocal -= dark;//Subtract the red line (and some shadows) from the binary of where the black gridlines likely are
	eraseSmallAbs(darkerThanLocal, darkerThanLocal); //Erase noise

	bitwise_not(darkerThanLocal, darkerThanLocal);
	Canny(image, canny, 15, 45, 3); //Canny that is so sensitive it detects noise
	canny -= darkerThanLocal;     //Erase noise, which is everything not near the black gridlines
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
