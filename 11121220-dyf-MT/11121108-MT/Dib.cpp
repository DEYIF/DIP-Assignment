#include "pch.h"
#include "Dib.h"
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

using namespace std;

CDib::CDib(void):m_pDibBits(NULL),m_pGrayValueCount(NULL),m_pRawBuffer(NULL)
{
	// initialized variables
	m_nBitCount = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nWidthBytes = 0;
	m_nImgNum = 0;
	m_nCurrentImgNum_Sagittal = 0;
	m_nCurrentImgNum_Transverse = 0;
	m_nCurrentImgNum_Coronal = 0;
	m_current_img_pos = 0;
}

CDib::CDib( CDib &Dib ):m_pDibBits(NULL),m_pGrayValueCount(NULL), m_pRawBuffer(NULL)
{
	// initialized variables
	m_nBitCount = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nWidthBytes = 0;
	m_nImgNum = 0;
	m_nCurrentImgNum_Sagittal = 0;
	m_nCurrentImgNum_Transverse = 0;
	m_nCurrentImgNum_Coronal = 0;
	m_current_img_pos = 0;

	if(&Dib == NULL)
	{
		return;
	}
	if(!IsNull())
	{
		Destroy();
	}
	Create(Dib.GetWidth(),Dib.GetHeight(),Dib.GetBPP(),0);
	m_nWidth = Dib.m_nWidth;
	m_nHeight = Dib.m_nHeight;
	if(IsIndexed())
	{
		int nColors=Dib.GetMaxColorTableEntries();	
		if(nColors > 0)
		{
			RGBQUAD* pal{};
			pal = new RGBQUAD[nColors];
			Dib.GetColorTable(0,nColors,pal);	//��ɫ��
			SetColorTable(0,nColors,pal);
			delete[] pal;
		} 
	}
	m_nWidthBytes =abs(GetPitch()) ;
	m_nBitCount = GetBPP();
	m_nImgNum = Dib.GetImgNum();
	m_pDibBits = (unsigned char*)GetBits()+(m_nHeight-1)*GetPitch();
	memcpy(m_pDibBits,Dib.m_pDibBits,m_nHeight*m_nWidthBytes);
	m_pRawBuffer = new uint16_t[m_nWidth * m_nHeight * m_nImgNum];
	if (Dib.m_pRawBuffer != NULL)
	{
		memcpy(m_pRawBuffer,Dib.m_pRawBuffer,m_nWidth*m_nHeight* m_nImgNum *sizeof(uint16_t));
	}
}

CDib::~CDib(void)
{
	m_pDibBits = NULL;
	if (m_pGrayValueCount != NULL)
	{
		delete []m_pGrayValueCount;
		m_pGrayValueCount = NULL;
	}
	if (m_pRawBuffer != NULL)
	{
		delete[]m_pRawBuffer;
		m_pRawBuffer = NULL;
	}
}

int CDib::getWidth()
{
	return m_nWidth;
}

int CDib::getHeight()
{
	return m_nHeight;
}

int CDib::GetImgNum()
{
	return m_nImgNum;
}

uint16_t* CDib::GetRawBuffer()
{
	return m_pRawBuffer;
}

