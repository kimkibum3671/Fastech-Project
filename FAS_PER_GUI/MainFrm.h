// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를 
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된 
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해 
// 추가적으로 제공되는 내용입니다.  
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.  
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은 
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.h: CMainFrame 클래스의 인터페이스
//

#pragma once

#include "CMFCRibbonBarEx.h"
#include "CMFCCaptionBarEx.h"
#include "CPaneDeviceInfo.h"
#include "CPaneOperationLOG.h"

//#include "CDlgNamingAssistant.h"
#include "CDlgNoticeSystem.h"
#include "CDlgSystemHistory.h"

#include "COutlookDevice.h"
#include "CDlgMotionGateManager.h"


#include "ReferD.h"



class COutlookBar : public CMFCOutlookBar
{
	virtual BOOL AllowShowOnPaneMenu() const;
	virtual void GetPaneName(CString& strName) const;
};

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNCREATE(CMainFrame)

public:
	CMainFrame() noexcept;

	// 특성입니다.
public:

	// 작업입니다.
public:

	// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CMFCRibbonBarEx		m_wndRibbonBar;
	//CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages	m_PanelImages;
	CMFCRibbonStatusBar	m_wndStatusBar;
//	CPaneDeviceInfo		m_paneDeviceInfo;
	CPaneOperationLog	m_paneOperationLog;
	CMFCCaptionBarEx	m_wndCaptionBar;

	COutlookDevice		m_wndOutlookDevice;
	
	// KKB
	CDlgSystemHistory	m_wndSystemHistory;
	CDlgMotionGateManager* m_pWndMotionManager;
	// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnTrayNotifycation(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();

	afx_msg void OnBnClickedBtnMotionTest();
	afx_msg void OnBnClickedPanelDeviceList();

	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

	afx_msg void OnUpdateOperator(CCmdUI* pCmdUI);
	
	afx_msg void OnBnClickedPanelConfig();
	afx_msg void OnBnClickedPanelUser();
	afx_msg void OnUpdatePanelUser(CCmdUI* pCmdUI);
	
	afx_msg void OnUpdateBtnMotionTest(CCmdUI* pCmdUI);
	
	afx_msg void OnBnClickedPanelRun();
	afx_msg void OnUpdatePanelRun(CCmdUI* pCmdUI);
	
	afx_msg void OnBnClickedPanelStop();
	afx_msg void OnUpdatePanelStop(CCmdUI* pCmdUI);

	afx_msg void OnCmdSystemReboot();

	afx_msg void OnBnClickedAlarmReset();
	afx_msg void OnUpdateAlarmReset(CCmdUI* pCmdUI);

	afx_msg void OnBnClickedBtnToggleBackup();
	afx_msg void OnUpdateBtnToggleBackup(CCmdUI* pCmdUI);

	afx_msg void OnBnClickedBtnStopExecution();
	afx_msg void OnUpdateBtnStopExecution(CCmdUI* pCmdUI);

	afx_msg void OnBnClickedBtnDeviceBackup();
	afx_msg void OnBnClickedBtnDeviceBackupData();
	afx_msg void OnUpdateBtnDeviceBackup(CCmdUI* pCmdUI);

	afx_msg void OnBnClickedBtnDeviceSelectRestoration();
	afx_msg void OnUpdateBtnDeviceSelectRestoration(CCmdUI* pCmdUI);

	afx_msg void OnBnClickedBtnDeviceRestoration();
	afx_msg void OnUpdateBtnDeviceRestoration(CCmdUI* pCmdUI);

	afx_msg void OnBnClickedBtnDeviceIdAssignment();
	afx_msg void OnUpdateBtnDeviceIdAssignment(CCmdUI* pCmdUI);

	afx_msg void OnBnClickedBtnDeviceFactoryReset();
	afx_msg void OnUpdateBtnDeviceFactoryReset(CCmdUI* pCmdUI);

	afx_msg void OnBnClickedBtnHoming();
	afx_msg void OnUpdateBtnHoming(CCmdUI* pCmdUI);
	
