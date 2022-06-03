#pragma once

#include "CViewDevice.h"

#include "ColorStaticST.h"
#include "CMFCButtonEx.h"
#include "CStaticSignalDisplay.h"

// CViewAssign form view
class CViewAssign : public CViewDevice
{
	DECLARE_DYNCREATE(CViewAssign)

protected:
	CViewAssign();           // protected constructor used by dynamic creation
	virtual ~CViewAssign();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEVICE_ASSIGN };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual void OnInitialUpdate();
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnRestoration();
	afx_msg void OnBnClickedBtnSelect1();
	afx_msg void OnBnClickedBtnSelect2();
	afx_msg void OnBnClickedBtnSelect3();
	afx_msg void OnBnClickedBtnSelect4();
	afx_msg void OnBnClickedBtnSelect5();
	afx_msg void OnBnClickedBtnSelect6();
	afx_msg void OnBnClickedBtnSelect7();
	afx_msg void OnBnClickedBtnSelect8();
	afx_msg void OnBnClickedBtnStart();
	
protected:
	typedef enum _TIMER_ID
	{
		TIMER_INITIALIZE = 0,
		TIMER_POLLING,
		TIMER_ENTER_MONITOR_MODE,
		TIMER_LBUTTONDOWN,
		TIMER_SHRINK,
		TIMER_EXPANSION,
		TIMER_REDRAW,
	}	TIMER_ID;

	typedef enum _TIMER_ELPASE
	{
		ELAPSE_TIMER_INITIALIZE = 1,
		ELAPSE_TIMER_POLLING = 100,
		ELPASE_TIMER_ENTER_MONITOR_MODE = 5000,
		ELAPSE_TIMER_LBUTTONDOWN = 1,
	}	TIMER_ELPASE;

protected:
	CColorStaticST m_wndName;

	/// <summary>
	/// 0 ~ 7	= Motor 1 ~ 8
	/// </summary>
	CMFCButtonEx m_btnSelection[8];
	CColorStaticST m_wndStatus[8];

	/// <summary>
	/// 0		= MotionGate
	/// 1 ~ 8	= Motor 1 ~ 8
	/// </summary>	
	CStaticSignalDisplay m_wndSignal[9];

	/// <summary>
	/// 0 ~ 2	= Condition
	/// </summary>
	CColorStaticST m_wndCondition[3];

	CColorStaticST m_wndAutoRestoration;

	INT m_nSelectedMotorIndex;

	BOOL m_bAutoRestoration;

protected:
	void DisplayMotionGateStatus(CDocument* pDoc);
	void DisplayName(CDeviceMotionGate* pDevice);
	void DisplayMotorStatus(CDocument* pDoc);
	void DisplayConditions(CDocument* pDoc);

	void ChangeSelection(const INT nSelectedIndex);
};
