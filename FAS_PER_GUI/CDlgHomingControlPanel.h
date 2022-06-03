#pragma once

#include "CDeviceMotionGate.h"
#include "CStaticImageViewer.h"
#include "CMFCButtonEx.h"
#include "ColorStaticST.h"

// CDlgHomingControlPanel 대화 상자

class CDlgHomingControlPanel : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgHomingControlPanel)

public:
	CDlgHomingControlPanel(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgHomingControlPanel();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYSTEM_HOMING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	typedef struct _MOTOR_STATUS
	{
		BOOL bHoming;
		BOOL bHomingFail;
		BOOL bHomingDone;
		BOOL bHomingReady;
		BOOL bHomingStop;
		INT  nStatus;
		INT  nStartWaiting;
	} MOTOR_STATUS;

	MOTOR_STATUS m_stMotor[8];

	enum _EXECUTE_TIMER
	{
		TIMER_HOMING_MOTOR1,
		TIMER_HOMING_MOTOR2,
		TIMER_HOMING_MOTOR3,
		TIMER_HOMING_MOTOR4,
		TIMER_HOMING_MOTOR5,
		TIMER_HOMING_MOTOR6,
		TIMER_HOMING_MOTOR7,
		TIMER_HOMING_MOTOR8,
		TIMER_HOMING_TOTAL,
		TIMER_LOAD_IMAGE,
		TIMER_MONITOR_BUTTON,
		TIMER_ALARM_RST_BUTTON
	} EXECUTE_TIMER;

	enum _HOMING_STATUS
	{
		sts_homing_ready,
		sts_homing_done,
		sts_homing_processing,
		sts_homing_failed
	} HOMING_STATUS;
	
	CDeviceMotionGate* m_pDevice;
	
	CMFCButtonEx m_btnRst, m_btnHoming, m_btnStop, m_btnClose, m_btnAlarmRst;
	CMFCButtonEx m_btnMotor[8];

	INT m_nSelectedNumber;

	CStaticImageViewer m_wndViewer;

protected:
	CColorStaticST m_wndColor[8];
	CColorStaticST m_wndTotalColor;

public:
	void SetMotionGate(CDeviceMotionGate* pDevice);

	void InitButtonImage();
	void InitStaticImage();
	void SetMotorStaticImage(INT nIndex, INT nState);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedMfcbuttonAlarmRst();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void HomingRun(INT nMotorIndex);
//	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void SelectedMotorIndex(INT nSelected);
	const INT GetSelectedMotorIndex();
	afx_msg void OnBnClickedMfcbuttonMotor1();
	afx_msg void OnBnClickedMfcbuttonMotor2();
	afx_msg void OnBnClickedMfcbuttonMotor3();
	afx_msg void OnBnClickedMfcbuttonMotor4();
	afx_msg void OnBnClickedMfcbuttonMotor5();
	afx_msg void OnBnClickedMfcbuttonMotor6();
	afx_msg void OnBnClickedMfcbuttonMotor7();
	afx_msg void OnBnClickedMfcbuttonMotor8();
	afx_msg void OnBnClickedMfcbuttonClose();
	afx_msg void OnBnClickedMfcbuttonAllStart();
	afx_msg void OnBnClickedMfcbuttonStopAll();
	void MonitorBtnStatus();
	void SetStatusBkColor(INT nAxis, INT nStatus);
	void OpenHomingMotion();
	void CloseHomingMotion();
	void MotorAlarmReset();
	void HomingAll();

	const BOOL AskHoming();
	const BOOL AskHomingStop();
	const BOOL AskHomingAllStart();
	const BOOL AskClose();
	void InitControlTextSize();
	CFont m_BigFont;
	void SetHomingAllTime(DWORD dwTime);
	void EraseProgressBarStatus();
	afx_msg void OnBnClickedCheckServo();
	afx_msg void OnBnClickedCheck2();
};
