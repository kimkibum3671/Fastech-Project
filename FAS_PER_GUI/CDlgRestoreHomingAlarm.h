﻿#pragma once
#include "CDeviceMotionGate.h"
#include "CStaticImageViewer.h"
#include "CMFCButtonEx.h"
// CDlgRestoreHomingAlarm 대화 상자

class CDlgRestoreHomingAlarm : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRestoreHomingAlarm)

public:
	//CDlgRestoreHomingAlarm(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	CDlgRestoreHomingAlarm(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgRestoreHomingAlarm();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEVICE_RESTORATION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

public:
	CDeviceMotionGate* m_pDevice;
	CStaticImageViewer m_wndViewer;
	CProgressCtrl m_progress;
	CMFCButtonEx m_btnHome, m_btnExit;
	//CMFCButton* pBtnExit;

	CString m_strStatus;
	BOOL m_bWait;
	BOOL m_bProcess;
	BOOL m_bHomingEnableText;
	enum _RESOTRE_PROCESS
	{
		sts_restore_wait = 0,
		sts_restore_processing,
		sts_restore_success,
		sts_restore_fail,
		sts_homing_wait,
		sts_homing_processing,
		sts_homing_success,
		sts_homing_fail
	};

	enum _PROGRESS_TIMER
	{
		TIMER_RESOTRE_WAIT = 0,
		TIMER_RESOTRE_PROGRESS,
		TIMER_HOMING_WAIT,
		TIMER_HOMING_PROGRESS,
	};

	void SetMotionGate(CDeviceMotionGate* pDevice);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
	void MonitoringRetoreStatus();
	void SetProgressBar(INT nPos, INT nMin, INT nMax);
	void InitButtonImage();
	afx_msg void OnBnClickedMfcbuttonHoming();
	const BOOL IsServoOff();
	const BOOL IsWait();
	const BOOL Waiting();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void InitDeviceImage();
	void SetProcess(INT nProcess);
	const INT GetProcess();
	afx_msg void OnBnClickedBtnHoming();
	afx_msg void OnBnClickedMfcbuttonExist();
	const BOOL AskRestorationStop();
	const BOOL AskHomingAll();
	void MonitoringHomingStatus();
	const BOOL IsHomingReady();
	const BOOL AskHomingStop();
	void StartRestoration();
	const BOOL IsRestorationStart();
	void InitControlTextSize();
	CFont m_BigFont;
};