void CDib::CreateDisplayDib(uint16_t* RawBuffer, int nWidth, int nHeight, int nBitCount)
{
	//Create new Dib
	if (!IsNull())
	{
		Destroy();
	}
	Create(nWidth, nHeight, 8, 0);
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

void CDib::LoadFile(LPCTSTR lpszPathName, int ImgNum, int ImgHeight, int ImgWidth)
{
	//����ļ���

	CString strPathName(lpszPathName);
	int nDotIndex = strPathName.ReverseFind(_T('.'));
	if (nDotIndex == -1)
	{
		//�ļ���û���ҵ�"."
		AfxMessageBox(_T("��֧�ֵ��ļ���ʽ"));
		return;
	}

	CString strExtension = strPathName.Mid(nDotIndex + 1);//ȡ���ļ���"."�Ժ������
	strExtension.MakeLower();//����չ��ת��ΪСд

	// �ж���չ���Ƿ�Ϊ"raw"
	if (strExtension == _T("raw"))
	{
		//ȡ��ͼ���ļ��Ŀ�ȡ��߶Ⱥ�ͼ����
		m_nImgNum = ImgNum;
		m_nWidth = ImgHeight;
		m_nHeight = ImgWidth;
		m_nRawNum = ImgNum;
		m_nRawWidth= ImgWidth;
		m_nRawHeight = ImgHeight;
		//�����ݴ��ļ�����ֵ�Ļ�����//16λ������raw
		if (m_pRawBuffer != NULL)
		{
			delete[] m_pRawBuffer;
			m_pRawBuffer = NULL;
		}

		m_pRawBuffer = new uint16_t[m_nWidth * m_nHeight * m_nImgNum];
		memset(m_pRawBuffer, 0, m_nWidth * m_nHeight * m_nImgNum * sizeof(uint16_t));
		//ʹ��fistream��ȡraw�ļ�������ֵ��buffer
		std::ifstream file(lpszPathName, std::ios::binary);
		file.read(reinterpret_cast<char*>(m_pRawBuffer), m_nWidth * m_nHeight * m_nImgNum * sizeof(uint16_t));
		//�ļ���ʧ�ܵ������棬Ȼ���˳�
		if (file.fail()) {
			AfxMessageBox(_T("��ȡ�ļ�ʧ��"));
			return;
		}
		file.close();
		//��Ϊ��ȡ���������ظ�8λ�͵�8λ˳���Ƿ��ģ���������ظߵ�8λ���жԵ���ͬʱ��ȡ���ֵ
		for (int i = 0; i < m_nWidth * m_nHeight * m_nImgNum; i++) {
			m_pRawBuffer[i] = ((m_pRawBuffer[i] & 0xff00) >> 8) | ((m_pRawBuffer[i] & 0x00ff) << 8);
		}
		CreateDisplayDib(m_pRawBuffer, m_nWidth, m_nHeight, 8);
	}
	else
	{
		//��Ϊraw��������ȡ
		Load(lpszPathName);
		m_nWidth = GetWidth();
		m_nHeight = GetHeight();
		m_nWidthBytes = abs(GetPitch());
		m_nBitCount = GetBPP();
		m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1) * GetPitch();
		m_nImgNum = 1;
	}
}

void CDib::UpdateDisplayDib(int imgnum)
{
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			*(m_pDibBits + i * m_nWidthBytes + j) = (BYTE)(m_pRawBuffer[imgnum * m_nWidth * m_nHeight + i * m_nWidth + j] >> 4);
		}
	}
}

long* CDib::GrayValueCount()	//��ȡͼ���а����Ĳ�ͬ�Ҷȸ��Ե�����
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
			unchValue = (*lpSrc)*pow(-1.0,x+y);
			pCTData[y*nTransWidth + x]=complex<double>(unchValue,0);
		}
	}
	FFT_2D(pCTData, nWidth, nHeight, pCFData) ;				// ����Ҷ���任

	///////////////////////////////////////////////////////////////////////////
	// code needed to insert here
	//
	//
	//
	//
	///////////////////////////////////////////////////////////////////////////

	IFFT_2D(pCFData, pCTData,nHeight, nWidth); 				// ����Ҷ���任

	for(y=0; y<nHeight; y++)								// ���任�����ݴ���lpDIBBits
	{
		for(x=0; x<nWidth; x++)
		{
			//��Ҫ�����źŵ����������Լ�ʵ�����õ�ͼ�������ǻҶ�ֵ����ԭʼ����
			dReal = pCTData[y*nTransWidth + x].real() ;		// ʵ��
			dImag = pCTData[y*nTransWidth + x].imag() ;		// �鲿
			unchValue = dReal*pow(-1.0,x+y);
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
			*lpSrc =(BYTE)unchValue ;
		}
	}	
	
	delete[] pCTData;										// �ͷ��ڴ�
	delete[] pCFData;										// �ͷ��ڴ�
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
	delete pCW;
	delete[] pCWork1;
	delete[] pCWork2;
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
	
	//����pCTData���ݸ���
	complex<double> *pCTmp = new complex<double>[nTransWidth * nTransHeight];

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

	//������洢��pCFData��
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCFData[nTransWidth * y + x] = pCTData[nTransWidth * y + x];
		}
	}



	//�ָ�pCTData����
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[nTransWidth * y + x] = pCTmp[nTransWidth * y + x];
		}
	}

	//�ͷ��ڴ�
	delete[] pCTmp;
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
			pCTmp = &pCFData[nTransWidth * y + x];
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
			pCTData[nTransWidth * y + x] = complex<double>( pCTmp->real()/(nTransWidth*nTransHeight), -pCTmp->imag()/(nTransWidth*nTransHeight) );
		}
	}
	// �ͷ��ڴ�
	delete[] pCWork;
	pCWork = NULL;
}

