#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int h, s, v, hm = 255, sm = 255, vm = 255;
Mat res, img, img_hsv;
static void on_trackbar(int, void*)
{
	inRange(img_hsv, Scalar(h, s, v), Scalar(hm, sm, vm), res);
	img.copyTo(res, res);
	imshow("Result", res);
}
int main(int argc, char** argv)
{
	img = imread(argv[1]);
	resize(img, img, Size(800, 600));
	img.convertTo(img_hsv, COLOR_BGR2HSV);
	imshow("Image", img);
	createTrackbar("H", "Image", &h, 255, on_trackbar); 
	createTrackbar("S", "Image", &s, 255, on_trackbar); 
	createTrackbar("V", "Image", &v, 255, on_trackbar); 
	createTrackbar("HM", "Image", &hm, 255, on_trackbar); 
	createTrackbar("SM", "Image", &sm, 255, on_trackbar); 
	createTrackbar("VM", "Image", &vm, 255, on_trackbar); 
	waitKey(0);
	return 0;
}
