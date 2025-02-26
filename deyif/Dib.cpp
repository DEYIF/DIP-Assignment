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
		//拷贝数据
		uchar* pucRow;									//指向数据区的行指针
		uchar* pucImage = (uchar*)GetBits();		//指向数据区的指针
		int nStep = GetPitch();					//每行的字节数,注意这个返回值有正有负
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
			//拷贝数据
			uchar* pucRow;									//指向数据区的行指针
			uchar* pucImage = (uchar*)GetBits();		//指向数据区的指针
			int nStep = GetPitch();					//每行的字节数,注意这个返回值有正有负
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
	unsigned char*	lpSrc;							// 指向源图像的指针
	int y ;										// 循环控制变量
	int x ;										// 循环控制变量
	double dTmpOne ;								//存放临时变量
	double dTmpTwo ;								//存放临时变量
	int nTransWidth ;								// 傅立叶变换的宽度（2的整数次幂）
	int nTransHeight;								// 傅立叶变换的高度（2的整数次幂）
	double unchValue;								// 存贮图像各像素灰度的临时变量
	complex<double> * pCTData ;						// 指向时域数据的指针
	complex<double> * pCFData ;						// 指向频域数据的指针
	// 计算进行傅立叶变换的点数－横向	（2的整数次幂）
	dTmpOne = log(1.0*nWidth)/log(2.0);
	dTmpTwo = ceil(dTmpOne)	;
	dTmpTwo = pow(2,dTmpTwo);
	nTransWidth = (int) dTmpTwo;	
	// 计算进行傅立叶变换的点数－纵向 （2的整数次幂）
	dTmpOne = log(1.0*nHeight)/log(2.0);
	dTmpTwo = ceil(dTmpOne)	;
	dTmpTwo = pow(2,dTmpTwo);
	nTransHeight = (int) dTmpTwo;
	double dReal;									// 傅立叶变换的实部
	double dImag;									// 傅立叶变换的虚部
	
	pCTData=new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	pCFData=new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	// 图像数据的宽和高不一定是2的整数次幂，所以pCTData有一部分数据需要补0
	for(y=0; y<nTransHeight; y++)
	{
		for(x=0; x<nTransWidth; x++)
		{
			pCTData[y*nTransWidth + x]=complex<double>(0,0);		// 补零
		}
	}
	//把图像数据传给pCTData
	for(y=0; y<nHeight; y++)
	{
		for(x=0; x<nWidth; x++)
		{
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			unchValue = (*lpSrc)*pow(-1.0,x+y);	//乘以-1的x+y次方，把频域0点移到中心
			pCTData[y*nTransWidth + x]=complex<double>(unchValue,0);
		}
	}
	m_bufferSpace = pCTData;	//保存原始数据到m_bufferSpace中

	FFT_2D(pCTData, nWidth, nHeight, pCFData) ;				// 傅立叶正变换
	//变完的结果还在pCTData中

	m_bufferFreq = pCTData;	//保存变换结果到m_bufferFFT中
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

	//IFFT_2D(pCFData, pCTData,nHeight, nWidth); 				// 傅立叶反变换

	//for(y=0; y<nHeight; y++)								// 反变换的数据传给lpDIBBits
	//{
	//	for(x=0; x<nWidth; x++)
	//	{
	//		//需要考虑信号的正负问题以及实际所用的图象数据是灰度值还是原始数据
	//		dReal = pCTData[y*nTransWidth + x].real() ;		// 实部
	//		dImag = pCTData[y*nTransWidth + x].imag() ;		// 虚部
	//		unchValue = dReal*pow(-1.0,x+y);
	//		if (unchValue < 0)
	//		{
	//			unchValue = 0;
	//		}
	//		if (unchValue > 0xff)
	//		{
	//			unchValue = 0xff;
	//		}
	//		// 指向DIB第y行，第x个象素的指针
	//		lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
	//		*lpSrc =(BYTE)unchValue ;
	//	}
	//}	
	
	delete []pCTData;										// 释放内存
	delete []pCFData;										// 释放内存
	delete []ampData;
	pCTData = NULL;
	pCFData = NULL;	
	return (true);										//返回结果
}

/*************************************************************************
 *
 * \函数名称：
 *   FFT_1D()
 *
 * \输入参数:
 *   complex<double> * pCTData	- 指向时域数据的指针，输入的需要变换的数据
 *   complex<double> * pCFData	- 指向频域数据的指针，输出的经过变换的数据
 *   nLevel						－傅立叶变换蝶形算法的级数，2的幂数，
 *
 * \返回值:
 *   无
 *
 * \说明:
 *   一维快速傅立叶变换。
 *
 *************************************************************************
 */
