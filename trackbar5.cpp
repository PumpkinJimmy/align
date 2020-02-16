#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
typedef Vec<double, 4> Vec4lf;

void enhance(Mat mask, int open=3, int close=7)
{
	// apply morphology ops to enhance the mask
	Mat k_op = getStructuringElement(MORPH_RECT, Size(open, open));
	Mat k_cl = getStructuringElement(MORPH_RECT, Size(close, close));
	morphologyEx(mask, mask, MORPH_OPEN, k_op);
	morphologyEx(mask, mask, MORPH_CLOSE, k_cl);
}
Rect extractROI(Mat mask)
{
	vector<vector<Point>> contours;
	findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	cout << "Contours Count: " << contours.size() << endl;
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
	Rect roi_rect = rrect.boundingRect() & Rect(0, 0, mask.cols, mask.rows);
	return roi_rect;
}
Mat extractLineMask(Mat mask, Size k_line_size=Size(4, 16), int open=7)
{
	Mat tmp;
	Mat k_line = getStructuringElement(MORPH_RECT, k_line_size);
	Mat k_op = getStructuringElement(MORPH_RECT, Size(open ,open));
	morphologyEx(mask, tmp, MORPH_OPEN, k_line);
	Mat line_mask = mask - tmp;
	morphologyEx(line_mask, line_mask, MORPH_OPEN, k_op);
	imshow("Line Mask", line_mask);
	return line_mask;
}
vector<int> kmeans(const vector<double>& obs)
{
	// saparate the value into 2 groups
	vector<double> bs = obs;
	double v[2]={0}, mse[2]={0};
	int len = bs.size();
	vector<int> tags; tags.resize(len);
    int i1 = rand() % len;
    v[0] = bs[i1];
	swap(bs[i1], bs[len-1]);
    int i2 = rand() % len;
    v[1] = bs[i2];
    swap(bs[i1], bs[len-1]);
    mse[0] = 0; mse[1] = 0;
    double lmse=0;
    int cnt[2], pk;
    double s[2];
    int loopcnt=0;
    do
    {
        lmse = mse[0] + mse[1];
        mse[0] = 0; mse[1] = 0;
        cnt[0] = 0; cnt[1] = 0;
        s[0] = 0; s[1] = 0;
        for (int i = 0; i < len; i++)
        {
            pk = (abs(bs[i] - v[0]) < abs(bs[i] - v[1]) ? 0 : 1);
            cnt[pk]++;
            mse[pk] += (bs[i] - v[pk]) * (bs[i] - v[pk]);
            s[pk] += bs[i];
			tags[i] = pk;
        }
		if (cnt[0] != 0)
		{
        	mse[0] /= cnt[0];
        	v[0] = s[0] / cnt[0];
		}
		if (cnt[1] != 0)
		{
			mse[1] /= cnt[1];
			v[1] = s[1] / cnt[1];
		}
        loopcnt++;
    } while (loopcnt <= 1000 && mse[0] + mse[1] - lmse > 1e-6);
	return tags;
}
Vec4lf extractLines(Mat line_mask)
{
	vector<Vec4i> lines;
	HoughLinesP(line_mask, lines, 2, CV_PI/180, 80, 100);
	cout << "Line Count: " << lines.size() << endl;
	Vec4lf res={0, 0, 0, 0};
	int cnt[2] = {0};
	vector<double> ks, bs;
	for (auto vec: lines)
	{
		double k = (double(vec[3]) - vec[1]) / (vec[2] - vec[0]);
		double b = vec[1] - k * vec[0];
		// cout << k << ' ' << b << endl;
		ks.push_back(k);
		bs.push_back(b);
	}
	vector<int> tags = kmeans(bs);
	for (int i = 0; i < bs.size(); i++)
	{
		int pk = tags[i] * 2;
		res[pk] += ks[i];
		res[pk + 1] += bs[i];
		cnt[tags[i]]++;
	}
	res[0] /= cnt[0]; res[1] /= cnt[0]; 
	res[2] /= cnt[1]; res[3] /= cnt[1];
	return res;
}
int r=94, g=127, b=139, rm = 255, gm = 255, bm = 255;
Mat res, img, img_res;

static void on_trackbar(int, void*)
{
	inRange(img, Scalar(b, g, r), Scalar(bm, gm, rm), res);
	enhance(res);
	img.copyTo(img_res, res);
	imshow("Mask", res);
}
void initTrackbars(const char* winName)
{
	createTrackbar("R", winName, &r, 255, on_trackbar); 
	createTrackbar("G", winName, &g, 255, on_trackbar); 
	createTrackbar("B", winName, &b, 255, on_trackbar); 
	createTrackbar("RM", winName, &rm, 255, on_trackbar); 
	createTrackbar("GM", winName, &gm, 255, on_trackbar); 
	createTrackbar("BM", winName, &bm, 255, on_trackbar); 
}
int main(int argc, char** argv)
{
	// init
	int seed = time(0);
	// int seed = 1581843309; // fail
	// int seed = 1581843499; // succeed
	srand(seed);
	img = imread(argv[1]);
	resize(img, img, Size(800, 600));
	imshow("Image", img);
	initTrackbars("Image");
	inRange(img, Scalar(b, g, r), Scalar(bm, gm, rm), res);
	enhance(res);
	imshow("Mask", res);
	waitKey(0);

    // op1: roi
	Rect roi_rect = extractROI(res);
	cout << "Roi Rect: " << roi_rect << endl;
	img = img(roi_rect).clone();
	resize(img, img, Size(800, 600));
	imshow("ROI", img);
	initTrackbars("ROI");
	waitKey(0);

	// op2: line
	Mat line_mask = extractLineMask(res, Size(2, 8), 3);
	Vec4lf ln = extractLines(line_mask);
	waitKey(0);
	

	// show result
	
	int xm = img.cols;
	line(img, Point(0, int(ln[1])), Point(xm, int(ln[0] * xm + ln[1])), Scalar(0, 0, 255), 2, 8);
	line(img, Point(0, int(ln[3])), Point(xm, int(ln[2] * xm + ln[3])), Scalar(0, 0, 255), 2, 8);
	imshow("Lines", img);
	waitKey(0);
	return 0;
}
