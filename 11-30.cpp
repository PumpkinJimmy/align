#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
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
	Mat img, img_view, img_hsv, img_mask, img_ec;
	VideoCapture cap(0);

	waitKey(0);
	return 0;
}
