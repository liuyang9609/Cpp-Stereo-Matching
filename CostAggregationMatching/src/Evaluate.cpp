#include "Evaluate.h"

void Evaluate(Mat standard, Mat myMap) {
	double sum = standard.rows * standard.cols;
	int count = 0;
	int ans = 0;
	for (int i = 0; i < standard.rows; i++) {
		for (int j = 0; j < standard.cols; j++) {
			ans = standard.ptr<uchar>(i)[j] - myMap.ptr<uchar>(i)[j];
			//��ԭͼ�Ҷ�������1����Ϊ��bad pixels����Ϊ��ǿ�Աȶȣ�����disparity maps������3��ʾ
			if (ans > 3 || ans < -3) count++;
		}
	}
	double result = (count + 0.0) / sum;
	cout << result * 100 << '%' << endl;
}