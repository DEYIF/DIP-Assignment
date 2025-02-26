#include "pch.h"
#include "Dib.h"
#include<iostream>
#include<math.h>
#include <iomanip>
#include<filesystem>
#include"DlgRawChoose.h"
#include "DlgRawChoose.h"
#include<vector>
#include <numeric>
#include<fstream>
#include<algorithm>



using namespace std;

#define pi 3.14159265358979323846

CDib::CDib(void):m_pDibBits(NULL),m_pGrayValueCount(NULL)
{
	// initialized variables
	m_nBitCount = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nWidthBytes = 0;

	m_pRawBuffer = NULL;
	m_bufferSpace = NULL;
	m_bufferFreq = NULL;
	pointerArray.clear();
	CronalArrary.clear();
	MedianArrary.clear();
	nowFrame = 0;
}


CDib::CDib( CDib &Dib ):m_pDibBits(NULL),m_pGrayValueCount(NULL)
{
	// initialized variables
	m_nBitCount = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nWidthBytes = 0;
	m_pRawBuffer = NULL;
	m_bufferSpace = NULL;
	m_bufferFreq = NULL;
	pointerArray.clear();
	CronalArrary.clear();
	MedianArrary.clear();
	m_Angle = NULL;
	m_Magnitude = NULL;
	nowFrame = 0;
	viewManyRawStatus = 0;
	if(&Dib == NULL)
	{
		return;
	}
	if(!IsNull())
	{
		Destroy();
	}
	Create(Dib.m_nWidth,Dib.m_nHeight,Dib.GetBPP(), 0);
	m_nWidth = Dib.m_nWidth;
	m_nHeight = Dib.m_nHeight;
	if(IsIndexed())
	{
		int nColors=Dib.GetMaxColorTableEntries();
		if(nColors > 0)
		{
			RGBQUAD* pal{};
			pal = new RGBQUAD[nColors];
			Dib.GetColorTable(0,nColors,pal);
			SetColorTable(0,nColors,pal);
			delete[] pal;
		} 
	}
	m_nWidthBytes =abs(GetPitch()) ;
	m_nBitCount = GetBPP();
	m_pDibBits = (unsigned char*)GetBits()+(m_nHeight-1)*GetPitch();
	memcpy(m_pDibBits,Dib.m_pDibBits,m_nHeight*m_nWidthBytes);
	pointerArray = Dib.pointerArray;
	CronalArrary = Dib.CronalArrary;
	MedianArrary = Dib.MedianArrary;
	m_pRGBBuffer = Dib.m_pRGBBuffer;
	viewManyRawStatus = Dib.viewManyRawStatus;
	if (cvRGBimg.empty()&&m_nBitCount==24)
	{
		cvRGBimg.create(m_nHeight, m_nWidth, CV_8UC3);
		//��������
		uchar* pucRow;									//ָ������������ָ��
		uchar* pucImage = (uchar*)GetBits();		//ָ����������ָ��
		int nStep = GetPitch();					//ÿ�е��ֽ���,ע���������ֵ�����и�
		int index = 0;
		for (int nRow = 0; nRow < m_nHeight; nRow++)
		{
			pucRow = (cvRGBimg.ptr<uchar>(nRow));
			for (int nCol = 0; nCol < m_nWidth; nCol++)
			{
				for (int nCha = 0; nCha < 3; nCha++)
				{
					pucRow[nCol * 3 + nCha] = *(pucImage + nRow * nStep + nCol * 3 + nCha);
				}
			}
		}
	}
}

CDib::~CDib(void)
{
	m_pDibBits = NULL;
	m_bufferSpace = NULL;
	m_bufferFreq = NULL;
	m_Angle= NULL;
	m_Magnitude = NULL;
	pointerArray.clear();
	CronalArrary.clear();
	MedianArrary.clear();
	nowFrame = 0;
	if (m_pGrayValueCount != NULL)
	{
		delete []m_pGrayValueCount;
		m_pGrayValueCount = NULL;
	}
	if (m_pRawBuffer != NULL)
	{
		delete []m_pRawBuffer;
		m_pRawBuffer = NULL;
	}
	cvRGBimg.release();
	
}


void CDib::LoadFile( LPCTSTR lpszPathName )
{
		Load(lpszPathName);
		m_nWidth = GetWidth();
		m_nHeight = GetHeight();
		m_nWidthBytes = abs(GetPitch());
		m_nBitCount = GetBPP();
		m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1) * GetPitch();
		if (m_nBitCount == 24) {
			for (int i = 0; i < m_nHeight * m_nWidthBytes; i++)
			{
				m_pRGBBuffer.push_back((double)m_pDibBits[i]);
			}
			int nHeight = m_nHeight;
			int nWidth = m_nWidth;
			cvRGBimg.create(nHeight, nWidth, CV_8UC3);
			//��������
			uchar* pucRow;									//ָ������������ָ��
			uchar* pucImage = (uchar*)GetBits();		//ָ����������ָ��
			int nStep = GetPitch();					//ÿ�е��ֽ���,ע���������ֵ�����и�
			int index = 0;
			for (int nRow = 0; nRow < nHeight; nRow++)
			{
				pucRow = (cvRGBimg.ptr<uchar>(nRow));
				for (int nCol = 0; nCol < nWidth; nCol++)
				{
					for (int nCha = 0; nCha < 3; nCha++)
					{
						pucRow[nCol * 3 + nCha] = *(pucImage + nRow * nStep + nCol * 3 + nCha);
					}
				}
			}
		}
		
}





void CDib::Invert()
{
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidthBytes; j++)
		{
			*(m_pDibBits + i*m_nWidthBytes + j) = 255 - *(m_pDibBits + i*m_nWidthBytes + j);
		}
	}
}

long* CDib::GrayValueCount()
{
	long nColors = GetMaxColorTableEntries();
	if (nColors == 0)
	{
		return NULL;
	}
	long *pGrayValueCount = new long[nColors];
	memset(pGrayValueCount,0,nColors*sizeof(long));
	for (int i=0;i<m_nHeight;i++)
	{
		for (int j=0;j<m_nWidth;j++)
		{
			pGrayValueCount[*(m_pDibBits + i*m_nWidthBytes +j)]++;
		}
	}
	return pGrayValueCount;
}


BOOL CDib::FFT(unsigned char* pDIBBits, long nWidth, long nHeight)
{ 
	unsigned char*	lpSrc;							// ָ��Դͼ���ָ��
	int y ;										// ѭ�����Ʊ���
	int x ;										// ѭ�����Ʊ���
	double dTmpOne ;								//�����ʱ����
	double dTmpTwo ;								//�����ʱ����
	int nTransWidth ;								// ����Ҷ�任�Ŀ�ȣ�2���������ݣ�
	int nTransHeight;								// ����Ҷ�任�ĸ߶ȣ�2���������ݣ�
	double unchValue;								// ����ͼ������ػҶȵ���ʱ����
	complex<double> * pCTData ;						// ָ��ʱ�����ݵ�ָ��
	complex<double> * pCFData ;						// ָ��Ƶ�����ݵ�ָ��
	// ������и���Ҷ�任�ĵ���������	��2���������ݣ�
	dTmpOne = log(1.0*nWidth)/log(2.0);
	dTmpTwo = ceil(dTmpOne)	;
	dTmpTwo = pow(2,dTmpTwo);
	nTransWidth = (int) dTmpTwo;	
	// ������и���Ҷ�任�ĵ��������� ��2���������ݣ�
	dTmpOne = log(1.0*nHeight)/log(2.0);
	dTmpTwo = ceil(dTmpOne)	;
	dTmpTwo = pow(2,dTmpTwo);
	nTransHeight = (int) dTmpTwo;
	double dReal;									// ����Ҷ�任��ʵ��
	double dImag;									// ����Ҷ�任���鲿
	
	pCTData=new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	pCFData=new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	// ͼ�����ݵĿ�͸߲�һ����2���������ݣ�����pCTData��һ����������Ҫ��0
	for(y=0; y<nTransHeight; y++)
	{
		for(x=0; x<nTransWidth; x++)
		{
			pCTData[y*nTransWidth + x]=complex<double>(0,0);		// ����
		}
	}
	//��ͼ�����ݴ���pCTData
	for(y=0; y<nHeight; y++)
	{
		for(x=0; x<nWidth; x++)
		{
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			unchValue = (*lpSrc)*pow(-1.0,x+y);	//����-1��x+y�η�����Ƶ��0���Ƶ�����
			pCTData[y*nTransWidth + x]=complex<double>(unchValue,0);
		}
	}
	m_bufferSpace = pCTData;	//����ԭʼ���ݵ�m_bufferSpace��

	FFT_2D(pCTData, nWidth, nHeight, pCFData) ;				// ����Ҷ���任
	//����Ľ������pCTData��

	m_bufferFreq = pCTData;	//����任�����m_bufferFFT��
	///////////////////////////////////////////////////////////////////////////
	// code needed to insert here

	int middleX = nTransWidth / 2;
	int middleY = nTransHeight / 2;
	double maxVal=sqrt(pow(pCTData[middleY * nTransWidth + middleX].real(),2)+ pow(pCTData[middleY * nTransWidth + middleX].imag(),2));
	double maxLog = log(1 + maxVal);
	double powNum= 1.5 / 5.0;
	double maxPow = pow(maxVal, powNum);
	double* ampData= new double[nTransWidth * nTransHeight];
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			//ampData[y * nTransWidth + x] = log(1 + sqrt(pow(pCTData[y * nTransWidth + x].real(), 2) + pow(pCTData[y * nTransWidth + x].imag(), 2))) / maxLog;
			ampData[y * nTransWidth + x]= pow(sqrt(pow(pCTData[y * nTransWidth + x].real(), 2) + pow(pCTData[y * nTransWidth + x].imag(), 2)), powNum) / maxPow;
			ampData[y * nTransWidth + x] = ampData[y * nTransWidth + x] * 255;
			pDIBBits[y * nWidth + x] = (unsigned char)ampData[y * nTransWidth + x];
		}
	}


	//
	//
	//
	//
	///////////////////////////////////////////////////////////////////////////

	//IFFT_2D(pCFData, pCTData,nHeight, nWidth); 				// ����Ҷ���任

	//for(y=0; y<nHeight; y++)								// ���任�����ݴ���lpDIBBits
	//{
	//	for(x=0; x<nWidth; x++)
	//	{
	//		//��Ҫ�����źŵ����������Լ�ʵ�����õ�ͼ�������ǻҶ�ֵ����ԭʼ����
	//		dReal = pCTData[y*nTransWidth + x].real() ;		// ʵ��
	//		dImag = pCTData[y*nTransWidth + x].imag() ;		// �鲿
	//		unchValue = dReal*pow(-1.0,x+y);
	//		if (unchValue < 0)
	//		{
	//			unchValue = 0;
	//		}
	//		if (unchValue > 0xff)
	//		{
	//			unchValue = 0xff;
	//		}
	//		// ָ��DIB��y�У���x�����ص�ָ��
	//		lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
	//		*lpSrc =(BYTE)unchValue ;
	//	}
	//}	
	
	delete []pCTData;										// �ͷ��ڴ�
	delete []pCFData;										// �ͷ��ڴ�
	delete []ampData;
	pCTData = NULL;
	pCFData = NULL;	
	return (true);										//���ؽ��
}

/*************************************************************************
 *
 * \�������ƣ�
 *   FFT_1D()
 *
 * \�������:
 *   complex<double> * pCTData	- ָ��ʱ�����ݵ�ָ�룬�������Ҫ�任������
 *   complex<double> * pCFData	- ָ��Ƶ�����ݵ�ָ�룬����ľ����任������
 *   nLevel						������Ҷ�任�����㷨�ļ�����2��������
 *
 * \����ֵ:
 *   ��
 *
 * \˵��:
 *   һά���ٸ���Ҷ�任��
 *
 *************************************************************************
 */
void CDib::FFT_1D(complex<double> * pCTData, complex<double> * pCFData, int nLevel)
{
		// ѭ�����Ʊ���
	int		i;
	int     j;
	int     k;

	// ����Ҷ�任����
	int	nCount =0 ;

	// ���㸵��Ҷ�任����
	nCount =(int)pow(2.0,nLevel) ;
	
	// ĳһ���ĳ���
	int		nBtFlyLen;
	nBtFlyLen = 0 ;
	
	// �任ϵ���ĽǶ� ��2 * PI * i / nCount
	double	dAngle;
	
	complex<double> *pCW ;
	
	// �����ڴ棬�洢����Ҷ�仯��Ҫ��ϵ����
	pCW  = new complex<double>[nCount / 2];

    // ���㸵��Ҷ�任��ϵ��
	for(i = 0; i < nCount / 2; i++)
	{
		dAngle = -2 * Pi * i / nCount;
		pCW[i] = complex<double> ( cos(dAngle), sin(dAngle) );
	}

	// �任��Ҫ�Ĺ����ռ�
	complex<double> *pCWork1,*pCWork2; 
	
	// ���乤���ռ�
	pCWork1 = new complex<double>[nCount];

	pCWork2 = new complex<double>[nCount];

	
	// ��ʱ����
	complex<double> *pCTmp;
	
	// ��ʼ����д������
	memcpy(pCWork1, pCTData, sizeof(complex<double>) * nCount);

	// ��ʱ����
	int nInter; 
	nInter = 0;

	// �����㷨���п��ٸ���Ҷ�任
	for(k = 0; k < nLevel; k++)
	{
		for(j = 0; j < (int)pow(2.0,k); j++)
		{
			//���㳤��
			nBtFlyLen = (int)pow( 2.0,(nLevel-k) );
			
			//�������ţ���Ȩ����
			for(i = 0; i < nBtFlyLen/2; i++)
			{
				nInter = j * nBtFlyLen;
				pCWork2[i + nInter] = 
					pCWork1[i + nInter] + pCWork1[i + nInter + nBtFlyLen / 2];
				pCWork2[i + nInter + nBtFlyLen / 2] =
					(pCWork1[i + nInter] - pCWork1[i + nInter + nBtFlyLen / 2]) 
					* pCW[(int)(i * pow(2.0,k))];
			}
		}

		// ���� pCWork1��pCWork2������
		pCTmp   = pCWork1	;
		pCWork1 = pCWork2	;
		pCWork2 = pCTmp		;
	}
	
	// ��������
	for(j = 0; j < nCount; j++)
	{
		nInter = 0;
		for(i = 0; i < nLevel; i++)
		{
			if ( j&(1<<i) )
			{
				nInter += 1<<(nLevel-i-1);
			}
		}
		pCFData[j]=pCWork1[nInter];
	}
	
	// �ͷ��ڴ�ռ�
	delete []pCW;
	delete []pCWork1;
	delete []pCWork2;
	pCW		=	NULL	;
	pCWork1 =	NULL	;
	pCWork2 =	NULL	;

}
/*************************************************************************
 *
 * \�������ƣ�
 *    IFFT_1D()
 *
 * \�������:
 *   complex<double> * pCTData	- ָ��ʱ�����ݵ�ָ�룬�������Ҫ���任������
 *   complex<double> * pCFData	- ָ��Ƶ�����ݵ�ָ�룬����ľ������任������
 *   nLevel						������Ҷ�任�����㷨�ļ�����2��������
 *
 * \����ֵ:
 *   ��
 *
 * \˵��:
 *   һά���ٸ���Ҷ���任��
 *
 ************************************************************************
 */
