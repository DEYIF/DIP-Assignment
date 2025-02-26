#pragma once
#include "atlimage.h"

#include <complex>
#include"COpenCVProcess.h"
#include "11121220-dyf-MTDoc.h"
using namespace std;

#define  MAX_SIZE 1000
const int Smoth_Kernel[9] = {1,1,1,1,1,1,1,1,1};
#define Pi 3.1415926535897932354626
#define Ei 2.71828

class CDib :public CImage
{
public:
	CDib(void);
	CDib(CDib &Dib);
	~CDib(void);
public:
	int GetImgNum();
	int getWidth();
	int getHeight();
	unsigned char* GetDibBits();
	void BilinearInterpolate(uint16_t* bilinear, uint16_t* normal, int width, int height, float ratio);

	uint16_t* GetRawBuffer();


public:
	enum chan_color {chan_red=1,chan_green,chan_blue,chan_black};
	enum HSIChannel{HUE=0,SATURATION,INTENSITY};

public:
	long* GrayValueCount();
	void ShowTransversePlane(int CurImgNum);
	void ShowCoronalPlane(int CurImgNum);
	void ShowSagittalPlane(int CurImgNum);
	void ShowNextPage();
	void ShowFormerPage();


public:
	void LoadFile(LPCTSTR,int FileNum, int ImageHeight, int ImageWidth);
	void CreateDisplayDib(uint16_t* , int, int, int);
	void UpdateDisplayDib(int ImgNum);

public:
	BOOL FFT(unsigned char*, long, long);
	void FFT_2D(complex<double> * , int, int , complex<double> * );
	void FFT_1D(complex<double> * , complex<double> * , int);
	void IFFT_2D(complex<double> * , complex<double> * , int, int);
	void IFFT_1D(complex<double> *, complex<double> *, int);

public:
	int m_nCurrentImgNum_Sagittal;
	int m_nCurrentImgNum_Transverse;
	int m_nCurrentImgNum_Coronal;

public:
	int m_nWidthBytes;
	long m_nWidth;
	long m_nHeight;	
	int m_nBitCount;
	int m_nRawWidth;
	int m_nRawHeight;
	int m_nRawNum;
	unsigned char *m_pDibBits;
	uint16_t* m_pRawBuffer;
	int m_nImgNum;
	int m_current_img_pos;//0ºá½ØÃæ1Ê¸×´2¹Ú×´
	int m_current_frame;

	
private:
	long *m_pGrayValueCount;

};

