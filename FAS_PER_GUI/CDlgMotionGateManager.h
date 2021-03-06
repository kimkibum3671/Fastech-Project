#pragma once
#include "CDlgMotionGateInform.h"
#include "CDeviceList.h"

// CDlgMotionGateManager 대화 상자

class CDlgMotionGateManager : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMotionGateManager)

public:
	CDlgMotionGateManager(CWnd* pParent = nullptr, int nMotionCnt = 0);   // 표준 생성자입니다.
	virtual ~CDlgMotionGateManager();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MOTION_MANAGER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	CWnd* m_pParent;

public:

	void SetParent(CWnd* pParent) { m_pParent = pParent; }
	void SetMotionCount(int nCnt) { m_nMotionGateCount = nCnt; }

private:
	CDeviceMotionGate* m_pSelDevice;
	CDeviceMotionGate* m_pPrevSelDevice;
	CToolTipCtrl	m_tooltip;

	unsigned int m_nMotionGateCount;

public:

	struct MGINFORM {
		CString strIP;
		CString strStatus;
		CString Connection;
		CString BackupSchedule;
		CString SystemName;
		COleDateTime BackupDateTime;
		DWORD dwMotorConnectStatus;
		int nMotorCnt;
		int nOPCode;
	};

	struct MOTOR_CHANGE {
		CDeviceMotionGate* pDevice;
		INT   nMotorsAlarmCnt;
		BOOL  bLifeCountAlarm;
		DWORD dwConnectionNum;
		DWORD dwAlarmNum;
		DWORD dwPTNum;
		DWORD dwPTLifeCount;
		WORD wMotorAlarm[9];
	};
	
	enum MOTOR_NUM
	{
		MOTOR0 = 0,
		MOTOR1,
		MOTOR2,
		MOTOR3,
		MOTOR4,
		MOTOR5,
		MOTOR6,
		MOTOR7,
		MOTOR8,
		MOTOR_MAX
	};

	typedef enum _OPCODE_STATUS_MACHINE
	{
		opCode_stop = 0,
		opCode_connect,
		opCode_disconenct,				// Cannot connect to conveter
		opCode_no_response,
		opCode_MotiongateBusy,			//	Teaching Pendant가 연결되었을 때
		opCode_read_motiongate_info,
		opCode_scan_device,
		opCode_read_motiongate_status,
		opCode_read_life_counter,
		opCode_scan_next_device_ok,
		opCode_scan_next_device_ng,
		opCode_standby_motiongate,
		opCode_backup_read_parameter,
		opCode_backup_read_position_table,
		opCode_enter_monitor_mode,
		opCode_exit_monitor_mode,
		opCode_check_monitor_mode_on,
		opCode_check_monitor_mode_off,
		opCode_notify_backup_complete,
		opCode_check_device_name,
		opCode_change_device_info,
		opCode_write_main_parameter,
		opCode_write_position_table,
		opCode_save_main_parameter,
		opCode_save_position_table,
		opCode_confirm_main_parameter,
		opCode_load_position_table,
		opCode_confirm_position_table,
		opCode_change_motiongate_id,
		opCode_reboot_motiongate,
		opCode_homing_servo_on,
		opCode_homing_start,
		opCode_homing_wait,
		opCode_homing_stop,
	}	OPCODE_STATUS_MACHINE;

	// 객체
	CDlgMotionGateInform* m_pMotionGateInform;

	// 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	int GetMotionGateCount();
	bool ReMakeInstance();
	bool ShowMotionInform();
	bool SetMotionStatus();
	bool ShowVertScroll(CDlgMotionGateInform* pInform, int nShow);
//	void DoMakeMotionInform(CDeviceList* pDeviceList, int nCount);
	void InitScrollObj();

	// 변수
	CScrollBar m_VScroll;
	CRect m_rectVScroll;
	CRect m_rectHScroll;
	CRect m_InformDeviceRect;

	unsigned int m_nCurrentCount;
	unsigned int m_nPrevCount;
	unsigned int m_nMotorIndex;


	// 스크롤바
	int m_nColumnCnt;

	bool m_bProgramStart;
	bool m_bChangedWindowRect;
	
	typedef struct _SCREEN_POSITION
	{
		INT cx;
		INT cy;
	}	SCREEN_POSITION;


	SCREEN_POSITION m_ScreenPos;

	int m_nViewWidth; 
	int m_nViewHeight; 
	int m_nVScrolPos; 
	int m_nVPageSize; 
	int m_nHScrolPos; 
	int m_nHPageSize; 

	INT m_nShowVertScroll;
	INT m_nShowHorzScroll;

	bool	m_bBackupToggle;
	
	CString m_strBackupDateTime;
	CString m_strBackupAutoSec;

	BOOL m_bVisibleBackupTime;
	BOOL m_bVisibleBackdupAutoSec;
	BOOL m_bVisibleBtnBackupToggle;
	//BOOL m_bEnableBtnToggle;

	CRect m_rectPrev, m_rectNow;

	bool m_bShow;
	CScrollBar m_HScroll;

	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	MOTOR_CHANGE m_CurMotorStatus, m_PrevMotorStatus;
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void ShowToolTip();
	void ShowMotorStatus(CDeviceMotionGate* pDevice);
	
	void SetBackupDateTime(CString strTime);
	void SetBackupAutoSec(CString strSec);
	void SetBackupToggle(BOOL bEnable);
	void SetVisibleBackupDate(BOOL bBackupAutoSecControl, BOOL bBackupToggle, BOOL bBdakupTime);
	void IncreaseMotorLifeCount(INT nLifeCount);
	void SetShowState(bool bShow);
	void InitPosition(CDlgMotionGateInform* pInform);
	void SetDeviceMotionGate(CDeviceMotionGate* pDevice);
	void SetScrollBar(CDlgMotionGateInform* pInform);
	
	CString GetBackupDateTime();
	CString GetBackupAutoSec();
	BOOL GetBackupToggle();
	
	
	
	
	const BOOL IsShowState();
	const BOOL IsStatusChanged();

	bool ShowHorzScroll(CDlgMotionGateInform* pInform, int nShow);
	
	
};
