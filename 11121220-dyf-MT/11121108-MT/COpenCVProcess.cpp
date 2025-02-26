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


	//�ؽ�mat
	if (1 == nChannels)
	{
		cvimg.create(nHeight, nWidth, CV_8UC1);
	}
	else if (3 == nChannels)
	{
		cvimg.create(nHeight, nWidth, CV_8UC3);
	}

	//��������
	uchar* pucRow;									//ָ������������ָ��
	uchar* pucImage = (uchar*)dib.GetBits();		//ָ����������ָ��
	int nStep = dib.GetPitch();					//ÿ�е��ֽ���,ע���������ֵ�����и�

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

	//�ؽ�CDib
	dib.Destroy();
	dib.Create(nWidth, nHeight, 8 * nChannels);

	//��������
	uchar* pucRow;									//ָ������������ָ��
	uchar* pucImage = (uchar*)dib.GetBits();		//ָ����������ָ��
	int nStep = dib.GetPitch();					//ÿ�е��ֽ���,ע���������ֵ�����и�
	dib.m_nWidth = nWidth;
	dib.m_nHeight = nHeight;
	dib.m_nWidthBytes = abs(nStep);
	dib.m_nBitCount = 8 * nChannels;
	dib.m_pDibBits = (unsigned char*)dib.GetBits() + (nHeight - 1) * dib.GetPitch();

	if (1 == nChannels)								//���ڵ�ͨ����ͼ����Ҫ��ʼ����ɫ��
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
	// TODO: �ڴ˴����ʵ�ִ���.

	int thresholdValue = 75;
	Mat threImg;
	threshold(cvimg, threImg, thresholdValue, 255, THRESH_BINARY);
	//imshow("Thresholded Image", threImg);
	
	//��ͨ�˲�
	Mat lowPass;
	medianBlur(threImg, lowPass, 5);
	//imshow("medianBlur", lowPass);

	//��ʴ����
	// ����Բ�νṹԪ�أ���Բ�Σ�
	int kernelSize = 3; // ����Ը�����Ҫ������С
	Mat kernel1 = getStructuringElement(MORPH_ELLIPSE, Size(kernelSize, kernelSize));
	Mat kernel2 = getStructuringElement(MORPH_ELLIPSE, Size(kernelSize, kernelSize));
	erode(lowPass, lowPass, kernel1);
	dilate(lowPass, lowPass, kernel2);
	//imshow("erode & dilate", lowPass);

	// Find total markers
	vector<vector<Point> > contours;
	findContours(lowPass, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	// �����µľ�����������˺�ķָ���
	Mat segmented = Mat::zeros(cvimg.size(), CV_8U);
	int minArea = 245;
	// ���˷ָ�����
	for (size_t i = 0; i < contours.size(); ++i) {
		double area = contourArea(contours[i]);
		if (area >= minArea) {
			// ����ָ���������ģ����ĵ㣩
			Moments mu = moments(contours[i]);
			Point2f center(mu.m10 / mu.m00, mu.m01 / mu.m00);

			if (center.x > cvimg.cols / 2 && center.y < 5 * cvimg.rows / 9 && center.y > 2 * cvimg.rows / 9) {
				drawContours(segmented, contours, (int)i, Scalar(255), FILLED);
			}
		}
	}
	//imshow("segmented", segmented);

	// ͳ����������
	int contourCount = (int)contours.size();

	// ����һ����ֵ
	const int maxContoursToShow = 1; // ��������Ϊ����������������

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
	// ����ɫ���أ�255, 255, 255��ת��Ϊ��ɫ��255, 0, 255��
	for (int y = 0; y < result.rows; y++)
	{
		for (int x = 0; x < result.cols; x++)
		{
			if (segmented.at<uchar>(y, x) == 255)
			{
				result.at<Vec3b>(y, x) = Vec3b(255, 0, 255); // ���ָ�������ɫ��ʾ
			}
		}
	}
	cvimg = result;
}


void COpenCVProcess::SegAll(Mat result)
{
	// TODO: �ڴ˴����ʵ�ִ���.
	//��result������ɫ����Ĳ�����ȡ������ʹcvimg�еĶ�Ӧ����Ҳ��Ϊ��ɫ
	cvtColor(cvimg, cvimg, COLOR_GRAY2BGR);
	for (int y = 0; y < result.rows; y++)
	{
		for (int x = 0; x < result.cols; x++)
		{
			if (result.at<Vec3b>(y, x) == Vec3b(255, 0, 255))
			{
				cvimg.at<Vec3b>(y, x) = Vec3b(255, 0, 255); // ���ָ�������ɫ��ʾ
			}
		}
	}
}

