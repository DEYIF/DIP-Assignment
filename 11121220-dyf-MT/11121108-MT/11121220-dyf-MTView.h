
// 11121220-dyf-MTView.h: CMy11121108MTView 类的接口
//

#pragma once


class CMy11121108MTView : public CView
{
protected: // 仅从序列化创建
	CMy11121108MTView() noexcept;
	DECLARE_DYNCREATE(CMy11121108MTView)

// 特性
public:
	CMy11121220MTDoc* GetDocument() const;

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
	virtual ~CMy11121108MTView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // 11121220-dyf-MTView.cpp 中的调试版本
inline CMy11121108MTDoc* CMy11121108MTView::GetDocument() const
   { return reinterpret_cast<CMy11121108MTDoc*>(m_pDocument); }
#endif

