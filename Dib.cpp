#include "pch.h"
#include "Dib.h"
#include<iostream>
#include<math.h>
#include<filesystem>
#include"DlgRawChoose.h"
#include "DlgRawChoose.h"
#include "resource.h"
#include<vector>
#include<fstream>

using namespace std;

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
}

CDib::~CDib(void)
{
	m_pDibBits = NULL;
	m_bufferSpace = NULL;
	m_bufferFreq = NULL;
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
}

void CDib::LoadFile( LPCTSTR lpszPathName )
{
		Load(lpszPathName);
		m_nWidth = GetWidth();
		m_nHeight = GetHeight();
		m_nWidthBytes = abs(GetPitch());
		m_nBitCount = GetBPP();
		m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1) * GetPitch();
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