void CDib::FFT_1D(complex<double> * pCTData, complex<double> * pCFData, int nLevel)
{
		// 循环控制变量
	int		i;
	int     j;
	int     k;

	// 傅立叶变换点数
	int	nCount =0 ;

	// 计算傅立叶变换点数
	nCount =(int)pow(2.0,nLevel) ;
	
	// 某一级的长度
	int		nBtFlyLen;
	nBtFlyLen = 0 ;
	
	// 变换系数的角度 ＝2 * PI * i / nCount
	double	dAngle;
	
	complex<double> *pCW ;
	
	// 分配内存，存储傅立叶变化需要的系数表
	pCW  = new complex<double>[nCount / 2];

    // 计算傅立叶变换的系数
	for(i = 0; i < nCount / 2; i++)
	{
		dAngle = -2 * Pi * i / nCount;
		pCW[i] = complex<double> ( cos(dAngle), sin(dAngle) );
	}

	// 变换需要的工作空间
	complex<double> *pCWork1,*pCWork2; 
	
	// 分配工作空间
	pCWork1 = new complex<double>[nCount];

	pCWork2 = new complex<double>[nCount];

	
	// 临时变量
	complex<double> *pCTmp;
	
	// 初始化，写入数据
	memcpy(pCWork1, pCTData, sizeof(complex<double>) * nCount);

	// 临时变量
	int nInter; 
	nInter = 0;

	// 蝶形算法进行快速傅立叶变换
	for(k = 0; k < nLevel; k++)
	{
		for(j = 0; j < (int)pow(2.0,k); j++)
		{
			//计算长度
			nBtFlyLen = (int)pow( 2.0,(nLevel-k) );
			
			//倒序重排，加权计算
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

		// 交换 pCWork1和pCWork2的数据
		pCTmp   = pCWork1	;
		pCWork1 = pCWork2	;
		pCWork2 = pCTmp		;
	}
	
	// 重新排序
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
	
	// 释放内存空间
	delete []pCW;
	delete []pCWork1;
	delete []pCWork2;
	pCW		=	NULL	;
	pCWork1 =	NULL	;
	pCWork2 =	NULL	;

}
/*************************************************************************
 *
 * \函数名称：
 *    IFFT_1D()
 *
 * \输入参数:
 *   complex<double> * pCTData	- 指向时域数据的指针，输入的需要反变换的数据
 *   complex<double> * pCFData	- 指向频域数据的指针，输出的经过反变换的数据
 *   nLevel						－傅立叶变换蝶形算法的级数，2的幂数，
 *
 * \返回值:
 *   无
 *
 * \说明:
 *   一维快速傅立叶反变换。
 *
 ************************************************************************
 */
void CDib::IFFT_1D(complex<double> * pCFData, complex<double> * pCTData, int nLevel)
{
	
	// 循环控制变量
	int		i;

	// 傅立叶反变换点数
	int nCount;

	// 计算傅立叶变换点数
	nCount = (int)pow(2.0,nLevel) ;
	
	// 变换需要的工作空间
	complex<double> *pCWork;	
	
	// 分配工作空间
	pCWork = new complex<double>[nCount];
	
	// 将需要反变换的数据写入工作空间pCWork
	memcpy(pCWork, pCFData, sizeof(complex<double>) * nCount);
	
	// 为了利用傅立叶正变换,可以把傅立叶频域的数据取共轭
	// 然后直接利用正变换，输出结果就是傅立叶反变换结果的共轭
	for(i = 0; i < nCount; i++)
	{
		pCWork[i] = complex<double> (pCWork[i].real(), -pCWork[i].imag());
	}
	
	// 调用快速傅立叶变换实现反变换，结果存储在pCTData中
	FFT_1D(pCWork, pCTData, nLevel);
	
	// 求时域点的共轭，求得最终结果
	// 根据傅立叶变换原理，利用这样的方法求得的结果和实际的时域数据
	// 相差一个系数nCount
	for(i = 0; i < nCount; i++)
	{
		pCTData[i] = 
			complex<double> (pCTData[i].real() / nCount, -pCTData[i].imag() / nCount);
	}
	
	// 释放内存
	delete pCWork;
	pCWork = NULL;
}

/*************************************************************************
 *
 * \函数名称：
 *   FFT_2D()
 *
 * \输入参数:
 *   complex<double> * pCTData	- 图像数据
 *   int    nWidth				- 数据宽度
 *   int    nHeight				- 数据高度
 *   complex<double> * pCFData	- 傅立叶变换后的结果
 *
 * \返回值:
 *   无
 *
 * \说明:
 *   二维傅立叶变换。
 *
 ************************************************************************
 */
void CDib::FFT_2D(complex<double> * pCTData, int nWidth, int nHeight, complex<double> * pCFData)
{
	
	// 循环控制变量
	int	x;
	int	y;
	
	// 临时变量
	double	dTmpOne;
	double  dTmpTwo;
	
	// 进行傅立叶变换的宽度和高度，（2的整数次幂）
	// 图像的宽度和高度不一定为2的整数次幂
	int		nTransWidth;
	int 	nTransHeight;

	// 计算进行傅立叶变换的宽度	（2的整数次幂）
	dTmpOne = log(1.0*nWidth)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransWidth = (int) dTmpTwo	   ;
	
	// 计算进行傅立叶变换的高度 （2的整数次幂）
	dTmpOne = log(1.0*nHeight)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransHeight = (int) dTmpTwo	   ;	
	
	// x，y（行列）方向上的迭代次数
	int		nXLev;
	int		nYLev;

	// 计算x，y（行列）方向上的迭代次数
	nXLev = (int) ( log(1.0*nTransWidth)/log(2.0) +  0.5 );
	nYLev = (int) ( log(1.0*nTransHeight)/log(2.0) + 0.5 );
	
	for(y = 0; y < nTransHeight; y++)
	{
		// x方向进行快速傅立叶变换
		FFT_1D(&pCTData[nTransWidth * y], &pCFData[nTransWidth * y], nXLev);
	}
	
	// pCFData中目前存储了pCTData经过行变换的结果
	// 为了直接利用FFT_1D，需要把pCFData的二维数据转置，再一次利用FFT_1D进行
	// 傅立叶行变换（实际上相当于对列进行傅立叶变换）
	for(y = 0; y < nTransHeight; y++)
	{
		for(x = 0; x < nTransWidth; x++)
		{
			pCTData[nTransHeight * x + y] = pCFData[nTransWidth * y + x];
		}
	}
	
	for(x = 0; x < nTransWidth; x++)
	{
		// 对x方向进行快速傅立叶变换，实际上相当于对原来的图象数据进行列方向的
		// 傅立叶变换
		FFT_1D(&pCTData[x * nTransHeight], &pCFData[x * nTransHeight], nYLev);
	}

	// pCFData中目前存储了pCTData经过二维傅立叶变换的结果，但是为了方便列方向
	// 的傅立叶变换，对其进行了转置，现在把结果转置回来
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
 * \函数名称：
 *   IFFT_2D()
 *
 * \输入参数:
 *   complex<double> * pCFData	- 频域数据
 *   complex<double> * pCTData	- 时域数据
 *   int    nWidth				- 图像数据宽度
 *   int    nHeight				- 图像数据高度
 *
 * \返回值:
 *   无
 *
 * \说明:
 *   二维傅立叶反变换。
 *
 ************************************************************************
 */
void CDib::IFFT_2D(complex<double> * pCFData, complex<double> * pCTData, int nWidth, int nHeight)
{
	// 循环控制变量
	int	x;
	int	y;
	
	// 临时变量
	double	dTmpOne;
	double  dTmpTwo;
	
	// 进行傅立叶变换的宽度和高度，（2的整数次幂）
	// 图像的宽度和高度不一定为2的整数次幂
	int		nTransWidth;
	int 	nTransHeight;

	// 计算进行傅立叶变换的宽度	（2的整数次幂）
	dTmpOne = log(1.0*nWidth)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransWidth = (int) dTmpTwo	   ;
	
	// 计算进行傅立叶变换的高度 （2的整数次幂）
	dTmpOne = log(1.0*nHeight)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransHeight = (int) dTmpTwo	   ;
	
	// 分配工作需要的内存空间
	complex<double> *pCWork= new complex<double>[nTransWidth * nTransHeight];

	//临时变量
	complex<double> *pCTmp ;
	
	// 为了利用傅立叶正变换,可以把傅立叶频域的数据取共轭
	// 然后直接利用正变换，输出结果就是傅立叶反变换结果的共轭
	for(y = 0; y < nTransHeight; y++)
	{
		for(x = 0; x < nTransWidth; x++)
		{
			pCTmp = &pCFData[nTransWidth * y + x] ;
			pCWork[nTransWidth * y + x] = complex<double>( pCTmp->real() , -pCTmp->imag() );
		}
	}

	// 调用傅立叶正变换
	FFT_2D(pCWork, nWidth, nHeight, pCTData) ;
	
	// 求时域点的共轭，求得最终结果
	// 根据傅立叶变换原理，利用这样的方法求得的结果和实际的时域数据
	// 相差一个系数
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


void CDib::PieLinTrans()//分段线性拉伸
{
	// TODO: 在此处添加实现代码.
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

void CDib::InvertPalette()//利用调色板实现反白
{
	// TODO: 在此处添加实现代码.

	// 确保图像是灰度图像
	if (m_nBitCount != 8) {
		AfxMessageBox(_T("这不是一个灰度图像，无法通过这种方式反白！"));
		return;
	}

	// 获取当前调色板
	RGBQUAD palette[256] = { 0 };
	GetColorTable(0, 256, palette);

	//改变调色板
	for (int i = 0; i < 256; i++) {
		palette[i].rgbRed = 255 - palette[i].rgbRed;
		palette[i].rgbBlue = 255 - palette[i].rgbBlue;
		palette[i].rgbGreen = 255 - palette[i].rgbGreen;
	}
	//重新赋值
	SetColorTable(0, 256, palette);
}


void CDib::showBitPlane0()//显示位平面0
{
	// TODO: 在此处添加实现代码.
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

void CDib::showBitPlane7()//显示位平面7
{
	// TODO: 在此处添加实现代码.
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
	// TODO: 在此处添加实现代码.
	
	int length = GetMaxColorTableEntries();
	long *Count = GrayValueCount();

	double *equHisto = new double[length];
	memset(equHisto, 0, length * sizeof(double));

	for (int i = 1; i < length; i++)
	{
		equHisto[i] = (double)Count[i] / (double)(m_nHeight * m_nWidth);	//将计数换算为概率（除以像素数）
	}

	double* equHistoSum = new double[length];
	memset(equHistoSum, 0, length * sizeof(double));

	equHistoSum[0] = equHisto[0];
	for (int i = 1; i < length; i++)
	{
		equHistoSum[i] = equHistoSum[i - 1] + equHisto[i];	//计算累积概率
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
	// TODO: 在此处添加实现代码.
	double SmoothKernel[3][3] = { {1.0/9, 1.0/9, 1.0/9},
						   {1.0/9, 1.0/9, 1.0/9},
						   {1.0/9, 1.0/9, 1.0/9} };
	int kernelSize = 3;
	double temp = 0;
	unsigned char* pic = new unsigned char[(m_nHeight + 2) * (m_nWidthBytes + 2)];	//创建一个新的图像，用来操作

	memset(pic, 0, (m_nHeight + 2) * (m_nWidthBytes + 2) * sizeof(unsigned char));
	for (int i = 1; i < m_nHeight + 1; i++) //创建补0图像
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
	// TODO: 在此处添加实现代码.
	int SobelKernelX[3][3] = { {-1, 0, 1},
								{-2, 0, 2},
								{-1, 0, 1} };
	int SobelKernelY[3][3] = { {-1, -2, -1},
								{0, 0, 0},
								{1, 2, 1} };
	int temp1 = 0;
	int temp2 = 0;
	int gradientModule = 0;
	unsigned char* pic= new unsigned char[(m_nHeight+2) * (m_nWidthBytes+2)];	//创建一个新的图像，用来操作

	memset(pic, 0, (m_nHeight + 2) * (m_nWidthBytes + 2) * sizeof(unsigned char));
	for (int i = 1; i < m_nHeight+1; i++) //创建补0图像
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
	// TODO: 在此处添加实现代码.
	int LapKernel[3][3] = { {-1, -1, -1},
							{-1, 9, -1},
							{-1, -1, -1} };
	double temp = 0;

	int kernelSize = 3;
	long newHeight = m_nHeight +(kernelSize - 1);
	long newWidth = m_nWidth + (kernelSize - 1);
	int newWidthBytes = newWidth*m_nBitCount;
	unsigned char* pic = new unsigned char[(m_nHeight + 2) * (m_nWidthBytes + 2)];	//创建一个新的图像，用来操作

	memset(pic, 0, (m_nHeight + 2) * (m_nWidthBytes + 2) * sizeof(unsigned char));

	for (int i = 1; i < m_nHeight + 1; i++) //创建补0图像
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

double CDib::ConvGaussian(int Dimenssion)//19*19高斯滤波
{
	// 获取程序开始运行时的时间
	double startTickCount = GetTickCount64();

	int KernelSize = 19;
	double sigma = (KernelSize - 1) / 6.0;
	int halfSize = (KernelSize - 1) / 2;
	long newHeight = m_nHeight + (KernelSize - 1);
	long newWidth = m_nWidth + (KernelSize - 1);
	int newWidthBytes = newWidth;
	// TODO: 在此处添加实现代码.
	if (Dimenssion == 1) {//两次一维卷积实现
		double GaussianKernel[19] = {0.0};
		//生成高斯核
		double sum = 0;
		for (int i = -9; i <= 9; i++) {
			GaussianKernel[i + 9] = exp(-i * i / (2 * sigma * sigma)) / (sqrt(2 * Pi) * sigma);
			sum += GaussianKernel[i + 9];
		}
		//归一化
		for (int i = 0; i < 19; i++) {
			GaussianKernel[i] /= sum;
		}
		double* pic = new double[newHeight * newWidth];	//创建一个新的图像，用来操作
		double *pictemp = new double[newHeight * newWidth];				
		memset(pic, 0, newHeight * newWidth* sizeof(double));//这里记得要用sizeof(double)而不是sizeof(unsigned char)
		memset(pictemp, 0, newHeight * newWidth* sizeof(double));
		//创建补0图像
		for (int i = 9; i < (newHeight - 9); i++) {
			for (int j = 9; j < (newWidth - 9); j++) {
				pic[i * newWidthBytes + j] = m_pDibBits[(i - 9) * m_nWidthBytes + j - 9];
				pictemp[i * newWidthBytes + j] = m_pDibBits[(i - 9) * m_nWidthBytes + j - 9];
			}
		}
		//第一次卷积
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
		//第二次卷积
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
		//19*19高斯核
		double GaussianKernel[19][19];
		//生成高斯核
		double sum = 0;
		for (int i = -9; i <= 9; i++) {
			for (int j = -9; j <= 9; j++) {
				GaussianKernel[i + 9][j + 9] = exp(-(i * i + j * j) / (2 * sigma * sigma)) / (2 * Pi * sigma * sigma);
				sum += GaussianKernel[i + 9][j + 9];
			}
		}
		//归一化
		for (int i = 0; i < 19; i++) {
			for (int j = 0; j < 19; j++) {
				GaussianKernel[i][j] /= sum;
			}
		}
		unsigned char* pic = new unsigned char[newHeight * newWidth];	//创建一个新的图像，用来操作
		memset(pic, 0, newHeight * newWidth*sizeof(unsigned char));
		//创建补0图像
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

	// 获取程序结束运行时的时间
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
	// TODO: 在此处添加实现代码.
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
	//创建暂存文件像素值的缓冲区//16位，保存raw
	if (m_pRawBuffer != NULL)
	{
		delete[] m_pRawBuffer;
		m_pRawBuffer = NULL;
	}
	m_pRawBuffer = new uint16_t[nWidth * nHeight];
	memset(m_pRawBuffer, 0, nWidth * nHeight * sizeof(uint16_t));

	//使用fistream读取raw文件的像素值进buffer
	std::ifstream file(lpszPathName, std::ios::binary);
	file.read(reinterpret_cast<char*>(m_pRawBuffer), nWidth * nHeight * sizeof(uint16_t));

	//文件打开失败弹出警告，然后退出
	if (file.fail()) {
		AfxMessageBox(_T("读取文件失败"));
		return;
	}
	file.close();
	//因为读取进来的像素高8位和低8位顺序是反的，这里对像素高低8位进行对调，同时获取最大值
	for (int i = 0; i < nWidth * nHeight; i++) {
		m_pRawBuffer[i] = ((m_pRawBuffer[i] & 0xff00) >> 8) | ((m_pRawBuffer[i] & 0x00ff) << 8);
	}
	CreateDisplayDib(m_pRawBuffer, nWidth, nHeight, 8);
}

void CDib::CreateDisplayDib(uint16_t* RawBuffer, int nWidth, int nHeight, int nBitCount)
{
	// TODO: 在此处添加实现代码.
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
	//将buffer中的像素值写入Dib
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
	// TODO: 在此处添加实现代码.
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
	//将buffer中的像素值写入Dib
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
	// TODO: 在此处添加实现代码.
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
				//红色
				m_pDibBits[i * m_nWidthBytes + j * 3 + 2] = 255;
			}
			if ((i - centerGy) * (i - centerGy) + (j - centerGx) * (j - centerGx) <= radius * radius)
			{
				//绿色
				m_pDibBits[i * m_nWidthBytes + j * 3 + 1] = 255;
			}
			if ((i - centerBy) * (i - centerBy) + (j - centerBx) * (j - centerBx) <= radius * radius)
			{
				//蓝色
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
	// TODO: 在此处添加实现代码.
	bool flag = false;
	flag=FFT(m_pDibBits, nWidth, nHeight);
	if (flag == false)
	{
		AfxMessageBox(_T("FFT失败"));
	}
}

BOOL CDib::myInverseFFT(long nWidth, long nHeight)
{
	unsigned char* lpSrc;							// 指向源图像的指针
	int y;										// 循环控制变量
	int x;										// 循环控制变量
	double dTmpOne;								//存放临时变量
	double dTmpTwo;								//存放临时变量
	int nTransWidth;								// 傅立叶变换的宽度（2的整数次幂）
	int nTransHeight;								// 傅立叶变换的高度（2的整数次幂）
	double unchValue;								// 存贮图像各像素灰度的临时变量
	complex<double>* pCTData;						// 指向时域数据的指针
	complex<double>* pCFData;						// 指向频域数据的指针
	// 计算进行傅立叶变换的点数－横向	（2的整数次幂）
	dTmpOne = log(1.0 * nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;
	// 计算进行傅立叶变换的点数－纵向 （2的整数次幂）
	dTmpOne = log(1.0 * nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;
	double dReal;									// 傅立叶变换的实部
	double dImag;									// 傅立叶变换的虚部

	pCTData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	pCFData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	// 图像数据的宽和高不一定是2的整数次幂，所以pCTData有一部分数据需要补0
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCFData[y * nTransWidth + x] = complex<double>(0, 0);		// 补零
		}
	}
	//把图像数据传给pCFData
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			unchValue = m_pDibBits[y * nWidth + x] * pow(-1.0, x + y);	//乘以-1的x+y次方，把频域0点移到中心
			pCFData[y * nTransWidth + x] = complex<double>(unchValue, 0);
		}
	}
	
	// code needed to insert here

	IFFT_2D(pCFData, pCTData,nHeight, nWidth); 				// 傅立叶反变换

	double maxVal = 0;
	double dVal = 0;
	for (y = 0; y < nHeight; y++) {
		for (x = 0; x < nWidth; x++) {
			dReal=pCTData[y * nTransWidth + x].real();		// 实部
			dVal = abs(dReal);
			if (dVal > maxVal) {
				maxVal = dReal;
			}
		}
	}

	double powNum = 1.0 / 5.0;
	double maxPow = pow(maxVal, powNum);

	for(y=0; y<nHeight; y++)								// 反变换的数据传给lpDIBBits
	{
		for(x=0; x<nWidth; x++)
		{
			//需要考虑信号的正负问题以及实际所用的图象数据是灰度值还是原始数据
			dReal = pCTData[y*nTransWidth + x].real() ;		// 实部
			dImag = pCTData[y*nTransWidth + x].imag() ;		// 虚部
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
			// 指向DIB第y行，第x个象素的指针
			m_pDibBits[y*nWidth+x] = (unsigned char)unchValue;
			//lpSrc = (unsigned char*)m_pDibBits + nWidth * (nHeight - 1 - y) + x;
			//*lpSrc =(BYTE)unchValue ;
		}
	}	

	delete[]pCTData;										// 释放内存
	delete[]pCFData;										// 释放内存
	pCTData = NULL;
	pCFData = NULL;
	return (true);										//返回结果
}



BOOL CDib::Filter(int type, double cutOff)
//type==0,理想低通滤波器 type==1,理想高通滤波器
//type==2,高斯低通滤波器 type==3,高斯高通滤波器
//type==4,巴特沃斯低通滤波器 type==5,巴特沃斯高通滤波器
{
	// TODO: 在此处添加实现代码.
	
	unsigned char* lpSrc;							// 指向源图像的指针
	int y;										// 循环控制变量
	int x;										// 循环控制变量
	double dTmpOne;								//存放临时变量
	double dTmpTwo;								//存放临时变量
	int nTransWidth;								// 傅立叶变换的宽度（2的整数次幂）
	int nTransHeight;								// 傅立叶变换的高度（2的整数次幂）
	double unchValue;								// 存贮图像各像素灰度的临时变量
	long nHeight = m_nHeight;
	long nWidth = m_nWidth;
	unsigned char* pDIBBits = m_pDibBits;			// 指向DIB数据的指针
	complex<double>* pCTData;						// 指向时域数据的指针
	complex<double>* pCFData;						// 指向频域数据的指针
	// 计算进行傅立叶变换的点数－横向	（2的整数次幂）
	dTmpOne = log(1.0 * m_nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;
	// 计算进行傅立叶变换的点数－纵向 （2的整数次幂）
	dTmpOne = log(1.0 * nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;
	double dReal;									// 傅立叶变换的实部
	double dImag;									// 傅立叶变换的虚部

	pCTData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	pCFData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	// 图像数据的宽和高不一定是2的整数次幂，所以pCTData有一部分数据需要补0
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[y * nTransWidth + x] = complex<double>(0, 0);		// 补零
		}
	}
	//把图像数据传给pCTData
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			unchValue = (*lpSrc) * pow(-1.0, x + y);	//乘以-1的x+y次方，把频域0点移到中心
			pCTData[y * nTransWidth + x] = complex<double>(unchValue, 0);
		}
	}
	FFT_2D(pCTData, nWidth, nHeight, pCFData);				// 傅立叶正变换
	//变完的结果还在pCTData中

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
			if (type == 0) {//理想低通
				if (dist >= cutOff)
				{
					pCFData[y * nTransWidth + x] = complex<double>(0, 0);
				}
			}
			else if (type == 1) {//理想高通
				if (dist <= cutOff)
				{
					pCFData[y * nTransWidth + x] = complex<double>(0, 0);
				}
			}
			else if (type == 2) {//高斯低通
				pCFData[y * nTransWidth + x] = pCFData[y * nTransWidth + x] * exp(-dist * dist / (2 * cutOff * cutOff));
			}
			else if (type == 3) {//高斯高通
				pCFData[y * nTransWidth + x] = pCFData[y * nTransWidth + x] * (1 - exp(-dist * dist / (2 * cutOff * cutOff)));
			}
			else if (type == 4) {//巴特沃斯低通
				pCFData[y * nTransWidth + x] = pCFData[y * nTransWidth + x] / (1 + pow(dist / cutOff, 2));
			}
			else if (type == 5) {//巴特沃斯高通
				pCFData[y * nTransWidth + x] = pCFData[y * nTransWidth + x] * (1 - 1 / (1 + pow(dist / cutOff, 2)));
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////

	IFFT_2D(pCFData, pCTData, nHeight, nWidth); 				// 傅立叶反变换

	for (y = 0; y < nHeight; y++)								// 反变换的数据传给lpDIBBits
	{
		for (x = 0; x < nWidth; x++)
		{
			//需要考虑信号的正负问题以及实际所用的图象数据是灰度值还是原始数据
			dReal = pCTData[y * nTransWidth + x].real();		// 实部
			dImag = pCTData[y * nTransWidth + x].imag();		// 虚部
			unchValue = dReal * pow(-1.0, x + y);
			if (unchValue < 0)
			{
				unchValue = 0;
			}
			if (unchValue > 0xff)
			{
				unchValue = 0xff;
			}
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			*lpSrc = (BYTE)unchValue;
		}
	}

	delete[]pCTData;										// 释放内存
	delete[]pCFData;										// 释放内存
	pCTData = NULL;
	pCFData = NULL;
	return (true);										//返回结果
}


void CDib::medianFilter()
{
	// TODO: 在此处添加实现代码.
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
	// TODO: 在此处添加实现代码.
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
					n += 2;	//保证n为奇数
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
	// TODO: 在此处添加实现代码.
	//湍流图像退化
	//Hufnagel and Stanley模型
	//H(u,v)=exp(-k*(u^2+v^2)^(5/6))
	unsigned char* lpSrc;							// 指向源图像的指针
	int y;										// 循环控制变量
	int x;										// 循环控制变量
	double dTmpOne;								//存放临时变量
	double dTmpTwo;								//存放临时变量
	int nTransWidth;								// 傅立叶变换的宽度（2的整数次幂）
	int nTransHeight;								// 傅立叶变换的高度（2的整数次幂）
	double unchValue;								// 存贮图像各像素灰度的临时变量
	long nHeight = m_nHeight;
	long nWidth = m_nWidth;
	unsigned char* pDIBBits = m_pDibBits;			// 指向DIB数据的指针
	complex<double>* pCTData;						// 指向时域数据的指针
	complex<double>* pCFData;						// 指向频域数据的指针
	// 计算进行傅立叶变换的点数－横向	（2的整数次幂）
	dTmpOne = log(1.0 * m_nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;
	// 计算进行傅立叶变换的点数－纵向 （2的整数次幂）
	dTmpOne = log(1.0 * nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;
	double dReal;									// 傅立叶变换的实部
	double dImag;									// 傅立叶变换的虚部

	pCTData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	pCFData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	// 图像数据的宽和高不一定是2的整数次幂，所以pCTData有一部分数据需要补0
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[y * nTransWidth + x] = complex<double>(0, 0);		// 补零
		}
	}
	//把图像数据传给pCTData
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			unchValue = (*lpSrc) * pow(-1.0, x + y);	//乘以-1的x+y次方，把频域0点移到中心
			pCTData[y * nTransWidth + x] = complex<double>(unchValue, 0);
		}
	}
	FFT_2D(pCTData, nWidth, nHeight, pCFData);				// 傅立叶正变换
	//变完的结果还在pCTData中

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

	IFFT_2D(pCFData, pCTData, nHeight, nWidth); 				// 傅立叶反变换

	for (y = 0; y < nHeight; y++)								// 反变换的数据传给lpDIBBits
	{
		for (x = 0; x < nWidth; x++)
		{
			//需要考虑信号的正负问题以及实际所用的图象数据是灰度值还是原始数据
			dReal = pCTData[y * nTransWidth + x].real();		// 实部
			dImag = pCTData[y * nTransWidth + x].imag();		// 虚部
			unchValue = dReal * pow(-1.0, x + y);
			if (unchValue < 0)
			{
				unchValue = 0;
			}
			if (unchValue > 0xff)
			{
				unchValue = 0xff;
			}
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			*lpSrc = (BYTE)unchValue;
		}
	}

	delete[]pCTData;										// 释放内存
	delete[]pCFData;										// 释放内存
	pCTData = NULL;
	pCFData = NULL;
}


void CDib::InverseFiltering(double k,double cutoff)
{
	// TODO: 在此处添加实现代码.
	// TODO: 在此处添加实现代码.
	unsigned char* lpSrc;							// 指向源图像的指针
	int y;										// 循环控制变量
	int x;										// 循环控制变量
	double dTmpOne;								//存放临时变量
	double dTmpTwo;								//存放临时变量
	int nTransWidth;								// 傅立叶变换的宽度（2的整数次幂）
	int nTransHeight;								// 傅立叶变换的高度（2的整数次幂）
	double unchValue;								// 存贮图像各像素灰度的临时变量
	long nHeight = m_nHeight;
	long nWidth = m_nWidth;
	unsigned char* pDIBBits = m_pDibBits;			// 指向DIB数据的指针
	complex<double>* pCTData;						// 指向时域数据的指针
	complex<double>* pCFData;						// 指向频域数据的指针
	// 计算进行傅立叶变换的点数－横向	（2的整数次幂）
	dTmpOne = log(1.0 * m_nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;
	// 计算进行傅立叶变换的点数－纵向 （2的整数次幂）
	dTmpOne = log(1.0 * nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;
	double dReal;									// 傅立叶变换的实部
	double dImag;									// 傅立叶变换的虚部

	pCTData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	pCFData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	// 图像数据的宽和高不一定是2的整数次幂，所以pCTData有一部分数据需要补0
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[y * nTransWidth + x] = complex<double>(0, 0);		// 补零
		}
	}
	//把图像数据传给pCTData
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			unchValue = (*lpSrc) * pow(-1.0, x + y);	//乘以-1的x+y次方，把频域0点移到中心
			pCTData[y * nTransWidth + x] = complex<double>(unchValue, 0);
		}
	}
	FFT_2D(pCTData, nWidth, nHeight, pCFData);				// 傅立叶正变换
	//变完的结果还在pCTData中

	///////////////////////////////////////////////////////////////////////////
	// code needed to insert here
	double dist = 0;
	int centralX = m_nWidth / 2;
	int centralY = m_nHeight / 2;
	//对图像湍流退化的逆滤波
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

	IFFT_2D(pCFData, pCTData, nHeight, nWidth); 				// 傅立叶反变换

	for (y = 0; y < nHeight; y++)								// 反变换的数据传给lpDIBBits
	{
		for (x = 0; x < nWidth; x++)
		{
			//需要考虑信号的正负问题以及实际所用的图象数据是灰度值还是原始数据
			dReal = pCTData[y * nTransWidth + x].real();		// 实部
			dImag = pCTData[y * nTransWidth + x].imag();		// 虚部
			unchValue = dReal * pow(-1.0, x + y);
			if (unchValue < 0)
			{
				unchValue = 0;
			}
			if (unchValue > 0xff)
			{
				unchValue = 0xff;
			}
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			*lpSrc = (BYTE)unchValue;
		}
	}

	delete[]pCTData;										// 释放内存
	delete[]pCFData;										// 释放内存
	pCTData = NULL;
	pCFData = NULL;
}


void CDib::WienerFilter()
{
	// TODO: 在此处添加实现代码.
	unsigned char* lpSrc;							// 指向源图像的指针
	int y;										// 循环控制变量
	int x;										// 循环控制变量
	double dTmpOne;								//存放临时变量
	double dTmpTwo;								//存放临时变量
	int nTransWidth;								// 傅立叶变换的宽度（2的整数次幂）
	int nTransHeight;								// 傅立叶变换的高度（2的整数次幂）
	double unchValue;								// 存贮图像各像素灰度的临时变量
	long nHeight = m_nHeight;
	long nWidth = m_nWidth;
	unsigned char* pDIBBits = m_pDibBits;			// 指向DIB数据的指针
	complex<double>* pCTData;						// 指向时域数据的指针
	complex<double>* pCFData;						// 指向频域数据的指针
	// 计算进行傅立叶变换的点数－横向	（2的整数次幂）
	dTmpOne = log(1.0 * m_nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;
	// 计算进行傅立叶变换的点数－纵向 （2的整数次幂）
	dTmpOne = log(1.0 * nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;
	double dReal;									// 傅立叶变换的实部
	double dImag;									// 傅立叶变换的虚部

	pCTData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	pCFData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	// 图像数据的宽和高不一定是2的整数次幂，所以pCTData有一部分数据需要补0
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[y * nTransWidth + x] = complex<double>(0, 0);		// 补零
		}
	}
	//把图像数据传给pCTData
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			unchValue = (*lpSrc) * pow(-1.0, x + y);	//乘以-1的x+y次方，把频域0点移到中心
			pCTData[y * nTransWidth + x] = complex<double>(unchValue, 0);
		}
	}
	FFT_2D(pCTData, nWidth, nHeight, pCFData);				// 傅立叶正变换
	//变完的结果还在pCTData中

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

	IFFT_2D(pCFData, pCTData, nHeight, nWidth); 				// 傅立叶反变换

	for (y = 0; y < nHeight; y++)								// 反变换的数据传给lpDIBBits
	{
		for (x = 0; x < nWidth; x++)
		{
			//需要考虑信号的正负问题以及实际所用的图象数据是灰度值还是原始数据
			dReal = pCTData[y * nTransWidth + x].real();		// 实部
			dImag = pCTData[y * nTransWidth + x].imag();		// 虚部
			unchValue = dReal * pow(-1.0, x + y);
			if (unchValue < 0)
			{
				unchValue = 0;
			}
			if (unchValue > 0xff)
			{
				unchValue = 0xff;
			}
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			*lpSrc = (BYTE)unchValue;
		}
	}

	delete[]pCTData;										// 释放内存
	delete[]pCFData;										// 释放内存
	pCTData = NULL;
	pCFData = NULL;
}


