#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
double bs[12] = {
334.237,
332.073,
338.945,
332.424,
328.821,
338.106,
228.766,
227.606,
347.587,
345.171,
345.839,
334.152,};
int tag[12];
using namespace std;
void kmeans(double bs[], int len, double v[], double mse[])
{
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
        }
        mse[0] /= cnt[0]; mse[1] /= cnt[1];
        v[0] = s[0] / cnt[0];
        v[1] = s[1] / cnt[1];
        loopcnt++;
    } while (loopcnt <= 1000 && mse[0] + mse[1] - lmse > 1e-6);
}
int main()
{
    srand(time(0));
    const int Len = 12;
    double v[2], mse[2];
    kmeans(bs, Len, v, mse);
    printf("v: %lf %lf\nmse: %lf %lf\n", v[0], v[1], mse[0], mse[1]);
    return 0;
}