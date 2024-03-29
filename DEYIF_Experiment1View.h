
// DEYIF_Experiment1View.h: CDEYIFExperiment1View 类的接口
//

#pragma once


class CDEYIFExperiment1View : public CView
{
protected: // 仅从序列化创建
	CDEYIFExperiment1View() noexcept;
	DECLARE_DYNCREATE(CDEYIFExperiment1View)

// 特性
public:
	CDEYIFExperiment1Doc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CDEYIFExperiment1View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	bool DrawRawPic(CDib* dib, BITMAPINFO bitmapInfo);
};

#ifndef _DEBUG  // DEYIF_Experiment1View.cpp 中的调试版本
inline CDEYIFExperiment1Doc* CDEYIFExperiment1View::GetDocument() const
   { return reinterpret_cast<CDEYIFExperiment1Doc*>(m_pDocument); }
#endif

