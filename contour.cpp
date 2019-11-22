#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	Mat img = imread(argv[1]), img_view, img_hsv, img_mask, img_ec;
	pyrDown(img, img_view);
	pyrDown(img_view, img_view);
//	imshow("Image", img_view);
	cvtColor(img_view, img_hsv, COLOR_BGR2HSV);
	inRange(img_hsv, Scalar(100, 80, 100), Scalar(124, 255, 233), img_mask); // Select the blue pixel
	imshow("Mask", img_mask);
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	erode(img_mask, img_mask, kernel);
	dilate(img_mask, img_mask, getStructuringElement(MORPH_RECT, Size(5, 5)));
	imshow("Dilate Mask", img_mask);
	img_view.copyTo(img_ec, img_mask);
//	imshow("Extract", img_ec);
	cvtColor(img_ec, img_ec, COLOR_BGR2GRAY);
	Mat img_canny, img_lines;
	Canny(img_mask, img_canny, 100, 175); 
	cvtColor(img_canny, img_lines, COLOR_GRAY2BGR);
	imshow("Canny", img_canny);
	vector<vector<Point> > contours;
	findContours(img_canny, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	Mat img_contours = Mat::zeros(img_view.size(), img_view.type());
	drawContours(img_contours, contours, -1, Scalar(0, 0, 255));
	imshow("Contours", img_contours);
	waitKey(0);
	return 0;
}
