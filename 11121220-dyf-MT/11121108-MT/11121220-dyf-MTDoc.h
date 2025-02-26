
// 11121220-dyf-MTDoc.h: CMy11121108MTDoc 类的接口
//
#pragma once
#include<vector>

using namespace std;
class CDib;


class CMy11121220MTDoc : public CDocument
{
protected: // 仅从序列化创建
	CMy11121220MTDoc() noexcept;
	DECLARE_DYNCREATE(CMy11121220MTDoc)

// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CMy11121220MTDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

public:
	vector<CString> m_rawFiles;
	LPCTSTR filePath;
	CDib *m_pDib;
	CDib *m_pBuffer;
	CDib *m_pT1NDib;
	CDib *m_pT1WDib;
	CDib *m_pT2WDib;
	CDib *m_pT2FDib;
	int m_current_img_pos;//0横截面1矢状2冠状
	afx_msg void OnTraversal();
	afx_msg void OnSagital();
	afx_msg void OnCoronal();
	afx_msg void OnNextPage();
	afx_msg void OnFormerPage();
	afx_msg void OnSegment();
	afx_msg void Onfolderopen();
	void ReadRawFilesFromFolder(const CString& folderPath);
};
