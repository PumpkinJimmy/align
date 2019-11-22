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
	imshow("Image", img_view);
	cvtColor(img_view, img_hsv, COLOR_BGR2HSV);
	imshow("HSV", img_hsv);
	inRange(img_hsv, Scalar(100, 80, 100), Scalar(124, 255, 233), img_mask); // Select the blue pixel
	imshow("Mask", img_mask);
	img_view.copyTo(img_ec, img_mask);
	imshow("Extract", img_ec);
	waitKey(0);
	return 0;
}