void CDib::IFFT_1D(complex<double> * pCFData, complex<double> * pCTData, int nLevel)
{
	
	// ѭ�����Ʊ���
	int		i;

	// ����Ҷ���任����
	int nCount;

	// ���㸵��Ҷ�任����
	nCount = (int)pow(2.0,nLevel) ;
	
	// �任��Ҫ�Ĺ����ռ�
	complex<double> *pCWork;	
	
	// ���乤���ռ�
	pCWork = new complex<double>[nCount];
	
	// ����Ҫ���任������д�빤���ռ�pCWork
	memcpy(pCWork, pCFData, sizeof(complex<double>) * nCount);
	
	// Ϊ�����ø���Ҷ���任,���԰Ѹ���ҶƵ�������ȡ����
	// Ȼ��ֱ���������任�����������Ǹ���Ҷ���任����Ĺ���
	for(i = 0; i < nCount; i++)
	{
		pCWork[i] = complex<double> (pCWork[i].real(), -pCWork[i].imag());
	}
	
	// ���ÿ��ٸ���Ҷ�任ʵ�ַ��任������洢��pCTData��
	FFT_1D(pCWork, pCTData, nLevel);
	
	// ��ʱ���Ĺ��������ս��
	// ���ݸ���Ҷ�任ԭ�����������ķ�����õĽ����ʵ�ʵ�ʱ������
	// ���һ��ϵ��nCount
	for(i = 0; i < nCount; i++)
	{
		pCTData[i] = 
			complex<double> (pCTData[i].real() / nCount, -pCTData[i].imag() / nCount);
	}
	
	// �ͷ��ڴ�
	delete pCWork;
	pCWork = NULL;
}

/*************************************************************************
 *
 * \�������ƣ�
 *   FFT_2D()
 *
 * \�������:
 *   complex<double> * pCTData	- ͼ������
 *   int    nWidth				- ���ݿ��
 *   int    nHeight				- ���ݸ߶�
 *   complex<double> * pCFData	- ����Ҷ�任��Ľ��
 *
 * \����ֵ:
 *   ��
 *
 * \˵��:
 *   ��ά����Ҷ�任��
 *
 ************************************************************************
 */
void CDib::FFT_2D(complex<double> * pCTData, int nWidth, int nHeight, complex<double> * pCFData)
{
	
	// ѭ�����Ʊ���
	int	x;
	int	y;
	
	// ��ʱ����
	double	dTmpOne;
	double  dTmpTwo;
	
	// ���и���Ҷ�任�Ŀ�Ⱥ͸߶ȣ���2���������ݣ�
	// ͼ��Ŀ�Ⱥ͸߶Ȳ�һ��Ϊ2����������
	int		nTransWidth;
	int 	nTransHeight;

	// ������и���Ҷ�任�Ŀ��	��2���������ݣ�
	dTmpOne = log(1.0*nWidth)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransWidth = (int) dTmpTwo	   ;
	
	// ������и���Ҷ�任�ĸ߶� ��2���������ݣ�
	dTmpOne = log(1.0*nHeight)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransHeight = (int) dTmpTwo	   ;	
	
	// x��y�����У������ϵĵ�������
	int		nXLev;
	int		nYLev;

	// ����x��y�����У������ϵĵ�������
	nXLev = (int) ( log(1.0*nTransWidth)/log(2.0) +  0.5 );
	nYLev = (int) ( log(1.0*nTransHeight)/log(2.0) + 0.5 );
	
	for(y = 0; y < nTransHeight; y++)
	{
		// x������п��ٸ���Ҷ�任
		FFT_1D(&pCTData[nTransWidth * y], &pCFData[nTransWidth * y], nXLev);
	}
	
	// pCFData��Ŀǰ�洢��pCTData�����б任�Ľ��
	// Ϊ��ֱ������FFT_1D����Ҫ��pCFData�Ķ�ά����ת�ã���һ������FFT_1D����
	// ����Ҷ�б任��ʵ�����൱�ڶ��н��и���Ҷ�任��
	for(y = 0; y < nTransHeight; y++)
	{
		for(x = 0; x < nTransWidth; x++)
		{
			pCTData[nTransHeight * x + y] = pCFData[nTransWidth * y + x];
		}
	}
	
	for(x = 0; x < nTransWidth; x++)
	{
		// ��x������п��ٸ���Ҷ�任��ʵ�����൱�ڶ�ԭ����ͼ�����ݽ����з����
		// ����Ҷ�任
		FFT_1D(&pCTData[x * nTransHeight], &pCFData[x * nTransHeight], nYLev);
	}

	// pCFData��Ŀǰ�洢��pCTData������ά����Ҷ�任�Ľ��������Ϊ�˷����з���
	// �ĸ���Ҷ�任�����������ת�ã����ڰѽ��ת�û���
	for(y = 0; y < nTransHeight; y++)
	{
		for(x = 0; x < nTransWidth; x++)
		{
			pCTData[nTransWidth * y + x] = pCFData[nTransHeight * x + y];
		}
	}
}

/*************************************************************************
 *
 * \�������ƣ�
 *   IFFT_2D()
 *
 * \�������:
 *   complex<double> * pCFData	- Ƶ������
 *   complex<double> * pCTData	- ʱ������
 *   int    nWidth				- ͼ�����ݿ��
 *   int    nHeight				- ͼ�����ݸ߶�
 *
 * \����ֵ:
 *   ��
 *
 * \˵��:
 *   ��ά����Ҷ���任��
 *
 ************************************************************************
 */
void CDib::IFFT_2D(complex<double> * pCFData, complex<double> * pCTData, int nWidth, int nHeight)
{
	// ѭ�����Ʊ���
	int	x;
	int	y;
	
	// ��ʱ����
	double	dTmpOne;
	double  dTmpTwo;
	
	// ���и���Ҷ�任�Ŀ�Ⱥ͸߶ȣ���2���������ݣ�
	// ͼ��Ŀ�Ⱥ͸߶Ȳ�һ��Ϊ2����������
	int		nTransWidth;
	int 	nTransHeight;

	// ������и���Ҷ�任�Ŀ��	��2���������ݣ�
	dTmpOne = log(1.0*nWidth)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransWidth = (int) dTmpTwo	   ;
	
	// ������и���Ҷ�任�ĸ߶� ��2���������ݣ�
	dTmpOne = log(1.0*nHeight)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransHeight = (int) dTmpTwo	   ;
	
	// ���乤����Ҫ���ڴ�ռ�
	complex<double> *pCWork= new complex<double>[nTransWidth * nTransHeight];

	//��ʱ����
	complex<double> *pCTmp ;
	
	// Ϊ�����ø���Ҷ���任,���԰Ѹ���ҶƵ�������ȡ����
	// Ȼ��ֱ���������任�����������Ǹ���Ҷ���任����Ĺ���
	for(y = 0; y < nTransHeight; y++)
	{
		for(x = 0; x < nTransWidth; x++)
		{
			pCTmp = &pCFData[nTransWidth * y + x] ;
			pCWork[nTransWidth * y + x] = complex<double>( pCTmp->real() , -pCTmp->imag() );
		}
	}

	// ���ø���Ҷ���任
	FFT_2D(pCWork, nWidth, nHeight, pCTData) ;
	
	// ��ʱ���Ĺ��������ս��
	// ���ݸ���Ҷ�任ԭ�����������ķ�����õĽ����ʵ�ʵ�ʱ������
	// ���һ��ϵ��
	for(y = 0; y < nTransHeight; y++)
	{
		for(x = 0; x < nTransWidth; x++)
		{
			pCTmp = &pCTData[nTransWidth * y + x] ;
			pCTData[nTransWidth * y + x] = 
				complex<double>( pCTmp->real()/(nTransWidth*nTransHeight),
								 -pCTmp->imag()/(nTransWidth*nTransHeight) );
		}
	}
	delete pCWork ;
	pCWork = NULL ;
}


void CDib::PieLinTrans()//�ֶ���������
{
	// TODO: �ڴ˴����ʵ�ִ���.
	double pixelVal = 0;
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidthBytes; j++)
		{
			pixelVal= *(m_pDibBits + i * m_nWidthBytes + j);
			if(pixelVal <100)
			{
				pixelVal = 0.5 * pixelVal;
			}
			else if (pixelVal < 150)
			{
				pixelVal = 3 * pixelVal - 250;
			}
			else
			{
				pixelVal = 0.5 * pixelVal + 125;
			}
			*(m_pDibBits + i * m_nWidthBytes + j) = (int)pixelVal;
		}
	}
}

void CDib::InvertPalette()//���õ�ɫ��ʵ�ַ���
{
	// TODO: �ڴ˴����ʵ�ִ���.

	// ȷ��ͼ���ǻҶ�ͼ��
	if (m_nBitCount != 8) {
		AfxMessageBox(_T("�ⲻ��һ���Ҷ�ͼ���޷�ͨ�����ַ�ʽ���ף�"));
		return;
	}

	// ��ȡ��ǰ��ɫ��
	RGBQUAD palette[256] = { 0 };
	GetColorTable(0, 256, palette);

	//�ı��ɫ��
	for (int i = 0; i < 256; i++) {
		palette[i].rgbRed = 255 - palette[i].rgbRed;
		palette[i].rgbBlue = 255 - palette[i].rgbBlue;
		palette[i].rgbGreen = 255 - palette[i].rgbGreen;
	}
	//���¸�ֵ
	SetColorTable(0, 256, palette);
}


void CDib::showBitPlane0()//��ʾλƽ��0
{
	// TODO: �ڴ˴����ʵ�ִ���.
	int temp = 0;
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidthBytes; j++)
		{
			temp = *(m_pDibBits + i * m_nWidthBytes + j) % 2;
			if (temp == 1) {
				*(m_pDibBits + i * m_nWidthBytes + j) = 255;
			}
			else {
				*(m_pDibBits + i * m_nWidthBytes + j) = 0;
			}
		}
	}

}

void CDib::showBitPlane7()//��ʾλƽ��7
{
	// TODO: �ڴ˴����ʵ�ִ���.
	int temp = 0;
	int k = 0;
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidthBytes; j++)
		{
			temp = *(m_pDibBits + i * m_nWidthBytes + j);
			if (temp < pow(2, 7)) {
				*(m_pDibBits + i * m_nWidthBytes + j) = 0;
			}
			else {
				*(m_pDibBits + i * m_nWidthBytes + j) = 255;
			}
		}
	}
}


void CDib::HistoEQU()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	
	int length = GetMaxColorTableEntries();
	long *Count = GrayValueCount();

	double *equHisto = new double[length];
	memset(equHisto, 0, length * sizeof(double));

	for (int i = 1; i < length; i++)
	{
		equHisto[i] = (double)Count[i] / (double)(m_nHeight * m_nWidth);	//����������Ϊ���ʣ�������������
	}

	double* equHistoSum = new double[length];
	memset(equHistoSum, 0, length * sizeof(double));

	equHistoSum[0] = equHisto[0];
	for (int i = 1; i < length; i++)
	{
		equHistoSum[i] = equHistoSum[i - 1] + equHisto[i];	//�����ۻ�����
	}

	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidthBytes; j++)
		{
			*(m_pDibBits + i * m_nWidthBytes + j) = (int)(equHistoSum[*(m_pDibBits + i * m_nWidthBytes + j)] * 255);
		}
	}

	delete[] Count;
	delete[] equHisto;
	delete[] equHistoSum;
}



void CDib::ConvSmoth()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	double SmoothKernel[3][3] = { {1.0/9, 1.0/9, 1.0/9},
						   {1.0/9, 1.0/9, 1.0/9},
						   {1.0/9, 1.0/9, 1.0/9} };
	int kernelSize = 3;
	double temp = 0;
	unsigned char* pic = new unsigned char[(m_nHeight + 2) * (m_nWidthBytes + 2)];	//����һ���µ�ͼ����������

	memset(pic, 0, (m_nHeight + 2) * (m_nWidthBytes + 2) * sizeof(unsigned char));
	for (int i = 1; i < m_nHeight + 1; i++) //������0ͼ��
	{
		for (int j = 1; j < m_nWidthBytes + 1; j++)
		{
			*(pic + i * m_nWidthBytes + j) = *(m_pDibBits + (i - 1) * m_nWidthBytes + j - 1);
		}
	}

	for (int i = 1; i < m_nHeight + 1; i++)
	{
		for (int j = 1; j < m_nWidthBytes + 1; j++)
		{
			temp = 0;
			for (int k = -1; k < 2; k++)
			{
				for (int p = -1; p < 2; p++)
				{
					temp+= *(pic + (i + k) * m_nWidthBytes + j + p) * SmoothKernel[k + 1][p + 1];
				}
			}
			*(m_pDibBits + (i - 1) * m_nWidthBytes + j - 1) = temp;
		}
	}
	delete[] pic;
}

