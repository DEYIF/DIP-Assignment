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
		cvimgbuffer.create(nHeight, nWidth, CV_8UC3);
		
	}

	//��������
	uchar* pucRow;									//ָ������������ָ��
	uchar* pucImage = (uchar*)dib.GetBits();		//ָ����������ָ��
	int nStep = dib.GetPitch();					//ÿ�е��ֽ���,ע���������ֵ�����и�
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

void COpenCVProcess::BinaryCV()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	cvtColor(cvimg, cvimg, cv::COLOR_BGR2GRAY);
	threshold(cvimg, cvimg, 218, 255, THRESH_BINARY);
}

void COpenCVProcess::InvertCV()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	cvimg= 255 - cvimg;
}




void COpenCVProcess::ErosionCV()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	// ���帯ʴ�ˣ��ṹԪ�أ�
	int erosionSize = 7; // ��ʴ�˵Ĵ�С
	Mat element = getStructuringElement(MORPH_RECT, Size(erosionSize, erosionSize));

	// ���и�ʴ����
	erode(cvimg, cvimg, element);
}


void COpenCVProcess::DilationCV()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	int n = 7; // �˵Ĵ�С
	Mat element = getStructuringElement(MORPH_RECT, Size(n, n));

	// ���и�ʴ����
	dilate(cvimg, cvimg, element);
}


void COpenCVProcess::FillHoleCV()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	// ���Ҷ�ֵͼ���е�����
	vector<std::vector<cv::Point>> contours;
	findContours(cvimg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	// ��������ڲ��Ŀ׶�
	//Mat filledImage = cv::Mat::zeros(cvimg.size(), CV_8U);
	drawContours(cvimg, contours, -1, cv::Scalar(255), cv::FILLED);

}


void COpenCVProcess::EdgeExtractionCV()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	int erosionSize = 3; // ��ʴ�˵Ĵ�С
	Mat element = getStructuringElement(MORPH_RECT, Size(erosionSize, erosionSize));

	// ���и�ʴ����
	Mat temp = Mat::zeros(cvimg.size(), CV_8U);
	erode(cvimg, temp, element);

	// ����ԭͼ���븯ʴ���ͼ��Ĳ�ֵ
	cvimg = cvimg - temp;
}



void COpenCVProcess::AddEdgeCV(CDib &dib)
{
	// TODO: �ڴ˴����ʵ�ִ���.
	
	//����ֵͼתΪ��ɫ
	
	Mat edgeImage = cvimg.clone();
	Mat OriginalImage = dib.cvRGBimg.clone();
	cvtColor(cvimg, cvimg, COLOR_GRAY2BGR);
	// ������Եͼ�������
	for (int y = 0; y < edgeImage.rows; ++y) {
		for (int x = 0; x < edgeImage.cols; ++x) {
			// �����Եͼ�������Ϊ��ɫ����Ե������ԭͼ���Ӧ���ص���ɫͨ��ֵ��Ϊ��
			if (edgeImage.at<uchar>(y, x) == 255) {
				OriginalImage.at<Vec3b>(y, x)[0] = 0; // ������ɫͨ��Ϊ��
				OriginalImage.at<Vec3b>(y, x)[1] = 0; // ����lvɫͨ��Ϊ��
				OriginalImage.at<Vec3b>(y, x)[2] = 0; // 
				cvimg.at<Vec3b>(y, x)[0] = 255; // ������ɫͨ��Ϊ255
				cvimg.at<Vec3b>(y, x)[1] = 0;   // ������ɫͨ��Ϊ0
				cvimg.at<Vec3b>(y, x)[2] = 0;   // ���ú�ɫͨ��Ϊ0
			}
		}
	}

	// ������ͼ�������һ��
	cvimg = cvimg + OriginalImage;

	// ������255�Ĳ�������Ϊ��ɫ
	
}





void COpenCVProcess::CVCanny()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	
	//1.��˹�˲�
	float sigma = 3.0;
	GaussianBlur(cvimg, cvimg, Size(0, 0), sigma, sigma);

	//2.�����ݶ�
	Mat edges;
	double minVal, maxVal;
	Point minLoc, maxLoc;
	minMaxLoc(cvimg, &minVal, &maxVal, &minLoc, &maxLoc);
	double lowThreshold = 0.1 * maxVal; // ����ֵ
	double highThreshold = 0.2 * maxVal; // ����ֵ
	Canny(cvimg, edges, lowThreshold, highThreshold);

	cvimg = edges;
}
