#include <iostream>
#include <cstring>
#include <algorithm>
#include <ctime>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
using namespace cv;
using namespace std;
Mat getMask(Mat img, Scalar lower, Scalar upper)//extract the mask of the target object
{
	Mat img_hsv, img_mask;
	cvtColor(img, img_hsv, COLOR_BGR2HSV);
	inRange(img_hsv, lower, upper, img_mask);
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	morphologyEx(img_mask, img_mask, MORPH_OPEN, kernel);
	erode(img_mask, img_mask, kernel);
	return img_mask;
}
RotatedRect getTargetRotatedRect(Mat mask)
{
	vector<vector<Point> > contours;
	findContours(mask, contours,  RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	RotatedRect max_rect;
	int max_area = -1;
	for (int i = 0; i < contours.size(); i++)
	{
		auto rect = minAreaRect(contours[i]);
		if (rect.size.area() > max_area)
		{
			max_area = rect.size.area();
			max_rect = rect;
		}
	}
	return max_rect;
}
Vec2f getOffset(Mat img, Scalar lower, Scalar upper)
{
	Mat mask = getMask(img, lower, upper);
	Rect rect = getTargetRotatedRect(mask).boundingRect();
	double midx = img.cols / 2.0, midy = img.rows / 2.0;
	double rectx = rect.x + rect.width / 2, recty = rect.y + rect.height / 2;
	int offset_x = int(0.5 + rectx - midx), offset_y = int(0.5 + recty - midy);
	return Vec2f(offset_x, offset_y);
}
Vec2f getOffset(Mat img)
{
	return getOffset(img, Scalar(100, 120, 60), Scalar(124, 255, 200));
}
Vec2f getOffsetDraw(Mat img, Scalar lower, Scalar upper)
{
	imshow("Camera", img);
	Mat mask = getMask(img, lower, upper);
	imshow("Mask", mask);
	Rect rect = getTargetRotatedRect(mask).boundingRect();
	Mat img_res = img.clone();
	double midx = img.cols / 2.0, midy = img.rows / 2.0;
	double rectx = rect.x + rect.width / 2, recty = rect.y + rect.height / 2;
	int offset_x = int(0.5 + rectx - midx), offset_y = int(0.5 + recty - midy);
	rectangle(img_res, rect, Scalar(0, 255, 0), 2);
	circle(img_res, Point2i(rectx, recty), 10, Scalar(0, 0, 255), FILLED);
	imshow("Result", img_res);
	return Vec2f(offset_x, offset_y);
}
Vec2f getOffsetDraw(Mat img)
{
	return getOffsetDraw(img, Scalar(100, 120, 60), Scalar(124, 255, 200));
}
int main(int argc, char** argv)
{
	VideoCapture cap;
	if (argc > 2)
	{
		cout << "Usage: 12-22a [video]" << endl;
		return 0;
	}
	else if (argc == 2) {cap.open(argv[1]); }
	else {cap.open(0);}
	Size frame_size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT));
	double fps = cap.get(CAP_PROP_FPS);
	int fourcc = cap.get(CAP_PROP_FOURCC);
	cout << "Fourcc: " << fourcc << "\nFPS: " << fps << endl << "Frame Size: " << frame_size << endl;
	Mat img; int cnt, key;
	while (1) //read and analysis the images
	{
		cap >> img;
		if (write) writer << img;
		if (img.empty())
		{
			waitKey(0);
			break;
		}
		cnt++;
		Vec2f offset;
		offset = getOffsetDraw(img);
		cout <<  offset[0] << ' ' << offset[1] << endl;
		if ((key = waitKey(33)) != -1)
		{
			break;
		}
	}
	return 0;
}