unsigned char* CDib::GetDibBits()
{
	return m_pDibBits;
}

void CDib::BilinearInterpolate(uint16_t * bilinear, uint16_t * normal, int width, int height, float ratio)
{
	// ˫���Բ�ֵ
	// bilinear: ��ֵ�������ֵ
	// normal: ԭʼ����ֵ
	// width: ԭʼͼ����
	// height: ԭʼͼ��߶�
	// ratio: ���ű���
	int newWidth = width;
	int newHeight = static_cast<int>(height * ratio);
	for (int i = 0; i < newHeight; i++)
	{
		for (int j = 0; j < newWidth; j++)
		{
			// ������ԭʼͼ���е�����
			float x = j;
			float y = i / ratio;
			// ������ԭʼͼ���е��ĸ���������
			int x1 = static_cast<int>(x);
			int y1 = static_cast<int>(y);
			int x2 = x1 + 1;
			int y2 = y1 + 1;
			// �����ֵϵ��
			float dx = x - x1;
			float dy = y - y1;
			// ˫���Բ�ֵ
			if (x2 < width && y2 < height)
			{
				bilinear[i * newWidth + j] = static_cast<uint16_t>((1 - dx) * (1 - dy) * normal[y1 * width + x1] +
					dx * (1 - dy) * normal[y1 * width + x2] +
					(1 - dx) * dy * normal[y2 * width + x1] +
					dx * dy * normal[y2 * width + x2]);
			}
		}
	}
}

void CDib::ShowTransversePlane(int CurImgNum)
{
	m_current_img_pos = 0;
	if (m_pRawBuffer == NULL)
	{
		return;
	}
	if (CurImgNum >= m_nImgNum)
	{
		CurImgNum = 0;
	}
	// �������ڴ洢��λ����Ƭ�Ļ�����
	uint16_t* pTemp = new uint16_t[m_nRawHeight * m_nRawWidth];
	for (int y = 0; y < m_nRawHeight; ++y) {
		for (int x = 0; x < m_nRawWidth; ++x) {
			// ������ԭʼ�������е�����
			int index = y * m_nRawWidth + x + CurImgNum * m_nRawHeight * m_nRawWidth;
			// ��ȡ����ֵ
			pTemp[x + y * m_nRawWidth] = m_pRawBuffer[index];
		}
	}
	CreateDisplayDib(pTemp, m_nRawWidth, m_nRawHeight, 8);
	delete[] pTemp;
}

void CDib::ShowCoronalPlane(int CurImgNum)
{
	//transform the coronal plane to the target coronal plane with bilinear interpolation
	m_current_img_pos = 2;
	//������ͼ������б�����������б�����Ϊ1������Ҫ���в�ֵ
	//const float row_col_ratio = 1.0f / 0.68359375f; 
	const float row_col_ratio = 1.0f;

	int newWidth = m_nRawWidth;
	int newHeight = static_cast<int>(m_nImgNum * row_col_ratio);
	if (m_pRawBuffer == NULL)
	{
		return;
	}
	uint16_t* pTemp = new uint16_t[m_nRawWidth * m_nImgNum];
	memset(pTemp, 255, m_nRawWidth * m_nImgNum * sizeof(uint16_t));
	for (int z = 0; z < m_nImgNum; ++z) {
		for (int x = 0; x < m_nRawWidth; ++x) {
			int index = z * m_nRawWidth * m_nRawHeight + x + CurImgNum * m_nRawWidth;
			pTemp[x + z * m_nRawWidth] = m_pRawBuffer[index];
		}
	}
	uint16_t* pbi_Temp = new uint16_t[m_nRawWidth * newHeight];
	memset(pbi_Temp, 255, newWidth * newHeight * sizeof(uint16_t));
	if (row_col_ratio != 1.0f)
	{
		//������б�����Ϊ1������Ҫ���в�ֵ
		BilinearInterpolate(pbi_Temp, pTemp, m_nRawWidth, m_nImgNum, row_col_ratio);
	}
	else {
		//������б���Ϊ1����ֱ�Ӹ���
		memcpy(pbi_Temp, pTemp, m_nRawWidth * m_nImgNum * sizeof(uint16_t));
	}
	CreateDisplayDib(pbi_Temp, newWidth, newHeight, 8);
	delete[] pbi_Temp;
	delete[] pTemp;
}

