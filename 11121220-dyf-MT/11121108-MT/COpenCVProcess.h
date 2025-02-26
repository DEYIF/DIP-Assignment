#pragma once
//--------------------------------------------------------------
//COpenCVProcess类,用于OpenCV图像处理
//--------------------------------------------------------------
using namespace cv;
class CDib;

class COpenCVProcess
{
public:
	COpenCVProcess(CDib* pDib);
	~COpenCVProcess(void);

public:
	Mat cvimg, cvimgbuffer;

public:
	void OpenCVGaussianBlur();

public:
	//transfering between CDib and cv::Mat
	void Dib2Mat(CDib& dib);
	void Mat2Dib(CDib& dib);
	void SegWatershed();

	void SegAll(Mat result);
};