void CDib::ConvSobel()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	int SobelKernelX[3][3] = { {-1, 0, 1},
								{-2, 0, 2},
								{-1, 0, 1} };
	int SobelKernelY[3][3] = { {-1, -2, -1},
								{0, 0, 0},
								{1, 2, 1} };
	int temp1 = 0;
	int temp2 = 0;
	int gradientModule = 0;
	unsigned char* pic= new unsigned char[(m_nHeight+2) * (m_nWidthBytes+2)];	//����һ���µ�ͼ����������

	memset(pic, 0, (m_nHeight + 2) * (m_nWidthBytes + 2) * sizeof(unsigned char));
	for (int i = 1; i < m_nHeight+1; i++) //������0ͼ��
	{
		for (int j = 1; j < m_nWidthBytes +1; j++)
		{
			*(pic + i * m_nWidthBytes + j) = *(m_pDibBits + (i - 1) * m_nWidthBytes + j - 1);
		}
	}
	for (int i = 1; i < m_nHeight+1; i++)
	{
		for (int j = 1; j < m_nWidthBytes +1; j++)
		{
			temp1 = 0;
			temp2 = 0;
			for (int k = -1; k < 2; k++)
			{
				for (int p = -1; p < 2; p++)
				{
					temp1 += *(pic + (i + k) * m_nWidthBytes + j + p) * SobelKernelX[k + 1][p + 1];
					temp2 += *(pic + (i + k) * m_nWidthBytes + j + p) * SobelKernelY[k + 1][p + 1];
				}
			}
			gradientModule = sqrt(temp1 * temp1 + temp2 * temp2);
			if (gradientModule > 255)
				gradientModule = 255;
			*(m_pDibBits + (i-1) * m_nWidthBytes + j-1) = gradientModule;
		}
	}
	delete[] pic;
}


void CDib::ConvLaplacian()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	int LapKernel[3][3] = { {-1, -1, -1},
							{-1, 9, -1},
							{-1, -1, -1} };
	double temp = 0;

	int kernelSize = 3;
	long newHeight = m_nHeight +(kernelSize - 1);
	long newWidth = m_nWidth + (kernelSize - 1);
	int newWidthBytes = newWidth*m_nBitCount;
	unsigned char* pic = new unsigned char[(m_nHeight + 2) * (m_nWidthBytes + 2)];	//����һ���µ�ͼ����������

	memset(pic, 0, (m_nHeight + 2) * (m_nWidthBytes + 2) * sizeof(unsigned char));

	for (int i = 1; i < m_nHeight + 1; i++) //������0ͼ��
	{
		for (int j = 1; j < m_nWidthBytes + 1; j++)
		{
			*(pic + i * m_nWidthBytes + j) = *(m_pDibBits + (i - 1) * m_nWidthBytes + j - 1);
		}
	}

	for (int i = 1; i < m_nHeight +1 ; i++)
	{
		for (int j = 1; j < m_nWidthBytes + 1; j++)
		{
			temp = 0;
			for (int k = -1; k < 2; k++)
			{
				for (int p = -1; p < 2; p++)
				{
					temp += *(pic + (i + k) * m_nWidthBytes + j + p) * LapKernel[k + 1][p + 1];
				}
			}
			if(temp>255)
				temp = 255;
			if(temp<0)
				temp = 0;
			m_pDibBits[(i-1) * m_nWidthBytes + j-1] = temp;
		}
	}
	delete[] pic;
}

double CalculateElapsedTime(double startTickCount) {
	return GetTickCount64() - startTickCount;
}

double CDib::ConvGaussian(int Dimenssion)//19*19��˹�˲�
{
	// ��ȡ����ʼ����ʱ��ʱ��
	double startTickCount = GetTickCount64();

	int KernelSize = 19;
	double sigma = (KernelSize - 1) / 6.0;
	int halfSize = (KernelSize - 1) / 2;
	long newHeight = m_nHeight + (KernelSize - 1);
	long newWidth = m_nWidth + (KernelSize - 1);
	int newWidthBytes = newWidth;
	// TODO: �ڴ˴����ʵ�ִ���.
	if (Dimenssion == 1) {//����һά���ʵ��
		double GaussianKernel[19] = {0.0};
		//���ɸ�˹��
		double sum = 0;
		for (int i = -9; i <= 9; i++) {
			GaussianKernel[i + 9] = exp(-i * i / (2 * sigma * sigma)) / (sqrt(2 * Pi) * sigma);
			sum += GaussianKernel[i + 9];
		}
		//��һ��
		for (int i = 0; i < 19; i++) {
			GaussianKernel[i] /= sum;
		}
		double* pic = new double[newHeight * newWidth];	//����һ���µ�ͼ����������
		double *pictemp = new double[newHeight * newWidth];				
		memset(pic, 0, newHeight * newWidth* sizeof(double));//����ǵ�Ҫ��sizeof(double)������sizeof(unsigned char)
		memset(pictemp, 0, newHeight * newWidth* sizeof(double));
		//������0ͼ��
		for (int i = 9; i < (newHeight - 9); i++) {
			for (int j = 9; j < (newWidth - 9); j++) {
				pic[i * newWidthBytes + j] = m_pDibBits[(i - 9) * m_nWidthBytes + j - 9];
				pictemp[i * newWidthBytes + j] = m_pDibBits[(i - 9) * m_nWidthBytes + j - 9];
			}
		}
		//��һ�ξ��
		double temp = 0;
		for (int i = 9; i < (newHeight - 9); i++) {
			for (int j = 9; j < (newWidth - 9); j++) {
				temp = 0;
				for (int k = -halfSize; k <= halfSize; k++) {
					temp += (pic[(i + k) * newWidthBytes + j] * GaussianKernel[k + halfSize]);
				}
				pictemp[i * newWidthBytes + j] = temp;
			}
		}
		//�ڶ��ξ��
		for (int i = 9; i < (newHeight - 9); i++) {
			for (int j = 9; j < (newWidth - 9); j++) {
				temp = 0;
				for (int k = -halfSize; k <= halfSize; k++) {
					temp += (pictemp[i * newWidthBytes + j + k] * GaussianKernel[k + halfSize]);
				}
				if (temp > 255)
					temp = 255;
				if (temp < 0)
					temp = 0;
				m_pDibBits[(i - 9) * m_nWidthBytes + j - 9] = temp;
			}
		}
		delete[] pic;
		delete[] pictemp;
	}

	else if (Dimenssion == 2) {
		//19*19��˹��
		double GaussianKernel[19][19];
		//���ɸ�˹��
		double sum = 0;
		for (int i = -9; i <= 9; i++) {
			for (int j = -9; j <= 9; j++) {
				GaussianKernel[i + 9][j + 9] = exp(-(i * i + j * j) / (2 * sigma * sigma)) / (2 * Pi * sigma * sigma);
				sum += GaussianKernel[i + 9][j + 9];
			}
		}
		//��һ��
		for (int i = 0; i < 19; i++) {
			for (int j = 0; j < 19; j++) {
				GaussianKernel[i][j] /= sum;
			}
		}
		unsigned char* pic = new unsigned char[newHeight * newWidth];	//����һ���µ�ͼ����������
		memset(pic, 0, newHeight * newWidth*sizeof(unsigned char));
		//������0ͼ��
		for (int i = 9; i < (newHeight - 9); i++) {
			for (int j = 9; j < (newWidth - 9); j++) {
				pic[i * newWidthBytes + j] = m_pDibBits[(i - 9) * m_nWidthBytes + j - 9];
			}
		}
		double temp = 0;
		for (int i = 9; i < (newHeight - 9); i++) {
			for (int j = 9; j < (newWidth - 9); j++) {
				temp = 0;
				for (int k = -halfSize; k <= halfSize; k++) {
					for (int p = -halfSize; p <= halfSize; p++) {
						temp += (pic[(i + k) * newWidthBytes + j + p] * GaussianKernel[k + halfSize][p + halfSize]);
					}
				}
				if (temp > 255)
					temp = 255;
				if (temp < 0)
					temp = 0;
				m_pDibBits[(i - 9) * m_nWidthBytes + j - 9] = temp;
			}
		}
		delete[] pic;
	}

	// ��ȡ�����������ʱ��ʱ��
	double elapsedTime = CalculateElapsedTime(startTickCount);

	
	return elapsedTime;
}

//
//void CDib::LoadRawFile(LPCTSTR lpszPathName, int nWidth, int nHeight, CString format, int nBitCount)
//{
//	std::vector<uint16_t> rawData(nWidth * nHeight);
//	std::ifstream file(lpszPathName, std::ios::binary);
//	file.read(reinterpret_cast<char*>(rawData.data()), rawData.size() * sizeof(uint16_t));
//
//	if (file.fail()) {
//		AfxMessageBox(_T("Failed to read file"));
//	}
//
//	this->Create(nWidth, nHeight, 8);
//	//this->Create(nWidth, nHeight, 16);
//	HBITMAP hBitmap = (HBITMAP)this->Detach();
//	RGBQUAD palette[256];
//	for (int i = 0; i < 256; ++i) {
//		palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = palette[i].rgbReserved = (BYTE)i;
//
//	}
//	HDC hDC = ::GetDC(NULL);
//	HDC hMemDC = ::CreateCompatibleDC(hDC);
//	::ReleaseDC(NULL, hDC);
//	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);
//	SetDIBColorTable(hMemDC, 0, 256, palette);
//	//SetDIBColorTable(hMemDC, 0, 65536, palette);
//	::SelectObject(hMemDC, hOldBitmap);
//	::DeleteDC(hMemDC);
//	Attach(hBitmap);
//
//	m_nWidth = GetWidth();
//	m_nHeight = GetHeight();
//	m_nWidthBytes = abs(GetPitch());
//	m_nBitCount = GetBPP();
//	m_pDibBits = (unsigned char*)this->GetBits() + (m_nHeight - 1) * this->GetPitch();
//	for (int i = 0; i < m_nHeight; i++)
//	{
//		for (int j = 0; j < m_nWidth; j++)
//		{
//			uint8_t grayVal = static_cast<uint8_t>((rawData[i * m_nWidth + j] / 65535.0) * 255.0);
//			int invertedY = m_nWidth - 1 - i;
//			*(m_pDibBits + invertedY * m_nWidthBytes + j) = grayVal;
//		}
//	}
//}
//


void CDib::DispWindow(CString win)
{
	// TODO: �ڴ˴����ʵ�ִ���.
	double pixelVal = 0;
	int min = 0;
	int max = 0;
	if (win == "Bone") {
		min = 98;
		max = 197;
	}
	else if (win == "Lung") {
		min = 25;
		max = 100;
	}

	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidthBytes; j++)
		{
			pixelVal = *(m_pDibBits + i * m_nWidthBytes + j);
			if (pixelVal < min)
			{
				pixelVal = 0;
			}
			else if (pixelVal < max)
			{
				pixelVal = 255*(pixelVal-min)/(max-min);
			}
			else
			{
				pixelVal = 0;
			}
			*(m_pDibBits + i * m_nWidthBytes + j) = (int)pixelVal;
		}
	}
}


void CDib::LoadRawFile(LPCTSTR lpszPathName, int nWidth, int nHeight, CString format, int nBitCount)
{
	//�����ݴ��ļ�����ֵ�Ļ�����//16λ������raw
	if (m_pRawBuffer != NULL)
	{
		delete[] m_pRawBuffer;
		m_pRawBuffer = NULL;
	}
	m_pRawBuffer = new uint16_t[nWidth * nHeight];
	memset(m_pRawBuffer, 0, nWidth * nHeight * sizeof(uint16_t));

	//ʹ��fistream��ȡraw�ļ�������ֵ��buffer
	std::ifstream file(lpszPathName, std::ios::binary);
	file.read(reinterpret_cast<char*>(m_pRawBuffer), nWidth * nHeight * sizeof(uint16_t));

	//�ļ���ʧ�ܵ������棬Ȼ���˳�
	if (file.fail()) {
		AfxMessageBox(_T("��ȡ�ļ�ʧ��"));
		return;
	}
	file.close();
	//��Ϊ��ȡ���������ظ�8λ�͵�8λ˳���Ƿ��ģ���������ظߵ�8λ���жԵ���ͬʱ��ȡ���ֵ
	for (int i = 0; i < nWidth * nHeight; i++) {
		m_pRawBuffer[i] = ((m_pRawBuffer[i] & 0xff00) >> 8) | ((m_pRawBuffer[i] & 0x00ff) << 8);
	}
	CreateDisplayDib(m_pRawBuffer, nWidth, nHeight, 8);
}

void CDib::CreateDisplayDib(uint16_t* RawBuffer, int nWidth, int nHeight, int nBitCount)
{
	// TODO: �ڴ˴����ʵ�ִ���.
	//Create new Dib
	this->Create(nWidth, nHeight, nBitCount, 0);
	if (IsIndexed())
	{
		int nColors = GetMaxColorTableEntries();
		if (nColors > 0)
		{
			RGBQUAD* pal = new RGBQUAD[nColors];
			for (int i = 0; i < nColors; i++)
			{
				pal[i].rgbBlue = i;
				pal[i].rgbGreen = i;
				pal[i].rgbRed = i;
			}
			SetColorTable(0, nColors, pal);
			delete[] pal;
		}
	}
	m_nWidth = GetWidth();
	m_nHeight = GetHeight();
	m_nWidthBytes = abs(GetPitch());
	m_nBitCount = GetBPP();
	m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1) * GetPitch();
	//��buffer�е�����ֵд��Dib
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			*(m_pDibBits + i * m_nWidthBytes + j) = (BYTE)(RawBuffer[i * m_nWidth + j] >> 4);
		}
	}

}


void CDib::CreateWhiteBlock(int nWidth, int nHeight, int nBitCount,int rectW,int rectH)
{
	// TODO: �ڴ˴����ʵ�ִ���.
	Create(nWidth, nHeight, nBitCount, 0);
	if (IsIndexed())
	{
		int nColors = GetMaxColorTableEntries();
		if (nColors > 0)
		{
			RGBQUAD* pal = new RGBQUAD[nColors];
			for (int i = 0; i < nColors; i++)
			{
				pal[i].rgbBlue = i;
				pal[i].rgbGreen = i;
				pal[i].rgbRed = i;
			}
			SetColorTable(0, nColors, pal);
			delete[] pal;
		}
	}
	unsigned char* temp = new unsigned char[nWidth * nHeight];
	memset(temp, 0, nWidth * nHeight * sizeof(unsigned char));
	int startX = (nWidth - rectW) / 2;
	int startY = (nHeight - rectH) / 2;
	int endX = startX + rectW;
	int endY = startY + rectH;
	for (int i = startY; i < endY; i++)
	{
		for (int j = startX; j < endX; j++)
		{
			temp[i * nWidth + j] = 255;
		}
	}

	m_nWidth = GetWidth();
	m_nHeight = GetHeight();
	m_nWidthBytes = abs(GetPitch());
	m_nBitCount = GetBPP();
	m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1) * GetPitch();
	//��buffer�е�����ֵд��Dib
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			*(m_pDibBits + i * m_nWidthBytes + j) = temp[i * m_nWidthBytes + j];
		}
	}
	delete[] temp;
	
}

