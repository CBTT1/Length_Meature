#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;


//�洢ֱ��k��b
struct Line_kb
{
    float k;
    float b;
}above_line,below_line;

//��С���˷����ֱ��
void Regression(int startline, int endline,int (*above)[20],int (*below)[20])
{
    int i = 0;
    int sumlines = endline - startline;
    if (sumlines == 0) sumlines = 1;
    int sumX = 0;
    int sumY = 0;
    float averageX = 0;
    float averageY = 0;
    float sumUp = 0;
    float sumDown = 0;
    for (i = startline; i < endline; i++)
    {
        sumX += above[i][0];
        sumY += above[i][1];
    }
    if (sumlines != 0)
    {
        averageX = sumX / sumlines;     //x��ƽ��ֵ
        averageY = sumY / sumlines;     //y��ƽ��ֵ
    }
    else
    {
        averageX = 0;     //x��ƽ��ֵ
        averageY = 0;     //y��ƽ��ֵ
    }
    for (i = startline; i < endline; i++)
    {
        sumUp += (above[i][1] - averageY) * (above[i][0] - averageX);
        sumDown += (above[i][0] - averageX) * (above[i][0] - averageX);
    }
    if (sumDown == 0) above_line.k = 0;
    else above_line.k = sumUp / sumDown;
    above_line.b = averageY - above_line.k * averageX;

    sumX = 0;
    sumY = 0;
    averageX = 0;
    averageY = 0;
    sumUp = 0;
    sumDown = 0;
    for (i = startline; i < endline; i++)
    {
        sumX += below[i][0];
        sumY += below[i][1];
    }
    if (sumlines != 0)
    {
        averageX = sumX / sumlines;     //x��ƽ��ֵ
        averageY = sumY / sumlines;     //y��ƽ��ֵ
    }
    else
    {
        averageX = 0;     //x��ƽ��ֵ
        averageY = 0;     //y��ƽ��ֵ
    }
    for (i = startline; i < endline; i++)
    {
        sumUp += (below[i][1] - averageY) * (below[i][0] - averageX);
        sumDown += (below[i][0] - averageX) * (below[i][0] - averageX);
    }
    if (sumDown == 0) below_line.k = 0;
    else below_line.k = sumUp / sumDown;
    below_line.b = averageY - below_line.k * averageX;

}

void Length_Meature()
{
    Mat srcMat = cv::imread("D:/Code/C++/houghsp/5.jpg");

    //�ж�ͼ���ȡ�Ƿ�ɹ�
    if (srcMat.empty()) {
        std::cout << "failed to read image!:" << std::endl;
        return;
    }
    //����ͼƬ����
    Mat gryMat;
    Mat cannyMat;
    Mat linesp_Mat;

    int above_point[20][20];
    int below_point[20][20];
    int above_count = 0;
    int below_count = 0;

    //ת��Ϊ�Ҷ�ͼ
    cvtColor(srcMat, gryMat, COLOR_BGR2GRAY);
    Canny(gryMat, cannyMat, 200, 230, 3);

    //����任���ֱ��
    HoughLinesP(cannyMat, linesp_Mat, 1, CV_PI / 180, 250);

    int heightsp = linesp_Mat.rows;
    int widthsp = linesp_Mat.cols;

    //��¼ֱ���ϵ������
    for (int i = 0; i < heightsp; i++)
    {
        for (int j = 0; j < widthsp; j++)
        {
            Point p1, p2;
            p1.x = linesp_Mat.at<Vec4i>(i, j)[0];
            p1.y = linesp_Mat.at<Vec4i>(i, j)[1];
            p2.x = linesp_Mat.at<Vec4i>(i, j)[2];
            p2.y = linesp_Mat.at<Vec4i>(i, j)[3];
            if (p1.y < cannyMat.rows / 3
                && above_count < 18)// ͼƬ�Ϸ�ֱ�߼�Ⲣ��¼��
            {
                above_point[above_count][0] = p1.x;
                above_point[above_count][1] = p1.y;
                above_count++;
                above_point[above_count][0] = p2.x;
                above_point[above_count][1] = p2.y;
                above_count++;
            }
            else if (p1.y > cannyMat.rows * 2 / 3 
                     && below_count < 18)// �����·�ֱ�߼�Ⲣ��¼��
            {
                below_point[below_count][0] = p1.x;
                below_point[below_count][1] = p1.y;
                below_count++;
                below_point[below_count][0] = p2.x;
                below_point[below_count][1] = p2.y;
                below_count++;
            }
        }
    }

    //��С���˷����ֱ��
    Regression(0, below_count <= above_count ? below_count : above_count,above_point,below_point);

    float average_k = above_line.k / 2 + below_line.k / 2;
    //���������
    Point start_point;
    Point end_point;
    start_point.x = 15;
    start_point.y = 15 * average_k + above_line.b;
    end_point.x = srcMat.cols - 15;
    end_point.y = (srcMat.cols - 15) * average_k + above_line.b;
    cv::line(srcMat, start_point, end_point, cv::Scalar(0, 0, 255), 3);

    //���������
    start_point.x = 15;
    start_point.y = 15 * average_k + below_line.b;
    end_point.x = srcMat.cols - 15;
    end_point.y = (srcMat.cols - 15) * average_k + below_line.b;
    cv::line(srcMat, start_point, end_point, cv::Scalar(0, 0, 255), 3);

    int length = 0;
    length = abs(below_line.b - above_line.b) / (sqrtf(average_k * average_k + 1 * 1));
    cout << "�����Ŀ��Ϊ" << length <<"����" << endl;
    imshow("srcMat", srcMat);
    waitKey(0);
}

int main()
{
    Length_Meature();
    return 0;
}