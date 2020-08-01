#include "KITTI_Evaluate.h"

using namespace std;

void KITTIEvaluate(Mat standard, Mat myMap, Mat errorMap) {

	//�ӲΧת��
	for (int i = 0; i < standard.rows; i++) {
		for (int j = 0; j < standard.cols; j++) {
			standard.ptr<uchar>(i)[j] = standard.ptr<uchar>(i)[j] * 3;  // * 3 256--->64
		}
	}

	//double sum = standard.rows * standard.cols;  //groundtruthͼ�ǳ��ܲ����ô�
	double sum = 0;
	int count = 0;
	int ans = 0;
	for (int i = 0; i < standard.rows; i++) {
		for (int j = 0; j < standard.cols; j++) {
			if (standard.ptr<uchar>(i)[j] != 0) {
				ans = standard.ptr<uchar>(i)[j] - myMap.ptr<uchar>(i)[j];
				sum++;  //����ɨ������
				//��ԭͼ�Ҷ�������1����Ϊ��bad pixels����Ϊ��ǿ�Աȶȣ�����disparity maps������3��ʾ
				if (ans > 3 || ans < -3) {
					count++;
					errorMap.ptr<Vec3b>(i)[j] = Vec3b(0, 0, 255);
				}
				else {
					errorMap.ptr<Vec3b>(i)[j] = Vec3b(0, 255, 0);
				}
			}
		}
	}
	double result = (count + 0.0) / sum;
	cout << "Error rate = " << result * 100 << "%\n" << endl;
}