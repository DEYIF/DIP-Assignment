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
		cvimgbuffer.create(nHeight, nWidth, CV_8UC3);
		
	}

	//拷贝数据
	uchar* pucRow;									//指向数据区的行指针
	uchar* pucImage = (uchar*)dib.GetBits();		//指向数据区的指针
	int nStep = dib.GetPitch();					//每行的字节数,注意这个返回值有正有负
	int index = 0;
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

void COpenCVProcess::BinaryCV()
{
	// TODO: 在此处添加实现代码.
	cvtColor(cvimg, cvimg, cv::COLOR_BGR2GRAY);
	threshold(cvimg, cvimg, 218, 255, THRESH_BINARY);
}

void COpenCVProcess::InvertCV()
{
	// TODO: 在此处添加实现代码.
	cvimg= 255 - cvimg;
}




void COpenCVProcess::ErosionCV()
{
	// TODO: 在此处添加实现代码.
	// 定义腐蚀核（结构元素）
	int erosionSize = 7; // 腐蚀核的大小
	Mat element = getStructuringElement(MORPH_RECT, Size(erosionSize, erosionSize));

	// 进行腐蚀操作
	erode(cvimg, cvimg, element);
}


void COpenCVProcess::DilationCV()
{
	// TODO: 在此处添加实现代码.
	int n = 7; // 核的大小
	Mat element = getStructuringElement(MORPH_RECT, Size(n, n));

	// 进行腐蚀操作
	dilate(cvimg, cvimg, element);
}


void COpenCVProcess::FillHoleCV()
{
	// TODO: 在此处添加实现代码.
	// 查找二值图像中的轮廓
	vector<std::vector<cv::Point>> contours;
	findContours(cvimg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	// 填充轮廓内部的孔洞
	//Mat filledImage = cv::Mat::zeros(cvimg.size(), CV_8U);
	drawContours(cvimg, contours, -1, cv::Scalar(255), cv::FILLED);

}


void COpenCVProcess::EdgeExtractionCV()
{
	// TODO: 在此处添加实现代码.
	int erosionSize = 3; // 腐蚀核的大小
	Mat element = getStructuringElement(MORPH_RECT, Size(erosionSize, erosionSize));

	// 进行腐蚀操作
	Mat temp = Mat::zeros(cvimg.size(), CV_8U);
	erode(cvimg, temp, element);

	// 计算原图像与腐蚀后的图像的差值
	cvimg = cvimg - temp;
}



void COpenCVProcess::AddEdgeCV(CDib &dib)
{
	// TODO: 在此处添加实现代码.
	
	//将二值图转为彩色
	
	Mat edgeImage = cvimg.clone();
	Mat OriginalImage = dib.cvRGBimg.clone();
	cvtColor(cvimg, cvimg, COLOR_GRAY2BGR);
	// 遍历边缘图像的像素
	for (int y = 0; y < edgeImage.rows; ++y) {
		for (int x = 0; x < edgeImage.cols; ++x) {
			// 如果边缘图像该像素为白色（边缘），则将原图像对应像素的蓝色通道值设为零
			if (edgeImage.at<uchar>(y, x) == 255) {
				OriginalImage.at<Vec3b>(y, x)[0] = 0; // 设置蓝色通道为零
				OriginalImage.at<Vec3b>(y, x)[1] = 0; // 设置lv色通道为零
				OriginalImage.at<Vec3b>(y, x)[2] = 0; // 
				cvimg.at<Vec3b>(y, x)[0] = 255; // 设置蓝色通道为255
				cvimg.at<Vec3b>(y, x)[1] = 0;   // 设置绿色通道为0
				cvimg.at<Vec3b>(y, x)[2] = 0;   // 设置红色通道为0
			}
		}
	}

	// 将两幅图像叠加在一起
	cvimg = cvimg + OriginalImage;

	// 将超过255的部分设置为白色
	
}





void COpenCVProcess::CVCanny()
{
	// TODO: 在此处添加实现代码.
	
	//1.高斯滤波
	float sigma = 3.0;
	GaussianBlur(cvimg, cvimg, Size(0, 0), sigma, sigma);

	//2.计算梯度
	Mat edges;
	double minVal, maxVal;
	Point minLoc, maxLoc;
	minMaxLoc(cvimg, &minVal, &maxVal, &minLoc, &maxLoc);
	double lowThreshold = 0.1 * maxVal; // 低阈值
	double highThreshold = 0.2 * maxVal; // 高阈值
	Canny(cvimg, edges, lowThreshold, highThreshold);

	cvimg = edges;
}
