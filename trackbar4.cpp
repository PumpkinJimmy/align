#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
vector<int> kmeans(vector<double> bs, double v[], double mse[])
{
	int len = bs.size();
	vector<int> tags; tags.resize(len);
    int i1 = rand() % len;
    v[0] = bs[i1];
    bs[i1] = bs[len - 1];
    bs[len-1] = v[0];
    int i2 = rand() % len;
    v[1] = bs[i2];
    bs[i2] = bs[len-2];
    bs[len-2] = v[1];
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
	// init
	srand(time(0));
	img = imread(argv[1]);
	resize(img, img, Size(800, 600));
	imshow("Image", img);
	createTrackbar("R", "Image", &r, 255, on_trackbar); 
	createTrackbar("G", "Image", &g, 255, on_trackbar); 
	createTrackbar("B", "Image", &b, 255, on_trackbar); 
	createTrackbar("RM", "Image", &rm, 255, on_trackbar); 
	createTrackbar("GM", "Image", &gm, 255, on_trackbar); 
	createTrackbar("BM", "Image", &bm, 255, on_trackbar); 
	
	// op
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

	// calculate
	cout << "Line Count: " << lines.size() << endl;
	Mat img_oline = img.clone();
	double k1=0, b1=0, k2=0, b2=0;
	int c1=0, c2=0;
	vector<double> ks, bs;
	for (auto vec: lines)
	{
		line(img_oline, Point(vec[0], vec[1]), Point(vec[2], vec[3]), Scalar(0, 0, 255), 2, 8);
		double k = (double(vec[3]) - vec[1]) / (vec[2] - vec[0]);
		double b = vec[1] - k * vec[0];
		cout << k << ' ' << b << endl;
		ks.push_back(k);
		bs.push_back(b);
	}
	imshow("Origin Lines", img_oline);
	double v[2], mse[2];
	vector<int> tags = kmeans(bs, v, mse);
	b1 = v[0]; b2= v[1];
	cout << "V: " << v[0] << ' ' << v[1] << endl;
	for (int i = 0; i < bs.size(); i++)
	{
		if (tags[i] == 0) {k1 += ks[i];c1++;}
		else { k2 += ks[i]; c2++; }
	}
	k1 /= c1; k2 /= c2;
	cout << "k1 " << k1 << " b1 " << b1 << endl
		<< "k2 " << k2 << " b2 " << b2 << endl;

	// show result
	int xm = img.cols;
	line(img, Point(0, int(b1)), Point(xm, int(k1 * xm + b1)), Scalar(0, 0, 255), 2, 8);
	line(img, Point(0, int(b2)), Point(xm, int(k2 * xm + b2)), Scalar(0, 0, 255), 2, 8);
	imshow("Lines", img);
	waitKey(0);
	return 0;
}
