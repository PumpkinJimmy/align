#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;
using namespace std;

void drawRect(Mat img, RotatedRect rect, Scalar color, int width)
{
	Point2f ps[4];
	rect.points(ps);
	for (int i = 0; i < 4; i++)
	{
		cv::line(img, ps[i], ps[(i + 1) % 4], color, width);
	}
}
int main(int argc, char** argv)
{
	Mat img = imread(argv[1]), img_view, img_hsv, img_mask, img_ec;
	pyrDown(img, img_view);
	pyrDown(img_view, img_view);
	imshow("Origin", img_view);
	cvtColor(img_view, img_hsv, COLOR_BGR2HSV);
	inRange(img_hsv, Scalar(100, 120, 60), Scalar(124, 255, 200), img_mask); // Select the blue pixel
	imshow("Mask", img_mask);
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	morphologyEx(img_mask, img_mask, MORPH_OPEN, kernel);
	erode(img_mask, img_mask, kernel);
	imshow("Dilate Mask", img_mask);
	img_view.copyTo(img_ec, img_mask);
	cvtColor(img_ec, img_ec, COLOR_BGR2GRAY);
	Mat img_canny, img_lines;
	Canny(img_mask, img_canny, 100, 175); 
	cvtColor(img_canny, img_lines, COLOR_GRAY2BGR);
	imshow("Canny", img_canny);
	vector<vector<Point> > contours;
	vector<Vec4i> hier;
	findContours(img_canny, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	Mat img_contours = Mat::zeros(img_view.size(), img_view.type());
	drawContours(img_contours, contours, -1, Scalar(0, 0, 255));
	RotatedRect max_rect; int max_area = -1;
	for (int i = 0; i < contours.size(); i++)
	{
		auto rect = minAreaRect(contours[i]);
		if (rect.size.area() > max_area)
		{
			max_area = rect.size.area();
			max_rect = rect;
		}
		//rectangle(img_contours, rect.boundingRect(), Scalar(0, 255, 0), 2);
	}
	drawRect(img_contours, max_rect, Scalar(0, 255, 0), 2);
	//rectangle(img_contours, max_rect, Scalar(0, 255, 0), 2);
	imshow("Contours", img_contours);
	waitKey(0);
	return 0;
}