void CDib::ShowSagittalPlane(int CurImgNum)
{
	m_current_img_pos = 1;
	//������ͼ������б�����������б�����Ϊ1������Ҫ���в�ֵ
	//const float row_col_ratio = 1.0f / 0.68359375f; 
	const float row_col_ratio = 1.0f;

	int newWidth = m_nRawHeight;
	int newHeight = static_cast<int>(m_nImgNum * row_col_ratio);
	if (m_pRawBuffer == NULL)
	{
		return;
	}
	uint16_t* pTemp = new uint16_t[m_nImgNum * m_nRawHeight];
	memset(pTemp, 255, m_nRawHeight * m_nImgNum * sizeof(uint16_t));
	for (int z = 0; z < m_nImgNum; ++z) {
		for (int y = 0; y < m_nRawHeight; ++y) {
			int index = z * m_nWidth * m_nRawHeight + y * m_nWidth + CurImgNum;
			pTemp[y + z * m_nRawHeight] = m_pRawBuffer[index];
		}
	}
	uint16_t* pbi_Temp = new uint16_t[newHeight * newWidth];
	memset(pbi_Temp, 255, newWidth * newHeight * sizeof(uint16_t));
	if (row_col_ratio != 1.0f)
	{
		//������б�����Ϊ1������Ҫ���в�ֵ
		BilinearInterpolate(pbi_Temp, pTemp, m_nRawWidth, m_nImgNum, row_col_ratio);
	}
	else {
		//������б���Ϊ1����ֱ�Ӹ���
		memcpy(pbi_Temp, pTemp, m_nRawWidth * m_nImgNum * sizeof(uint16_t));
	}
	CreateDisplayDib(pbi_Temp, newWidth, newHeight, 8);
	delete[] pbi_Temp;
	delete[] pTemp;
}

void CDib::ShowNextPage()
{
	switch (m_current_img_pos)
	{
		//0�����1ʸ״2��״
	case 0:
		if (++m_nCurrentImgNum_Transverse > m_nImgNum || m_nCurrentImgNum_Transverse < 0)
			m_nCurrentImgNum_Transverse = m_nImgNum;
		ShowTransversePlane(m_nCurrentImgNum_Transverse);
		break;
	case 1:
		if (++m_nCurrentImgNum_Sagittal > m_nWidth || m_nCurrentImgNum_Sagittal < 0)
			m_nCurrentImgNum_Sagittal = m_nWidth;
		ShowSagittalPlane(m_nCurrentImgNum_Sagittal);
		break;
	case 2:
		if (++m_nCurrentImgNum_Coronal >m_nHeight || m_nCurrentImgNum_Coronal < 0)
			m_nCurrentImgNum_Coronal = m_nHeight;
		ShowCoronalPlane(m_nCurrentImgNum_Coronal);
		break;
	default:
		AfxMessageBox(_T("Unknown Mode"));
	}
}

void CDib::ShowFormerPage()
{
	switch (m_current_img_pos)
	{
	case 0:
		if (--m_nCurrentImgNum_Transverse > m_nImgNum || m_nCurrentImgNum_Transverse < 0)
			m_nCurrentImgNum_Transverse = 0;
		ShowTransversePlane(m_nCurrentImgNum_Transverse);
		break;
	case 1:
		if (--m_nCurrentImgNum_Sagittal > m_nWidth || m_nCurrentImgNum_Sagittal < 0)
			m_nCurrentImgNum_Sagittal = 0;
		ShowSagittalPlane(m_nCurrentImgNum_Sagittal);
		break;
	case 2:
		if (--m_nCurrentImgNum_Coronal > m_nHeight || m_nCurrentImgNum_Coronal < 0)
			m_nCurrentImgNum_Coronal = 0;
		ShowCoronalPlane(m_nCurrentImgNum_Coronal);
		break;
	default:
		AfxMessageBox(_T("Unknown Mode"));
	}
}