void CDib::CreateRGBCircle(int nWidth, int nHeight, int nBitCount, int radius)
{
	// TODO: �ڴ˴����ʵ�ִ���.
	if (m_pRGBBuffer.size()!=0)
	{
		m_pRGBBuffer.clear();
	}
	Create(nWidth, nHeight, nBitCount, 0);

	if (IsIndexed())
	{
		int nColors = GetMaxColorTableEntries();
		if (nColors > 0)
		{
			RGBQUAD* pal = new RGBQUAD[nColors];
			for (int i = 0; i < nColors; i++)
			{
				pal[i].rgbBlue = i;
				pal[i].rgbGreen = i;
				pal[i].rgbRed = i;
			}
			SetColorTable(0, nColors, pal);
			delete[] pal;
		}
	}
	m_nWidth = GetWidth();
	m_nHeight = GetHeight();
	m_nWidthBytes = abs(GetPitch());
	m_nBitCount = GetBPP();
	m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1) * GetPitch();

	int centerRx = m_nWidth / 2;
	int centerRy = m_nHeight / 2 + radius/2;
	int centerGx = m_nWidth / 2 - radius/2;
	int centerGy = m_nHeight / 2 - radius/2;
	int centerBx = m_nWidth / 2 + radius/2;
	int centerBy = m_nHeight / 2 - radius/2;

	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			if ((i - centerRy) * (i - centerRy) + (j - centerRx) * (j - centerRx) <= radius * radius)
			{
				//��ɫ
				m_pDibBits[i * m_nWidthBytes + j * 3 + 2] = 255;
			}
			if ((i - centerGy) * (i - centerGy) + (j - centerGx) * (j - centerGx) <= radius * radius)
			{
				//��ɫ
				m_pDibBits[i * m_nWidthBytes + j * 3 + 1] = 255;
			}
			if ((i - centerBy) * (i - centerBy) + (j - centerBx) * (j - centerBx) <= radius * radius)
			{
				//��ɫ
				m_pDibBits[i * m_nWidthBytes + j * 3] = 255;
			}
		}
	}
	for (int i = 0; i < m_nHeight*m_nWidthBytes; i++)
	{
		m_pRGBBuffer.push_back(m_pDibBits[i]);
	}
}

void CDib::myFFT(long nWidth, long nHeight)
{
	// TODO: �ڴ˴����ʵ�ִ���.
	bool flag = false;
	flag=FFT(m_pDibBits, nWidth, nHeight);
	if (flag == false)
	{
		AfxMessageBox(_T("FFTʧ��"));
	}
}

BOOL CDib::myInverseFFT(long nWidth, long nHeight)
{
	unsigned char* lpSrc;							// ָ��Դͼ���ָ��
	int y;										// ѭ�����Ʊ���
	int x;										// ѭ�����Ʊ���
	double dTmpOne;								//�����ʱ����
	double dTmpTwo;								//�����ʱ����
	int nTransWidth;								// ����Ҷ�任�Ŀ�ȣ�2���������ݣ�
	int nTransHeight;								// ����Ҷ�任�ĸ߶ȣ�2���������ݣ�
	double unchValue;								// ����ͼ������ػҶȵ���ʱ����
	complex<double>* pCTData;						// ָ��ʱ�����ݵ�ָ��
	complex<double>* pCFData;						// ָ��Ƶ�����ݵ�ָ��
	// ������и���Ҷ�任�ĵ���������	��2���������ݣ�
	dTmpOne = log(1.0 * nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;
	// ������и���Ҷ�任�ĵ��������� ��2���������ݣ�
	dTmpOne = log(1.0 * nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;
	double dReal;									// ����Ҷ�任��ʵ��
	double dImag;									// ����Ҷ�任���鲿

	pCTData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	pCFData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	// ͼ�����ݵĿ�͸߲�һ����2���������ݣ�����pCTData��һ����������Ҫ��0
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCFData[y * nTransWidth + x] = complex<double>(0, 0);		// ����
		}
	}
	//��ͼ�����ݴ���pCFData
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			unchValue = m_pDibBits[y * nWidth + x] * pow(-1.0, x + y);	//����-1��x+y�η�����Ƶ��0���Ƶ�����
			pCFData[y * nTransWidth + x] = complex<double>(unchValue, 0);
		}
	}
	
	// code needed to insert here

	IFFT_2D(pCFData, pCTData,nHeight, nWidth); 				// ����Ҷ���任

	double maxVal = 0;
	double dVal = 0;
	for (y = 0; y < nHeight; y++) {
		for (x = 0; x < nWidth; x++) {
			dReal=pCTData[y * nTransWidth + x].real();		// ʵ��
			dVal = abs(dReal);
			if (dVal > maxVal) {
				maxVal = dReal;
			}
		}
	}

	double powNum = 1.0 / 5.0;
	double maxPow = pow(maxVal, powNum);

	for(y=0; y<nHeight; y++)								// ���任�����ݴ���lpDIBBits
	{
		for(x=0; x<nWidth; x++)
		{
			//��Ҫ�����źŵ����������Լ�ʵ�����õ�ͼ�������ǻҶ�ֵ����ԭʼ����
			dReal = pCTData[y*nTransWidth + x].real() ;		// ʵ��
			dImag = pCTData[y*nTransWidth + x].imag() ;		// �鲿
			dVal = abs(dReal);
			dVal=pow(dVal,powNum);
			dVal= dVal / maxPow * 255;
			unchValue = dVal*pow(-1.0,x+y);
			if (unchValue < 0)
			{
				unchValue = 0;
			}
			if (unchValue > 255)
			{
				unchValue = 255;
			}
			// ָ��DIB��y�У���x�����ص�ָ��
			m_pDibBits[y*nWidth+x] = (unsigned char)unchValue;
			//lpSrc = (unsigned char*)m_pDibBits + nWidth * (nHeight - 1 - y) + x;
			//*lpSrc =(BYTE)unchValue ;
		}
	}	

	delete[]pCTData;										// �ͷ��ڴ�
	delete[]pCFData;										// �ͷ��ڴ�
	pCTData = NULL;
	pCFData = NULL;
	return (true);										//���ؽ��
}



BOOL CDib::Filter(int type, double cutOff)
//type==0,�����ͨ�˲��� type==1,�����ͨ�˲���
//type==2,��˹��ͨ�˲��� type==3,��˹��ͨ�˲���
//type==4,������˹��ͨ�˲��� type==5,������˹��ͨ�˲���
{
	// TODO: �ڴ˴����ʵ�ִ���.
	
	unsigned char* lpSrc;							// ָ��Դͼ���ָ��
	int y;										// ѭ�����Ʊ���
	int x;										// ѭ�����Ʊ���
	double dTmpOne;								//�����ʱ����
	double dTmpTwo;								//�����ʱ����
	int nTransWidth;								// ����Ҷ�任�Ŀ�ȣ�2���������ݣ�
	int nTransHeight;								// ����Ҷ�任�ĸ߶ȣ�2���������ݣ�
	double unchValue;								// ����ͼ������ػҶȵ���ʱ����
	long nHeight = m_nHeight;
	long nWidth = m_nWidth;
	unsigned char* pDIBBits = m_pDibBits;			// ָ��DIB���ݵ�ָ��
	complex<double>* pCTData;						// ָ��ʱ�����ݵ�ָ��
	complex<double>* pCFData;						// ָ��Ƶ�����ݵ�ָ��
	// ������и���Ҷ�任�ĵ���������	��2���������ݣ�
	dTmpOne = log(1.0 * m_nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;
	// ������и���Ҷ�任�ĵ��������� ��2���������ݣ�
	dTmpOne = log(1.0 * nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;
	double dReal;									// ����Ҷ�任��ʵ��
	double dImag;									// ����Ҷ�任���鲿

	pCTData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	pCFData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	// ͼ�����ݵĿ�͸߲�һ����2���������ݣ�����pCTData��һ����������Ҫ��0
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[y * nTransWidth + x] = complex<double>(0, 0);		// ����
		}
	}
	//��ͼ�����ݴ���pCTData
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			unchValue = (*lpSrc) * pow(-1.0, x + y);	//����-1��x+y�η�����Ƶ��0���Ƶ�����
			pCTData[y * nTransWidth + x] = complex<double>(unchValue, 0);
		}
	}
	FFT_2D(pCTData, nWidth, nHeight, pCFData);				// ����Ҷ���任
	//����Ľ������pCTData��

	///////////////////////////////////////////////////////////////////////////
	// code needed to insert here
	double dist = 0;
	int centralX = m_nWidth / 2;
	int centralY = m_nHeight / 2;

	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			dist = sqrt((x - centralX) * (x - centralX) + (y - centralY) * (y - centralY));
			if (type == 0) {//�����ͨ
				if (dist >= cutOff)
				{
					pCFData[y * nTransWidth + x] = complex<double>(0, 0);
				}
			}
			else if (type == 1) {//�����ͨ
				if (dist <= cutOff)
				{
					pCFData[y * nTransWidth + x] = complex<double>(0, 0);
				}
			}
			else if (type == 2) {//��˹��ͨ
				pCFData[y * nTransWidth + x] = pCFData[y * nTransWidth + x] * exp(-dist * dist / (2 * cutOff * cutOff));
			}
			else if (type == 3) {//��˹��ͨ
				pCFData[y * nTransWidth + x] = pCFData[y * nTransWidth + x] * (1 - exp(-dist * dist / (2 * cutOff * cutOff)));
			}
			else if (type == 4) {//������˹��ͨ
				pCFData[y * nTransWidth + x] = pCFData[y * nTransWidth + x] / (1 + pow(dist / cutOff, 2));
			}
			else if (type == 5) {//������˹��ͨ
				pCFData[y * nTransWidth + x] = pCFData[y * nTransWidth + x] * (1 - 1 / (1 + pow(dist / cutOff, 2)));
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////

	IFFT_2D(pCFData, pCTData, nHeight, nWidth); 				// ����Ҷ���任

	for (y = 0; y < nHeight; y++)								// ���任�����ݴ���lpDIBBits
	{
		for (x = 0; x < nWidth; x++)
		{
			//��Ҫ�����źŵ����������Լ�ʵ�����õ�ͼ�������ǻҶ�ֵ����ԭʼ����
			dReal = pCTData[y * nTransWidth + x].real();		// ʵ��
			dImag = pCTData[y * nTransWidth + x].imag();		// �鲿
			unchValue = dReal * pow(-1.0, x + y);
			if (unchValue < 0)
			{
				unchValue = 0;
			}
			if (unchValue > 0xff)
			{
				unchValue = 0xff;
			}
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			*lpSrc = (BYTE)unchValue;
		}
	}

	delete[]pCTData;										// �ͷ��ڴ�
	delete[]pCFData;										// �ͷ��ڴ�
	pCTData = NULL;
	pCFData = NULL;
	return (true);										//���ؽ��
}


void CDib::medianFilter()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	int i, j, k, l;
	int nWidth = m_nWidth;
	int nHeight = m_nHeight;
	int nWidthBytes = m_nWidthBytes;
	unsigned char* pDIBBits = m_pDibBits;
	unsigned char* pTemp = new unsigned char[nWidth * nHeight];
	for (i = 1; i < nHeight - 1; i++)
	{
		for (j = 1; j < nWidth - 1; j++)
		{
			int temp[9];
			for (k = 0; k < 3; k++)
			{
				for (l = 0; l < 3; l++)
				{
					temp[k * 3 + l] = *(pDIBBits + (i + k - 1) * nWidthBytes + j + l - 1);
				}
			}
			for (k = 0; k < 9; k++)
			{
				for (l = 0; l < 8; l++)
				{
					if (temp[l] > temp[l + 1])
					{
						int t = temp[l];
						temp[l] = temp[l + 1];
						temp[l + 1] = t;
					}
				}
			}
			pTemp[i * nWidth + j] = temp[4];
		}
	}
	for (i = 1; i < nHeight - 1; i++)
	{
		for (j = 1; j < nWidth - 1; j++)
		{
			*(pDIBBits + i * nWidthBytes + j) = pTemp[i * nWidth + j];
		}
	}
	delete[] pTemp;

}


void CDib::AdaptiveMedianFilter()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	int i, j, k, l;
	int nWidth = m_nWidth;
	int nHeight = m_nHeight;
	int nWidthBytes = m_nWidthBytes;
	unsigned char* pDIBBits = m_pDibBits;
	unsigned char* pTemp = new unsigned char[nWidth * nHeight];

	int zmin, zmax, zmed, zxy;
	int Smax = 11;
	//int Sxy = 3;
	int n;
	for (i = 0; i < nHeight ; i++)
	{
		for (j = 0; j < nWidth ; j++)
		{
			n = 1;
			while (n < Smax)
			{
				zmin = 255;
				zmax = 0;
				zmed = 0;
				vector<int> zxyList;
				for (k = -(n-1)/2; k <= (n-1)/2; k++)
				{
					for (l = -(n-1)/2; l <= (n-1)/2; l++)
					{
						if (i + k >= 0 && i + k < nHeight && j + l >= 0 && j + l < nWidth) {
							zxy = *(pDIBBits + (i + k) * nWidthBytes + j + l);							
						}
						else if (i + k < 0)
						{
							zxy = *(pDIBBits + abs(k) * nWidthBytes + j + l);
						}
						else if (i + k >= nHeight)
						{
							zxy = *(pDIBBits + (nHeight - 1 - abs(k)) * nWidthBytes + j + l);
						}
						else if (j + l < 0)
						{
							zxy = *(pDIBBits + (i + k) * nWidthBytes + abs(l));
						}
						else if (j + l >= nWidth)
						{
							zxy = *(pDIBBits + (i + k) * nWidthBytes + nWidth - 1 - abs(l));
						}
						if (zxy < zmin)
						{
							zmin = zxy;
						}
						if (zxy > zmax)
						{
							zmax = zxy;
						}
						zxyList.push_back(zxy);
					}
				}
				sort(zxyList.begin(), zxyList.end());
				zmed = zxyList[(n * n - 1) / 2];

				if (zmed > zmin && zmed < zmax)
				{
					zxy = *(pDIBBits + i * nWidthBytes + j);
					if (zxy > zmin && zxy < zmax)
					{
						pTemp[i * nWidthBytes + j] = zxy;
					}
					else
					{
						pTemp[i * nWidthBytes + j] = zmed;
					}
					break;
				}
				else
				{
					n += 2;	//��֤nΪ����
					if (n == Smax)
					{
						pTemp[i * nWidth + j] = zmed;
					}
				}
			}
		}
	}
	for (i = 0; i < nHeight; i++)
	{
		for (j = 0; j < nWidth; j++)
		{
			*(pDIBBits + i * nWidthBytes + j) = pTemp[i * nWidthBytes + j];
		}
	}
	delete[] pTemp;
}


