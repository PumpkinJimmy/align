#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int r, g, b, rm = 255, gm = 255, bm = 255;
Mat res, img, img_res;
static void on_trackbar(int, void*)
{
	inRange(img, Scalar(b, g, r), Scalar(bm, gm, rm), res);
	img.copyTo(img_res, res);
	imshow("Result", res);
}
int main(int argc, char** argv)
{
	img = imread(argv[1]);
	resize(img, img, Size(800, 600));
	imshow("Image", img);
	createTrackbar("R", "Image", &r, 255, on_trackbar); 
	createTrackbar("G", "Image", &g, 255, on_trackbar); 
	createTrackbar("B", "Image", &b, 255, on_trackbar); 
	createTrackbar("RM", "Image", &rm, 255, on_trackbar); 
	createTrackbar("GM", "Image", &gm, 255, on_trackbar); 
	createTrackbar("BM", "Image", &bm, 255, on_trackbar); 
	waitKey(0);
	vector<vector<Point>> contours;
	findContours(res, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	cout << contours.size() << endl;
	drawContours(img, contours, -1, Scalar(0, 0, 255));
	int max_area = 0; RotatedRect rrect;
	for (auto ps: contours)
	{
		RotatedRect rr = minAreaRect(ps);
		if (rr.size.width * rr.size.height > max_area)
		{
			max_area = rr.size.width * rr.size.height;
			rrect = rr;
		}
	}
	rectangle(img, rrect.boundingRect(), Scalar(255, 0, 0), 3);
	imshow("ROI", img);
	waitKey(0);
	return 0;
}