//	afx_msg void OnBnClickedBtnDeviceConfig();
	afx_msg void OnUpdateViewBtnDeviceConfig(CCmdUI* pCmdUI);

	afx_msg void OnBnClickedBtnDeviceStatus();
	afx_msg void OnUpdateBtnDeviceStatus(CCmdUI* pCmdUI);
	
	afx_msg void OnBnClickedBtnOperationLog();
	afx_msg void OnUpdateBtnOperationLog(CCmdUI* pCmdUI);

	afx_msg void OnBnClickedBtnBackupSchedule();
	afx_msg void OnUpdateBtnDeviceBackupSchedule(CCmdUI* pCmdUI);

	afx_msg void OnUpdateLabelDeviceName(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLabelDeviceStatus(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProgressExecution(CCmdUI* pCmdUI);

	afx_msg void OnBnClickedViewCaptionBar();
	afx_msg void OnUpdateViewCaptionBar(CCmdUI* pCmdUI);

	afx_msg void OnUpdateViewAutoDatetime(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewAutoSeconds(CCmdUI* pCmdUI);

	afx_msg void OnUpdateBtnSystemBackupManage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtnSystemCompare(CCmdUI* pCmdUI);

	////////////////////////////////////////
	//	Menu - System
	afx_msg void OnBnClickedMenuOperationRun();
	afx_msg void OnBnClickedMenuOperationStop();
	afx_msg void OnBnClickedMenuSchedule();
	afx_msg void OnBnClickedMenuConfiguration();

	////////////////////////////////////////	
	//	Menu - Device
	afx_msg void OnBnClickedMenuDeviceStop();
	afx_msg void OnBnClickedMenuDeviceBackup();
	afx_msg void OnBnClickedMenuDeviceRestore();
	afx_msg void OnBnClickedMenuDeviceAssignment();
	afx_msg void OnBnClickedMenuFactoryReset();
	afx_msg void OnBnClickedMenuDeviceOperation();

	afx_msg void OnBnClickedBtnSystemOption();
	afx_msg void OnUpdateBtnSystemOption(CCmdUI* pCmdUI);
		// IDP_BTN_SELECT

	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	typedef enum _TIMER_ID
	{
		TIMER_INITIALIZE = 0,
		TIMER_DEVICE_AUTOHOME,
		TIMER_DEVICE_CHECK,
	}	TIMER_ID;

	typedef enum _TIMER_ELPASE
	{
		ELAPSE_TIMER_INITIALIZE = 100,
		ELAPSE_TIMER_POLLING = 100,
	}	TIMER_ELPASE;

protected:
	const BOOL	CreateDockingWindows();
	const BOOL	CreateCaptionBar();
	const BOOL	CreateStatusBar();
	void		InitializeGlobalForm();

	void DisplayOperatorInformation();
	void DisplayLogoutInformation();	

public:
	const DWORD GetSelectedDeviceAddress();
	CDeviceMotionGate* GetSelectedMotiongateCtrl();

public:
	typedef enum _USER_MESSAGE
	{
		UM_OPERATION_LOG = WM_USER + 0,
		UM_SHOW_DEIVCE_CONFIG,
		UM_SHOW_HOMING_VIEW,
		UM_SHOW_AUTO_RESTORATION_VIEW,
		UM_SHOW_ASSIGNMENT_VIEW,
		UM_APPLY_ALIAS,
		UM_SEND_LOGMESSAGE_FOR_OPERATION_LOG, // KKB
//		UM_NOTICE_FACTORY_RESET_OK,
//		UM_NOTICE_ASSIGN_OK,
//		UM_NOTICE_CAPTION_BAR,
	}	USER_MESSAGE;

public:
	typedef struct _ST_OPERAION_LOG_INFO
	{
		CDeviceMotionGate* pDevice;
		INT					nLevel;
		INT					nMsgID;
		BOOL				bEnableLog;
	}	ST_OPERAION_MESSAGE, * PST_OPERAION_MESSAGE;

protected:
	INT m_nLogLevel;
	// KKB
	INT m_nMotionGateCnt;
	CDeviceMotionGate* m_pDevice;

protected:
	void UpdateStatusBar(PST_OPERAION_MESSAGE pMsg);
	void UpdateCaptionBar(PST_OPERAION_MESSAGE pMsg);

protected:
	LRESULT OnMsgOperationLog(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgShowHomingView(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgShowAutoRestorationView(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgShowAssignView(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgShowDeviceConfig(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgShowApplyAlias(WPARAM wParam, LPARAM lParam);

//	LRESULT OnMsgNoticeCaptionBar(WPARAM wParam, LPARAM lParam);

public:
	void DestroyDeviceWindow(CFormView* pException = NULL);
	void ShowHomingWindow(const DWORD dwAddress);

protected:
//	CDlgNamingAssistant m_dlgAssistant;
	CDlgNoticeSystem m_dlgNewSystem;

protected:
	const BOOL	RequestElevatedPermissions(LPCTSTR lpszCaption = NULL);
	void		ShowRequireElevatedPermissions(LPCTSTR lpszCaption = NULL);

	const BOOL	DoLoginSequence(const BOOL bEnableReLogin = FALSE);

protected:
	COutlookBar	m_wndNavigationBar;

protected:
	BOOL CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, int nInitialWidth = 250);

protected:
	const INT ConfirmModifyDatabase();
	const INT ConfirmReboot();

public:
	const BOOL CheckControlGranted();

protected:
	typedef enum _MAIN_PANEL_IMAGE_INDEX
	{
		idx_log_in = 0,
		idx_log_out,
		idx_system_run,
		idx_system_stop,
		idx_system_config_all,
		idx_system_config_one,
		idx_toggle_off,
		idx_toggle_on,
		idx_backup,
		idx_restoration,
		idx_pack,			// Factory reset
		idx_unpack,			// ID Assignment
		idx_schedule,
		idx_motion,
		idx_home
	}	MAIN_PANEL_IMAGE_INDEX;

	const BOOL ChangePanelButtonImage(const INT nPanelID, const INT nIndex, const BOOL bLargeImage = TRUE);

protected:
	const BOOL EditSingleSystemConfiguration(const DWORD dwAddress);
public:
	const BOOL CreateModBusServer();

	afx_msg void OnBtnDeviceRestorationSelect();
	afx_msg void OnUpdateBtnDeviceRestorationSelect(CCmdUI* pCmdUI);
	afx_msg void OnBnClickedBtnDeviceReboot();
	afx_msg void OnUpdateBtnDeviceReboot(CCmdUI* pCmdUI);
	afx_msg void OnBnClickedBtnMGDeviceFactoryReset();
	afx_msg void OnUpdateBtnMGDeviceFactoryReset(CCmdUI* pCmdUI);
	const BOOL DoMGFactoryReset(const DWORD dwAddress, const BOOL bAct);
	void SetSelectedAddress(DWORD dwAddress);
	DWORD GetSelectedAddress();

	public:
	DWORD	m_dwSelectedAddress;
	afx_msg	void OnBnClickedInformBackup();
	afx_msg void OnBnClickedInformRestore();
	afx_msg void OnBnClickedInformComapre();
	afx_msg void OnBnClickedInformAssign();
	afx_msg void OnBnClickedInformReboot();
	afx_msg void OnBnClickedInformFactoryReset();
	afx_msg void OnBnClickedInformMotorReset();
	afx_msg void OnBnClickedInformBackupSchedule();
	afx_msg void OnBnClickedInformConfig();
	void OnBnClickedInformBackupManage();
	void OnBnClickedInformSystemRun();
	
	// Operation Log Tool Bar
	afx_msg	void OnUpdateToolBar(CCmdUI* pCmdUI);
	afx_msg	void OnUpdateOperationToolBar(CCmdUI* pCmdUI);
	afx_msg	void OnBnClickedOperationAll();
	afx_msg	void OnBnClickedOperationWarning();
	afx_msg	void OnBnClickedOperationError();
	afx_msg	void OnBnClickedOperationFilter();
	afx_msg	void OnBnClickedOperationFocusName();
	afx_msg	void OnBnClickedOperationFocusAddress();

	// Status Bar
	afx_msg	void OnBnClickedStatusCheckName();
	afx_msg	void OnBnClickedStatusAddress();
	const BOOL SetOperationLogAll();
	const BOOL GetLogInformationAll(LPVOID pDatabaseRecordData, INT& nRowid, CString& strInform, CString& strIP, CString& strID, CString& strTime);
	LRESULT OnMsgDetailOperationLog(WPARAM wParam, LPARAM lParam);
	
	typedef enum _INTERNAL_MESSAGE_LEVEL
	{
		msgInternal = 0,
		msgError,
		msgWarning,
		msgNormal,
		msgSequence,
		msgAll,
		msgNone,
	}	INTERNAL_MESSAGE_LEVEL;

	typedef enum _STATUS_LOG_LEVEL
	{
		STS_LOG_NORMAL = 0,
		STS_LOG_WARNING,
		STS_LOG_ERROR,
		STS_LOG_MAX
	}	STATUS_LOG_LEVEL;
	
	typedef struct _STATUS_LOG
	{
		BOOL bLogAll;
		BOOL bWarn;
		BOOL bError;
		BOOL bSelect;
		BOOL bName;
		BOOL bIP;
		BOOL bMix;
		BOOL bTreeSelect;
	} STATUS_LOG;

	STATUS_LOG m_LogCheck;

	BOOL m_bMade;
	BOOL m_bRun;
	INT m_logStatus;
	DWORD m_pDeviceAddress;
	
	bool m_bShowToggleBtn;
	bool m_bEnableToggleButton;
	bool m_bEnableTextAutoSec;
	bool m_bEnbaleTextTime;
	bool m_bSystemCompare;
	bool m_bSystemErrorDetected;
	CString m_strCompareSystemName;

	void OnTreeClickedOperationLog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void OnUpdateOutlookToolBar(CCmdUI* pCmdUI);
	void OnBnClickedOperationFocusMix();
	void UpdateOperationLog(PST_OPERAION_MESSAGE pMsg, CString strMessage);
	void OnUpdateBackupSchedule(CCmdUI* pCmdUI);
	afx_msg void OnBtnSystemBackupManage();

	afx_msg void OnBtnSystemCompare();
	void SetSystemCompare(BOOL bCtrl);
	const BOOL IsSystemCompare();
	void SetSystemCompareProjectName(CString strName);
	CString GetSystemCompareProjectName();
	BOOL IsNominalIDExist(CString strName);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	void SaveLogData();
	afx_msg void OnClose();
	afx_msg void OnBtnSystemBackup();
	afx_msg void OnUpdateBtnSystemBackup(CCmdUI* pCmdUI);
	void TestLogSpeed();
	const BOOL ProjectDBVacuum();
	const BOOL ConfigDBVacuum();
	const BOOL LogDBVacuum();
	const BOOL NoticeSingleSystemConfiguration(const DWORD dwAddress);
	const BOOL NoticeSingleSystemCompareResult(const DWORD dwAddress);
	afx_msg void OnBnClickedStatusIPSearching();
	void OnTreeLoadDevice();
	
	afx_msg void OnBtnSystemHistory();
	afx_msg void OnUpdateBtnSystemHistory(CCmdUI* pCmdUI);
//	afx_msg void OnBtnSysHistory();
	const BOOL RequestAdministratorPermissions(LPCTSTR lpszCaption);
	void CheckDeviceMotionGate();
	void OnOutlookTreeClicked();
	const BOOL GetOutlookVisible();
	CDeviceMotionGate* GetSelectedMotionGateControl();
	void OnUpdateSelcetedDeviceBackupSchedule();
	void NoticeNoChangeValue();
};