void CDib::Degradation(double k)
{
	// TODO: �ڴ˴����ʵ�ִ���.
	//����ͼ���˻�
	//Hufnagel and Stanleyģ��
	//H(u,v)=exp(-k*(u^2+v^2)^(5/6))
	unsigned char* lpSrc;							// ָ��Դͼ���ָ��
	int y;										// ѭ�����Ʊ���
	int x;										// ѭ�����Ʊ���
	double dTmpOne;								//�����ʱ����
	double dTmpTwo;								//�����ʱ����
	int nTransWidth;								// ����Ҷ�任�Ŀ�ȣ�2���������ݣ�
	int nTransHeight;								// ����Ҷ�任�ĸ߶ȣ�2���������ݣ�
	double unchValue;								// ����ͼ������ػҶȵ���ʱ����
	long nHeight = m_nHeight;
	long nWidth = m_nWidth;
	unsigned char* pDIBBits = m_pDibBits;			// ָ��DIB���ݵ�ָ��
	complex<double>* pCTData;						// ָ��ʱ�����ݵ�ָ��
	complex<double>* pCFData;						// ָ��Ƶ�����ݵ�ָ��
	// ������и���Ҷ�任�ĵ���������	��2���������ݣ�
	dTmpOne = log(1.0 * m_nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;
	// ������и���Ҷ�任�ĵ��������� ��2���������ݣ�
	dTmpOne = log(1.0 * nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;
	double dReal;									// ����Ҷ�任��ʵ��
	double dImag;									// ����Ҷ�任���鲿

	pCTData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	pCFData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	// ͼ�����ݵĿ�͸߲�һ����2���������ݣ�����pCTData��һ����������Ҫ��0
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[y * nTransWidth + x] = complex<double>(0, 0);		// ����
		}
	}
	//��ͼ�����ݴ���pCTData
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			unchValue = (*lpSrc) * pow(-1.0, x + y);	//����-1��x+y�η�����Ƶ��0���Ƶ�����
			pCTData[y * nTransWidth + x] = complex<double>(unchValue, 0);
		}
	}
	FFT_2D(pCTData, nWidth, nHeight, pCFData);				// ����Ҷ���任
	//����Ľ������pCTData��

	///////////////////////////////////////////////////////////////////////////
	// code needed to insert here
	double dist = 0;
	int centralX = m_nWidth / 2;
	int centralY = m_nHeight / 2;

	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			dist = sqrt((x - centralX) * (x - centralX) + (y - centralY) * (y - centralY));
			pCFData[y * nTransWidth + x] = pCFData[y * nTransWidth + x] * exp(-k * pow(dist*dist, 5.0 / 6.0));
		}
	}
	///////////////////////////////////////////////////////////////////////////

	IFFT_2D(pCFData, pCTData, nHeight, nWidth); 				// ����Ҷ���任

	for (y = 0; y < nHeight; y++)								// ���任�����ݴ���lpDIBBits
	{
		for (x = 0; x < nWidth; x++)
		{
			//��Ҫ�����źŵ����������Լ�ʵ�����õ�ͼ�������ǻҶ�ֵ����ԭʼ����
			dReal = pCTData[y * nTransWidth + x].real();		// ʵ��
			dImag = pCTData[y * nTransWidth + x].imag();		// �鲿
			unchValue = dReal * pow(-1.0, x + y);
			if (unchValue < 0)
			{
				unchValue = 0;
			}
			if (unchValue > 0xff)
			{
				unchValue = 0xff;
			}
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			*lpSrc = (BYTE)unchValue;
		}
	}

	delete[]pCTData;										// �ͷ��ڴ�
	delete[]pCFData;										// �ͷ��ڴ�
	pCTData = NULL;
	pCFData = NULL;
}


void CDib::InverseFiltering(double k,double cutoff)
{
	// TODO: �ڴ˴����ʵ�ִ���.
	// TODO: �ڴ˴����ʵ�ִ���.
	unsigned char* lpSrc;							// ָ��Դͼ���ָ��
	int y;										// ѭ�����Ʊ���
	int x;										// ѭ�����Ʊ���
	double dTmpOne;								//�����ʱ����
	double dTmpTwo;								//�����ʱ����
	int nTransWidth;								// ����Ҷ�任�Ŀ�ȣ�2���������ݣ�
	int nTransHeight;								// ����Ҷ�任�ĸ߶ȣ�2���������ݣ�
	double unchValue;								// ����ͼ������ػҶȵ���ʱ����
	long nHeight = m_nHeight;
	long nWidth = m_nWidth;
	unsigned char* pDIBBits = m_pDibBits;			// ָ��DIB���ݵ�ָ��
	complex<double>* pCTData;						// ָ��ʱ�����ݵ�ָ��
	complex<double>* pCFData;						// ָ��Ƶ�����ݵ�ָ��
	// ������и���Ҷ�任�ĵ���������	��2���������ݣ�
	dTmpOne = log(1.0 * m_nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;
	// ������и���Ҷ�任�ĵ��������� ��2���������ݣ�
	dTmpOne = log(1.0 * nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;
	double dReal;									// ����Ҷ�任��ʵ��
	double dImag;									// ����Ҷ�任���鲿

	pCTData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	pCFData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	// ͼ�����ݵĿ�͸߲�һ����2���������ݣ�����pCTData��һ����������Ҫ��0
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[y * nTransWidth + x] = complex<double>(0, 0);		// ����
		}
	}
	//��ͼ�����ݴ���pCTData
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			unchValue = (*lpSrc) * pow(-1.0, x + y);	//����-1��x+y�η�����Ƶ��0���Ƶ�����
			pCTData[y * nTransWidth + x] = complex<double>(unchValue, 0);
		}
	}
	FFT_2D(pCTData, nWidth, nHeight, pCFData);				// ����Ҷ���任
	//����Ľ������pCTData��

	///////////////////////////////////////////////////////////////////////////
	// code needed to insert here
	double dist = 0;
	int centralX = m_nWidth / 2;
	int centralY = m_nHeight / 2;
	//��ͼ�������˻������˲�
	//H(u,v)=exp(-k*((u+M/2)^2+(v+N/2)^2)^(5/6))
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			dist = sqrt((x - centralX) * (x - centralX) + (y - centralY) * (y - centralY));
			if (dist <= cutoff) {
				pCFData[y * nTransWidth + x] = pCFData[y * nTransWidth + x] / exp(-k * pow(dist * dist, 5.0 / 6.0));
			}
			else {
				pCFData[y * nTransWidth + x] = complex<double>(0, 0);
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////

	IFFT_2D(pCFData, pCTData, nHeight, nWidth); 				// ����Ҷ���任

	for (y = 0; y < nHeight; y++)								// ���任�����ݴ���lpDIBBits
	{
		for (x = 0; x < nWidth; x++)
		{
			//��Ҫ�����źŵ����������Լ�ʵ�����õ�ͼ�������ǻҶ�ֵ����ԭʼ����
			dReal = pCTData[y * nTransWidth + x].real();		// ʵ��
			dImag = pCTData[y * nTransWidth + x].imag();		// �鲿
			unchValue = dReal * pow(-1.0, x + y);
			if (unchValue < 0)
			{
				unchValue = 0;
			}
			if (unchValue > 0xff)
			{
				unchValue = 0xff;
			}
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			*lpSrc = (BYTE)unchValue;
		}
	}

	delete[]pCTData;										// �ͷ��ڴ�
	delete[]pCFData;										// �ͷ��ڴ�
	pCTData = NULL;
	pCFData = NULL;
}


void CDib::WienerFilter()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	unsigned char* lpSrc;							// ָ��Դͼ���ָ��
	int y;										// ѭ�����Ʊ���
	int x;										// ѭ�����Ʊ���
	double dTmpOne;								//�����ʱ����
	double dTmpTwo;								//�����ʱ����
	int nTransWidth;								// ����Ҷ�任�Ŀ�ȣ�2���������ݣ�
	int nTransHeight;								// ����Ҷ�任�ĸ߶ȣ�2���������ݣ�
	double unchValue;								// ����ͼ������ػҶȵ���ʱ����
	long nHeight = m_nHeight;
	long nWidth = m_nWidth;
	unsigned char* pDIBBits = m_pDibBits;			// ָ��DIB���ݵ�ָ��
	complex<double>* pCTData;						// ָ��ʱ�����ݵ�ָ��
	complex<double>* pCFData;						// ָ��Ƶ�����ݵ�ָ��
	// ������и���Ҷ�任�ĵ���������	��2���������ݣ�
	dTmpOne = log(1.0 * m_nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;
	// ������и���Ҷ�任�ĵ��������� ��2���������ݣ�
	dTmpOne = log(1.0 * nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;
	double dReal;									// ����Ҷ�任��ʵ��
	double dImag;									// ����Ҷ�任���鲿

	pCTData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	pCFData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	// ͼ�����ݵĿ�͸߲�һ����2���������ݣ�����pCTData��һ����������Ҫ��0
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[y * nTransWidth + x] = complex<double>(0, 0);		// ����
		}
	}
	//��ͼ�����ݴ���pCTData
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			unchValue = (*lpSrc) * pow(-1.0, x + y);	//����-1��x+y�η�����Ƶ��0���Ƶ�����
			pCTData[y * nTransWidth + x] = complex<double>(unchValue, 0);
		}
	}
	FFT_2D(pCTData, nWidth, nHeight, pCFData);				// ����Ҷ���任
	//����Ľ������pCTData��

	///////////////////////////////////////////////////////////////////////////
	// code needed to insert here
	double dist = 0;
	int centralX = m_nWidth / 2;
	int centralY = m_nHeight / 2;
	//WienerFilter
	
	double K= 0.01;
	double k = 0.0025;
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			dist = sqrt((x - centralX) * (x - centralX) + (y - centralY) * (y - centralY));
			double H = exp(-k * pow(dist * dist, 5.0 / 6.0));
			pCFData[y * nTransWidth + x] = pCFData[y * nTransWidth + x] * ((1 / H) * ((H * H) / (H * H + K)));
		}
	}
	///////////////////////////////////////////////////////////////////////////

	IFFT_2D(pCFData, pCTData, nHeight, nWidth); 				// ����Ҷ���任

	for (y = 0; y < nHeight; y++)								// ���任�����ݴ���lpDIBBits
	{
		for (x = 0; x < nWidth; x++)
		{
			//��Ҫ�����źŵ����������Լ�ʵ�����õ�ͼ�������ǻҶ�ֵ����ԭʼ����
			dReal = pCTData[y * nTransWidth + x].real();		// ʵ��
			dImag = pCTData[y * nTransWidth + x].imag();		// �鲿
			unchValue = dReal * pow(-1.0, x + y);
			if (unchValue < 0)
			{
				unchValue = 0;
			}
			if (unchValue > 0xff)
			{
				unchValue = 0xff;
			}
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			*lpSrc = (BYTE)unchValue;
		}
	}

	delete[]pCTData;										// �ͷ��ڴ�
	delete[]pCFData;										// �ͷ��ڴ�
	pCTData = NULL;
	pCFData = NULL;
}


double CDib::CalculatePSNR(unsigned char* trueImg)
{
	// TODO: �ڴ˴����ʵ�ִ���.
	//����ͼ��ķ�ֵ�����
	//PSNR=10*log10(255^2/MSE)
	//MSE=1/MN*sum((f(i,j)-f'(i,j))^2)
	//f(i,j)Ϊԭͼ��f'(i,j)Ϊ������ͼ��
	double MSE = 0.0;
	double PSNR = 0.0;
	unsigned char* lpSrc;							// ָ��Դͼ���ָ��
	unsigned char* lpDst;							// ָ��Ŀ��ͼ���ָ��
	int y;										// ѭ�����Ʊ���
	int x;										// ѭ�����Ʊ���
	long nHeight = m_nHeight;
	long nWidth = m_nWidth;
	lpSrc = new unsigned char[nHeight * nWidth];
	lpDst = new unsigned char[nHeight * nWidth];
	memset(lpSrc, 0, nHeight * nWidth);
	memset(lpDst, 0, nHeight * nWidth);
	//����MSE
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			lpSrc = (unsigned char*)m_pDibBits + nWidth * (nHeight - 1 - y) + x;
			lpDst = (unsigned char*)trueImg + nWidth * (nHeight - 1 - y) + x;
			MSE += pow(*lpSrc - *lpDst, 2);
		}
	}
	MSE = MSE / (nHeight * nWidth);
	PSNR = 10 * log10(255 * 255 / MSE);
	return PSNR;
}


