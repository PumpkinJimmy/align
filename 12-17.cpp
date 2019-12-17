#include <iostream>
#include <algorithm>
#include <ctime>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
using namespace cv;
using namespace std;
vector<double>  areas;
void drawRect(Mat img, RotatedRect rect, Scalar color, int width)
{
	Point2f ps[4];
	rect.points(ps);
	for (int i = 0; i < 4; i++)
	{
		cv::line(img, ps[i], ps[(i + 1) % 4], color, width);
	}
}
void getOffset(Mat img, int& offset_x, int& offset_y)
{
	Mat img_hsv, img_mask, img_tg, img_canny;
	cvtColor(img, img_hsv, COLOR_BGR2HSV);
	inRange(img_hsv, Scalar(100, 120, 60), Scalar(124, 255, 200), img_mask);
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	morphologyEx(img_mask, img_mask, MORPH_OPEN, kernel);
	erode(img_mask, img_mask, kernel);
	img.copyTo(img_tg, img_mask);
	cvtColor(img_tg, img_tg, COLOR_BGR2GRAY);
	imshow("mask", img_mask);
	vector<vector<Point> > contours;
	findContours(img_mask, contours,  RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	Mat img_contours = Mat::zeros(img.size(), img.type());
	//drawContours(img_contours, contours, -1, Scalar(0, 0, 255));
	RotatedRect max_rect; int max_area = -1, max_c = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		auto rect = minAreaRect(contours[i]);
		if (rect.size.area() > max_area)
		{
			max_area = rect.size.area();
			max_c = i;
			max_rect = rect;
		}
	}
	areas.push_back(max_rect.size.area());
	drawContours(img_contours, contours, max_c, Scalar(0, 0, 255));
	double midx = img.cols / 2.0, midy = img.rows / 2.0;
	drawRect(img_contours, max_rect, Scalar(255, 0, 0), 1);
	Rect rect = max_rect.boundingRect();
	rectangle(img_contours, rect, Scalar(0, 255, 0), 2);
	int rectx = rect.x + rect.width / 2, recty = rect.y + rect.height / 2;
	circle(img_contours, Point2i(rectx, recty), 10, Scalar(0, 0, 255), FILLED);
	circle(img_contours, Point2f(midx, midy), 10, Scalar(255, 0, 0), 2);
	offset_x = int(0.5 + rectx - midx); offset_y = int(0.5 + recty - midy);
	imshow("Contour", img_contours);
}
int main(int argc, char** argv)
{
	if (argc < 2)
	{
		cout << "No video specific" << endl;
		return 0;
	}
	VideoCapture cap(argv[1]);
	cout << "Frames counts:" << cap.get(CAP_PROP_FRAME_COUNT) << endl;
	VideoWriter writer;
	string name = to_string(time(0));
	Size s(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT));
	Mat img, img_view;
	bool first = true;
	int cnt = 0;
	while (1)
	{
		cap >> img;
		if (img.empty())
		{
			waitKey(0);
			break;
		}
		cnt++;
		img_view = img.clone();
		imshow("Cam", img_view);
		int ox, oy;
		getOffset(img_view, ox, oy);
		auto tmp = areas.end() - 1;
		double diff = *tmp - *(tmp - 1);
		cout << "Diff: " << diff << endl;
		if (abs(diff) > 1000) cout << "Rejected" << endl;
		else cout << ox << ' ' << oy << endl;
		writer << img;
		if (first) first = false;
		waitKey(330);
	}
	double sum = 0, aver, var = 0;
	for (double a: areas) sum += a;
	aver = sum / areas.size();
	for (double a: areas) var += (a - aver) * (a - aver);
	var /= areas.size();

	cout << "Mean: " << aver << endl << "Varient: " << var << endl;
	return 0;
}
