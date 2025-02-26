#include "pch.h"
#include "COpenCVProcess.h"
#include "Dib.h"

COpenCVProcess::COpenCVProcess(CDib* pDib)
{
	Dib2Mat(*pDib);
	cvimgbuffer = cvimg.clone();
}

COpenCVProcess::~COpenCVProcess(void)
{
	cvimg.release();
	cvimgbuffer.release();
}

void COpenCVProcess::OpenCVGaussianBlur()
{
	//Mat tmp;
	GaussianBlur(cvimg, cvimg, Size(7, 7), 0, 0, BORDER_DEFAULT);
	//cvimg = tmp;
}

void COpenCVProcess::Dib2Mat(CDib& dib)
{
	if (true == dib.IsNull())
	{
		return;
	}
	int nChannels = dib.GetBPP() / 8;
	if ((1 != nChannels) && (3 != nChannels))
	{
		return;
	}
	int nWidth = dib.GetWidth();
	int nHeight = dib.GetHeight();


	//重建mat
	if (1 == nChannels)
	{
		cvimg.create(nHeight, nWidth, CV_8UC1);
	}
	else if (3 == nChannels)
	{
		cvimg.create(nHeight, nWidth, CV_8UC3);
	}

	//拷贝数据
	uchar* pucRow;									//指向数据区的行指针
	uchar* pucImage = (uchar*)dib.GetBits();		//指向数据区的指针
	int nStep = dib.GetPitch();					//每行的字节数,注意这个返回值有正有负

	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pucRow = (cvimg.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			if (1 == nChannels)
			{
				pucRow[nCol] = *(pucImage + nRow * nStep + nCol);
			}
			else if (3 == nChannels)
			{
				for (int nCha = 0; nCha < 3; nCha++)
				{
					pucRow[nCol * 3 + nCha] = *(pucImage + nRow * nStep + nCol * 3 + nCha);
				}
			}
		}
	}
}

void COpenCVProcess::Mat2Dib(CDib& dib)
{
	if (0 == cvimg.total())
	{
		return;
	}
	int nChannels = cvimg.channels();
	if ((1 != nChannels) && (3 != nChannels))
	{
		return;
	}
	int nWidth = cvimg.cols;
	int nHeight = cvimg.rows;

	//重建CDib
	dib.Destroy();
	dib.Create(nWidth, nHeight, 8 * nChannels);

	//拷贝数据
	uchar* pucRow;									//指向数据区的行指针
	uchar* pucImage = (uchar*)dib.GetBits();		//指向数据区的指针
	int nStep = dib.GetPitch();					//每行的字节数,注意这个返回值有正有负
	dib.m_nWidth = nWidth;
	dib.m_nHeight = nHeight;
	dib.m_nWidthBytes = abs(nStep);
	dib.m_nBitCount = 8 * nChannels;
	dib.m_pDibBits = (unsigned char*)dib.GetBits() + (nHeight - 1) * dib.GetPitch();

	if (1 == nChannels)								//对于单通道的图像需要初始化调色板
	{
		RGBQUAD* rgbquadColorTable;
		int nMaxColors = 256;
		rgbquadColorTable = new RGBQUAD[nMaxColors];
		dib.GetColorTable(0, nMaxColors, rgbquadColorTable);
		for (int nColor = 0; nColor < nMaxColors; nColor++)
		{
			rgbquadColorTable[nColor].rgbBlue = (uchar)nColor;
			rgbquadColorTable[nColor].rgbGreen = (uchar)nColor;
			rgbquadColorTable[nColor].rgbRed = (uchar)nColor;
		}
		dib.SetColorTable(0, nMaxColors, rgbquadColorTable);
		delete[]rgbquadColorTable;
	}

	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pucRow = (cvimg.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			if (1 == nChannels)
			{
				*(pucImage + nRow * nStep + nCol) = pucRow[nCol];
			}
			else if (3 == nChannels)
			{
				for (int nCha = 0; nCha < 3; nCha++)
				{
					*(pucImage + nRow * nStep + nCol * 3 + nCha) = pucRow[nCol * 3 + nCha];
				}
			}
		}
	}
}