double CDib::CalculatePSNR(unsigned char* trueImg)
{
	// TODO: 在此处添加实现代码.
	//计算图像的峰值信噪比
	//PSNR=10*log10(255^2/MSE)
	//MSE=1/MN*sum((f(i,j)-f'(i,j))^2)
	//f(i,j)为原图像，f'(i,j)为处理后的图像
	double MSE = 0.0;
	double PSNR = 0.0;
	unsigned char* lpSrc;							// 指向源图像的指针
	unsigned char* lpDst;							// 指向目标图像的指针
	int y;										// 循环控制变量
	int x;										// 循环控制变量
	long nHeight = m_nHeight;
	long nWidth = m_nWidth;
	lpSrc = new unsigned char[nHeight * nWidth];
	lpDst = new unsigned char[nHeight * nWidth];
	memset(lpSrc, 0, nHeight * nWidth);
	memset(lpDst, 0, nHeight * nWidth);
	//计算MSE
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
	// TODO: 在此处添加实现代码.
	unsigned char* lpSrc;							// 指向源图像的指针
	int y;										// 循环控制变量
	int x;										// 循环控制变量
	double dTmpOne;								//存放临时变量
	double dTmpTwo;								//存放临时变量
	int nTransWidth;								// 傅立叶变换的宽度（2的整数次幂）
	int nTransHeight;								// 傅立叶变换的高度（2的整数次幂）
	double unchValue;								// 存贮图像各像素灰度的临时变量
	long nHeight = m_nHeight;
	long nWidth = m_nWidth;
	unsigned char* pDIBBits = m_pDibBits;			// 指向DIB数据的指针
	complex<double>* pCTData;						// 指向时域数据的指针
	complex<double>* pCFData;						// 指向频域数据的指针
	// 计算进行傅立叶变换的点数－横向	（2的整数次幂）
	dTmpOne = log(1.0 * m_nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;
	// 计算进行傅立叶变换的点数－纵向 （2的整数次幂）
	dTmpOne = log(1.0 * nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;
	double dReal;									// 傅立叶变换的实部
	double dImag;									// 傅立叶变换的虚部

	pCTData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	pCFData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	// 图像数据的宽和高不一定是2的整数次幂，所以pCTData有一部分数据需要补0
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[y * nTransWidth + x] = complex<double>(0, 0);		// 补零
		}
	}
	//把图像数据传给pCTData
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			unchValue = (*lpSrc) * pow(-1.0, x + y);	//乘以-1的x+y次方，把频域0点移到中心
			pCTData[y * nTransWidth + x] = complex<double>(unchValue, 0);
		}
	}
	FFT_2D(pCTData, nWidth, nHeight, pCFData);				// 傅立叶正变换
	//变完的结果还在pCTData中

	///////////////////////////////////////////////////////////////////////////
	// code needed to insert here
	//使用高斯低通滤波器
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

	IFFT_2D(pCFData, pCTData, nHeight, nWidth); 				// 傅立叶反变换

	for (y = 0; y < nHeight; y++)								// 反变换的数据传给lpDIBBits
	{
		for (x = 0; x < nWidth; x++)
		{
			//需要考虑信号的正负问题以及实际所用的图象数据是灰度值还是原始数据
			dReal = pCTData[y * nTransWidth + x].real();		// 实部
			dImag = pCTData[y * nTransWidth + x].imag();		// 虚部
			unchValue = dReal * pow(-1.0, x + y);
			if (unchValue < 0)
			{
				unchValue = 0;
			}
			if (unchValue > 0xff)
			{
				unchValue = 0xff;
			}
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			*lpSrc = (BYTE)unchValue;
		}
	}

	delete[]pCTData;										// 释放内存
	delete[]pCFData;										// 释放内存
	pCTData = NULL;
	pCFData = NULL;
	
}