void CDib::DeNoiseGaussian(double cutGau, double sigma)
{
	// TODO: �ڴ˴����ʵ�ִ���.
	unsigned char* lpSrc;							// ָ��Դͼ���ָ��
	int y;										// ѭ�����Ʊ���
	int x;										// ѭ�����Ʊ���
	double dTmpOne;								//�����ʱ����
	double dTmpTwo;								//�����ʱ����
	int nTransWidth;								// ����Ҷ�任�Ŀ�ȣ�2���������ݣ�
	int nTransHeight;								// ����Ҷ�任�ĸ߶ȣ�2���������ݣ�
	double unchValue;								// ����ͼ������ػҶȵ���ʱ����
	long nHeight = m_nHeight;
	long nWidth = m_nWidth;
	unsigned char* pDIBBits = m_pDibBits;			// ָ��DIB���ݵ�ָ��
	complex<double>* pCTData;						// ָ��ʱ�����ݵ�ָ��
	complex<double>* pCFData;						// ָ��Ƶ�����ݵ�ָ��
	// ������и���Ҷ�任�ĵ���������	��2���������ݣ�
	dTmpOne = log(1.0 * m_nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;
	// ������и���Ҷ�任�ĵ��������� ��2���������ݣ�
	dTmpOne = log(1.0 * nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;
	double dReal;									// ����Ҷ�任��ʵ��
	double dImag;									// ����Ҷ�任���鲿

	pCTData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	pCFData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	// ͼ�����ݵĿ�͸߲�һ����2���������ݣ�����pCTData��һ����������Ҫ��0
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[y * nTransWidth + x] = complex<double>(0, 0);		// ����
		}
	}
	//��ͼ�����ݴ���pCTData
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			unchValue = (*lpSrc) * pow(-1.0, x + y);	//����-1��x+y�η�����Ƶ��0���Ƶ�����
			pCTData[y * nTransWidth + x] = complex<double>(unchValue, 0);
		}
	}
	FFT_2D(pCTData, nWidth, nHeight, pCFData);				// ����Ҷ���任
	//����Ľ������pCTData��

	///////////////////////////////////////////////////////////////////////////
	// code needed to insert here
	//ʹ�ø�˹��ͨ�˲���
	double dist = 0;
	int centralX = m_nWidth / 2;
	int centralY = m_nHeight / 2;

	double K = 0.01;
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			dist = sqrt((x - centralX) * (x - centralX) + (y - centralY) * (y - centralY));
			//double H = exp(-2 * pi * pi * sigma * sigma * dist * dist);
			//pCFData[y * nTransWidth + x] = pCFData[y * nTransWidth + x] / H;
			pCFData[y * nTransWidth + x] = pCFData[y * nTransWidth + x] * exp(-dist * dist / (2 * cutGau * cutGau));
			
		}
	}
	///////////////////////////////////////////////////////////////////////////

	IFFT_2D(pCFData, pCTData, nHeight, nWidth); 				// ����Ҷ���任

	for (y = 0; y < nHeight; y++)								// ���任�����ݴ���lpDIBBits
	{
		for (x = 0; x < nWidth; x++)
		{
			//��Ҫ�����źŵ����������Լ�ʵ�����õ�ͼ�������ǻҶ�ֵ����ԭʼ����
			dReal = pCTData[y * nTransWidth + x].real();		// ʵ��
			dImag = pCTData[y * nTransWidth + x].imag();		// �鲿
			unchValue = dReal * pow(-1.0, x + y);
			if (unchValue < 0)
			{
				unchValue = 0;
			}
			if (unchValue > 0xff)
			{
				unchValue = 0xff;
			}
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			*lpSrc = (BYTE)unchValue;
		}
	}

	delete[]pCTData;										// �ͷ��ڴ�
	delete[]pCFData;										// �ͷ��ڴ�
	pCTData = NULL;
	pCFData = NULL;
	
}


void CDib::AdaptiveLocalDenoise()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	//����Ӧ�ֲ�����ռ��˲�
	//��ֵ������ƽ��ֵ�������ϵ�ƽ���Ҷ�
	//����������ϵ�ͼ��Աȶ�

	//���ȼ��㷽��
	long nColors = GetMaxColorTableEntries();
	long* Count = GrayValueCount();
	double* p = new double[nColors];
	double mean = 0.0;
	double variance = 0.0;
	memset(p, 0, nColors * sizeof(double));
	for (int i = 0; i < nColors; i++)
	{
		p[i] = (double)Count[i] / (double)(m_nHeight * m_nWidth);	//����������Ϊ���ʣ�������������
		mean += i * p[i];	//�����ֵ
	}
	for (int i = 0; i < nColors; i++)
	{
		variance += p[i] * (i - mean) * (i - mean);	//���㷽��
	}

	//����ֲ���ֵ�;ֲ�����
	int nWidth = m_nWidth;
	int nHeight = m_nHeight;
	int nWidthBytes = m_nWidthBytes;
	unsigned char* pDIBBits = m_pDibBits;
	unsigned char* pTemp = new unsigned char[nWidth * nHeight];
	int n = 9;
	int zxy;
	double ratio = 0.0;
	double meanSxy = 0.0;
	double varianceSxy = 0.0;

	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			ratio = 0.0;
			varianceSxy = 0.0;
			meanSxy = 0.0;
			vector<int> Sxy;
			for (int k = -(n - 1) / 2; k <= (n - 1) / 2; k++)
			{
				for (int l = -(n - 1) / 2; l <= (n - 1) / 2; l++)
				{
					if (i + k >= 0 && i + k < nHeight && j + l >= 0 && j + l < nWidth) {
						zxy = *(pDIBBits + (i + k) * nWidthBytes + j + l);
					}
					else if (i + k < 0)
					{
						zxy = *(pDIBBits + abs(k) * nWidthBytes + j + l);
					}
					else if (i + k >= nHeight)
					{
						zxy = *(pDIBBits + (nHeight - 1 - abs(k)) * nWidthBytes + j + l);
					}
					else if (j + l < 0)
					{
						zxy = *(pDIBBits + (i + k) * nWidthBytes + abs(l));
					}
					else if (j + l >= nWidth)
					{
						zxy = *(pDIBBits + (i + k) * nWidthBytes + nWidth - 1 - abs(l));
					}
					Sxy.push_back(zxy);
				}
			}
			meanSxy = (std::accumulate(Sxy.begin(), Sxy.end(), 0.0))/Sxy.size();
			for (int i = 0; i < Sxy.size(); i++)
			{
				varianceSxy += pow(Sxy[i] - meanSxy, 2);
			}
			if (variance > varianceSxy) 
			{
				ratio = 1;
			}
			else
			{
				ratio = variance / varianceSxy;
			}
			*(pTemp + i * nWidthBytes + j) = *(pDIBBits + i * nWidthBytes + j)-ratio*(*(pDIBBits + i * nWidthBytes + j)-meanSxy);
		}
	}
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			*(pDIBBits + i * nWidthBytes + j) = *(pTemp + i * nWidthBytes + j);
		}
	}
}


void CDib::ImgAdd(unsigned char* origin, unsigned char* add)
{
	// TODO: �ڴ˴����ʵ�ִ���.
	//����ͼ���������
	//����ͼ��Ĵ�С������ͬ

	long nHeight = m_nHeight;
	long nWidth = m_nWidth;
	double temp1 = 0.0;
	double temp2 = 0.0;
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			temp1=*(origin + i * nWidth + j);
			temp2=*(add + i * nWidth + j);
			temp1=temp1+temp2/5.0;
			temp1=pow(temp1, 0.9);
			*(origin + i * nWidth + j) = temp1;
			
		}
	}
}


void CDib::tempOpet()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			if (*(m_pDibBits + i * m_nWidthBytes + j) < 50)
			{
				*(m_pDibBits + i * m_nWidthBytes + j) = 0;
			}
		}
	}
}


void CDib::LoadManyRawFiles(LPCTSTR lpszPathName, int nWidth, int nHeight,int frames, int nBitCount)
{
	// TODO: �ڴ˴����ʵ�ִ���.
	//�����ݴ��ļ�����ֵ�Ļ�����//16λ������raw
	if (m_pRawBuffer != NULL)
	{
		delete[] m_pRawBuffer;
		m_pRawBuffer = NULL;
	}
	if (pointerArray.size() != 0)
	{
		for (int i = 0; i < pointerArray.size(); i++)
		{
			delete[] pointerArray[i];
		}
		pointerArray.clear();
	}
	std::ifstream file(lpszPathName, std::ios::binary);
	for (int i = 0; i < frames; i++)
	{
		m_pRawBuffer = new uint16_t[nWidth * nHeight];
		memset(m_pRawBuffer, 0, nWidth * nHeight * sizeof(uint16_t));

		//ʹ��fistream��ȡraw�ļ�������ֵ��buffer
		
		file.read(reinterpret_cast<char*>(m_pRawBuffer), nWidth * nHeight * sizeof(uint16_t));
		
		//��Ϊ��ȡ���������ظ�8λ�͵�8λ˳���Ƿ��ģ���������ظߵ�8λ���жԵ���ͬʱ��ȡ���ֵ
		for (int i = 0; i < nWidth * nHeight; i++) {
			m_pRawBuffer[i] = ((m_pRawBuffer[i] & 0xff00) >> 8) | ((m_pRawBuffer[i] & 0x00ff) << 8);
		}
		pointerArray.push_back(m_pRawBuffer);
	}
	file.close();
	DispManyRawFiles(pointerArray[0], nWidth, nHeight, 8);
	nowFrame = 0;
	viewManyRawStatus = 0;
}


void CDib::DispManyRawFiles(uint16_t* RawBuffer, int nWidth, int nHeight, int nBitCount)
{
	// TODO: �ڴ˴����ʵ�ִ���.
	//Create new Dib
	this->Create(nWidth, nHeight, nBitCount, 0);
	if (IsIndexed())
	{
		int nColors = GetMaxColorTableEntries();
		if (nColors > 0)
		{
			RGBQUAD* pal = new RGBQUAD[nColors];
			for (int i = 0; i < nColors; i++)
			{
				pal[i].rgbBlue = i;
				pal[i].rgbGreen = i;
				pal[i].rgbRed = i;
			}
			SetColorTable(0, nColors, pal);
			delete[] pal;
		}
	}
	m_nWidth = GetWidth();
	m_nHeight = GetHeight();
	m_nWidthBytes = abs(GetPitch());
	m_nBitCount = GetBPP();
	m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1) * GetPitch();
	//��buffer�е�����ֵд��Dib
	uint16_t* pRawBuffer = new uint16_t[nWidth*nHeight];
	for (int k = 0; k < nWidth * nHeight; k++)
	{
		pRawBuffer[k] = RawBuffer[k];
	}
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			*(m_pDibBits + i * m_nWidthBytes + j) = (BYTE)(pRawBuffer[i * m_nWidth + j] >> 4);
		}
	}
	delete[] pRawBuffer;
}


int CDib::ChangeCurrentFrame()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	return 0;
}


void CDib::DispDiffView(int status, int frame)
{
	// TODO: �ڴ˴����ʵ�ִ���.
	if (status == 0)//�����
	{
		Destroy();
		DispManyRawFiles(pointerArray[frame], 512, 512, 8);
	}
	else if (status == 1)//��״��
	{
		int frameNum = pointerArray.size();
		//ȷ���µ�ͼ��߶�
		double pixWidth = 0.68359375;
		double layerHei = 1;
		int newHeight = (layerHei / pixWidth) * (double)frameNum;
		double scaleX = 1.0;
		double scaleY = (double)frameNum / newHeight;

		if (CronalArrary.size() != 0)
		{
			//���CronalArray��Ϊ�գ���ֱ�ӿ�����ʾ�ˣ�����Ҫ��ֵ
			DispManyRawFiles(CronalArrary[frame], m_nWidth, newHeight, 8);
		}
		else {
			
			for (int k = 0; k < newHeight; k++) {
				m_pRawBuffer = new uint16_t[frameNum * m_nWidth];
				memset(m_pRawBuffer, 0, frameNum * m_nWidth * sizeof(uint16_t));
				for (int i = 0; i < frameNum; i++) 
				{
					for (int j = 0; j < m_nWidth; j++) 
					{
						m_pRawBuffer[i * m_nWidth + j] = pointerArray[i][k * m_nWidth + j];
					}
				}
				CronalArrary.push_back(m_pRawBuffer);
			}
			vector<uint16_t*> temp(CronalArrary);
			CronalArrary.clear();
			//˫���Բ�ֵ
			for (int k = 0; k < temp.size(); k++) {
				m_pRawBuffer = new uint16_t[newHeight * m_nWidth];
				memset(m_pRawBuffer, 0, newHeight * m_nWidth * sizeof(uint16_t));
				for (int y = 0; y < newHeight; y++) {
					for (int x = 0; x < m_nWidth; x++) {
						// ������ԭʼͼ���е�λ��
						double xOriginal = (x * scaleX);
						double yOriginal = (y * scaleY);

						// �ҵ�������ĸ�����
						int x1 = (int)xOriginal;
						int y1 = (int)yOriginal;
						int x2 = min(x1 + 1, m_nWidth - 1);
						int y2 = min(y1 + 1, m_nHeight - 1);

						// ִ��˫���Բ�ֵ
						uint16_t q11 = temp[k][y1 * m_nWidth + x1];
						uint16_t q12 = temp[k][y2 * m_nWidth + x1];
						uint16_t q21 = temp[k][y1 * m_nWidth + x2];
						uint16_t q22 = temp[k][y2 * m_nWidth + x2];

						uint16_t r1 = (q21 - q11) * (xOriginal - x1) + q11;
						uint16_t r2 = (q22 - q12) * (xOriginal - x1) + q12;
						uint16_t val = (r2 - r1) * (yOriginal - y1) + r1;

						m_pRawBuffer[y * m_nWidth + x] = val;
					}
				}
				CronalArrary.push_back(m_pRawBuffer);
			}
			DispManyRawFiles(CronalArrary[frame], m_nWidth, newHeight, 8);
			temp.clear();
		}
	}
	else if (status == 2)
	{
		int frameNum = pointerArray.size();
		//ȷ���µ�ͼ��߶�
		double pixWidth = 0.68359375;
		double layerHei = 1;
		int newHeight = (layerHei / pixWidth) * (double)frameNum;
		double scaleX = 1.0;
		double scaleY = (double)frameNum / newHeight;

		if (MedianArrary.size() != 0)
		{
			DispManyRawFiles(MedianArrary[frame], m_nWidth, newHeight, 8);
		}
		else {
			for (int k = 0; k < newHeight; k++) {
				m_pRawBuffer = new uint16_t[frameNum * m_nWidth];
				memset(m_pRawBuffer, 0, frameNum * m_nWidth * sizeof(uint16_t));
				for (int i = 0; i < frameNum; i++) 
				{
					for (int j = 0; j < m_nWidth; j++) 
					{
						m_pRawBuffer[i * m_nWidth + j] = pointerArray[i][j * m_nWidth + k];
					}
				}
				MedianArrary.push_back(m_pRawBuffer);
			}
			vector<uint16_t*> temp(MedianArrary);
			MedianArrary.clear();
			//˫���Բ�ֵ
			for (int k = 0; k < temp.size(); k++) {
				m_pRawBuffer = new uint16_t[newHeight * m_nWidth];
				memset(m_pRawBuffer, 0, newHeight * m_nWidth * sizeof(uint16_t));
				for (int y = 0; y < newHeight; y++) {
					for (int x = 0; x < m_nWidth; x++) {
						// ������ԭʼͼ���е�λ��
						double xOriginal = (x * scaleX);
						double yOriginal = (y * scaleY);

						// �ҵ�������ĸ�����
						int x1 = (int)xOriginal;
						int y1 = (int)yOriginal;
						int x2 = min(x1 + 1, m_nWidth - 1);
						int y2 = min(y1 + 1, m_nHeight - 1);

						// ִ��˫���Բ�ֵ
						uint16_t q11 = temp[k][y1 * m_nWidth + x1];
						uint16_t q12 = temp[k][y2 * m_nWidth + x1];
						uint16_t q21 = temp[k][y1 * m_nWidth + x2];
						uint16_t q22 = temp[k][y2 * m_nWidth + x2];

						uint16_t r1 = (q21 - q11) * (xOriginal - x1) + q11;
						uint16_t r2 = (q22 - q12) * (xOriginal - x1) + q12;
						uint16_t val = (r2 - r1) * (yOriginal - y1) + r1;

						m_pRawBuffer[y * m_nWidth + x] = val;
					}
				}
				MedianArrary.push_back(m_pRawBuffer);
			}
			temp.clear();
			DispManyRawFiles(MedianArrary[frame], m_nWidth, newHeight, 8);
		}
	}
}

