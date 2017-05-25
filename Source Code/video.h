#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/highgui.h>
#include <iostream>
#include <cmath>

#define PI 3.1415926
#define ScreenX1 0
#define ScreenX2 160

cv::Mat BinaryRed(cv::Mat frame) ;
void BinaryGreen(cv::Mat &frame) ;
cv::Mat HoughRedLine(cv::Mat &frame, bool *angle, int *x1, int *y1, int *x2, int *y2, float *Rho, float *Theta, bool *Left) ;
void DeleteAboveLine(cv::Mat temp, cv::Mat &frame, int x1, int y1, int x2, int y2, float rho , float theta) ;
bool FindRed(cv::Mat frame) ;
short ExistAndDistance(cv::Mat &frame) ;
void ImageProcess() ;
