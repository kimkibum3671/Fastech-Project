#pragma once

//#include "CViewDevice.h"
#include "CDlgDevice.h"
#include "ColorStaticST.h"

#include "CStaticSignalDisplay.h"

#include "CDeviceMotionGate.h"

// CDlgHoming form view

class CDlgHoming : public CDlgDevice
{
	DECLARE_DYNCREATE(CDlgHoming)

public:
	CDlgHoming();           // protected constructor used by dynamic creation
	virtual ~CDlgHoming();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HOMING };
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

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnBnClickedBtnAdmin();
	afx_msg void OnBnClickedBtnReset();
	afx_msg void OnBnClickedBtnEstop();
	afx_msg void OnBnClickedBtnStop();

public:
//	virtual void OnInitialUpdate();

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
	CColorStaticST m_wndColor[8][3], m_wndName, m_wndHomingTimer[8];
	INT m_nClickItemID, m_nLastClickItemID;
	DWORD m_dwTimerOn, m_dwTickCount;
	CToolTipCtrl m_tooltip;
	CStaticSignalDisplay m_wndSignal[9];	// 0 = MotionGate, 1 ~ 8 = Motor
	BOOL m_bHoming;

protected:
	const DWORD GetSelectedMotorIndex();

	void DisplayMotionGateStatus(CDeviceMotionGate* pDevice);
	void DisplayMotorStatus(CDeviceMotionGate* pDevice);
	void DisplayName(CDeviceMotionGate* pDevice);

	void EnableControls(CDeviceMotionGate* pDevice);
	void EnableMotorSelection(CDeviceMotionGate* pDevice);
	void EnableReset(CDeviceMotionGate* pDevice);
	void EnableNormalMotionControl(CDeviceMotionGate* pDevice);
	void EnableStopMotionControl(CDeviceMotionGate* pDevice);

	void ShowControls(const BOOL bEnable);
	void CheckHomingResult();
	
	void ShowMotorInformation(const INT nIndex, const INT nCmdShow);

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnSelAll();
	afx_msg void OnBnClickedBtnSelNot();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};