void CDib::RGB2HSI(int type)
{
	// TODO: �ڴ˴����ʵ�ִ���.
	//RGBתHSI
	//type=0:RGB to HSI
	//type=1:H�ռ�
	//type=2:S�ռ�
	//type=3:I�ռ�
	//type=4:HSI to RGB
	double theta = 0.0;
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			//RGB��һ��
			if (type != 4) {
				double dR = m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 2] / 255.0;
				double dG = m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 1] / 255.0;
				double dB = m_pRGBBuffer[i * m_nWidthBytes + j * 3] / 255.0;

				double H = 0.0;
				double S = 0.0;
				double I = 0.0;
				double max = max(max(dR, dG), dB);
				double min = min(min(dR, dG), dB);

				theta = acos(0.5 * ((dR - dG) + (dR - dB)) / (sqrt((dR - dG) * (dR - dG) + (dR - dB) * (dG - dB))+0.000001));
				if (dB <= dG) {
					H = theta;
				}
				else if (dB > dG)
				{
					H = 2 * Pi - theta;
				}
				H = H / (2 * Pi);

				if (dR == dG && dG == dB) {
					S = 0;
					H = 0;
				}
				else {
					S = 1 - ((3.0 * min) / (dR + dG + dB));
				}

				I = (dR + dG + dB) / 3.0;

				if (type == 0)
				{
					//RGB2HSI
					m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 2] = H * 255.0;
					m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 1] = S * 255.0;
					m_pRGBBuffer[i * m_nWidthBytes + j * 3] = I * 255.0;

					*(m_pDibBits + i * m_nWidthBytes + j * 3 + 2) = H * 255.0;
					*(m_pDibBits + i * m_nWidthBytes + j * 3 + 1) = S * 255.0;
					*(m_pDibBits + i * m_nWidthBytes + j * 3) = I * 255.0;
				}
				else if (type == 1)
				{
					//Hue
					m_pDibBits[i * m_nWidthBytes + j * 3] = H * 255.0;
					m_pDibBits[i * m_nWidthBytes + j * 3 + 1] = H * 255.0;
					m_pDibBits[i * m_nWidthBytes + j * 3 + 2] = H * 255.0;
				}
				else if (type == 2)
				{
					//Saturation
					m_pDibBits[i * m_nWidthBytes + j * 3] = S * 255.0;
					m_pDibBits[i * m_nWidthBytes + j * 3 + 1] = S * 255.0;
					m_pDibBits[i * m_nWidthBytes + j * 3 + 2] = S * 255.0;
				}
				else if (type == 3)
				{
					//Intensity
					m_pDibBits[i * m_nWidthBytes + j * 3] = I * 255.0;
					m_pDibBits[i * m_nWidthBytes + j * 3 + 1] = I * 255.0;
					m_pDibBits[i * m_nWidthBytes + j * 3 + 2] = I * 255.0;
				}
			}
			else if (type == 4) {
				//HSI2RGB
				double R = 1.0;
				double G = 1.0;
				double B = 1.0;
				double H = (m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 2] / 255.0) * 360;
				double dH = (m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 2] / 255.0) * 2 * Pi;//�ȹ�һ����Ȼ��ת����
				double dS = m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 1] / 255.0;
				double dI = m_pRGBBuffer[i * m_nWidthBytes + j * 3] / 255.0;

				//RG����
				if (H >= 0 && H < 120)
				{
					B = dI * (1 - dS);
					R = dI * (1 + ((dS * cos(dH)) / cos(Pi / 3.0 - dH)));
					G = 3 * dI - (R + B);
				}
				//GB����
				else if (H >= 120 && H < 240)
				{
					dH = dH - 2.0 * Pi / 3.0;
					R = dI * (1 - dS);
					G = dI * (1 + (dS * cos(dH) / cos(Pi / 3.0 - dH)));
					B = 3 * dI - (R + G);
				}
				//BR����
				else if (H >= 240 && H <= 360)
				{
					dH = dH - 4.0 * Pi / 3.0;
					G = dI * (1 - dS);
					B = dI * (1 + (dS * cos(dH) / cos(Pi / 3.0 - dH)));
					R = 3 * dI - (G + B);
				}

				m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 2] = R * 255.0;
				m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 1] = G * 255.0;
				m_pRGBBuffer[i * m_nWidthBytes + j * 3] = B * 255.0;

				m_pDibBits[i * m_nWidthBytes + j * 3 + 2] = R * 255.0;
				m_pDibBits[i * m_nWidthBytes + j * 3 + 1] = G * 255.0;
				m_pDibBits[i * m_nWidthBytes + j * 3] = B * 255.0;

			}
		}
	}
}


void CDib::RGBTrans()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	//ʹ���ݱ任����������
	//RGBתHSI
	vector<double> temp(m_pRGBBuffer);
	vector<double> tempI;
	int k = 0;
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			double theta = 0.0;
			double dR = temp[i * m_nWidthBytes + j * 3 + 2] / 255.0;
			double dG = temp[i * m_nWidthBytes + j * 3 + 1] / 255.0;
			double dB = temp[i * m_nWidthBytes + j * 3] / 255.0;

			double H = 0.0;
			double S = 0.0;
			double I = 0.0;
			double max = max(max(dR, dG), dB);
			double min = min(min(dR, dG), dB);

			theta = acos(0.5 * ((dR - dG) + (dR - dB)) / (sqrt((dR - dG) * (dR - dG) + (dR - dB) * (dG - dB)) + 0.000001));
			if (dB <= dG) {
				H = theta;
			}
			else if (dB > dG)
			{
				H = 2 * Pi - theta;
			}
			H = H / (2 * Pi);
			S = 1 - ((3.0 * min) / (dR + dG + dB));

			I = (dR + dG + dB) / 3.0;
			I = pow(I, 1.3);

			m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 2] = H;
			m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 1] = S;
			m_pRGBBuffer[i * m_nWidthBytes + j * 3] = I;
			
			tempI.push_back(I);
			k++;
		}
	}
	double max= *max_element(tempI.begin(), tempI.end());
	double min= *min_element(tempI.begin(), tempI.end());
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			m_pRGBBuffer[i * m_nWidthBytes + j * 3] = (m_pRGBBuffer[i * m_nWidthBytes + j * 3] - min) / max;
		}
	}
	temp.clear();
	tempI.clear();
	//HSIתRGB
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			double R = 1.0;
			double G = 1.0;
			double B = 1.0;
			double H = (m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 2]) * 360;
			double dH = (m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 2]) * 2 * Pi;//�ȹ�һ����Ȼ��ת����
			double dS = m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 1];
			double dI = m_pRGBBuffer[i * m_nWidthBytes + j * 3];

			//RG����
			if (H >= 0 && H < 120)
			{
				B = dI * (1 - dS);
				R = dI * (1 + ((dS * cos(dH)) / cos(Pi / 3.0 - dH)));
				G = 3 * dI - (R + B);
			}
			//GB����
			else if (H >= 120 && H < 240)
			{
				dH = dH - 2.0 * Pi / 3.0;
				R = dI * (1 - dS);
				G = dI * (1 + (dS * cos(dH) / cos(Pi / 3.0 - dH)));
				B = 3 * dI - (R + G);
			}
			//BR����
			else if (H >= 240 && H <= 360)
			{
				dH = dH - 4.0 * Pi / 3.0;
				G = dI * (1 - dS);
				B = dI * (1 + (dS * cos(dH) / cos(Pi / 3.0 - dH)));
				R = 3 * dI - (G + B);
			}

			m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 2] = R * 255.0;
			m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 1] = G * 255.0;
			m_pRGBBuffer[i * m_nWidthBytes + j * 3] = B * 255.0;

			m_pDibBits[i * m_nWidthBytes + j * 3 + 2] = R * 255.0;
			m_pDibBits[i * m_nWidthBytes + j * 3 + 1] = G * 255.0;
			m_pDibBits[i * m_nWidthBytes + j * 3] = B * 255.0;
		}
	}
}


void CDib::Binary()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	//����ɫ��Ҷ�ͼ��תΪ��ֵ��ͼ��
	if (m_nBitCount == 8)
	{
		for (int i = 0; i < m_nHeight; i++)
		{
			for (int j = 0; j < m_nWidth; j++)
			{
				if (*(m_pDibBits + i * m_nWidthBytes + j) > 218)
				{
					*(m_pDibBits + i * m_nWidthBytes + j) = 255;
				}
				else
				{
					*(m_pDibBits + i * m_nWidthBytes + j) = 0;
				}
			}
		}
	}
	else if (m_nBitCount == 24)
	{
		for (int i = 0; i < m_nHeight; i++)
		{
			for (int j = 0; j < m_nWidth; j++)
			{
				if ((m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 2]+ m_pDibBits[i * m_nWidthBytes + j * 3 + 1]+ m_pDibBits[i * m_nWidthBytes + j * 3])/3 > 218)
				{
					m_pDibBits[i * m_nWidthBytes + j * 3 + 2] = 255;
					m_pDibBits[i * m_nWidthBytes + j * 3 + 1] = 255;
					m_pDibBits[i * m_nWidthBytes + j * 3 + 0] = 255;
				}
				else
				{
					m_pDibBits[i * m_nWidthBytes + j * 3 + 2] = 0;
					m_pDibBits[i * m_nWidthBytes + j * 3 + 1] = 0;
					m_pDibBits[i * m_nWidthBytes + j * 3 + 0] = 0;
				}
			}
		}
	}
}


void CDib::Erosion()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	//��ʴ
	int nWidth = m_nWidth;
	int nHeight = m_nHeight;
	int nWidthBytes = m_nWidthBytes;

	vector<unsigned char> temp;

	for (int i = 0; i < nHeight * nWidthBytes; i++) {
		temp.push_back(m_pDibBits[i]);
	}
	int n = 7;
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			int count = 0;
			for (int k = -(n - 1) / 2; k <= (n - 1) / 2; k++)
			{
				for (int l = -(n - 1) / 2; l <= (n - 1) / 2; l++)
				{
					if (i + k >= 0 && i + k < nHeight && j + l >= 0 && j + l < nWidth) {
						if (temp[(i + k) * nWidthBytes + (j + l) * 3 + 0] == 255) {
							count++;
						}
					}
					else if (i + k < 0)
					{
						if (temp[(abs(k)) * nWidthBytes + (j + l) * 3 + 0] == 255) {
							count++;
						}
					}
					else if (i + k >= nHeight)
					{
						if (temp[(nHeight - 1 - abs(k)) * nWidthBytes + (j + l) * 3 + 0] == 255) {
							count++;
						}
					}
					else if (j + l < 0)
					{
						if (temp[(i + k) * nWidthBytes + ( abs(l)) * 3 + 0] == 255) {
							count++;
						}
					}
					else if (j + l >= nWidth)
					{
						if (temp[(i + k) * nWidthBytes + (nWidth - 1 - abs(l)) * 3 + 0] == 255) {
							count++;
						}
					}
					
				}
			}
			if (count == n*n) {
				m_pDibBits[i * nWidthBytes + j * 3 + 0] = 255;
				m_pDibBits[i * nWidthBytes + j * 3 + 1] = 255;
				m_pDibBits[i * nWidthBytes + j * 3 + 2] = 255;
			}
			else {
				m_pDibBits[i * nWidthBytes + j * 3 + 0] = 0;
				m_pDibBits[i * nWidthBytes + j * 3 + 1] = 0;
				m_pDibBits[i * nWidthBytes + j * 3 + 2] = 0;
			}
		}
	}
}


void CDib::Dilate()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	//����
	int nWidth = m_nWidth;
	int nHeight = m_nHeight;
	int nWidthBytes = m_nWidthBytes;
	unsigned char* temp = new unsigned char[nHeight * nWidthBytes];
	memset(temp, 0, nHeight * nWidthBytes);
	for (int i = 0; i < nHeight * nWidthBytes; i++) {
		temp[i] = m_pDibBits[i];
	}
	int n = 7;
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			int count = 0;
			for (int k = -(n - 1) / 2; k <= (n - 1) / 2; k++)
			{
				for (int l = -(n - 1) / 2; l <= (n - 1) / 2; l++)
				{
					if (i + k >= 0 && i + k < nHeight && j + l >= 0 && j + l < nWidth) {
						if (temp[(i + k) * nWidthBytes + (j + l) * 3 + 0] == 255) {
							count++;
						}
					}
					else if (i + k < 0)
					{
						if (temp[(abs(k)) * nWidthBytes + (j + l) * 3 + 0] == 255) {
							count++;
						}
					}
					else if (i + k >= nHeight)
					{
						if (temp[(nHeight - 1 - abs(k)) * nWidthBytes + (j + l) * 3 + 0] == 255) {
							count++;
						}
					}
					else if (j + l < 0)
					{
						if (temp[(i + k) * nWidthBytes + (abs(l)) * 3 + 0] == 255) {
							count++;
						}
					}
					else if (j + l >= nWidth)
					{
						if (temp[(i + k) * nWidthBytes + (nWidth - 1 - abs(l)) * 3 + 0] == 255) {
							count++;
						}
					}

				}
			}
			if (count >0) {
				m_pDibBits[i * nWidthBytes + j * 3 + 0] = 255;
				m_pDibBits[i * nWidthBytes + j * 3 + 1] = 255;
				m_pDibBits[i * nWidthBytes + j * 3 + 2] = 255;
			}
			else {
				m_pDibBits[i * nWidthBytes + j * 3 + 0] = 0;
				m_pDibBits[i * nWidthBytes + j * 3 + 1] = 0;
				m_pDibBits[i * nWidthBytes + j * 3 + 2] = 0;
			}
		}
	}
	delete[] temp;
}