void COpenCVProcess::SegWatershed()
{
	// TODO: 在此处添加实现代码.

	int thresholdValue = 75;
	Mat threImg;
	threshold(cvimg, threImg, thresholdValue, 255, THRESH_BINARY);
	//imshow("Thresholded Image", threImg);
	
	//低通滤波
	Mat lowPass;
	medianBlur(threImg, lowPass, 5);
	//imshow("medianBlur", lowPass);

	//腐蚀膨胀
	// 创建圆形结构元素（椭圆形）
	int kernelSize = 3; // 你可以根据需要调整大小
	Mat kernel1 = getStructuringElement(MORPH_ELLIPSE, Size(kernelSize, kernelSize));
	Mat kernel2 = getStructuringElement(MORPH_ELLIPSE, Size(kernelSize, kernelSize));
	erode(lowPass, lowPass, kernel1);
	dilate(lowPass, lowPass, kernel2);
	//imshow("erode & dilate", lowPass);

	// Find total markers
	vector<vector<Point> > contours;
	findContours(lowPass, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	// 创建新的矩阵来保存过滤后的分割结果
	Mat segmented = Mat::zeros(cvimg.size(), CV_8U);
	int minArea = 245;
	// 过滤分割区域
	for (size_t i = 0; i < contours.size(); ++i) {
		double area = contourArea(contours[i]);
		if (area >= minArea) {
			// 计算分割区域的质心（中心点）
			Moments mu = moments(contours[i]);
			Point2f center(mu.m10 / mu.m00, mu.m01 / mu.m00);

			if (center.x > cvimg.cols / 2 && center.y < 5 * cvimg.rows / 9 && center.y > 2 * cvimg.rows / 9) {
				drawContours(segmented, contours, (int)i, Scalar(255), FILLED);
			}
		}
	}
	//imshow("segmented", segmented);

	// 统计轮廓数量
	int contourCount = (int)contours.size();

	// 定义一个阈值
	const int maxContoursToShow = 1; // 设置你认为合理的最大轮廓数量

	int max = 6;
	int count = 0;
	while (contourCount > maxContoursToShow) {
		
		morphologyEx(segmented, segmented, MORPH_OPEN, kernel1);
		count++;
		if (count >= max)
			break;
		findContours(segmented, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		contourCount = (int)contours.size();
		
	}
	if (count > 0) {
		for (int i = 0; i < count; i++) {
			morphologyEx(segmented, segmented, MORPH_CLOSE, kernel1);
		}
	}
	//imshow("after morphologyEx", segmented);
	Mat result;
	cvtColor(cvimg, result, COLOR_GRAY2BGR);
	// 将白色像素（255, 255, 255）转换为紫色（255, 0, 255）
	for (int y = 0; y < result.rows; y++)
	{
		for (int x = 0; x < result.cols; x++)
		{
			if (segmented.at<uchar>(y, x) == 255)
			{
				result.at<Vec3b>(y, x) = Vec3b(255, 0, 255); // 将分割结果以紫色显示
			}
		}
	}
	cvimg = result;
}


void COpenCVProcess::SegAll(Mat result)
{
	// TODO: 在此处添加实现代码.
	//将result中是紫色区域的部分提取出来，使cvimg中的对应部分也变为紫色
	cvtColor(cvimg, cvimg, COLOR_GRAY2BGR);
	for (int y = 0; y < result.rows; y++)
	{
		for (int x = 0; x < result.cols; x++)
		{
			if (result.at<Vec3b>(y, x) == Vec3b(255, 0, 255))
			{
				cvimg.at<Vec3b>(y, x) = Vec3b(255, 0, 255); // 将分割结果以紫色显示
			}
		}
	}
}