void CDib::AdaptiveLocalDenoise()
{
	// TODO: 在此处添加实现代码.
	//自适应局部降噪空间滤波
	//均值：计算平均值的区域上的平均灰度
	//方差：该区域上的图像对比度

	//首先计算方差
	long nColors = GetMaxColorTableEntries();
	long* Count = GrayValueCount();
	double* p = new double[nColors];
	double mean = 0.0;
	double variance = 0.0;
	memset(p, 0, nColors * sizeof(double));
	for (int i = 0; i < nColors; i++)
	{
		p[i] = (double)Count[i] / (double)(m_nHeight * m_nWidth);	//将计数换算为概率（除以像素数）
		mean += i * p[i];	//计算均值
	}
	for (int i = 0; i < nColors; i++)
	{
		variance += p[i] * (i - mean) * (i - mean);	//计算方差
	}

	//计算局部均值和局部方差
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
	// TODO: 在此处添加实现代码.
	//两个图像数组相加
	//两个图像的大小必须相同

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
	// TODO: 在此处添加实现代码.
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
	// TODO: 在此处添加实现代码.
	//创建暂存文件像素值的缓冲区//16位，保存raw
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

		//使用fistream读取raw文件的像素值进buffer
		
		file.read(reinterpret_cast<char*>(m_pRawBuffer), nWidth * nHeight * sizeof(uint16_t));
		
		//因为读取进来的像素高8位和低8位顺序是反的，这里对像素高低8位进行对调，同时获取最大值
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
	// TODO: 在此处添加实现代码.
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
	//将buffer中的像素值写入Dib
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
	// TODO: 在此处添加实现代码.
	return 0;
}


