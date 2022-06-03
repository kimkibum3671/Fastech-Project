#pragma once


// CMFCCaptionBarEx

class CMFCCaptionBarEx : public CMFCCaptionBar
{
	DECLARE_DYNAMIC(CMFCCaptionBarEx)

public:
	CMFCCaptionBarEx();
	virtual ~CMFCCaptionBarEx();

protected:
	DECLARE_MESSAGE_MAP()

	virtual void RecalcLayout();
	
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);

protected:
	ULONGLONG m_tickCount;
	ULONGLONG m_timeAutoClose;

protected:
	typedef enum _TIMER_ID
	{
		TIMER_POLLING,
	}	TIMER_ID;

	typedef enum _TIMER_ELPASE
	{
		ELAPSE_TIMER_POLLING = 100,
	}	TIMER_ELPASE;

public:
	BOOL CreateEx(DWORD dwStyle, CWnd* pParentWnd, UINT uID, int nHeight, BOOL bIsMessageBarMode);

	void SetIcon(LPCTSTR lpIconName, COLORREF clrTransparent, BOOL bStretch = FALSE, BarElementAlignment bmpAlignment = ALIGN_RIGHT);
	void SetIcon(UINT uiBmpResID, COLORREF clrTransparent, BOOL bStretch = FALSE, BarElementAlignment bmpAlignment = ALIGN_RIGHT);

	void EnableAutoClose(const BOOL bEnable = TRUE, const INT nSecond = 30000);	
};
