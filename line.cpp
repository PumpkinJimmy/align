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
	inRange(img_hsv, Scalar(100, 80, 100), Scalar(124, 255, 233), img_mask); // Select the blue pixel
	imshow("Mask", img_mask);
	img_view.copyTo(img_ec, img_mask);
	imshow("Extract", img_ec);
	cvtColor(img_ec, img_ec, COLOR_BGR2GRAY);
	Mat img_canny, img_lines;
	Canny(img_ec, img_canny, 100, 175); 
	cvtColor(img_canny, img_lines, COLOR_GRAY2BGR);
	imshow("Canny", img_canny);
	vector<Vec4i> lines;
	HoughLinesP(img_canny, lines, 1, 2 * CV_PI / 180, 50);
	for( size_t i = 0; i < lines.size(); i++ )
	{
		line( img_lines, Point(lines[i][0], lines[i][1]),
				Point( lines[i][2], lines[i][3]), Scalar(0,0,255), 2, 8 );
	}
	imshow("HoughLinesP", img_lines);
	waitKey(0);
	return 0;
}
