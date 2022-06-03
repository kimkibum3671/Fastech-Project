#pragma once
#include "CMFCListCtrlEX.h"
#include "CDeviceMotionGate.h"

// CDlgSystemBackup 대화 상자

class CDlgSystemBackup : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSystemBackup)

public:
	CDlgSystemBackup(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgSystemBackup();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYSTEM_BACKUP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	typedef struct _BACKUP_LIST
	{
		CDeviceMotionGate* pDevice;
		CString strName;
		BOOL bBackup;
		BOOL bCheck;
		BOOL bBackupStart;
	 }BACKUP_LIST;
	
	CList<BACKUP_LIST, BACKUP_LIST&> m_list;

	typedef enum __STATE_MACHINE
	{
		state_stop = 0,
		state_connect,
		state_reboot,
		state_motion_gate_get_system_status_all,
		state_motion_gate_command,
		state_backup,
		state_restoration,
		state_factory_reset,
		state_assign_motor_id,
		state_home_all,
		state_change_information,
	}	STATE_MACHINE;

	typedef enum _BACKUP_PROCESS
	{
		process_Initate = 0,
		process_searching,
		process_checking,
		process_backup,
		process_done

	} BACKUP_PROCESS;

	INT m_process;
	BOOL m_bDoNext;
	BOOL m_bIsRun;
	CMFCListCtrlEx m_listCtrlDisable;
	CMFCListCtrlEx m_listCtrl;
	CFont m_BigFont;
	BACKUP_LIST m_device;
	virtual BOOL OnInitDialog();
	void InitalizeListControl();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonBackup();
	afx_msg void OnBnClickedOk();
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CProgressCtrl m_progress;
	void EnableControlWindow(BOOL bEnable);
	afx_msg void OnBnClickedCheckAll();
	const BOOL IsEnableBackupScheduleTime(CDeviceMotionGate* pDevice);
	CEdit m_EditTimer;
	CSpinButtonCtrl m_spTimer;
	afx_msg void OnDeltaposSpinTimer(NMHDR* pNMHDR, LRESULT* pResult);
	void AddFailDevice(BACKUP_LIST device);
	afx_msg void OnBnClickedButtonListSelectALL();
	afx_msg void OnLvnItemchangedListBackupEnable(NMHDR* pNMHDR, LRESULT* pResult);
	CEdit m_editComment;
	void ExecuteBackup();
	void InitControlTextSize();
};
