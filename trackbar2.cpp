#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int r, g, b=80, rm = 60, gm = 255, bm = 255;
Mat res, img, img_res;
Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
Mat kernel2 = getStructuringElement(MORPH_RECT, Size(7, 7));
static void on_trackbar(int, void*)
{
	inRange(img, Scalar(b, g, r), Scalar(bm, gm, rm), res);
	morphologyEx(res, res, MORPH_OPEN, kernel);
	morphologyEx(res, res, MORPH_CLOSE, kernel2);
	//img.copyTo(img_res, res);
	img.copyTo(img_res, res);
	imshow("Mask", res);
	//imshow("Masked Image", img_res);
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
	// drawContours(img, contours, -1, Scalar(0, 0, 255));
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

	Rect roi = rrect.boundingRect() & Rect(0, 0, img.cols, img.rows);
	cout << roi << endl;
	// imwrite("a.png", img(roi));
	inRange(img(roi), Scalar(b, g, r), Scalar(bm, gm, rm), res);

	contours.clear();
	findContours(res, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	drawContours(img(roi), contours, -1, Scalar(0, 0, 255));
	cout << contours.size() << endl;
	rectangle(img, rrect.boundingRect(), Scalar(255, 0, 0), 3);
	imshow("Result", img);
	waitKey(0);
	return 0;
}
