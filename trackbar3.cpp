#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int r=94, g=127, b=139, rm = 255, gm = 255, bm = 255;
Mat res, img, img_res;
Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
Mat kernel2 = getStructuringElement(MORPH_RECT, Size(7, 7));
Mat kernel3 = getStructuringElement(MORPH_RECT, Size(4, 16));
Mat kernel4 = getStructuringElement(MORPH_RECT, Size(2, 8));
static void on_trackbar(int, void*)
{
	inRange(img, Scalar(b, g, r), Scalar(bm, gm, rm), res);
	morphologyEx(res, res, MORPH_OPEN, kernel);
	morphologyEx(res, res, MORPH_CLOSE, kernel2);
	img.copyTo(img_res, res);
	imshow("Mask", res);
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
	inRange(img, Scalar(b, g, r), Scalar(bm, gm, rm), res);
	morphologyEx(res, res, MORPH_OPEN, kernel);
	morphologyEx(res, res, MORPH_CLOSE, kernel2);
	imshow("Mask", res);
	waitKey(0);
	Mat res2;
	erode(res, res2, kernel3);
	imshow("Erode", res2);
	dilate(res2, res2, kernel3);
	Mat minus = res - res2;
	morphologyEx(minus, minus, MORPH_OPEN, kernel2);
	imshow("Minus", minus);
	vector<Vec4i> lines;
	HoughLinesP(minus, lines, 2, CV_PI/180, 80, 100);
	cout << "Line Count: " << lines.size() << endl;
	Mat img_oline = img.clone();
	double k1=0, b1=0, k2=0, b2=0;
	int c1=0, c2=0;
	for (auto vec: lines)
	{
		line(img_oline, Point(vec[0], vec[1]), Point(vec[2], vec[3]), Scalar(0, 0, 255), 2, 8);
		double k = (double(vec[3]) - vec[1]) / (vec[2] - vec[0]);
		double b = vec[1] - k * vec[0];
		cout << k << ' ' << b << endl;
		if (b > 300)
		{
			c1++; k1 += k; b1 += b;
		}
		else
		{
			c2++; k2 += k; b2 += b;
		}
	}
	imshow("Origin Lines", img_oline);
	k1 /= c1; b1 /= c1; k2 /= c2; b2 /= c2;
	cout << "k1 " << k1 << " b1 " << b1 << endl
		<< "k2 " << k2 << " b2 " << b2 << endl;
	int xm = img.cols;
	line(img, Point(0, int(b1)), Point(xm, int(k1 * xm + b1)), Scalar(0, 0, 255), 2, 8);
	line(img, Point(0, int(b2)), Point(xm, int(k2 * xm + b2)), Scalar(0, 0, 255), 2, 8);
	imshow("Lines", img);
	waitKey(0);
	return 0;
}