void CDib::DispDiffView(int status, int frame)
{
	// TODO: 在此处添加实现代码.
	if (status == 0)//横断面
	{
		Destroy();
		DispManyRawFiles(pointerArray[frame], 512, 512, 8);
	}
	else if (status == 1)//冠状面
	{
		int frameNum = pointerArray.size();
		//确定新的图像高度
		double pixWidth = 0.68359375;
		double layerHei = 1;
		int newHeight = (layerHei / pixWidth) * (double)frameNum;
		double scaleX = 1.0;
		double scaleY = (double)frameNum / newHeight;

		if (CronalArrary.size() != 0)
		{
			//如果CronalArray不为空，就直接可以显示了，不需要赋值
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
			//双线性插值
			for (int k = 0; k < temp.size(); k++) {
				m_pRawBuffer = new uint16_t[newHeight * m_nWidth];
				memset(m_pRawBuffer, 0, newHeight * m_nWidth * sizeof(uint16_t));
				for (int y = 0; y < newHeight; y++) {
					for (int x = 0; x < m_nWidth; x++) {
						// 计算在原始图像中的位置
						double xOriginal = (x * scaleX);
						double yOriginal = (y * scaleY);

						// 找到最近的四个像素
						int x1 = (int)xOriginal;
						int y1 = (int)yOriginal;
						int x2 = min(x1 + 1, m_nWidth - 1);
						int y2 = min(y1 + 1, m_nHeight - 1);

						// 执行双线性插值
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
		//确定新的图像高度
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
			//双线性插值
			for (int k = 0; k < temp.size(); k++) {
				m_pRawBuffer = new uint16_t[newHeight * m_nWidth];
				memset(m_pRawBuffer, 0, newHeight * m_nWidth * sizeof(uint16_t));
				for (int y = 0; y < newHeight; y++) {
					for (int x = 0; x < m_nWidth; x++) {
						// 计算在原始图像中的位置
						double xOriginal = (x * scaleX);
						double yOriginal = (y * scaleY);

						// 找到最近的四个像素
						int x1 = (int)xOriginal;
						int y1 = (int)yOriginal;
						int x2 = min(x1 + 1, m_nWidth - 1);
						int y2 = min(y1 + 1, m_nHeight - 1);

						// 执行双线性插值
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
	// TODO: 在此处添加实现代码.
	//RGB转HSI
	//type=0:RGB to HSI
	//type=1:H空间
	//type=2:S空间
	//type=3:I空间
	//type=4:HSI to RGB
	double theta = 0.0;
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			//RGB归一化
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
				double dH = (m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 2] / 255.0) * 2 * Pi;//先归一化，然后转弧度
				double dS = m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 1] / 255.0;
				double dI = m_pRGBBuffer[i * m_nWidthBytes + j * 3] / 255.0;

				//RG扇区
				if (H >= 0 && H < 120)
				{
					B = dI * (1 - dS);
					R = dI * (1 + ((dS * cos(dH)) / cos(Pi / 3.0 - dH)));
					G = 3 * dI - (R + B);
				}
				//GB扇区
				else if (H >= 120 && H < 240)
				{
					dH = dH - 2.0 * Pi / 3.0;
					R = dI * (1 - dS);
					G = dI * (1 + (dS * cos(dH) / cos(Pi / 3.0 - dH)));
					B = 3 * dI - (R + G);
				}
				//BR扇区
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
	// TODO: 在此处添加实现代码.
	//使用幂变换做亮度拉伸
	//RGB转HSI
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
	//HSI转RGB
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			double R = 1.0;
			double G = 1.0;
			double B = 1.0;
			double H = (m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 2]) * 360;
			double dH = (m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 2]) * 2 * Pi;//先归一化，然后转弧度
			double dS = m_pRGBBuffer[i * m_nWidthBytes + j * 3 + 1];
			double dI = m_pRGBBuffer[i * m_nWidthBytes + j * 3];

			//RG扇区
			if (H >= 0 && H < 120)
			{
				B = dI * (1 - dS);
				R = dI * (1 + ((dS * cos(dH)) / cos(Pi / 3.0 - dH)));
				G = 3 * dI - (R + B);
			}
			//GB扇区
			else if (H >= 120 && H < 240)
			{
				dH = dH - 2.0 * Pi / 3.0;
				R = dI * (1 - dS);
				G = dI * (1 + (dS * cos(dH) / cos(Pi / 3.0 - dH)));
				B = 3 * dI - (R + G);
			}
			//BR扇区
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
	// TODO: 在此处添加实现代码.
	//将彩色或灰度图像转为二值化图像
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
	// TODO: 在此处添加实现代码.
	//腐蚀
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
	// TODO: 在此处添加实现代码.
	//膨胀
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
	// TODO: 在此处添加实现代码.
	//填充孔洞
	int nWidth = m_nWidth;
	int nHeight = m_nHeight;
	vector<unsigned char> I;
	for (int i = 0; i < nHeight; i++) {
		for (int j = 0; j < nWidth; j++) {
			I.push_back(m_pDibBits[i * m_nWidthBytes + j * 3]);
		}
	}
	//补集Ic
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
	//构建F
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
	//开始测地膨胀
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
	//取反
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
	
	//将F写入Dib
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
	// TODO: 在此处添加实现代码.
	//边缘提取
	int nWidth = m_nWidth;
	int nHeight = m_nHeight;
	vector<unsigned char> I;
	for (int i = 0; i < nHeight; i++) {
		for (int j = 0; j < nWidth; j++) {
			I.push_back(m_pDibBits[i * m_nWidthBytes + j * 3]);
		}
	}
	//补集
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
	//腐蚀
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
	
	//求差
	for (int i = 0; i < nHeight; i++) {
		for (int j = 0; j < nWidth; j++) {
			I[i * nWidth + j] = I[i * nWidth + j] - m_pDibBits[i * m_nWidthBytes + j * 3];
		}
	}
	//赋值
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
	// TODO: 在此处添加实现代码.
	//边缘增强
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
	// TODO: 在此处添加实现代码.
	//step==1: 计算梯度的大小和方向
	//step==2: 非极大值抑制
	//step==3: 双阈值检测
	//step==4: 边缘细化

	if (step == 1) {

		m_Magnitude = new double[m_nHeight * m_nWidth];
		m_Angle = new double[m_nHeight * m_nWidth];
		memset(m_Magnitude, 0, m_nHeight * m_nWidth * sizeof(double));
		memset(m_Angle, 0, m_nHeight * m_nWidth * sizeof(double));
		//将Sobel算子应用于图像，存储大小和方向
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
		unsigned char* pic = new unsigned char[(m_nHeight + 2) * (m_nWidthBytes + 2)];	//创建一个新的图像，用来操作

		memset(pic, 0, (m_nHeight + 2) * (m_nWidthBytes + 2) * sizeof(unsigned char));
		for (int i = 1; i < m_nHeight + 1; i++) //创建补0图像
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
		//非极大值抑制
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
		//双阈值检测
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
		//边缘细化
		//创建一个新图像

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
