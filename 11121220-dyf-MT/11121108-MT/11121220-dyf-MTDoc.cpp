
// 11121220-dyf-MTDoc.cpp: CMy11121108MTDoc 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "11121220-dyf-MT.h"
#endif

#include "11121220-dyf-MTDoc.h"

#include <propkey.h>

#include "Dib.h"
#include "CDlgFileCfg.h"
#include"CDlg_SegmentView.h"
#include"FolderSegView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMy11121108MTDoc

IMPLEMENT_DYNCREATE(CMy11121220MTDoc, CDocument)

BEGIN_MESSAGE_MAP(CMy11121220MTDoc, CDocument)
	ON_COMMAND(ID_TRAVERSAL, &CMy11121220MTDoc::OnTraversal)
	ON_COMMAND(ID_SAGITAL, &CMy11121220MTDoc::OnSagital)
	ON_COMMAND(ID_CORONAL, &CMy11121220MTDoc::OnCoronal)
	ON_COMMAND(ID_32774, &CMy11121220MTDoc::OnFormerPage)
	ON_COMMAND(ID_32775, &CMy11121220MTDoc::OnNextPage)
	ON_COMMAND(ID_Segment, &CMy11121220MTDoc::OnSegment)
	ON_COMMAND(ID_folder_open, &CMy11121220MTDoc::Onfolderopen)
END_MESSAGE_MAP()


// CMy11121108MTDoc 构造/析构

CMy11121220MTDoc::CMy11121220MTDoc() noexcept
{
	 m_pDib=NULL;
	 m_pBuffer = NULL;
	 filePath = NULL;
}

CMy11121220MTDoc::~CMy11121220MTDoc()
{
	if (m_pDib != NULL) {
		delete m_pDib;
	}
	if (m_pBuffer != NULL) {
		delete m_pBuffer;
	}
}

BOOL CMy11121220MTDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CMy11121108MTDoc 序列化

void CMy11121220MTDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CMy11121108MTDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CMy11121108MTDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CMy11121108MTDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMy11121108MTDoc 诊断

#ifdef _DEBUG
void CMy11121220MTDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMy11121220MTDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMy11121108MTDoc 命令


BOOL CMy11121220MTDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  在此添加您专用的创建代码
	if (m_pDib != NULL)
	{
		delete m_pDib;
		m_pDib = NULL;
	}
	if (m_pBuffer != NULL)
	{
		delete m_pBuffer;
		m_pBuffer = NULL;
	}

	CDlgFileCfg dlg;
	if (dlg.DoModal() != IDOK)
	{
		return FALSE;
	}
	filePath = lpszPathName;
	m_pDib = new CDib;
	m_pDib->LoadFile(lpszPathName,dlg.m_nImgNum,dlg.m_nImgHeight,dlg.m_nImgWidth);
	m_pBuffer = new CDib(*m_pDib);

	OnTraversal();
	UpdateAllViews(NULL);

	return TRUE;
}


void CMy11121220MTDoc::OnTraversal()
{
	if (m_pDib != NULL)
	{
		m_pDib->ShowTransversePlane(m_pDib->m_nCurrentImgNum_Transverse);
		m_current_img_pos = 0;
		UpdateAllViews(NULL);
	}
}


void CMy11121220MTDoc::OnSagital()
{
	if (m_pDib != NULL)
	{
		m_pDib->ShowSagittalPlane(m_pDib->m_nCurrentImgNum_Sagittal);
		m_current_img_pos = 1;
		UpdateAllViews(NULL);
	}
}


void CMy11121220MTDoc::OnCoronal()
{
	if (m_pDib != NULL)
	{
		m_pDib->ShowCoronalPlane(m_pDib->m_nCurrentImgNum_Coronal);
		m_current_img_pos = 2;
		UpdateAllViews(NULL);
	}
}


void CMy11121220MTDoc::OnNextPage()
{
	// TODO: 在此添加命令处理程序代码
	m_pDib->ShowNextPage();
	UpdateAllViews(NULL);
}


void CMy11121220MTDoc::OnFormerPage()
{
	// TODO: 在此添加命令处理程序代码
	m_pDib->ShowFormerPage();
	UpdateAllViews(NULL);
}


void CMy11121220MTDoc::OnSegment()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib == NULL)
	{
		return;
	}
	CDlg_SegmentView dlg(m_pDib);
	dlg.filePath = filePath;
	dlg.DoModal();

}


void CMy11121220MTDoc::Onfolderopen()
{
	CComPtr<IFileOpenDialog> pFileOpen;
	// 创建对话框对象
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
	if (SUCCEEDED(hr))
	{
		// 设置对话框为选择文件夹模式
		DWORD dwOptions;
		pFileOpen->GetOptions(&dwOptions);
		pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS);

		// 显示对话框
		hr = pFileOpen->Show(NULL);
		if (SUCCEEDED(hr))
		{
			// 获取选择的文件夹路径
			CComPtr<IShellItem> pItem;
			hr = pFileOpen->GetResult(&pItem);
			if (SUCCEEDED(hr))
			{
				PWSTR pszFilePath = NULL;
				hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
				if (SUCCEEDED(hr))
				{
					// 将选择的文件夹路径转换为 CString
					CString folderPath(pszFilePath);
					CoTaskMemFree(pszFilePath);

					// 读取文件夹中的 raw 文件
					ReadRawFilesFromFolder(folderPath);
				}
			}
		}
	}

}

void CMy11121220MTDoc::ReadRawFilesFromFolder(const CString& folderPath)
{
	// TODO: 在此处添加实现代码.
	// 清空之前的文件列表
	m_rawFiles.clear();

	// 构建搜索模式
	CString searchPattern = folderPath + _T("\\*.raw");

	CFileFind finder;
	BOOL bWorking = finder.FindFile(searchPattern);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (!finder.IsDots() && !finder.IsDirectory())
		{
			CString filePath = finder.GetFilePath();
			m_rawFiles.push_back(filePath);
		}
	}
	finder.Close();
	CString t1N = _T("20_t1native_155_240_240_16.raw");
	CString t1W = _T("20_t1weighted_155_240_240_16.raw");
	CString t2W = _T("20_t2weighted_155_240_240_16.raw");
	CString t2F = _T("20_t2flair_155_240_240_16.raw");
	// 读取四种协议的 raw 文件
	for (const auto& filePath : m_rawFiles)
	{
		if (filePath.Find(t1N) != -1)
		{
			m_pT1NDib = new CDib;
			m_pT1NDib->LoadFile(filePath, 155, 240, 240);
		}
		else if (filePath.Find(t1W) != -1)
		{
			m_pT1WDib = new CDib;
			m_pT1WDib->LoadFile(filePath, 155, 240, 240);
		}
		else if (filePath.Find(t2W) != -1)
		{
			m_pT2WDib = new CDib;
			m_pT2WDib->LoadFile(filePath, 155, 240, 240);
		}
		else if (filePath.Find(t2F) != -1)
		{
			m_pT2FDib = new CDib;
			m_pT2FDib->LoadFile(filePath, 155, 240, 240);
		}
	}
	FolderSegView dlg(m_pT1NDib, m_pT1WDib, m_pT2WDib, m_pT2FDib);
	dlg.DoModal();

}
