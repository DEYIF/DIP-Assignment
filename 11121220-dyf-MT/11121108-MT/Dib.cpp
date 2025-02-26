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
			Dib.GetColorTable(0,nColors,pal);	//调色板
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
	
	//将buffer中的像素值写入Dib
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
	//检查文件名

	CString strPathName(lpszPathName);
	int nDotIndex = strPathName.ReverseFind(_T('.'));
	if (nDotIndex == -1)
	{
		//文件名没有找到"."
		AfxMessageBox(_T("不支持的文件格式"));
		return;
	}

	CString strExtension = strPathName.Mid(nDotIndex + 1);//取出文件名"."以后的内容
	strExtension.MakeLower();//将扩展名转换为小写

	// 判断扩展名是否为"raw"
	if (strExtension == _T("raw"))
	{
		//取得图像文件的宽度、高度和图像数
		m_nImgNum = ImgNum;
		m_nWidth = ImgHeight;
		m_nHeight = ImgWidth;
		m_nRawNum = ImgNum;
		m_nRawWidth= ImgWidth;
		m_nRawHeight = ImgHeight;
		//创建暂存文件像素值的缓冲区//16位，保存raw
		if (m_pRawBuffer != NULL)
		{
			delete[] m_pRawBuffer;
			m_pRawBuffer = NULL;
		}

		m_pRawBuffer = new uint16_t[m_nWidth * m_nHeight * m_nImgNum];
		memset(m_pRawBuffer, 0, m_nWidth * m_nHeight * m_nImgNum * sizeof(uint16_t));
		//使用fistream读取raw文件的像素值进buffer
		std::ifstream file(lpszPathName, std::ios::binary);
		file.read(reinterpret_cast<char*>(m_pRawBuffer), m_nWidth * m_nHeight * m_nImgNum * sizeof(uint16_t));
		//文件打开失败弹出警告，然后退出
		if (file.fail()) {
			AfxMessageBox(_T("读取文件失败"));
			return;
		}
		file.close();
		//因为读取进来的像素高8位和低8位顺序是反的，这里对像素高低8位进行对调，同时获取最大值
		for (int i = 0; i < m_nWidth * m_nHeight * m_nImgNum; i++) {
			m_pRawBuffer[i] = ((m_pRawBuffer[i] & 0xff00) >> 8) | ((m_pRawBuffer[i] & 0x00ff) << 8);
		}
		CreateDisplayDib(m_pRawBuffer, m_nWidth, m_nHeight, 8);
	}
	else
	{
		//不为raw，正常读取
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

long* CDib::GrayValueCount()	//获取图像中包含的不同灰度各自的数量
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
			unchValue = (*lpSrc)*pow(-1.0,x+y);
			pCTData[y*nTransWidth + x]=complex<double>(unchValue,0);
		}
	}
	FFT_2D(pCTData, nWidth, nHeight, pCFData) ;				// 傅立叶正变换

	///////////////////////////////////////////////////////////////////////////
	// code needed to insert here
	//
	//
	//
	//
	///////////////////////////////////////////////////////////////////////////

	IFFT_2D(pCFData, pCTData,nHeight, nWidth); 				// 傅立叶反变换

	for(y=0; y<nHeight; y++)								// 反变换的数据传给lpDIBBits
	{
		for(x=0; x<nWidth; x++)
		{
			//需要考虑信号的正负问题以及实际所用的图象数据是灰度值还是原始数据
			dReal = pCTData[y*nTransWidth + x].real() ;		// 实部
			dImag = pCTData[y*nTransWidth + x].imag() ;		// 虚部
			unchValue = dReal*pow(-1.0,x+y);
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
			*lpSrc =(BYTE)unchValue ;
		}
	}	
	
	delete[] pCTData;										// 释放内存
	delete[] pCFData;										// 释放内存
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
	delete pCW;
	delete[] pCWork1;
	delete[] pCWork2;
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
	
	//保存pCTData数据副本
	complex<double> *pCTmp = new complex<double>[nTransWidth * nTransHeight];

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

	//将结果存储在pCFData中
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCFData[nTransWidth * y + x] = pCTData[nTransWidth * y + x];
		}
	}



	//恢复pCTData数据
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[nTransWidth * y + x] = pCTmp[nTransWidth * y + x];
		}
	}

	//释放内存
	delete[] pCTmp;
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
			pCTmp = &pCFData[nTransWidth * y + x];
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
			pCTData[nTransWidth * y + x] = complex<double>( pCTmp->real()/(nTransWidth*nTransHeight), -pCTmp->imag()/(nTransWidth*nTransHeight) );
		}
	}
	// 释放内存
	delete[] pCWork;
	pCWork = NULL;
}

unsigned char* CDib::GetDibBits()
{
	return m_pDibBits;
}

void CDib::BilinearInterpolate(uint16_t * bilinear, uint16_t * normal, int width, int height, float ratio)
{
	// 双线性插值
	// bilinear: 插值后的像素值
	// normal: 原始像素值
	// width: 原始图像宽度
	// height: 原始图像高度
	// ratio: 缩放比例
	int newWidth = width;
	int newHeight = static_cast<int>(height * ratio);
	for (int i = 0; i < newHeight; i++)
	{
		for (int j = 0; j < newWidth; j++)
		{
			// 计算在原始图像中的坐标
			float x = j;
			float y = i / ratio;
			// 计算在原始图像中的四个像素坐标
			int x1 = static_cast<int>(x);
			int y1 = static_cast<int>(y);
			int x2 = x1 + 1;
			int y2 = y1 + 1;
			// 计算插值系数
			float dx = x - x1;
			float dy = y - y1;
			// 双线性插值
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
	// 创建用于存储轴位面切片的缓冲区
	uint16_t* pTemp = new uint16_t[m_nRawHeight * m_nRawWidth];
	for (int y = 0; y < m_nRawHeight; ++y) {
		for (int x = 0; x < m_nRawWidth; ++x) {
			// 计算在原始缓冲区中的索引
			int index = y * m_nRawWidth + x + CurImgNum * m_nRawHeight * m_nRawWidth;
			// 提取像素值
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
	//定义新图像的行列比例，如果行列比例不为1，则需要进行插值
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
		//如果行列比例不为1，则需要进行插值
		BilinearInterpolate(pbi_Temp, pTemp, m_nRawWidth, m_nImgNum, row_col_ratio);
	}
	else {
		//如果行列比例为1，则直接复制
		memcpy(pbi_Temp, pTemp, m_nRawWidth * m_nImgNum * sizeof(uint16_t));
	}
	CreateDisplayDib(pbi_Temp, newWidth, newHeight, 8);
	delete[] pbi_Temp;
	delete[] pTemp;
}

void CDib::ShowSagittalPlane(int CurImgNum)
{
	m_current_img_pos = 1;
	//定义新图像的行列比例，如果行列比例不为1，则需要进行插值
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
		//如果行列比例不为1，则需要进行插值
		BilinearInterpolate(pbi_Temp, pTemp, m_nRawWidth, m_nImgNum, row_col_ratio);
	}
	else {
		//如果行列比例为1，则直接复制
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
		//0横截面1矢状2冠状
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

