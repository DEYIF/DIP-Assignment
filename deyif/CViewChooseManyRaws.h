#pragma once



// CViewChooseManyRaws 窗体视图

class CViewChooseManyRaws : public CFormView
{
	DECLARE_DYNCREATE(CViewChooseManyRaws)

protected:
	CViewChooseManyRaws();           // 动态创建所使用的受保护的构造函数
	virtual ~CViewChooseManyRaws();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SliderManyRaws };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_progressBar;
	CSliderCtrl m_slider;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnInitialUpdate();
private:
	static const UINT_PTR TIMER_ID = 98767; // 定义定时器的标识符
};


