#pragma once

//#include "CViewDevice.h"
#include "CDlgDevice.h"

#include "CStaticSignalDisplay.h"

#include "ColorStaticST.h"

#include "CDeviceMotionGate.h"

// CDlgMotionTest form view

class CDlgMotionTest : public CDlgDevice
{
	DECLARE_DYNCREATE(CDlgMotionTest)

public:
	CDlgMotionTest();           // protected constructor used by dynamic creation
	virtual ~CDlgMotionTest();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MOTION_TEST };
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
	afx_msg void OnBnClickedBtnStopAll();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

protected:
	CStaticSignalDisplay m_wndSignal[9];	// 0 = MotionGate, 1 ~ 8 = Motor

public:
	//	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

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
	CColorStaticST m_wndColor[8][3], m_wndName;
	INT m_nClickItemID, m_nLastClickItemID;
	DWORD m_dwTimerOn, m_dwTickCount;
	CToolTipCtrl m_tooltip;

protected:
	const DWORD GetSelectedMotorIndex();

	void DisplayMotionGateStatus(CDeviceMotionGate* pDevice);
	void DisplayMotorStatus(CDeviceMotionGate* pDevice);
	void DisplayName(CDeviceMotionGate* pDevice);

	void EnableControls(CDeviceMotionGate* pDevice);
	void EnableMotorSelection(CDeviceMotionGate* pDevice);
	void EnableServoOn(CDeviceMotionGate* pDevice);
	void EnableJogMotionControl(CDeviceMotionGate* pDevice);
	void EnableReset(CDeviceMotionGate* pDevice);
	void EnableNormalMotionControl(CDeviceMotionGate* pDevice);
	void EnableStopMotionControl(CDeviceMotionGate* pDevice);

	void ShowControls(const BOOL bEnable);

	void ShowMotorInformation(const INT nIndex, const INT nCmdShow);

public:
	afx_msg void OnBnClickedBtnServoOn();
	afx_msg void OnBnClickedBtnServoOff();
	afx_msg void OnBnClickedBtnSelAll();
	afx_msg void OnBnClickedBtnSelNot();
	afx_msg void OnBnClickedBtnAdmin();
	afx_msg void OnBnClickedBtnEstop();
	afx_msg void OnBnClickedBtnStop();
	//	afx_msg void OnBnClickedBtnName();
	afx_msg void OnBnClickedBtnReset();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
