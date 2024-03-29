#pragma once
#include "atlimage.h"
#include <complex>

using namespace std;

#define  MAX_SIZE 1000
const int Smoth_Kernel[9] = {1,1,1,1,1,1,1,1,1};
#define Pi 3.1415926535897932354626
#define Ei 2.71828

class CDib :
	public CImage
{
public:
	CDib(void);
	CDib(CDib &Dib);
	~CDib(void);

public:
	enum chan_color {chan_red=1,chan_green,chan_blue,chan_black};
	enum HSIChannel{HUE=0,SATURATION,INTENSITY};

public:
	void LoadFile(LPCTSTR lpszPathName);

public:
	void Invert();

public:
	long* GrayValueCount();

public:
	BOOL FFT(unsigned char*, long, long);
	void FFT_2D(complex<double> * , int, int , complex<double> * );
	void FFT_1D(complex<double> * , complex<double> * , int);
	void IFFT_2D(complex<double> * , complex<double> * , int, int);
	void IFFT_1D(complex<double> *, complex<double> *, int);

public:
	long m_nWidth;
	long m_nHeight;
	int m_nWidthBytes;
	int m_nBitCount;
	unsigned char *m_pDibBits;
	complex<double> *m_bufferSpace;
	complex<double> *m_bufferFreq;

private:
	uint16_t* m_pRawBuffer;	//操作使用buffer，显示仍然用8位
	
private:
	long *m_pGrayValueCount;

public:
	void PieLinTrans();
	void InvertPalette();
	void showBitPlane0();
	void showBitPlane7();
	void HistoEQU();
	void ConvSmoth();
	void ConvSobel();
	void ConvLaplacian();
	double ConvGaussian(int Dimenssion);
	void LoadRawFile(LPCTSTR lpszPathName, int editWidth, int editHeight, CString format, int bitCount);
	void DispWindow(CString win);
	void CreateDisplayDib(uint16_t* RawBuffer, int nWidth, int nHeight, int nBitCount);
	void CreateWhiteBlock(int nWidth, int nHeight, int nBitCount, int rectW, int rectH);
	void myFFT(long nWidth, long nHeight);
	BOOL myInverseFFT(long nWidth, long nHeight);
	BOOL Filter(int type, double cutOff);
};

