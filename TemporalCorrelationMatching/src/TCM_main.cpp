#include <iostream>
#include "TCM_main.h"

using namespace cv;
using namespace std;

int main() {

	//load image
	Mat lImg = imread("image/left_img/000040_10.png", 1);
	Mat rImg = imread("image/right_img/000040_10.png", 1);
	Mat GroundTruth = imread("image/disp_noc_0/000040_10.png", 0);  //_10

	if (lImg.empty() || rImg.empty() || GroundTruth.empty()) {
		printf("Error: can not open image\n");
		printf("\nPress any key to continue...\n");
		system("pause");
		return -1;
	}
	std::cout << "Load Image Done!\n" << std::endl;

	Mat ErrorMap = Mat(GroundTruth.rows, GroundTruth.cols, CV_8UC3);
	std::cout << "Error Map Initialization Done!\n" << std::endl;

	//initialize disparity image 
	int hei = lImg.rows;
	int wid = rImg.cols;
	Mat lDis = Mat::zeros(hei, wid, CV_32FC1);
	Mat postlDis = Mat::zeros(hei, wid, CV_32FC1);

	Mat left_row_disp = Mat::zeros(hei, wid, CV_32FC1);
	Mat right_row_disp = Mat::zeros(hei, wid, CV_32FC1);

	//sgm compute
	SGMStereo sgm;
	sgm.compute(lImg, rImg, lDis, left_row_disp, right_row_disp);
	std::cout << "Image Matching Done!\n" << std::endl;

	//post processing
	MedianFlitering(lDis, postlDis);
	std::cout << "Image Postprocessing Done!\n" << std::endl;

	//export result
	imwrite("image/disp_img/000040_10_res_64d.png", postlDis); 
	imwrite("image/disp_img/000040_10_leftrow_64d.png", left_row_disp);
	imwrite("image/disp_img/000040_10_rightrow_64d.png", right_row_disp);
	std::cout << "Image Write Done!\n" << std::endl;

	//evaluate
	Mat MyDisp = imread("image/disp_img/000040_10_res_64d.png", 0);
	KITTIEvaluate(GroundTruth, MyDisp, ErrorMap);
	imwrite("image/disp_img/000040_10_res_64d_err.png", ErrorMap);
	std::cout << "Image Evaluation Done!\n" << std::endl;

	Mat lRow = imread("image/disp_img/000040_10_leftrow_64d.png", 1);
	Mat RRow = imread("image/disp_img/000040_10_rightrow_64d.png", 1);

	return 0;

}