void CDib::FillHole()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	//���׶�
	int nWidth = m_nWidth;
	int nHeight = m_nHeight;
	vector<unsigned char> I;
	for (int i = 0; i < nHeight; i++) {
		for (int j = 0; j < nWidth; j++) {
			I.push_back(m_pDibBits[i * m_nWidthBytes + j * 3]);
		}
	}
	//����Ic
	vector<unsigned char> Ic;
	for (int i = 0; i < nHeight; i++) {
		for (int j = 0; j < nWidth; j++) {
			if (I[i * nWidth + j] == 0) {
				Ic.push_back(255);
			}
			else {
				Ic.push_back(0);
			}
		}
	}
	//����F
	vector<unsigned char> F;
	for (int i = 0; i < nHeight; i++) {
		for (int j = 0; j < nWidth; j++) {
			if (i == 0 || i == nHeight - 1 || j == 0 || j == nWidth - 1)
			{
				F.push_back(Ic[i * nWidth + j]);
			}
			else
			{
				F.push_back(0);
			}
		}
	}
	//��ʼ�������
	int n = 9;
	for (int Max = 0; Max < 50; Max++) {
		for (int i = 0; i < nHeight; i++) {
			for (int j = 0; j < nWidth; j++) {
				for (int k = -(n - 1) / 2; k <= (n - 1) / 2; k++)
				{
					for (int l = -(n - 1) / 2; l <= (n - 1) / 2; l++)
					{
						if (i + k >= 0 && i + k < nHeight && j + l >= 0 && j + l < nWidth) {
							if (F[(i + k) * nWidth + j + l] == 255) {
								m_pDibBits[i * m_nWidthBytes + j * 3] = 255;
							}
						}
						else if (i + k < 0) {
							if (F[(abs(k)) * nWidth + j + l] == 255)
							{
								m_pDibBits[i * m_nWidthBytes + j * 3] = 255;
							}
						}
						else if (i + k >= nHeight) {
							if (F[(nHeight - 1 - abs(k)) * nWidth + j + l] == 255)
							{
								m_pDibBits[i * m_nWidthBytes + j * 3] = 255;
							}
						}
						else if (j + l < 0) {
							if (F[(i + k) * nWidth + abs(l)] == 255)
							{
								m_pDibBits[i * m_nWidthBytes + j * 3] = 255;
							}
						}
						else if (j + l >= nWidth) {
							if (F[(i + k) * nWidth + nWidth - 1 - abs(l)] == 255)
							{
								m_pDibBits[i * m_nWidthBytes + j * 3] = 255;
							}
						}
					}
				}
			}
		}
		bool flag = false;
		for (int i = 0; i < nHeight; i++) {
			for (int j = 0; j < nWidth; j++) {
				if (F[i * nWidth + j] != m_pDibBits[i * m_nWidthBytes + j * 3])
				{
					flag = true;
					break;
				}
			}
		}
		for (int i = 0; i < nHeight; i++) {
			for (int j = 0; j < nWidth; j++) {
				F[i * nWidth + j] = m_pDibBits[i * m_nWidthBytes + j * 3] & Ic[i * nWidth + j];
			}
		}
		if (!flag) {
			break;
		}
	}
	//ȡ��
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			if (F[i * nWidth + j] == 255)
			{
				F[i * nWidth + j] = 0;
			}
			else
			{
				F[i * nWidth + j] = 255;
			}
		}
	}
	
	//��Fд��Dib
	for (int i = 0; i < nHeight; i++) {
		for (int j = 0; j < nWidth; j++) {
			m_pDibBits[i * m_nWidthBytes + j * 3] = F[i * nWidth + j];
			m_pDibBits[i * m_nWidthBytes + j * 3 + 1] = F[i * nWidth + j];
			m_pDibBits[i * m_nWidthBytes + j * 3 + 2] = F[i * nWidth + j];
		}
	}
}


void CDib::EdgeExtraction()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	//��Ե��ȡ
	int nWidth = m_nWidth;
	int nHeight = m_nHeight;
	vector<unsigned char> I;
	for (int i = 0; i < nHeight; i++) {
		for (int j = 0; j < nWidth; j++) {
			I.push_back(m_pDibBits[i * m_nWidthBytes + j * 3]);
		}
	}
	//����
	vector<unsigned char> Ic;
	for (int i = 0; i < nHeight; i++) {
		for (int j = 0; j < nWidth; j++) {
			if (I[i * nWidth + j] == 0) {
				Ic.push_back(255);
			}
			else {
				Ic.push_back(0);
			}
		}
	}
	//��ʴ
	vector<unsigned char> temp;
	int nWidthBytes = m_nWidthBytes;
	for (int i = 0; i < nHeight * nWidthBytes; i++) {
		temp.push_back(m_pDibBits[i]);
	}
	int n = 3;
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			int count = 0;
			for (int k = -(n - 1) / 2; k <= (n - 1) / 2; k++)
			{
				for (int l = -(n - 1) / 2; l <= (n - 1) / 2; l++)
				{
					if (i + k >= 0 && i + k < nHeight && j + l >= 0 && j + l < nWidth) {
						if (temp[(i + k) * nWidthBytes + (j + l) * 3 + 0] == 255) {
							count++;
						}
					}
					else if (i + k < 0)
					{
						if (temp[(abs(k)) * nWidthBytes + (j + l) * 3 + 0] == 255) {
							count++;
						}
					}
					else if (i + k >= nHeight)
					{
						if (temp[(nHeight - 1 - abs(k)) * nWidthBytes + (j + l) * 3 + 0] == 255) {
							count++;
						}
					}
					else if (j + l < 0)
					{
						if (temp[(i + k) * nWidthBytes + (abs(l)) * 3 + 0] == 255) {
							count++;
						}
					}
					else if (j + l >= nWidth)
					{
						if (temp[(i + k) * nWidthBytes + (nWidth - 1 - abs(l)) * 3 + 0] == 255) {
							count++;
						}
					}

				}
			}
			if (count == n * n) {
				m_pDibBits[i * nWidthBytes + j * 3 + 0] = 255;
				m_pDibBits[i * nWidthBytes + j * 3 + 1] = 255;
				m_pDibBits[i * nWidthBytes + j * 3 + 2] = 255;
			}
			else {
				m_pDibBits[i * nWidthBytes + j * 3 + 0] = 0;
				m_pDibBits[i * nWidthBytes + j * 3 + 1] = 0;
				m_pDibBits[i * nWidthBytes + j * 3 + 2] = 0;
			}
		}
	}
	
	//���
	for (int i = 0; i < nHeight; i++) {
		for (int j = 0; j < nWidth; j++) {
			I[i * nWidth + j] = I[i * nWidth + j] - m_pDibBits[i * m_nWidthBytes + j * 3];
		}
	}
	//��ֵ
	for (int i = 0; i < nHeight; i++) {
		for (int j = 0; j < nWidth; j++) {
			m_pDibBits[i * m_nWidthBytes + j * 3] = I[i * nWidth + j];
			m_pDibBits[i * m_nWidthBytes + j * 3 + 1] = I[i * nWidth + j];
			m_pDibBits[i * m_nWidthBytes + j * 3 + 2] = I[i * nWidth + j];
		}
	}
		
}


void CDib::AddEdge()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	//��Ե��ǿ
	int nWidth = m_nWidth;
	int nHeight = m_nHeight;
	int nWidthBytes = m_nWidthBytes;
	for (int i = 0; i < nHeight; i++) {
		for (int j = 0; j < nWidth; j++) {
			if (m_pDibBits[i * m_nWidthBytes + j * 3] == 0) {
				m_pDibBits[i * m_nWidthBytes + j * 3] = m_pRGBBuffer[i * m_nWidthBytes + j * 3];
				m_pDibBits[i * m_nWidthBytes + j * 3 + 1] = m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 1];
				m_pDibBits[i * m_nWidthBytes + j * 3 + 2] = m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 2];

			}
			else {
				m_pDibBits[i * m_nWidthBytes + j * 3] = 255;
				m_pDibBits[i * m_nWidthBytes + j * 3 + 1] = 0;
				m_pDibBits[i * m_nWidthBytes + j * 3 + 2] = 0;

			}
			
		}
	}
}


void CDib::CannyEdge(int step)
{
	// TODO: �ڴ˴����ʵ�ִ���.
	//step==1: �����ݶȵĴ�С�ͷ���
	//step==2: �Ǽ���ֵ����
	//step==3: ˫��ֵ���
	//step==4: ��Եϸ��

	if (step == 1) {

		m_Magnitude = new double[m_nHeight * m_nWidth];
		m_Angle = new double[m_nHeight * m_nWidth];
		memset(m_Magnitude, 0, m_nHeight * m_nWidth * sizeof(double));
		memset(m_Angle, 0, m_nHeight * m_nWidth * sizeof(double));
		//��Sobel����Ӧ����ͼ�񣬴洢��С�ͷ���
		int SobelKernelX[3][3] = { {-1, 0, 1},
									{-2, 0, 2},
									{-1, 0, 1} };
		int SobelKernelY[3][3] = { {-1, -2, -1},
									{0, 0, 0},
									{1, 2, 1} };
		double temp1 = 0;
		double temp2 = 0;
		double gradientModule = 0;
		double angle = 0;
		unsigned char* pic = new unsigned char[(m_nHeight + 2) * (m_nWidthBytes + 2)];	//����һ���µ�ͼ����������

		memset(pic, 0, (m_nHeight + 2) * (m_nWidthBytes + 2) * sizeof(unsigned char));
		for (int i = 1; i < m_nHeight + 1; i++) //������0ͼ��
		{
			for (int j = 1; j < m_nWidthBytes + 1; j++)
			{
				*(pic + i * m_nWidthBytes + j) = *(m_pDibBits + (i - 1) * m_nWidthBytes + j - 1);
			}
		}
		for (int i = 1; i < m_nHeight + 1; i++)
		{
			for (int j = 1; j < m_nWidthBytes + 1; j++)
			{
				temp1 = 0;
				temp2 = 0;
				for (int k = -1; k < 2; k++)
				{
					for (int p = -1; p < 2; p++)
					{
						temp1 += *(pic + (i + k) * m_nWidthBytes + j + p) * SobelKernelX[k + 1][p + 1];
						temp2 += *(pic + (i + k) * m_nWidthBytes + j + p) * SobelKernelY[k + 1][p + 1];
					}
				}
				gradientModule = sqrt(temp1 * temp1 + temp2 * temp2);
				angle = atan2(temp2, temp1);
				m_Magnitude[(i - 1) * m_nWidth + j - 1] = gradientModule;
				m_Angle[(i - 1) * m_nWidth + j - 1] = angle;
				if (gradientModule > 255)
					gradientModule = 255;
				*(m_pDibBits + (i - 1) * m_nWidthBytes + j - 1) = gradientModule;
			}
		}
		delete[] pic;
	}

	if (step == 2) {
		//�Ǽ���ֵ����
		for (int i = 1; i < m_nHeight - 1; i++)
		{
			for (int j = 1; j < m_nWidth - 1; j++)
			{
				double angle = m_Angle[i * m_nWidth + j];
				double ang = angle * (180.0 / Pi);
				double gradientModule = m_Magnitude[i * m_nWidth + j];
				double temp1 = 0;
				double temp2 = 0;

				if ((ang >= -22.5 && ang < 22.5) || (ang >= 157.5 && ang <= 180) || (ang >= -180 && ang <= -157.5)) {
					temp1 = m_Magnitude[(i + 1) * m_nWidth + j];
					temp2 = m_Magnitude[(i - 1) * m_nWidth + j];
				}
				else if ((ang >= 22.5 && ang < 67.5) || (ang >= -157.5 && ang < -112.5)) {
					temp1 = m_Magnitude[(i + 1) * m_nWidth + j + 1];
					temp2 = m_Magnitude[(i - 1) * m_nWidth + j - 1];
				}
				else if ((ang >= 67.5 && ang < 112.5) || (ang >= -112.5 && ang < -67.5)) {
					temp1 = m_Magnitude[i * m_nWidth + j + 1];
					temp2 = m_Magnitude[i * m_nWidth + j - 1];
				}
				else if ((ang >= 112.5 && ang < 157.5) || (ang >= -67.5 && ang < -22.5)) {
					temp1 = m_Magnitude[(i - 1) * m_nWidth + j + 1];
					temp2 = m_Magnitude[(i + 1) * m_nWidth + j - 1];
				}
				if (gradientModule < temp1 || gradientModule < temp2)
				{
					m_Magnitude[i * m_nWidth + j] = 0;
					*(m_pDibBits + i * m_nWidthBytes + j) = 0;
				}
			}
		}


	}

	if (step == 3) {
		//˫��ֵ���
		double max = *max_element(m_Magnitude, m_Magnitude + m_nHeight * m_nWidth);
		double min = *min_element(m_Magnitude, m_Magnitude + m_nHeight * m_nWidth);
		double T1 = 0.075 * max;
		double T2 = 0.085 * max;
		for (int i = 0; i < m_nHeight; i++)
		{
			for (int j = 0; j < m_nWidth; j++)
			{
				if (m_Magnitude[i * m_nWidth + j] > T2)
				{
					m_Magnitude[i * m_nWidth + j] = 255;
				}
				else if (m_Magnitude[i * m_nWidth + j] < T1)
				{
					m_Magnitude[i * m_nWidth + j] = 0;
				}
				else
				{
					m_Magnitude[i * m_nWidth + j] = 128;
				}
				*(m_pDibBits + i * m_nWidthBytes + j) = m_Magnitude[i * m_nWidth + j];
			}
		}
	}

	if (step == 4) {
		//��Եϸ��
		//����һ����ͼ��

		for (int i = 1; i < m_nHeight - 1; i++)
		{
			for (int j = 1; j < m_nWidth - 1; j++)
			{
				if (m_Magnitude[i * m_nWidth + j] == 128)
				{
					if (m_Magnitude[(i - 1) * m_nWidth + j] == 255 || m_Magnitude[(i + 1) * m_nWidth + j] == 255 
						|| m_Magnitude[i * m_nWidth + j - 1] == 255 || m_Magnitude[i * m_nWidth + j + 1] == 255 
						|| m_Magnitude[(i - 1) * m_nWidth + j - 1] == 255 || m_Magnitude[(i - 1) * m_nWidth + j + 1] == 255 
						|| m_Magnitude[(i + 1) * m_nWidth + j - 1] == 255 || m_Magnitude[(i + 1) * m_nWidth + j + 1] == 255)
					{
						*(m_pDibBits + i * m_nWidthBytes + j) = 255;
					}
					else
					{
						*(m_pDibBits + i * m_nWidthBytes + j) = 0;
					}
					//*(m_pDibBits + i * m_nWidthBytes + j) = m_Magnitude[i * m_nWidth + j];
				}
			}
		}
	}
}
