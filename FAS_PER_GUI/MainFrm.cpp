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

// MainFrm.cpp: CMainFrame 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "FAS_PER_GUI.h"

#include "MainFrm.h"

#include "CDocDevice.h"

#include "CDlgMotionTest.h"
#include "CViewBackup.h"
#include "CViewRestoration.h"
#include "CViewAssign.h"
#include "CViewFactoryReset.h"
#include "CDlgHoming.h"
#include "CViewDeviceConfig.h"
#include "CDlgSchedule.h"

#include "CMultiDocTemplateEx.h" 

#include "CStaticSignalDisplayGlobalForm.h"

#include "CDlgDeviceList.h"

#include "resource.h"

#include "CDataBaseManager.h"

#include "Define_Message.h"

#include "CSheetLogin.h"
#include "CSheetBackup.h"
#include "CSheetRestoration.h"
#include "CSheetSelectRestoration.h"
#include "CSheetFactoryReset.h"
#include "CSheetAssignID.h"
#include "CSheetConfig.h"				//	Total Config
#include "CSheetConfigDeviceSingle.h"	//	Single Config
#include "CDlgDeviceTotalConfiguration.h" // Total Configuration

#include "CDlgManageBackup.h"

#include "CInterfaceManager.h"

#include "CDlgSystemOption.h"

#include "CModbusServer.h"

#include "CDlgSystemManage.h"

#include "CDlgSaveLogData.h"

#include "CDlgSystemBackup.h"

#include "CDlgRestoreHomingAlarm.h"

#include "CDlgHomingControlPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



BOOL COutlookBar::AllowShowOnPaneMenu() const
{
	return TRUE;
}

void COutlookBar::GetPaneName(CString& strName) const
{
	BOOL bNameValid = strName.LoadString(IDS_OUTLOOKBAR);
	ASSERT(bNameValid);
	if (!bNameValid)
	{
		strName.Empty();
	}
}

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_DESTROY()

	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)

	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_MESSAGE(WM_TRAY_NOTIFYCATION, &CMainFrame::OnTrayNotifycation)

	ON_UPDATE_COMMAND_UI(IDC_ST_OPERATOR, &CMainFrame::OnUpdateOperator)

	ON_BN_CLICKED(ID_PANEL_CONFIG, &CMainFrame::OnBnClickedPanelConfig)

	ON_BN_CLICKED(ID_PANEL_DEVICE_LIST, &CMainFrame::OnBnClickedPanelDeviceList)

ON_BN_CLICKED(IDP_BTN_MOTION_TEST, &CMainFrame::OnBnClickedBtnMotionTest)
ON_UPDATE_COMMAND_UI(IDP_BTN_MOTION_TEST, &CMainFrame::OnUpdateBtnMotionTest)

ON_BN_CLICKED(ID_PANEL_RUN, &CMainFrame::OnBnClickedPanelRun)
ON_UPDATE_COMMAND_UI(ID_PANEL_RUN, &CMainFrame::OnUpdatePanelRun)

ON_BN_CLICKED(ID_PANEL_STOP, &CMainFrame::OnBnClickedPanelStop)
ON_UPDATE_COMMAND_UI(ID_PANEL_STOP, &CMainFrame::OnUpdatePanelStop)

ON_COMMAND(ID_OPERATION_REBOOT, &CMainFrame::OnCmdSystemReboot)

ON_BN_CLICKED(ID_PANEL_USER, &CMainFrame::OnBnClickedPanelUser)
ON_UPDATE_COMMAND_UI(ID_PANEL_USER, &CMainFrame::OnUpdatePanelUser)

ON_BN_CLICKED(IDP_BTN_TOGGLE_BACKUP, &CMainFrame::OnBnClickedBtnToggleBackup)
ON_UPDATE_COMMAND_UI(IDP_BTN_TOGGLE_BACKUP, &CMainFrame::OnUpdateBtnToggleBackup)

ON_BN_CLICKED(IDP_BTN_STOP_EXECUTION, &CMainFrame::OnBnClickedBtnStopExecution)
ON_UPDATE_COMMAND_UI(IDP_BTN_STOP_EXECUTION, &CMainFrame::OnUpdateBtnStopExecution)

ON_BN_CLICKED(IDP_BTN_DEVICE_BACKUP, &CMainFrame::OnBnClickedBtnDeviceBackup)
ON_BN_CLICKED(IDP_BTN_DEVICE_BACKUP_DATA, &CMainFrame::OnBnClickedBtnDeviceBackupData)
ON_UPDATE_COMMAND_UI(IDP_BTN_DEVICE_BACKUP, &CMainFrame::OnUpdateBtnDeviceBackup)

//ON_COMMAND(IDP_BTN_DEVICE_SELECT_RESTORATION, &CMainFrame::OnBnClickedBtnDeviceSelectRestoration)
ON_BN_CLICKED(IDP_BTN_DEVICE_SELECT_RESTORATION, &CMainFrame::OnBnClickedBtnDeviceSelectRestoration)
ON_UPDATE_COMMAND_UI(IDP_BTN_DEVICE_SELECT_RESTORATION, &CMainFrame::OnUpdateBtnDeviceSelectRestoration)

ON_COMMAND(IDP_BTN_DEVICE_RESTORATION_SELECT, &CMainFrame::OnBtnDeviceRestorationSelect) // Selected!!
ON_UPDATE_COMMAND_UI(IDP_BTN_DEVICE_RESTORATION_SELECT, &CMainFrame::OnUpdateBtnDeviceRestorationSelect)

ON_BN_CLICKED(IDP_BTN_DEVICE_RESTORATION, &CMainFrame::OnBnClickedBtnDeviceRestoration) // Normal !!
ON_UPDATE_COMMAND_UI(IDP_BTN_DEVICE_RESTORATION, &CMainFrame::OnUpdateBtnDeviceRestoration)

ON_BN_CLICKED(IDP_BTN_DEVICE_ID_ASSIGNMENT, &CMainFrame::OnBnClickedBtnDeviceIdAssignment)
ON_UPDATE_COMMAND_UI(IDP_BTN_DEVICE_ID_ASSIGNMENT, &CMainFrame::OnUpdateBtnDeviceIdAssignment)

ON_BN_CLICKED(IDP_BTN_DEVICE_FACTORY_RESET, &CMainFrame::OnBnClickedBtnDeviceFactoryReset)
ON_UPDATE_COMMAND_UI(IDP_BTN_DEVICE_FACTORY_RESET, &CMainFrame::OnUpdateBtnDeviceFactoryReset)

ON_COMMAND(IDP_BTN_TOOL_FACTORY_RESET, &CMainFrame::OnBnClickedBtnMGDeviceFactoryReset)
ON_UPDATE_COMMAND_UI(IDP_BTN_TOOL_FACTORY_RESET, &CMainFrame::OnUpdateBtnMGDeviceFactoryReset)

ON_BN_CLICKED(IDP_BTN_HOMING, &CMainFrame::OnBnClickedBtnHoming)
ON_UPDATE_COMMAND_UI(IDP_BTN_HOMING, &CMainFrame::OnUpdateBtnHoming)

ON_UPDATE_COMMAND_UI(IDP_BTN_DEVICE_FACTORY_RESET, &CMainFrame::OnUpdateBtnDeviceFactoryReset)

ON_BN_CLICKED(IDP_DEVICE_STATUS, &CMainFrame::OnBnClickedBtnDeviceStatus)
ON_UPDATE_COMMAND_UI(IDP_DEVICE_STATUS, &CMainFrame::OnUpdateBtnDeviceStatus)

ON_BN_CLICKED(IDP_OPERATION_LOG, &CMainFrame::OnBnClickedBtnOperationLog)
ON_UPDATE_COMMAND_UI(IDP_OPERATION_LOG, &CMainFrame::OnUpdateBtnOperationLog)

ON_BN_CLICKED(IDP_BTN_DEVICE_BACKUP_SCHEDULE, &CMainFrame::OnBnClickedBtnBackupSchedule)
ON_UPDATE_COMMAND_UI(IDP_BTN_DEVICE_BACKUP_SCHEDULE, &CMainFrame::OnUpdateBtnDeviceBackupSchedule)

ON_UPDATE_COMMAND_UI(IDC_STATIC_BACKUP_TOGGLE, &CMainFrame::OnUpdateBackupSchedule)
ON_UPDATE_COMMAND_UI(IDC_ST_DEVICE_NAME, &CMainFrame::OnUpdateLabelDeviceName)
ON_UPDATE_COMMAND_UI(IDC_ST_DEVICE_STATUS, &CMainFrame::OnUpdateLabelDeviceStatus)
ON_UPDATE_COMMAND_UI(IDP_PROGRESS_EXECUTION, &CMainFrame::OnUpdateProgressExecution)
ON_UPDATE_COMMAND_UI(IDR_PANE_LOG, &CMainFrame::OnUpdateToolBar)

//	ON_COMMAND(IDP_BTN_DEVICE_CONFIG, &CMainFrame::OnBnClickedBtnDeviceConfig)
//	ON_UPDATE_COMMAND_UI(IDP_BTN_DEVICE_CONFIG, &CMainFrame::OnUpdateViewBtnDeviceConfig)

ON_UPDATE_COMMAND_UI(ID_DEVICE_CONFIG, &CMainFrame::OnUpdateViewBtnDeviceConfig)

ON_COMMAND(ID_VIEW_CAPTION_BAR, &CMainFrame::OnBnClickedViewCaptionBar)
ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CMainFrame::OnUpdateViewCaptionBar)

ON_UPDATE_COMMAND_UI(IDP_EDIT_AUTO_DATETIME, &CMainFrame::OnUpdateViewAutoDatetime)
ON_UPDATE_COMMAND_UI(IDC_ST_AUTO_SECOND, &CMainFrame::OnUpdateViewAutoSeconds)

ON_MESSAGE(UM_OPERATION_LOG, &CMainFrame::OnMsgOperationLog)
ON_MESSAGE(UM_SHOW_HOMING_VIEW, &CMainFrame::OnMsgShowHomingView)
ON_MESSAGE(UM_SHOW_AUTO_RESTORATION_VIEW, &CMainFrame::OnMsgShowAutoRestorationView)
ON_MESSAGE(UM_SHOW_ASSIGNMENT_VIEW, &CMainFrame::OnMsgShowAssignView)
ON_MESSAGE(UM_SHOW_DEIVCE_CONFIG, &CMainFrame::OnMsgShowDeviceConfig)
ON_MESSAGE(UM_APPLY_ALIAS, &CMainFrame::OnMsgShowApplyAlias)
ON_MESSAGE(UM_SEND_LOGMESSAGE_FOR_OPERATION_LOG, &CMainFrame::OnMsgDetailOperationLog)
/*ON_MESSAGE(UM_NOTICE_CAPTION_BAR, &CMainFrame::OnMsgNoticeCaptionBar)*/
//	ON_MESSAGE(UM_NOTICE_FACTORY_RESET_OK, &CMainFrame::OnMsgFactoryResetOK)
//	ON_MESSAGE(UM_NOTICE_ASSIGN_OK, &CMainFrame::OnMsgAssignOK)

	////////////////////////////////////////
	//	Menu - System
	ON_COMMAND(ID_OPERATION_RUN, &CMainFrame::OnBnClickedMenuOperationRun)
	ON_COMMAND(ID_OPERATION_STOP, &CMainFrame::OnBnClickedMenuOperationStop)
	ON_COMMAND(ID_SYSTEM_SCHEDULE, &CMainFrame::OnBnClickedMenuSchedule)
	ON_COMMAND(ID_DEVICE_CONFIG, &CMainFrame::OnBnClickedMenuConfiguration)
	ON_COMMAND(ID_OPERATION_SELECT, &CMainFrame::OnTreeClickedOperationLog)
	ON_COMMAND(ID_OPERATION_CHANGED, &CMainFrame::OnOutlookTreeClicked)
	////////////////////////////////////////
	//	Menu - Device
	ON_COMMAND(ID_DEVICE_STOP, &CMainFrame::OnBnClickedMenuDeviceStop)
	ON_COMMAND(ID_DEVICE_BACKUP, &CMainFrame::OnBnClickedMenuDeviceBackup)
	ON_COMMAND(ID_DEVICE_RESTORE, &CMainFrame::OnBnClickedMenuDeviceRestore)
	ON_COMMAND(ID_DEVICE_ASSIGNMENT, &CMainFrame::OnBnClickedMenuDeviceAssignment)
	ON_COMMAND(ID_DEVICE_FACTORY_RESET, &CMainFrame::OnBnClickedMenuFactoryReset)
	ON_COMMAND(ID_DEVICE_OPERATION, &CMainFrame::OnBnClickedMenuDeviceOperation)

	// INFORM - Device
	ON_COMMAND(ID_INFORM_BACKUP, &CMainFrame::OnBnClickedInformBackup)
	ON_COMMAND(ID_INFORM_BACKUP_MANAGE, &CMainFrame::OnBnClickedInformBackupManage)
	ON_COMMAND(ID_INFORM_RESTORATION, &CMainFrame::OnBnClickedInformRestore)
	ON_COMMAND(ID_INFORM_COMPARE, &CMainFrame::OnBnClickedInformComapre)
	ON_COMMAND(ID_INFORM_ASSIGN, &CMainFrame::OnBnClickedInformAssign)
	ON_COMMAND(ID_INFORM_REBOOT, &CMainFrame::OnBnClickedInformReboot)
	ON_COMMAND(ID_INFORM_FACTORY_RESET, &CMainFrame::OnBnClickedInformFactoryReset)
	ON_COMMAND(ID_INFORM_MOTOR_RESET, &CMainFrame::OnBnClickedInformMotorReset)
	ON_COMMAND(ID_INFORM_BACKUP_SCHEDULE, &CMainFrame::OnBnClickedInformBackupSchedule)
	ON_COMMAND(ID_INFORM_CONFIG, &CMainFrame::OnBnClickedInformConfig)
	ON_COMMAND(ID_INFORM_SYSTEM_RUN, &CMainFrame::OnBnClickedInformSystemRun)
	ON_COMMAND(ID_INFORM_BACKUP_TOGGLE, &CMainFrame::OnBnClickedBtnToggleBackup)
	ON_COMMAND(ID_INFORM_ALARM_RST, &CMainFrame::OnBnClickedAlarmReset)
	
	//Tool Bar
	ON_UPDATE_COMMAND_UI(IDR_PANE_LOG, &CMainFrame::OnUpdateOperationToolBar)
	ON_COMMAND(IDC_BTN_ALL, &CMainFrame::OnBnClickedOperationAll)
	ON_COMMAND(IDC_CHK_WARNING, &CMainFrame::OnBnClickedOperationWarning)
	ON_COMMAND(IDC_CHK_ERROR, &CMainFrame::OnBnClickedOperationError)
	ON_COMMAND(IDC_CHK_FILTER, &CMainFrame::OnBnClickedOperationFilter)
	ON_COMMAND(IDC_CHK_NAME, &CMainFrame::OnBnClickedOperationFocusName)
	ON_COMMAND(IDC_CHK_ADDRESS, &CMainFrame::OnBnClickedOperationFocusAddress)
	ON_COMMAND(IDC_CHK_MIX, &CMainFrame::OnBnClickedOperationFocusMix)

	// Status 
	ON_UPDATE_COMMAND_UI(IDR_PANE_DEVICE, &CMainFrame::OnUpdateOutlookToolBar)
	ON_COMMAND(IDC_CHK_STATUS_NAME, &CMainFrame::OnBnClickedStatusCheckName)
	ON_COMMAND(IDC_CHK_STATUS_ADDRESS, &CMainFrame::OnBnClickedStatusAddress)
	ON_COMMAND(IDC_CHK_STATUS_SEARCH, &CMainFrame::OnBnClickedStatusIPSearching)

	// Outlook Load Tree
	ON_COMMAND(ID_OPERATION_LOAD_TREE, &CMainFrame::OnTreeLoadDevice)
//	ON_BN_CLICKED(IDP_BTN_SELECT, &CMainFrame::OnBnClickedBtnSystemOption)
//	ON_UPDATE_COMMAND_UI(IDP_BTN_SELECT, &CMainFrame::OnUpdateBtnSystemOption)

	
		
	ON_COMMAND(IDP_BTN_TOOL_REBOOT, &CMainFrame::OnBnClickedBtnDeviceReboot)
	ON_UPDATE_COMMAND_UI(IDP_BTN_TOOL_REBOOT, &CMainFrame::OnUpdateBtnDeviceReboot)

	ON_COMMAND(IDP_BTN_SYSTEM_BACKUP_MANAGE, &CMainFrame::OnBtnSystemBackupManage)
	ON_UPDATE_COMMAND_UI(IDP_BTN_SYSTEM_BACKUP_MANAGE, &CMainFrame::OnUpdateBtnSystemBackupManage)
	
	ON_COMMAND(IDP_BTN_SYSTEM_COMPARE, &CMainFrame::OnBtnSystemCompare)
	ON_UPDATE_COMMAND_UI(IDP_BTN_SYSTEM_COMPARE, &CMainFrame::OnUpdateBtnSystemCompare)
	
	ON_COMMAND(IDP_BTN_SYSTEM_BACKUP, &CMainFrame::OnBtnSystemBackup)
	ON_UPDATE_COMMAND_UI(IDP_BTN_SYSTEM_BACKUP, &CMainFrame::OnUpdateBtnSystemBackup)

	ON_COMMAND(IDP_BTN_SYSTEM_HISTORY, &CMainFrame::OnBtnSystemHistory)
	ON_UPDATE_COMMAND_UI(IDP_BTN_SYSTEM_HISTORY, &CMainFrame::OnUpdateBtnSystemHistory)

	ON_WM_CLOSE()
	//ON_COMMAND(ID_BTN_SYS_HISTORY, &CMainFrame::OnBtnSysHistory)
	END_MESSAGE_MAP()

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() noexcept
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLACK);

	m_nLogLevel = msgNormal;

	m_bMade = FALSE;
}

CMainFrame::~CMainFrame()
{
	
	
	
	

}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // 사용할 수 있는 다른 스타일...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // FALSE로 설정하여 탭 영역 오른쪽에 닫기 단추를 배치합니다.
	mdiTabParams.m_bTabIcons = FALSE;    // TRUE로 설정하여 MDI 탭의 문서 아이콘을 활성화합니다.
	mdiTabParams.m_bAutoColor = TRUE;    // FALSE로 설정하여 MDI 탭의 자동 색 지정을 비활성화합니다.
	mdiTabParams.m_bDocumentMenu = TRUE; // 탭 영역의 오른쪽 가장자리에 문서 메뉴를 활성화합니다.
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);
	m_wndRibbonBar.DeleteQAT();
	m_wndRibbonBar.SetApplicationButton(m_wndRibbonBar.GetApplicationButton(), CSize());
	

	// Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
		

	if (!CreateStatusBar())
	{
		TRACE0("Failed to create status bar\n");
		return -1;
	}

	if (!CreateCaptionBar())
	{
		TRACE0("Failed to create caption bar\n");
		return -1;
	}

	if (!CreateOutlookBar(m_wndNavigationBar, IDC_STATIC))
	{
		TRACE0("Outlook 창을 만들지 못했습니다.\n");
		return -1;
	}

	if (!CreateDockingWindows())
	{
		TRACE0("도킹 창을 만들지 못했습니다.\n");
		return -1;
	}

	// 보관된 값에 따라 비주얼 관리자 및 스타일을 설정합니다.
	OnApplicationLook(theApp.m_nAppLook);

	// 향상된 창 관리 대화 상자를 활성화합니다.
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// 창 제목 표시줄에서 문서 이름 및 애플리케이션 이름의 순서를 전환합니다.
	// 문서 이름이 축소판 그림과 함께 표시되므로 작업 표시줄의 기능성이 개선됩니다.
	ModifyStyle(0, FWS_PREFIXTITLE);

		//m_wndRibbonBar.ShowContextCategories(ID_CONTEXT1);

	InitializeGlobalForm();

	CInterfaceManager::GetInterface();


	CTime time = CTime::GetCurrentTime();
	CString strTime;
	strTime.Format(_T("%04d-%02d-%02d  %02d:%02d:%02d"),
		time.GetYear(), time.GetMonth(), time.GetDay(),
		time.GetHour(), time.GetMinute(), time.GetSecond());
#if defined(LOG_START)
	CDataBaseLog::GetInstance()->InsertLog(0, _T("Program Executed"));
	m_paneOperationLog.SaveLogList(0, strTime, _T("SYSTEM"), L"", _T("Program Executed"), CTime::GetCurrentTime());
	m_wndSystemHistory.SaveLogList(0, strTime, _T("SYSTEM"), L"", _T("Program Executed"), CTime::GetCurrentTime());
#endif
	m_paneOperationLog.SetLogTime();
	//	추후에 이걸 어떻게 할지 고민합시다.
	this->SetTitle(_T(""));
	this->SetWindowText(_T("FASTECH Backup & Restoration Management System"));

	SetTimer(TIMER_INITIALIZE, ELAPSE_TIMER_INITIALIZE, NULL);
	SetTimer(TIMER_DEVICE_CHECK, 100, NULL);
	
	// KKB
	m_LogCheck = { 0, };
	
	m_wndRibbonBar.ShowCategory(2, FALSE); // 카테고리 삭제..
	m_wndRibbonBar.RecalcLayout();
	m_bSystemErrorDetected = m_bShowToggleBtn = m_bEnableToggleButton = m_bEnableTextAutoSec = m_bEnbaleTextTime = FALSE;
	m_pDeviceAddress = 0;
	m_dwSelectedAddress = 0;
	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	m_nMotionGateCnt = pDeviceList->GetAddressCount();
	m_pDevice = GetSelectedMotiongateCtrl();
	
	m_pWndMotionManager = new CDlgMotionGateManager(this, m_nMotionGateCnt);
	m_pWndMotionManager->Create(IDD_MOTION_MANAGER, this);
	m_pWndMotionManager->ShowWindow(SW_SHOW);
	m_bMade = TRUE;
	m_bRun = FALSE;
	m_pWndMotionManager->SetShowState(TRUE);
	
	m_pWndMotionManager->SetDeviceMotionGate(m_pDevice);
	//CreateModBusServer();
	m_logStatus = 0;
	m_bSystemCompare = FALSE;
	m_strCompareSystemName = _T("");
	
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CMDIFrameWndEx::PreCreateWindow(cs))
		return FALSE;

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}


LRESULT CMainFrame::OnTrayNotifycation(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case WM_RBUTTONDOWN:
	{

	}	break;

	case WM_LBUTTONDBLCLK:
	{
		if (this->m_bIsMinimized)
		{
			this->ShowWindow(SW_RESTORE);
		}
		else if (FALSE == this->IsWindowVisible())
		{
			this->ShowWindow(SW_SHOWMAXIMIZED);
		}
	}	break;
	}
	return 0;
}

const BOOL CMainFrame::CreateDockingWindows()
{
	// 속성 창을 만듭니다.
	CString strPropertiesWnd;
	BOOL bNameValid = FALSE;

	//bNameValid = strPropertiesWnd.LoadString(IDS_DEVICE_INFO);
	//ASSERT(bNameValid);
	//if (!m_wndOutlookDevice.Create(strPropertiesWnd, this, CRect(0, 0, 300, 200), TRUE, ID_PANE_DEVICELIST, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	//{
	//	TRACE0("속성 창을 만들지 못했습니다.\n");
	//	return FALSE; // 만들지 못했습니다.
	//}
	//
	//m_wndOutlookDevice.EnableDocking(CBRS_ALIGN_ANY);
	//DockPane(&m_wndOutlookDevice);

	bNameValid = strPropertiesWnd.LoadString(IDS_OPERATION_LOG);
	ASSERT(bNameValid);
	if (!m_paneOperationLog.Create(strPropertiesWnd, this, CRect(0, 0, 300, 200), TRUE, ID_PANE_OPERATION_LOG, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("속성 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	m_paneOperationLog.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_paneOperationLog);

	//	m_wndRibbonBar.ShowContextCategories(ID_CONTEXT_MOTOR);
	//	m_wndRibbonBar.ActivateContextCategory(ID_CONTEXT_MOTOR);

	return TRUE;
}

const BOOL CMainFrame::CreateCaptionBar()
{
	if (!m_wndCaptionBar.CreateEx(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
		TRACE0("Failed to create caption bar\n");
		return FALSE;
	}

	BOOL bNameValid = FALSE;

	CString strTemp, strTemp2;

	//	Button을 보여주는 예제
	/*bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButton(strTemp, ID_TOOLS_OPTIONS, CMFCCaptionBar::ALIGN_LEFT, FALSE);
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON_TIP);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButtonToolTip(strTemp);*/

	//	Text를 보여주는 예제
	/*bNameValid = strTemp.LoadString(IDS_CAPTION_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetText(strTemp, CMFCCaptionBar::ALIGN_LEFT);*/

	//	Image를 보여주는 예제
	/*m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);
	bNameValid = strTemp.LoadString(IDS_CAPTION_IMAGE_TIP);
	ASSERT(bNameValid);
	bNameValid = strTemp2.LoadString(IDS_CAPTION_IMAGE_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetImageToolTip(strTemp, strTemp2);*/

	m_wndCaptionBar.SetButton(_T(""), ID_VIEW_CAPTION_BAR, CMFCCaptionBar::ALIGN_RIGHT, FALSE);	//	닫기 버튼으로 이용하는 방법
//	m_wndCaptionBar.SetIcon(IDI_WARNING, RGB(0x00, 0x00, 0x00), FALSE, CMFCCaptionBar::ALIGN_LEFT);
//	m_wndCaptionBar.SetText(_T("Test"), CMFCCaptionBar::ALIGN_LEFT);

	return TRUE;
}

const BOOL CMainFrame::CreateStatusBar()
{
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return FALSE;      // 만들지 못했습니다.
	}

	CString strTitlePane1(_T("Program Started"));
	CString strTitlePane2;

	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(0, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(0, strTitlePane2, TRUE), strTitlePane2);

	return TRUE;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	if (m_bMade)
	{
		m_pWndMotionManager->Invalidate(FALSE);
	}

	CMDIFrameWndEx::OnSize(nType, cx, cy);
}

void CMainFrame::OnBnClickedBtnMotionTest()
{
	if (FALSE == CheckControlGranted())
	{
		return;
	}

	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();
	if (NULL == pDevice)
	{
		return;
	}

	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();

	CString strCaption(_T("Motion test"));

	if ((TRUE == pConfigDB->IsDeviceBackupGranted()) ||
		(TRUE == RequestElevatedPermissions(strCaption) &&
			TRUE == pConfigDB->IsDeviceBackupGranted()))
	{
		
		CDlgMotionTest  dlg;
		dlg.SetAddress(pDevice->GetAddress());
		dlg.DoModal();

		//CFASPERGUIApp* pApp = static_cast<CFASPERGUIApp*>(AfxGetApp());
		//POSITION posTemp = pApp->GetFirstDocTemplatePosition();
		//const DWORD dwAddress = pDevice->GetAddress();

		//while (posTemp)
		//{
		//	CMultiDocTemplateEx* pDocTemplate = static_cast<CMultiDocTemplateEx*>(pApp->GetNextDocTemplate(posTemp));
		//	if (pDocTemplate->IsMatchedTemplate(RUNTIME_CLASS(CDocDevice), RUNTIME_CLASS(CMDIChildWndEx), RUNTIME_CLASS(CDlgMotionTest)))
		//	{
		//		//	이미 열린 View가 있는지 확인
		//		POSITION posDoc = pDocTemplate->GetFirstDocPosition();
		//		while (posDoc)
		//		{
		//			CDocument* pDoc = pDocTemplate->GetNextDoc(posDoc);
		//			POSITION posView = pDoc->GetFirstViewPosition();
		//			CDlgMotionTest* pView = static_cast<CDlgMotionTest*>(pDoc->GetNextView(posView));

		//			if (pView && dwAddress == static_cast<CDocDevice*>(pView->GetDocument())->GetAddress())
		//			{
		//				static_cast<CMDIChildWndEx*>(pView->GetParent())->ActivateFrame();
		//				return;
		//			}
		//		}

		//		CDocDevice* pDoc = static_cast<CDocDevice*>(pDocTemplate->OpenDocumentFile(NULL));
		//		pDoc->SetAddress(dwAddress);
		//		pDoc->SetDeviceTitle(strCaption);
		//	}
		//}
	}
	else
	{
		ShowRequireElevatedPermissions(strCaption);
	}
}

void CMainFrame::InitializeGlobalForm()
{
	const COLORREF colorBrightRed = RGB(0xc8, 0x00, 0x00);
	const COLORREF colorDarkRed = RGB(0x32, 0x00, 0x00);
	const COLORREF colorBrightGreen = RGB(0x00, 0xc8, 0x00);
	const COLORREF colorDarkGreen = RGB(0x00, 0x32, 0x00);
	const COLORREF colorNormalGray = RGB(0x60, 0x60, 0x60);
	const COLORREF colorDarkGray = RGB(0x32, 0x32, 0x32);
	const COLORREF colorBrightYellow = RGB(0xc8, 0xc8, 0x00);
	const COLORREF colorDarkYellow = RGB(0x32, 0x32, 0x00);

	if (1)
	{	//	Setup Motiongate Signal Form
		const INT nFormID = 0;
		CStaticSignalDisplayForm* pForm = CStaticSignalDisplayGlobalForm::GetInstance()->GetForm(nFormID);

		const COLORREF colorCode[] = {
			colorBrightRed,		colorBrightGreen,	colorBrightRed,		colorBrightYellow,
			colorBrightGreen,	colorBrightGreen,	colorBrightYellow,	colorBrightYellow,
			colorBrightGreen,	colorBrightRed,		colorBrightYellow,	colorBrightYellow,
			colorBrightYellow,	colorBrightYellow,	colorBrightYellow,	colorBrightYellow,
		};

		const UINT nToolTipID[] = {
			IDS_TOOLTIP_MG_BIT00,	IDS_TOOLTIP_MG_BIT01,	IDS_TOOLTIP_MG_BIT02,	IDS_TOOLTIP_MG_BIT03,
			IDS_TOOLTIP_MG_BIT04,	IDS_TOOLTIP_MG_BIT05,	IDS_TOOLTIP_MG_BIT06,	IDS_TOOLTIP_MG_BIT07,
			IDS_TOOLTIP_MG_BIT08,	IDS_TOOLTIP_MG_BIT09,	IDS_TOOLTIP_RESERVED,	IDS_TOOLTIP_RESERVED,
			IDS_TOOLTIP_RESERVED,	IDS_TOOLTIP_RESERVED,	IDS_TOOLTIP_RESERVED,	IDS_TOOLTIP_RESERVED,
		};

		const UINT nToolTipType[] = {
			TTI_ERROR,	TTI_INFO,	TTI_ERROR,		TTI_INFO,
			TTI_INFO,	TTI_INFO,	TTI_WARNING,	TTI_WARNING,
			TTI_INFO,	TTI_INFO,	TTI_NONE,		TTI_NONE,
			TTI_NONE,	TTI_NONE,	TTI_NONE,		TTI_NONE,
		};

		const COLORREF colorDisable = colorDarkGray;

		pForm->SetTooltipTitle(_T("Motiongate Status"));

		CString strText;
		for (INT nIndex = 0; nIndex < 16; nIndex++)
		{
			strText.Format(_T("%d"), nIndex);
			pForm->SetItemText(nIndex, strText);

			pForm->SetBkColor(nIndex, colorDisable, colorDisable);
			pForm->SetTextColor(nIndex, colorCode[nIndex], colorNormalGray);

			pForm->SetTooltipID(nIndex, nToolTipID[nIndex]);
			pForm->SetTooltipType(nIndex, nToolTipType[nIndex]);
		}
	}

	if (1)
	{	//	Setup Driver Signal Form
		const INT nFormID = 1;
		CStaticSignalDisplayForm* pForm = CStaticSignalDisplayGlobalForm::GetInstance()->GetForm(nFormID);

		const COLORREF colorCode[] = {
			colorBrightYellow,	colorBrightYellow,	colorBrightYellow,	colorBrightGreen,
			colorBrightYellow,	colorBrightGreen,	colorBrightRed,		colorBrightYellow,
			colorBrightRed,		colorBrightRed,		colorBrightRed,		colorBrightYellow,
			colorBrightYellow,	colorBrightYellow,	colorBrightYellow,	colorBrightYellow,
		};

		const UINT nToolTipID[] = {
			IDS_TOOLTIP_MOTOR_BIT00,	IDS_TOOLTIP_MOTOR_BIT01,	IDS_TOOLTIP_MOTOR_BIT02,	IDS_TOOLTIP_MOTOR_BIT03,
			IDS_TOOLTIP_MOTOR_BIT04,	IDS_TOOLTIP_MOTOR_BIT05,	IDS_TOOLTIP_MOTOR_BIT06,	IDS_TOOLTIP_MOTOR_BIT07,
			IDS_TOOLTIP_MOTOR_BIT08,	IDS_TOOLTIP_MOTOR_BIT09,	IDS_TOOLTIP_MOTOR_BIT10,	IDS_TOOLTIP_RESERVED,
			IDS_TOOLTIP_RESERVED,		IDS_TOOLTIP_RESERVED,		IDS_TOOLTIP_RESERVED,		IDS_TOOLTIP_RESERVED,
		};

		const UINT nToolTipType[] = {
			TTI_WARNING,	TTI_WARNING,	TTI_WARNING,	TTI_INFO,
			TTI_WARNING,	TTI_INFO,		TTI_ERROR,		TTI_WARNING,
			TTI_ERROR,		TTI_ERROR,		TTI_ERROR,		TTI_NONE,
			TTI_NONE,		TTI_NONE,		TTI_NONE,		TTI_NONE,
		};

		const COLORREF colorDisable = colorDarkGray;

		pForm->SetTooltipTitle(_T("Motor Status"));

		CString strText;
		for (INT nIndex = 0; nIndex < 16; nIndex++)
		{
			strText.Format(_T("%d"), nIndex);
			pForm->SetItemText(nIndex, strText);

			pForm->SetBkColor(nIndex, colorDisable, colorDisable);
			pForm->SetTextColor(nIndex, colorCode[nIndex], colorNormalGray);

			pForm->SetTooltipID(nIndex, nToolTipID[nIndex]);
			pForm->SetTooltipType(nIndex, nToolTipType[nIndex]);
		}
	}
}

void CMainFrame::OnBnClickedPanelDeviceList()
{
	CDlgDeviceList dlg;
	if (IDOK == dlg.DoModal())
	{
		m_wndOutlookDevice.LoadDeviceInformation();
	}
}

void CMainFrame::OnUpdateBtnMotionTest(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;

	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();

	if (pDevice)
	{
		bEnable = (pDevice->IsMotiongateControllable(FALSE, FALSE));
	}

	pCmdUI->Enable(bEnable);
}

void CMainFrame::OnUpdatePanelRun(CCmdUI* pCmdUI)
{
	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	const BOOL bEnable = (FALSE == pDeviceList->IsMonitoringEnabled()) && (0 < pDeviceList->GetAddressCount());

	pCmdUI->Enable(bEnable);
}

void CMainFrame::OnUpdatePanelStop(CCmdUI* pCmdUI)
{
	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	const BOOL bEnable = (TRUE == pDeviceList->IsMonitoringEnabled()) && (0 < pDeviceList->GetAddressCount());

	pCmdUI->Enable(bEnable);
}

void CMainFrame::OnCmdSystemReboot()
{
	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();
	CString strCaption(_T("Reboot System"));
	
	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();
	pDevice->PauseAutoBackup();

	if ((TRUE == pConfigDB->IsDeviceBackupGranted()) ||
		(TRUE == RequestElevatedPermissions(strCaption) &&
			TRUE == pConfigDB->IsDeviceBackupGranted()))
	{
		CString message("");
		CString mainInstruction("Would you reboot this system now?");
		CString title(_T("Confirm to reboot the system"));

		CTaskDialog taskDialog(message, mainInstruction, title, TDCBF_CANCEL_BUTTON);

		taskDialog.AddCommandControl(IDYES, _T("Yes"));
		taskDialog.AddCommandControl(IDNO, _T("No"));

		taskDialog.SetDefaultCommandControl(IDYES);

		taskDialog.SetMainIcon(IDI_QUESTION);

		taskDialog.SetDialogWidth(350);

		if (FALSE == CheckControlGranted())
		{
			return;
		}

		CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();
		if (NULL == pDevice)
		{
			return;
		}

		switch (taskDialog.DoModal())
		{
		case IDYES:
		{
			pDevice->RebootSystem();
		}
		break;

		case IDNO:
		default:
			break;
		}
	}
	else
	{
		pDevice->PauseAutoBackup(FALSE);

		ShowRequireElevatedPermissions(strCaption);
	}
}

void CMainFrame::OnBnClickedPanelRun()
{
	if (FALSE == CheckControlGranted())
	{
		return;
	}
	
	CDeviceList::GetInstance()->StartMonitoring();

	SetTimer(TIMER_DEVICE_CHECK, 100, NULL);
}

void CMainFrame::OnBnClickedPanelStop()
{
	if (FALSE == CheckControlGranted())
	{
		return;
	}

	CDeviceList::GetInstance()->StopMonitoring();
}

void CMainFrame::DisplayOperatorInformation()
{
	/*
	if (1)
	{
		CString strLogin;
		INT nImage = 0;
		if (pConfig->IsDefaultOperator())
		{
			strLogin.LoadString(IDS_LOGIN);
		}
		else
		{
			strLogin.LoadString(IDS_LOGOUT);
		}

		CMFCRibbonBaseElement* pElement = m_wndRibbonBar.FindByID(ID_PANEL_USER);
		if (strLogin != pElement->GetText())
		{
			pElement->SetText(strLogin);
		}
	}
	*/
}

void CMainFrame::DisplayLogoutInformation()
{
	CDataBaseConfig* pConfig = CDataBaseConfig::GetInstance();

	CString strTimer;
	if (FALSE == pConfig->IsDefaultOperator())
	{

	}

	CMFCRibbonBaseElement* pElement = m_wndRibbonBar.FindByID(IDC_ST_LOGOUT);
	if (strTimer != pElement->GetText())
	{
		pElement->SetText(strTimer);
	}
}

void CMainFrame::OnBnClickedPanelUser()
{
	DoLoginSequence();
}

void CMainFrame::OnUpdatePanelUser(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

void CMainFrame::OnUpdateBtnDeviceBackup(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;

	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();

	if (pDevice)
	{
		bEnable = (pDevice->IsMotiongateControllable(FALSE) && pDevice->IsMotorConnectionContinuity());
	}

	pCmdUI->Enable(bEnable);
}

void CMainFrame::OnUpdateBtnDeviceRestoration(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;

	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();

	if (pDevice)
	{
//		bEnable = (pDevice->IsMotiongateControllable() && pDevice->IsMotorConnectionContinuity());
		bEnable = (pDevice->IsMotiongateControllable(FALSE) && pDevice->IsMotorConnectionContinuity());
	}

	pCmdUI->Enable(bEnable);
}

void CMainFrame::OnUpdateBtnDeviceIdAssignment(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;

	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();

	if (pDevice)
	{
		bEnable = (
			pDevice->IsMotiongateControllable() &&
			TRUE == pDevice->IsBareMotorConnected()
			);
	}

	pCmdUI->Enable(bEnable);
}

void CMainFrame::OnUpdateBtnDeviceFactoryReset(CCmdUI* pCmdUI)
{ 
	BOOL bEnable = FALSE;

	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();

	if (pDevice)
	{
		bEnable = (
			pDevice->IsMotiongateControllable() &&
			FALSE == pDevice->IsBareMotorConnected()
			);
	}

	pCmdUI->Enable(bEnable);
}

void CMainFrame::OnBnClickedBtnDeviceStatus()
{
	//	m_wndOutlookDevice.ShowPane(static_cast<CMFCRibbonCheckBox*>(m_wndRibbonBar.FindByID(IDP_DEVICE_STATUS))->IsChecked() ? FALSE : TRUE, FALSE, TRUE);
}

void CMainFrame::OnUpdateBtnDeviceStatus(CCmdUI* pCmdUI)
{
	//	pCmdUI->SetCheck(m_wndOutlookDevice.IsWindowVisible());
}

void CMainFrame::OnBnClickedBtnOperationLog()
{
	m_paneOperationLog.SetLogTime();
	m_paneOperationLog.ShowPane(static_cast<CMFCRibbonCheckBox*>(m_wndRibbonBar.FindByID(IDP_OPERATION_LOG))->IsChecked() ? FALSE : TRUE, FALSE, TRUE);
	m_paneOperationLog.RemoveList();
}

void CMainFrame::OnUpdateBtnOperationLog(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_paneOperationLog.IsWindowVisible());
	
	if (m_paneOperationLog.IsWindowVisible())
	{
		//SetOperationLogAll();
	}
}

const BOOL CMainFrame::SetOperationLogAll()
{
	CDataBaseLog* pBase = CDataBaseLog::GetInstance();
	CString strInform, strID, strIP, strTime;
	/*
	if (pBase->OpenDataBase())
	{
		if (pBase->GetLogList(_T("Program Executed")))
		{
			while (pBase->SeekToBegin())
			{
				CDataBaseRecordData* pRecord = pBase->SeekToNext();
			}

			
			
			/*do
			{
				
				pBase->GetLogInformationAll(strInform, strIP, strID, strTime);
			} while ();

	pBase->SeekToBegin();
}
	}
	*/
	
	INT nRowid;
	if (pBase->OpenDataBase())
	{
		if (pBase->GetLogList(_T("Program Executed")))
		{
			if (pBase->SeekToBegin())
			{
				GetLogInformationAll(pBase, nRowid, strInform, strIP, strID, strTime);
			}
			
		}
	}
	return TRUE;
}


void CMainFrame::OnUpdateBtnDeviceBackupSchedule(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;

	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();

	if (pDevice)
	{
		bEnable = (pDevice->IsMotiongateControllable(FALSE, FALSE));
	}

	pCmdUI->Enable(bEnable);
}

void CMainFrame::OnUpdateAlarmReset(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;

	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();

	if (pDevice)
	{
		bEnable = (pDevice->IsMotiongateControllable(FALSE, FALSE));
	}

	pCmdUI->Enable(bEnable);
}


void CMainFrame::OnUpdateBtnToggleBackup(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;

	BOOL bRun = FALSE;
	//CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();
	CDeviceMotionGate* pDevice = GetSelectedMotionGateControl();
	if (pDevice && pDevice->IsMotiongateConnected())
	{
		if (pDevice->IsAutoBackupScheduleExist())
		{
			bEnable = TRUE;

			bRun = pDevice->IsAutoRestorationEnabled();
		}
	}

	pCmdUI->Enable(bEnable);


	CString strText(bRun ? _T("ON") : _T("OFF"));
	CMFCRibbonBaseElement* pElement = m_wndRibbonBar.FindByID(IDP_BTN_TOGGLE_BACKUP);
	if (pElement->GetText() != strText)
	{
		pElement->SetText(strText);
	}

	ChangePanelButtonImage(IDP_BTN_TOGGLE_BACKUP, bRun ? idx_toggle_on : idx_toggle_off);
}

void CMainFrame::OnBnClickedBtnStopExecution()
{
	if (FALSE == CheckControlGranted())
	{
		return;
	}

	const DWORD dwAddress = GetSelectedDeviceAddress();
	if (!dwAddress)
	{
		return;
	}

	CDeviceMotionGate* pDevice = NULL;
	if (CDeviceList::GetInstance()->GetMotiongateByAddress(dwAddress, pDevice))
	{
		if (FALSE == pDevice->StopExecution())
		{

		}
	}
}

void CMainFrame::OnUpdateBtnStopExecution(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	const DWORD dwAddress = GetSelectedDeviceAddress();
	if (dwAddress)
	{
		CDeviceMotionGate* pDevice = NULL;
		if (CDeviceList::GetInstance()->GetMotiongateByAddress(dwAddress, pDevice))
		{
			bEnable = pDevice->IsGrantedToStop();
		}
	}

	pCmdUI->Enable(bEnable);
}

//void CMainFrame::OnBnClickedBtnDeviceBackupNominal()
//{
//	//	정말로 Nominal Backup을 수행할 것인지 묻는다.
//	CString strMessage, strCaption;
//	strCaption.LoadString(IDS_CAPTION_CONFIRM);
//	strMessage.LoadString(IDS_QUESTION_NOMINAL_BACKUP);
//
//	if (IDYES == MessageBox(strMessage, strCaption, MB_ICONQUESTION | MB_YESNOCANCEL))
//	{
//		const DWORD dwAddress = GetSelectedDeviceAddress();
//
//		if (dwAddress)
//		{
//			CDeviceMotionGate* pDevice = NULL;
//			if (CDeviceList::GetInstance()->GetMotiongateByAddress(dwAddress, pDevice))
//			{
//				//	Nominal Backup Flag을 활성화한다.
//				pDevice->SetNominalBackup();
//
//				//	Backup을 수행한다.
//				OnBnClickedBtnDeviceBackup();
//			}
//		}
//	}
//}

//void CMainFrame::OnUpdateBtnDeviceBackupNominal(CCmdUI* pCmdUI)
//{
//	BOOL bEnable = FALSE;
//
//	const DWORD dwAddress = GetSelectedDeviceAddress();
//
//	if (dwAddress && CDataBaseConfig::GetInstance()->IsDeviceBackupGranted())
//	{
//		CDeviceMotionGate* pDevice = NULL;
//		if (CDeviceList::GetInstance()->GetMotiongateByAddress(dwAddress, pDevice))
//		{
//			bEnable = \
//				pDevice->IsMotiongateControllable() &&
//				pDevice->IsMotorConnectionContinuity();
//		}
//	}
//
//	pCmdUI->Enable(bEnable);
//}

void CMainFrame::OnBnClickedBtnDeviceBackup()
{
	//CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl(); 권한 수정..
	CDeviceMotionGate* pDevice = GetSelectedMotionGateControl();
	if (NULL == pDevice)
	{
		return;
	}

	pDevice->PauseAutoBackup();

	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();

	CString strCaption(_T("Backup"));

	if ((TRUE == pConfigDB->IsDeviceBackupGranted()) ||
		(TRUE == RequestElevatedPermissions(strCaption) &&
			TRUE == pConfigDB->IsDeviceBackupGranted()))
	{
		CSheetBackup propSheet;
		propSheet.SetCaption(_T("System Backup"));
		propSheet.SetAddress(pDevice->GetAddress());
		if (IDOK == propSheet.DoModal())
		{
			pDevice->SetProjectName(pDevice->GetSystemName());
			pDevice->SetBackupName(propSheet.GetComment());
			pDevice->SetNominalBackup(propSheet.IsNominalBackupEnabled());
			pDevice->StartBackup();
		}
	}
	else
	{
		pDevice->PauseAutoBackup(FALSE);

		ShowRequireElevatedPermissions(strCaption);
	}

	pDevice->PauseAutoBackup(FALSE);
}

const DWORD CMainFrame::GetSelectedDeviceAddress()
{
	DWORD dwResult = 0;
	CString strAddress;
	static INT nContinue = 1;

	
	// COutlookDevice Tree 선택시 동작
	if (m_wndOutlookDevice.IsWindowVisible())
	{
		CPaneDeviceInfo::TREE_ITEM_DATA_FLAG data;
		data.orgValue = m_wndOutlookDevice.GetSelectedItemData();

		dwResult = m_dwSelectedAddress = data.info_motiongate.dwDeviceAddress;
	}
	
	/*
	 //BackGround MotionGate 선택시 동작 로직..
	if (nContinue == 1)
	{
		CDeviceList* pDevice = CDeviceList::GetInstance();
		for (INT nIndex = 0; ; nIndex++)
		{
			dwResult = pDevice->GetAddressByIndex(nIndex);
			if (dwResult != 0)	break;
		}
		m_dwSelectedAddress = dwResult;
		nContinue++;
	}
	else
	{
		dwResult = m_dwSelectedAddress;
	}
	*/
	return dwResult;
}


CDeviceMotionGate* CMainFrame::GetSelectedMotiongateCtrl()
{
	const DWORD dwAddress = GetSelectedDeviceAddress();
	if (0 == dwAddress)
	{
		return NULL;
	}

	CDeviceMotionGate* pDevice = NULL;
	if (FALSE == CDeviceList::GetInstance()->GetMotiongateByAddress(dwAddress, pDevice))
	{
		return NULL;
	}

	return pDevice;
}

void CMainFrame::OnBnClickedBtnDeviceRestoration()
{
	// CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl(); 권한 수정..
	CDeviceMotionGate* pDevice = GetSelectedMotionGateControl();

	if (NULL == pDevice)
	{
		return;
	}

	if (-1 == CDataBaseProject::GetInstance()->GetNominalBackupID(pDevice->GetSystemName()) )
	{
		CString message("You have to backup as nominal first.");
		CString mainInstruction("Nominal backup data does not exist.");
		CString title(_T("Compare backup data with nominal"));

		CTaskDialog taskDialog(message, mainInstruction, title, TDCBF_CLOSE_BUTTON);

		taskDialog.AddCommandControl(IDYES, _T("OK, I understand."));

		taskDialog.SetMainIcon(TD_ERROR_ICON);

		taskDialog.SetDialogWidth(300);

		taskDialog.DoModal();

		return;
	}

	pDevice->PauseAutoBackup();

	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();

	CString strCaption(_T("Restoration"));

	if ((TRUE == pConfigDB->IsDeviceRestorationGranted()) ||
		(TRUE == RequestElevatedPermissions(strCaption) &&
			TRUE == pConfigDB->IsDeviceRestorationGranted()))
	{
		CSheetRestoration propSheet;
		//propSheet.SetCaption(_T("System Restoration"));
		propSheet.SetCaption(_T("System Compare"));
		propSheet.SetAddress(pDevice->GetAddress());
		propSheet.SetSystemControl(FALSE);

		if (IDOK == propSheet.DoModal())
		{
			pDevice->SetProjectName(pDevice->GetSystemName());
			pDevice->SelectBackupID(propSheet.GetProjectBackupID());
			pDevice->EnableAutoHoming(propSheet.IsAutoHomingModeEnabled());
			pDevice->StartRestoration();
		}
		else
		{
			if (pDevice->IsBackupState())
			{
				pDevice->StopExecution();
			}
		}
	}
	else
	{
		pDevice->PauseAutoBackup(FALSE);

		ShowRequireElevatedPermissions(strCaption);
	}

	pDevice->PauseAutoBackup(FALSE);
}


void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize = CPoint(1024, 768);

	CMDIFrameWndEx::OnGetMinMaxInfo(lpMMI);
}

void CMainFrame::DestroyDeviceWindow(CFormView* pException)
{
	const DWORD dwAddress = pException ? static_cast<CDocDevice*>(pException->GetDocument())->GetAddress() : 0;

	CRuntimeClass* pRuntimeDocClass = RUNTIME_CLASS(CDocDevice);
	CRuntimeClass* pFrameTemplate = RUNTIME_CLASS(CMDIChildWndEx);
	CRuntimeClass* pViewTemplate = pException ? pException->GetRuntimeClass() : NULL;

	CFASPERGUIApp* pApp = static_cast<CFASPERGUIApp*>(AfxGetApp());
	POSITION posTemp = pApp->GetFirstDocTemplatePosition();
	while (posTemp)
	{
		CMultiDocTemplateEx* pDocTemplate = static_cast<CMultiDocTemplateEx*>(pApp->GetNextDocTemplate(posTemp));

		//		for (INT i = 0; pRuntimeDocClass[i]; i++)
		{
			if (pViewTemplate && pDocTemplate->IsMatchedTemplate(pRuntimeDocClass, pFrameTemplate, pViewTemplate))
			{
				// 여기서 같은 템플릿을 사용하는 경우는 자신밖에 없으니 그냥 패스
				continue;
			}

			//	이미 열린 View가 있는지 확인
			POSITION posDoc = pDocTemplate->GetFirstDocPosition();
			while (posDoc)
			{
				CDocument* pDoc = pDocTemplate->GetNextDoc(posDoc);
				POSITION posView = pDoc->GetFirstViewPosition();
				CViewBackup* pView = static_cast<CViewBackup*>(pDoc->GetNextView(posView));

				if (pView)
				{
					if (0 == dwAddress || dwAddress == static_cast<CDocDevice*>(pView->GetDocument())->GetAddress())
					{
						pView->PrepareDestroy();
						pView->GetParent()->DestroyWindow();
					}
				}
			}
		}
	}
}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_INITIALIZE:
	{
		KillTimer(nIDEvent);

		if (m_wndCaptionBar.IsWindowVisible())
		{
			m_wndCaptionBar.ShowWindow(SW_HIDE);
			//			m_wndRibbonBar.ForceRecalcLayout();
		}

		::InvalidateRect(m_hWndMDIClient, NULL, FALSE);

	}	break;

	case TIMER_DEVICE_CHECK:
	{
		if (m_wndOutlookDevice.IsWindowVisible())
		{
			CheckDeviceMotionGate();
			KillTimer(nIDEvent);

			SetTimer(TIMER_DEVICE_AUTOHOME, 100, NULL);
		}
		
	}	break;

	case TIMER_DEVICE_AUTOHOME:
	{
		OnUpdateSelcetedDeviceBackupSchedule();
	}	break;
	}

	CMDIFrameWndEx::OnTimer(nIDEvent);
}

void CMainFrame::OnUpdateLabelDeviceName(CCmdUI* pCmdUI)
{
	CString strName(_T("Name  : "));

	CDeviceMotionGate* pDevice = this->GetSelectedMotiongateCtrl();
	if (pDevice)
	{
		strName += pDevice->GetSystemName();
	}

	m_wndRibbonBar.SetLabelText(IDC_ST_DEVICE_NAME, strName);
}

void CMainFrame::OnUpdateLabelDeviceStatus(CCmdUI* pCmdUI)
{
	CString strStatus;
	CDeviceMotionGate* pDevice = this->GetSelectedMotiongateCtrl();
	if (pDevice)
	{
		strStatus += pDevice->GetDetailStatus();
	}

	m_wndRibbonBar.SetLabelText(IDC_ST_DEVICE_STATUS, strStatus);
}


void CMainFrame::OnUpdateProgressExecution(CCmdUI* pCmdUI)
{
	CMFCRibbonProgressBar* pProgress = static_cast<CMFCRibbonProgressBar*>(m_wndRibbonBar.FindByID(IDP_PROGRESS_EXECUTION));
	const INT nCurPos = pProgress->GetPos();
	const INT nCurMin = pProgress->GetRangeMin();
	const INT nCurMax = pProgress->GetRangeMax();

	CDeviceMotionGate* pDevice = this->GetSelectedMotiongateCtrl();
	if (pDevice)
	{
		INT nNewPos, nNewMin, nNewMax;
		pDevice->GetDetailStatus(&nNewPos, &nNewMin, &nNewMax);

		if (nCurPos != nNewPos ||
			nCurMin != nNewMin ||
			nCurMax != nNewMax)
		{
			pProgress->SetPos(nNewPos);
			pProgress->SetRange(nNewMin, nNewMax);
			pProgress->Redraw();
		}
	}
}

void CMainFrame::OnBnClickedBtnHoming()
{
	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();
	if (NULL == pDevice)
	{
		return;
	}

	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();

	CString strCaption(_T("Motion test"));

	if ((TRUE == pConfigDB->IsDeviceBackupGranted()) ||
		(TRUE == RequestElevatedPermissions(strCaption) &&
			TRUE == pConfigDB->IsDeviceBackupGranted()))
	{
		const DWORD dwAddress = pDevice->GetAddress();

		ShowHomingWindow(dwAddress);
	}
	else
	{
		ShowRequireElevatedPermissions(strCaption);
	}
}

void CMainFrame::OnUpdateBtnHoming(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;

	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();

	if (pDevice)
	{
		bEnable = (pDevice->IsMotiongateControllable(FALSE, FALSE));
	}

	pCmdUI->Enable(bEnable);
}

/*
void CMainFrame::OnBnClickedBtnDeviceConfig()
{
	const DWORD dwAddress = GetSelectedDeviceAddress();//(m_globalParamW) ? static_cast<DWORD>(m_globalParamW) : GetSelectedDeviceAddress();
	if (NULL == dwAddress)
	{
		return;
	}

	CFASPERGUIApp* pApp = static_cast<CFASPERGUIApp*>(AfxGetApp());
	POSITION posTemp = pApp->GetFirstDocTemplatePosition();
	while (posTemp)
	{
		CMultiDocTemplateEx* pDocTemplate = static_cast<CMultiDocTemplateEx*>(pApp->GetNextDocTemplate(posTemp));
		if (pDocTemplate->IsMatchedTemplate(RUNTIME_CLASS(CDocDevice), RUNTIME_CLASS(CMDIChildWndEx), RUNTIME_CLASS(CViewDeviceConfig)))
		{
			//	이미 열린 View가 있는지 확인
			POSITION posDoc = pDocTemplate->GetFirstDocPosition();
			while (posDoc)
			{
				CDocument* pDoc = pDocTemplate->GetNextDoc(posDoc);
				POSITION posView = pDoc->GetFirstViewPosition();
				CViewFactoryReset* pView = static_cast<CViewFactoryReset*>(pDoc->GetNextView(posView));

				if (pView && dwAddress == static_cast<CDocDevice*>(pView->GetDocument())->GetAddress())
				{
					static_cast<CMDIChildWndEx*>(pView->GetParent())->ActivateFrame();
					return;
				}
			}

			CDocDevice* pDoc = static_cast<CDocDevice*>(pDocTemplate->OpenDocumentFile(NULL));
			pDoc->SetAddress(dwAddress);
			pDoc->SetDeviceTitle(_T("Motiongate Setting"));
		}
	}
}
*/

void CMainFrame::OnUpdateViewBtnDeviceConfig(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;

	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();
	if (pDevice)
	{
		bEnable = (pDevice->IsMotiongateControllable(FALSE, FALSE));
	}

	pCmdUI->Enable(bEnable);
}

void CMainFrame::ShowHomingWindow(const DWORD dwAddress)
{
	CDlgHoming dlg;
	dlg.SetAddress(dwAddress);
	dlg.DoModal();
	
	//CFASPERGUIApp* pApp = static_cast<CFASPERGUIApp*>(AfxGetApp());
	//POSITION posTemp = pApp->GetFirstDocTemplatePosition();
	//while (posTemp)
	//{
	//	CMultiDocTemplateEx* pDocTemplate = static_cast<CMultiDocTemplateEx*>(pApp->GetNextDocTemplate(posTemp));
	//	if (pDocTemplate->IsMatchedTemplate(RUNTIME_CLASS(CDocDevice), RUNTIME_CLASS(CMDIChildWndEx), RUNTIME_CLASS(CViewHoming)))
	//	{
	//		//	이미 열린 View가 있는지 확인
	//		POSITION posDoc = pDocTemplate->GetFirstDocPosition();
	//		while (posDoc)
	//		{
	//			CDocument* pDoc = pDocTemplate->GetNextDoc(posDoc);
	//			POSITION posView = pDoc->GetFirstViewPosition();
	//			CViewHoming* pView = static_cast<CViewHoming*>(pDoc->GetNextView(posView));

	//			if (pView && dwAddress == static_cast<CDocDevice*>(pView->GetDocument())->GetAddress())
	//			{
	//				static_cast<CMDIChildWndEx*>(pView->GetParent())->ActivateFrame();
	//				return;
	//			}
	//		}
	
	//		CDocDevice* pDoc = static_cast<CDocDevice*>(pDocTemplate->OpenDocumentFile(NULL));
	//		pDoc->SetAddress(dwAddress);
	//		pDoc->SetDeviceTitle(_T("Homing"));
	//	}
	//}
}

LRESULT CMainFrame::OnMsgShowHomingView(WPARAM wParam, LPARAM lParam)
{
	/*m_globalParamW = wParam;
	m_globalParamL = lParam;*/

	ShowHomingWindow(static_cast<DWORD>(wParam));

	return 0;
}

LRESULT CMainFrame::OnMsgShowAutoRestorationView(WPARAM wParam, LPARAM lParam)
{
	/*m_globalParamW = wParam;
	m_globalParamL = lParam;*/

	OnBnClickedBtnDeviceRestoration();

	return 0;
}

LRESULT CMainFrame::OnMsgShowAssignView(WPARAM wParam, LPARAM lParam)
{
	/*m_globalParamW = wParam;
	m_globalParamL = lParam;*/

	OnBnClickedBtnDeviceIdAssignment();

	return 0;
}

LRESULT CMainFrame::OnMsgShowDeviceConfig(WPARAM wParam, LPARAM lParam)
{
	if (FALSE == m_dlgNewSystem.IsDeviceOK(static_cast<DWORD>(wParam)))
	{
		if (NULL == m_dlgNewSystem.GetSafeHwnd())
		{
			while (1)
			{
				if (FALSE == m_dlgNewSystem.IsErrorDeviceExist())
				{
					break;
				}

				if (IDOK == m_dlgNewSystem.DoModal())
				{
					if (const DWORD dwAddress = m_dlgNewSystem.GetSelectedDeviceAddress())
					{
						if (NoticeSingleSystemConfiguration(dwAddress))
						{
							if (NoticeSingleSystemCompareResult(dwAddress))
							{
								m_dlgNewSystem.FilterRegisteredAddress(dwAddress);
							}
							
							//break;
						}
					}
					else
					{
						break;
					}
				
				}
				else
				{
					break;
				}
				
				
			}
		}
	}

	
	return 0;
}


void CMainFrame::OnBnClickedBtnDeviceFactoryReset()
{
	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();
	if (NULL == pDevice)
	{
		return;
	}

	pDevice->PauseAutoBackup();

	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();

	CString strCaption(_T("Factory Reset"));

	if ((TRUE == pConfigDB->IsDeviceFactoryResetGranted()) ||
		(TRUE == RequestElevatedPermissions(strCaption) &&
			TRUE == pConfigDB->IsDeviceFactoryResetGranted()))
	{
		CSheetFactoryReset propSheet;
		propSheet.SetCaption(strCaption);
		propSheet.SetAddress(pDevice->GetAddress());
		if (IDOK == propSheet.DoModal())
		{
			if (FALSE == pDevice->IsMotorConnected(0))
			{
				pDevice->StartFactoryReset(propSheet.GetSelectedMotorID());
			}
			else
			{
				//	Bare Motor가 이미 존재함
			}
		}
	}
	else
	{
		pDevice->PauseAutoBackup(FALSE);

		ShowRequireElevatedPermissions(strCaption);
	}

	pDevice->PauseAutoBackup(FALSE);
}

void CMainFrame::OnBnClickedBtnDeviceIdAssignment()
{
	//CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl(); 권한 수정..
	CDeviceMotionGate* pDevice = GetSelectedMotionGateControl();
	if (NULL == pDevice)
	{
		return;
	}

	pDevice->PauseAutoBackup();

	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();

	CString strCaption(_T("ID Assignment"));

	if ((TRUE == pConfigDB->IsDeviceIdAssignmentGranted()) ||
		(TRUE == RequestElevatedPermissions(strCaption) &&
			TRUE == pConfigDB->IsDeviceIdAssignmentGranted()))
	{
		CSheetAssignID propSheet;
		propSheet.SetCaption(strCaption);
		propSheet.SetAddress(pDevice->GetAddress());
		if (IDOK == propSheet.DoModal())
		{
			pDevice->SelectAssignID(propSheet.GetSelectedMotorID());
			pDevice->StartAssignID();
		}
	}
	else
	{
		pDevice->PauseAutoBackup(FALSE);

		ShowRequireElevatedPermissions(strCaption);
	}

	pDevice->PauseAutoBackup(FALSE);
}

void CMainFrame::OnBnClickedViewCaptionBar()
{
	m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
	RecalcLayout(FALSE);
	m_wndCaptionBar.EnableAutoClose(FALSE);
}

void CMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}

LRESULT CMainFrame::OnMsgOperationLog(WPARAM wParam, LPARAM lParam)
{
	PST_OPERAION_MESSAGE pMsg = reinterpret_cast<PST_OPERAION_MESSAGE>(wParam);

	if (pMsg->nLevel > m_nLogLevel)
	{
		return 1;
	}

	//this->UpdateDetailDeviceStatus(pMsg);
	this->UpdateStatusBar(pMsg);
	this->UpdateCaptionBar(pMsg);

	return 0;
}


LRESULT CMainFrame::OnMsgDetailOperationLog(WPARAM wParam, LPARAM lParam)
{
	CDeviceMotionGate* pDevice;
	CString str;
	
	PST_OPERATIONLOG_INFORMATION inform = reinterpret_cast<PST_OPERATIONLOG_INFORMATION>(wParam);

	OPERATIONLOG_INFORMATION stInform = *inform;

	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	
	pDeviceList->GetMotiongateByAddress(stInform.dwAddress, pDevice);

	if (stInform.strInform != L"")
	{
		m_paneOperationLog.SaveLogList(stInform.nLevel, stInform.strTime, stInform.strDeviceName, stInform.strDeviceIP, stInform.strInform, stInform.time);
		m_wndSystemHistory.SaveLogList(stInform.nLevel, stInform.strTime, stInform.strDeviceName, stInform.strDeviceIP, stInform.strInform, stInform.time);
	}

	return LRESULT();
}


void CMainFrame::UpdateStatusBar(PST_OPERAION_MESSAGE pMsg)
{
	BOOL bRecalcLayout = FALSE;
	CMFCRibbonBaseElement* pElement = m_wndStatusBar.GetElement(0);
	if (NULL == pElement)
	{
		return;
	}

	CString strHeader, strMessage, strFormat, strInformation, strDetail;
	if (pMsg->pDevice)
	{
		pMsg->pDevice->GetAddress(strMessage);
		strHeader.Format(_T("[ %s - %s ] "), strMessage, pMsg->pDevice->GetSystemName());
	}

	switch (pMsg->nMsgID)
	{
	
	case msg_program_start:					strInformation.LoadString(IDS_STATUS_PROGRAM_START);			break;
	case msg_program_end:					strInformation.LoadString(IDS_STATUS_PROGRAM_END);				break;
	case msg_monitoring_start:				strInformation.LoadString(IDS_STATUS_THREAD_START);				break;
	case msg_monitoring_stop:				strInformation.LoadString(IDS_STATUS_THREAD_END);				break;
	case msg_motiongate_connecting:			strInformation.LoadString(IDS_STATUS_CONNECTING);				break;
	case msg_motiongate_connected:			strInformation.LoadString(IDS_STATUS_CONNECTED);				break;
	case msg_motiongate_disconnected:		strInformation.LoadString(IDS_STATUS_DISCONNECTED);				break;
	case msg_motiongate_no_response:		strInformation.LoadString(IDS_STATUS_NO_RESPONSE);				break;
	case msg_motiongate_reboot:				strInformation.LoadString(IDS_STATUS_REBOOT);					break;
	case msg_motiongate_read_info:			strInformation.LoadString(IDS_STATUS_GET_MOTIONGATE_INFO);		break;
	case msg_motiongate_enter_admin:		strInformation.LoadString(IDS_STATUS_ENTER_ADMIN);				break;
	case msg_motiongate_confirm_admin_on:	strInformation.LoadString(IDS_STATUS_CHECK_ADMIN_ON);			break;
	case msg_motiongate_exit_admin:			strInformation.LoadString(IDS_STATUS_EXIT_ADMIN);				break;
	case msg_motiongate_confirm_admin_off:	strInformation.LoadString(IDS_STATUS_CHECK_ADMIN_OFF);			break;
	case msg_motiongate_change_system_info:	strInformation.LoadString(IDS_STATUS_CHANGE_NAME);				break;
	case msg_change_motor_id:				strInformation.LoadString(IDS_STATUS_CHANGE_ID);				break;
	case msg_scan_slave_progress:			strInformation.LoadString(IDS_STATUS_SCAN_SLAVE);				break;
	case msg_read_parameter:				strInformation.LoadString(IDS_STATUS_READ_PARAMETER);			break;
	case msg_write_parameter:				strInformation.LoadString(IDS_STATUS_WRITE_PARAMETER);			break;
	case msg_save_parameter:				strInformation.LoadString(IDS_STATUS_SAVE_PARAMETER);			break;
	case msg_compare_parameter:				strInformation.LoadString(IDS_STATUS_COMPARE_PARAMETER);		break;
	case msg_read_position:					strInformation.LoadString(IDS_STATUS_READ_POSITION_TABLE);		break;
	case msg_write_position:				strInformation.LoadString(IDS_STATUS_WRITE_POSITION_TABLE);		break;
	case msg_save_position:					strInformation.LoadString(IDS_STATUS_SAVE_POSITION_TABLE);		break;
	case msg_load_position:					strInformation.LoadString(IDS_STATUS_LOAD_POSITION_TABLE);		break;
	case msg_compare_position:				strInformation.LoadString(IDS_STATUS_COMPARE_POSITION_TABLE);	break;
	case msg_backup_start:					strInformation.LoadString(IDS_STATUS_BACKUP_START);				break;
	case msg_backup_done:					strInformation.LoadString(IDS_STATUS_BACKUP_DONE);				break;
	case msg_backup_fail:					strInformation.LoadString(IDS_STATUS_BACKUP_FAIL);				break;
	case msg_restoration_start:				strInformation.LoadString(IDS_STATUS_RESTORATION_START);		break;
	case msg_restoration_done:				strInformation.LoadString(IDS_STATUS_RESTORATION_DONE);			break;
	case msg_restoration_fail:				strInformation.LoadString(IDS_STATUS_RESTORATION_FAIL);			break;
	case msg_factory_reset_start:			strInformation.LoadString(IDS_STATUS_RESET_START);				break;
	case msg_factory_reset_done:			strInformation.LoadString(IDS_STATUS_RESET_DONE);				break;
	case msg_factory_reset_fail:			strInformation.LoadString(IDS_STATUS_RESET_FAIL);				break;
	case msg_initialize_start:				strInformation.LoadString(IDS_STATUS_INIT_START);				break;
	case msg_initialize_done:				strInformation.LoadString(IDS_STATUS_INIT_DONE);				break;
	case msg_initialize_fail:				strInformation.LoadString(IDS_STATUS_INIT_FAIL);				break;
	case msg_homing_all_start:				strInformation.LoadString(IDS_STATUS_HOMING_ALL_START);			break;
	case msg_homing_all_done:				strInformation.LoadString(IDS_STATUS_HOMING_ALL_DONE);			break;
	case msg_homing_all_fail:				strInformation.LoadString(IDS_STATUS_HOMING_ALL_FAIL);			break;
	/*case msg_life_standard_detected:		strInformation.LoadString(IDS_STATUS_LIFE_STAND);			break;
	case msg_life_cycle_detected:			strInformation.LoadString(IDS_STATUS_LIFE_CYCLE);			break;*/
	}

	strMessage = strHeader + strInformation;

	if (_tcscmp(pElement->GetText(), strMessage))
	{
		pElement->SetText(strMessage);

		m_wndStatusBar.RecalcLayout();
		m_wndStatusBar.Invalidate(FALSE);

#if defined(LOG_START)
		if (pMsg->bEnableLog)
		{// 실제 이전값과 같을때 Log를 진행하라는 명령을 준다.
			//this->UpdateOperationLog(pMsg, strInformation);
			// 영향 X
		}
		
#endif
	}
}

void CMainFrame::UpdateCaptionBar(PST_OPERAION_MESSAGE pMsg)
{
	CString strFormat, strInformation;
	switch (pMsg->nMsgID)
	{
	case msg_backup_done:				strFormat.LoadString(IDS_CAPTION_BAR_BACKUP_DONE);				strInformation.Format(strFormat, pMsg->pDevice->GetBackupName());	break;
	case msg_backup_fail:				strInformation.LoadString(IDC_CAPTION_BAR_BACK_FAIL);			break;
	case msg_backup_stop:				strInformation.LoadString(IDC_CAPTION_BAR_BACK_STOP);			break;
	case msg_restoration_done:			strFormat.LoadString(IDS_CAPTION_BAR_RESTORATION_DONE);			break;
	case msg_restoration_fail:			strInformation.LoadString(IDC_CAPTION_BAR_RESTORATION_FAIL);	break;
	case msg_restoration_stop:			strInformation.LoadString(IDC_CAPTION_BAR_RESTORATION_STOP);	break;
	case msg_factory_reset_done:		strInformation.LoadString(IDC_CAPTION_BAR_RESET_DONE);			break;
	case msg_factory_reset_fail:		strInformation.LoadString(IDC_CAPTION_BAR_RESET_FAIL);			break;
	case msg_factory_reset_stop:		strInformation.LoadString(IDC_CAPTION_BAR_RESET_STOP);			break;
	case msg_initialize_done:			strInformation.LoadString(IDC_CAPTION_BAR_INITIALIZE_DONE);		break;
	case msg_initialize_fail:			strInformation.LoadString(IDC_CAPTION_BAR_INITIALIZE_FAIL);		break;
	case msg_initialize_stop:			strInformation.LoadString(IDC_CAPTION_BAR_INITIALIZE_STOP);		break;
	case msg_homing_all_done:			strInformation.LoadString(IDC_CAPTION_BAR_HOMING_DONE);			break;
	case msg_homing_all_fail:			strInformation.LoadString(IDC_CAPTION_BAR_HOMING_FAIL);			break;
	case msg_homing_all_stop:			strInformation.LoadString(IDC_CAPTION_BAR_HOMING_STOP);			break;
	case msg_motiongate_change_system_done:	strInformation.LoadString(IDC_CAPTION_BAR_CHANGE_NAME_DONE);	break;
	case msg_motiongate_change_system_fail:	strInformation.LoadString(IDC_CAPTION_BAR_CHANGE_NAME_FAIL);	break;
	default:							return;
	case msg_motiongate_check_status:
	{
		if (pMsg->pDevice->IsMotiongateAlarmEnabled())
		{
			strInformation.LoadString(IDC_CAPTION_BAR_ALARM);
		}
		else if (pMsg->pDevice->IsInvalidMotorConnected())
		{
			strInformation.LoadString(IDC_CAPTION_BAR_INVALID_MOTOR);
		}
		else
		{
			return;
		}
	}	break;
	}

	LPCTSTR lpszResouceName = NULL;
	switch (pMsg->nLevel)
	{
	case msgNormal:		lpszResouceName = IDI_INFORMATION;	break;
	case msgWarning:	lpszResouceName = IDI_WARNING;		break;
	case msgError:		lpszResouceName = IDI_ERROR;		break;
	}

	if (lpszResouceName)
	{
		m_wndCaptionBar.SetIcon(lpszResouceName, RGB(0x00, 0x00, 0x00), FALSE, CMFCCaptionBar::ALIGN_LEFT);
	}

	CString strAddress, strMessage;
	pMsg->pDevice->GetAddress(strAddress);
	strMessage.Format(_T("[ %s - %s ] %s"), pMsg->pDevice->GetSystemName(), strAddress, strInformation);

	m_wndCaptionBar.SetText(strMessage, CMFCCaptionBar::ALIGN_LEFT);

	if (FALSE == m_wndCaptionBar.IsVisible())
	{
		m_wndCaptionBar.ShowWindow(SW_SHOW);
	}

	RecalcLayout(FALSE);
	m_wndCaptionBar.EnableAutoClose();
}

void CMainFrame::OnBnClickedBtnBackupSchedule()
{
	// CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl(); 권한 수정..
	CDeviceMotionGate* pDevice = GetSelectedMotionGateControl();
	if (NULL == pDevice)
	{
		return;
	}

	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();
	CString strCaption(_T("Backup Schedule"));

	if ((TRUE == pConfigDB->IsControlGranted()) ||
		(TRUE == RequestElevatedPermissions(strCaption) &&
			TRUE == pConfigDB->IsControlGranted()))
	{
		CDlgSchedule dlg;
		dlg.SetAddress(pDevice->GetAddress());
		dlg.DoModal();
		
		//CFASPERGUIApp* pApp = static_cast<CFASPERGUIApp*>(AfxGetApp());
		//POSITION posTemp = pApp->GetFirstDocTemplatePosition();
		//const DWORD dwAddress = pDevice->GetAddress();
		//while (posTemp)
		//{
		//	CMultiDocTemplateEx* pDocTemplate = static_cast<CMultiDocTemplateEx*>(pApp->GetNextDocTemplate(posTemp));
		//	if (pDocTemplate->IsMatchedTemplate(RUNTIME_CLASS(CDocDevice), RUNTIME_CLASS(CMDIChildWndEx), RUNTIME_CLASS(CDlgSchedule)))
		//	{
		//		//	이미 열린 View가 있는지 확인
		//		POSITION posDoc = pDocTemplate->GetFirstDocPosition();
		//		while (posDoc)
		//		{
		//			CDocument* pDoc = pDocTemplate->GetNextDoc(posDoc);
		//			POSITION posView = pDoc->GetFirstViewPosition();
		//			CDlgSchedule* pView = static_cast<CDlgSchedule*>(pDoc->GetNextView(posView));

		//			if (pView && dwAddress == static_cast<CDocDevice*>(pView->GetDocument())->GetAddress())
		//			{
		//				static_cast<CMDIChildWndEx*>(pView->GetParent())->ActivateFrame();
		//				return;
		//			}
		//		}

		//		CDocDevice* pDoc = static_cast<CDocDevice*>(pDocTemplate->OpenDocumentFile(NULL));
		//		pDoc->SetAddress(dwAddress);
		//		pDoc->SetDeviceTitle(strCaption);
		//	}
		//}
	}
	else
	{
		ShowRequireElevatedPermissions(strCaption);
	}
}

void CMainFrame::OnUpdateViewAutoDatetime(CCmdUI* pCmdUI)
{
	CString strText;

	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();
	if (pDevice)
	{
		if (pDevice->IsMotiongateConnected() && pDevice->IsAutoRestorationEnabled())
		{
			COleDateTime dateTime;
			if (pDevice->GetAutoBackupDatetime(dateTime))
			{
				strText = dateTime.Format(_T("%c"));
			}

			pCmdUI->Enable();
		}
		else
		{
			strText = _T("Stopped.");
		}
	}

	CMFCRibbonEdit* pEdit = static_cast<CMFCRibbonEdit*>(m_wndRibbonBar.FindByID(IDP_EDIT_AUTO_DATETIME));

	m_wndRibbonBar.SetEditText(IDP_EDIT_AUTO_DATETIME, strText);
}

void CMainFrame::OnUpdateViewAutoSeconds(CCmdUI* pCmdUI)
{
	CString strText;
	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();
	if (pDevice)
	{
		if (pDevice->IsMotiongateConnected() && pDevice->IsAutoRestorationEnabled())
		{
			const DOUBLE dbLeftSecond = pDevice->GetAutoBackupLeftSeconds();

			if (86400 <= dbLeftSecond)	// 60 * 60 * 24
			{
				strText.Format(_T("It starts in %d day(s)"), static_cast<INT>(dbLeftSecond) / 86400);
			}
			else if (3600 <= dbLeftSecond)	// 60 * 60
			{
				strText.Format(_T("It starts in %d Hour(s)"), static_cast<INT>(dbLeftSecond) / 3600);
			}
			else if (60 <= dbLeftSecond)
			{
				strText.Format(_T("It starts in %d Minute(s)"), static_cast<INT>(dbLeftSecond) / 60);
			}
			else if (0 < dbLeftSecond)
			{
				strText.Format(_T("It starts in %d Second(s)"), static_cast<INT>(dbLeftSecond));
			}
		}
	}

	m_wndRibbonBar.SetLabelText(IDC_ST_AUTO_SECOND, strText);
}


void CMainFrame::OnPaint()
{
	CPaintDC dc(this);

	/*ShowWindow(SW_SHOWMAXIMIZED);
	UpdateWindow();*/
	static int nCnt = 0;
	CRect rect;
	m_wndClientArea.GetClientRect(rect);

	CDC* pDC = m_wndClientArea.GetDC();
	
// KKB
	CRect rectRibbon;
	CRect rectOutlook;
	m_wndRibbonBar.GetClientRect(rectRibbon);
	m_wndOutlookDevice.GetClientRect(rectOutlook);

	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	m_nMotionGateCnt = pDeviceList->GetAddressCount();
	
	m_pWndMotionManager->SetMotionCount(m_nMotionGateCnt); // 그릴때마다 현재 MotionGate의 개수를 가져온다.
	m_pWndMotionManager->SetWindowPos(NULL, rectOutlook.Width() + 5, rectRibbon.Height(), rect.Width(), rect.Height(), SWP_NOZORDER);

	::ReleaseDC(m_wndClientArea.m_hWnd, pDC->m_hDC);
}


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if (WM_PAINT == pMsg->message)
	{
		if (0 && pMsg->hwnd == m_hWndMDIClient)
		{
			Invalidate(FALSE);
		}
	}

	return CMDIFrameWndEx::PreTranslateMessage(pMsg);
}

void CMainFrame::OnBnClickedBtnDeviceBackupData()
{
	// CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl(); 권한 수정..
	CDeviceMotionGate* pDevice = GetSelectedMotionGateControl();
	CString str;
	pDevice->GetAddress(str);
	
	TRACE(L"%s", str);
	if (NULL == pDevice)
	{
		return;
	}

	pDevice->PauseAutoBackup();
	 
	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();

	CString strCaption(_T("Backup"));

	if ((TRUE == pConfigDB->IsDeviceBackupGranted()) ||
		(TRUE == RequestElevatedPermissions(strCaption) &&
			TRUE == pConfigDB->IsDeviceBackupGranted()))
	{
		CDlgManageBackup dlg;
		dlg.SetAddress(GetSelectedDeviceAddress());
		dlg.SetSystemControl(FALSE);
		if (IDOK == dlg.DoModal())
		{
			POSITION pos = dlg.GetDeleteItemStartPosition();

			CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();
			CString strProjectName(pDevice->GetSystemName());

			if (pos)
			{
				if (pProjectDB->OpenDataBase())
				{
					pProjectDB->Begin();
					while (pos)
					{
						INT nBackupID = dlg.GetNextDeleteItem(pos);
						pProjectDB->DeleteBackupData(nBackupID, strProjectName);
						pProjectDB->DeleteBackupList(nBackupID, strProjectName);
					}

					pProjectDB->Commit();

					pProjectDB->DoVacuum();

					pProjectDB->CloseDataBase();
				}
			}

			if (dlg.IsNominalSelectionChanged())
			{
				pProjectDB->ChangeNominalBackupID(dlg.GetSelectedNominalID(), strProjectName);
			}

			if (FALSE == dlg.IsExistLastestBackupID())
			{
				pProjectDB->RemoveLastestBackupID(strProjectName);
			}
		}
	}
	else
	{
		pDevice->PauseAutoBackup(FALSE);

		ShowRequireElevatedPermissions(strCaption);
	}

	pDevice->PauseAutoBackup(FALSE);
}

void CMainFrame::OnUpdateOperator(CCmdUI* pCmdUI)
{
	m_wndRibbonBar.SetEditText(IDC_ST_OPERATOR, CDataBaseConfig::GetInstance()->GetOperatorName());
	pCmdUI->Enable();
}

void CMainFrame::OnDestroy()
{
	if (m_pWndMotionManager->DestroyWindow())
	{
		delete m_pWndMotionManager;
	}

	CMDIFrameWndEx::OnDestroy();

	m_wndRibbonBar.DestroyWindow();
}

LRESULT CMainFrame::OnMsgShowApplyAlias(WPARAM wParam, LPARAM lParam)
{
	//	이걸 어따 쓸까?
	return 0;
}

void CMainFrame::OnBnClickedMenuOperationRun()
{
	if (FALSE == CheckControlGranted())
	{
		return;
	}

	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();
	if (NULL == pDevice)
	{
		return;
	}

	if (FALSE == pDevice->IsRunning())
	{
		pDevice->StartThread();
	}
}

void CMainFrame::OnBnClickedMenuOperationStop()
{
	if (FALSE == CheckControlGranted())
	{
		return;
	}

	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();
	if (NULL == pDevice)
	{
		return;
	}

	if (TRUE == pDevice->IsRunning())
	{
		pDevice->StopThread();
	}
}

void CMainFrame::OnBnClickedMenuSchedule()
{
	if (FALSE == CheckControlGranted())
	{
		return;
	}

	OnBnClickedBtnBackupSchedule();
}

void CMainFrame::OnBnClickedMenuConfiguration()
{
	
	// 기존
	/*if (FALSE == CheckControlGranted())
	{
		return;
	}

	EditSingleSystemConfiguration(GetSelectedDeviceAddress());*/

	CString strCaption(_T("System Control"));
	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();
	CDeviceMotionGate* pDevice = GetSelectedMotionGateControl();
	if ((TRUE == pConfigDB->IsDeviceConfigGranted()) ||
		(TRUE == RequestElevatedPermissions(strCaption) &&
			TRUE == pConfigDB->IsDeviceConfigGranted()))
	{
		//EditSingleSystemConfiguration(GetSelectedDeviceAddress()); 권한 수정..
		EditSingleSystemConfiguration(pDevice->GetAddress());
	}
	else
	{
		ShowRequireElevatedPermissions(strCaption);
	}

}

void CMainFrame::OnBnClickedMenuDeviceStop()
{
	OnBnClickedBtnStopExecution();
}

void CMainFrame::OnBnClickedMenuDeviceBackup()
{
	OnBnClickedBtnDeviceBackup();
}

void CMainFrame::OnBnClickedMenuDeviceRestore()
{
	OnBnClickedBtnDeviceRestoration();
}

void CMainFrame::OnBnClickedMenuDeviceAssignment()
{
	OnBnClickedBtnDeviceIdAssignment();
}

void CMainFrame::OnBnClickedMenuFactoryReset()
{
	OnBnClickedBtnDeviceFactoryReset();
}

void CMainFrame::OnBnClickedMenuDeviceOperation()
{
	OnBnClickedBtnMotionTest();
}

void CMainFrame::OnBnClickedPanelConfig()
{
	if (FALSE == CheckControlGranted())
	{
		return;
	}

	if (CDeviceList::GetInstance()->IsStartMonitoring())
	{
		CString message("If you want to use 'System Configuration', please stop system monitoring.");
		CString mainInstruction("Failed to execute 'System Configuration'.");
		CString title(_T("System monitoring stop is required."));

		CTaskDialog taskDialog(message, mainInstruction, title, !(TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON));

		taskDialog.AddCommandControl(IDYES, _T("OK, I understand."));

		taskDialog.SetMainIcon(TD_ERROR_ICON);

		taskDialog.SetDialogWidth(300);

		taskDialog.DoModal();

		return;
	}
	

	CSheetConfig dlg;
	CString strUser = _T("");


	CDeviceMotionGate* pDevice = CDeviceList::GetInstance()->GetMotiongateForSerial();
	pDevice->EnterEditMode();

	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();
	const INT nCurInterface = pConfigDB->GetInterface();

	if (IDOK == dlg.DoModal())
	{
		pDevice->ExitEditMode(FALSE);

		if (pConfigDB->GetInterface() == nCurInterface)
		{
			m_wndOutlookDevice.LoadDeviceInformation();
		}

		if (1)
		{ // 만일 User Control 설정을 변경하였을 경우 Imform Class에서 알맞게 변경할 수 있게끔 db에서 ReLoad를 진행하여준다.
			if (pConfigDB->IsAdministrator()) strUser = _T("Administrator");
			if (pConfigDB->IsEngineer()) strUser = _T("Engineer");
			if (pConfigDB->IsOperator()) strUser = _T("Operator");
			if (_T("") != strUser)
			{
				pConfigDB->ReLoadAuthorizeControl(strUser);
			}
		}

	}
	else
	{
		pDevice->ExitEditMode();
	}
}

const BOOL CMainFrame::RequestElevatedPermissions(LPCTSTR lpszCaption)
{
	CString message("");
	CString mainInstruction("This task requires current user to have elevated permissions.");
	CString title(lpszCaption ? lpszCaption : _T("Require permissions"));

	CTaskDialog taskDialog(message, mainInstruction, title, !TDCBF_CANCEL_BUTTON);

	if (CDataBaseConfig::GetInstance()->IsDefaultOperator())
	{
		taskDialog.AddCommandControl(IDYES, _T("Log in with a ID that has the appropriate permissions."), TRUE, TRUE);
	}
	else
	{
		taskDialog.AddCommandControl(IDYES, _T("Re Log in with a another ID that has the appropriate permissions."), TRUE, TRUE);
	}

	taskDialog.AddCommandControl(IDNO, L"No, cancel the task.");

	taskDialog.SetMainIcon(TD_SHIELD_ICON);

	taskDialog.SetDialogWidth(300);

	// Change the default command button control
	taskDialog.SetDefaultCommandControl(IDYES);

	if (IDYES == taskDialog.DoModal())
	{
		if (DoLoginSequence(TRUE))
		{
			return TRUE;
		}
	}

	return FALSE;
}

const BOOL CMainFrame::DoLoginSequence(const BOOL bEnableReLogin)
{
	BOOL bResult = TRUE;
	
	CMFCRibbonButton* pButton = static_cast<CMFCRibbonButton*>(m_wndRibbonBar.FindByID(ID_PANEL_USER));
	CString strText(pButton->GetText());
	CString strStatus;
	BOOL	bEnableLog = FALSE;
	BOOL	bLogin = FALSE;
	CDataBaseConfig* pConfig = CDataBaseConfig::GetInstance();

	if (FALSE == bEnableReLogin && FALSE == pConfig->IsDefaultOperator())
	{
		strText = _T("LOG IN");
		pConfig->ChangeDefaultUser();

		ChangePanelButtonImage(ID_PANEL_USER, idx_log_in);
	
		if (1)
		{
			strStatus = _T("Log Out");
			bLogin = FALSE;
			bEnableLog = TRUE;
		}
	}
	else
	{ 
		CSheetLogin dlg;
		dlg.SetGroupMode();
		if (IDOK == dlg.DoModal())
		{ 
			strText = _T("LOG OUT");

			ChangePanelButtonImage(ID_PANEL_USER, idx_log_out);

			if (1)
			{
				if (pConfig->IsAdministrator()) strStatus.Format(_T("[Administrator Mode] Log In"));

				else if (pConfig->IsEngineer()) strStatus.Format(_T("[Engineer Mode] Log In"));

				else if (pConfig->IsOperator())	strStatus.Format(_T("[Operator Mode] Log In"));
				
				bEnableLog = TRUE;
				bLogin = TRUE;
			}
		}
		else
		{
			bResult = FALSE;
		}
	}
	
	if (strText.GetLength() && strText != pButton->GetText())
	{
		pButton->SetText(strText);
		pButton->Redraw();
	}

#if defined(LOG_START)
	if (1)
	{ // db Log 저장
		PST_OPERAION_MESSAGE pMsg;
		CTime time = CTime::GetCurrentTime();
		CString strTime;
		strTime.Format(_T("%04d-%02d-%02d  %02d:%02d:%02d"),
			time.GetYear(), time.GetMonth(), time.GetDay(),
			time.GetHour(), time.GetMinute(), time.GetSecond());

		if (bEnableLog)
		{
			CDataBaseLog::GetInstance()->InsertLog(0, strStatus/*bLogin ? _T("Log In") : _T("Log Out")*/);
			m_paneOperationLog.SaveLogList(0/*Level*/, strTime, _T("SYSTEM"), L"", strStatus, time);
		}
	}
#endif

	return bResult;
}

void CMainFrame::ShowRequireElevatedPermissions(LPCTSTR lpszCaption)
{
	CString message("You need appropriate permissions to execute this task.");
	CString mainInstruction("Failed to execute this task.");
	CString title(lpszCaption ? lpszCaption : _T("Appropriate permissions are required."));

	CTaskDialog taskDialog(message, mainInstruction, title, !(TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON));

	taskDialog.AddCommandControl(IDYES, _T("OK, I understand."));

	taskDialog.SetMainIcon(TD_ERROR_ICON);

	taskDialog.SetDialogWidth(300);

	taskDialog.DoModal();

	return;
}

BOOL CMainFrame::CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, int nInitialWidth)
{
	bar.SetMode2003();

	BOOL bNameValid;
	CString strTemp;
	bNameValid = strTemp.LoadString(IDS_SHORTCUTS);
	ASSERT(bNameValid);
	if (!bar.Create(strTemp, this, CRect(0, 0, nInitialWidth, 32000), uiID, WS_CHILD | WS_VISIBLE | CBRS_LEFT))
	{
		return FALSE; // fail to create
	}

	CMFCOutlookBarTabCtrl* pOutlookBar = (CMFCOutlookBarTabCtrl*)bar.GetUnderlyingWindow();
	
	if (pOutlookBar == nullptr)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	pOutlookBar->EnableInPlaceEdit(FALSE);

	// can float, can autohide, can resize, CAN NOT CLOSE
	DWORD dwStyle = AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE | AFX_CBRS_RESIZE;

	CRect rectDummy(0, 0, 0, 0);

	m_wndOutlookDevice.Create(rectDummy, &bar, 1200);
	bNameValid = strTemp.LoadString(IDS_DEVICE_INFO);
	ASSERT(bNameValid);
	
	// KKB OutlookBar 삭제시 아래 구문 주석처리..
	pOutlookBar->AddControl(&m_wndOutlookDevice, strTemp, 0, TRUE, dwStyle);

	bar.SetPaneStyle(bar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	if (1)
	{
		pOutlookBar->SetImageList(IDB_PAGES_HC, 24);
		pOutlookBar->SetToolbarImageList(IDB_PAGES_SMALL_HC, 16);
	}
	//	else
	//	{
	//		pOutlookBar->SetImageList(IDB_PAGES, 24);
	//		pOutlookBar->SetToolbarImageList(IDB_PAGES_SMALL, 16);
	//	}

	pOutlookBar->RecalcLayout();

	BOOL bAnimation = theApp.GetInt(_T("OutlookAnimation"), TRUE);
	CMFCOutlookBarTabCtrl::EnableAnimation(bAnimation);

	bar.SetButtonsFont(&afxGlobalData.fontRegular);

	return TRUE;
}

void CMainFrame::NoticeNoChangeValue()
{
	CString strContent("");
	CString strMainInstruction("The number of device motors cannot be synchronized with the number of database motors.");
	CString strTitle("Can not execute configuration!");
	CString strMessage;
	
	CTaskDialog taskDlg(strContent, strMainInstruction, strTitle, TDCBF_OK_BUTTON);
	
	taskDlg.SetMainIcon(TD_ERROR_ICON);

	taskDlg.SetDialogWidth(300);
	taskDlg.DoModal();
}


const INT CMainFrame::ConfirmModifyDatabase()
{
	/// <summary>
	/// 사용자에게 System의 이름을 바꿀 때 DB도 함께 바꿀지 문의한다.
	/// </summary>
	/// <returns>
	/// 1	: Apply to database
	///	0	: Don't apply to database
	/// -1	: cancel this method
	/// </returns>

	typedef enum _COMMAND_CONTROL_ID
	{
		CMD_YES = 1000,
		CMD_NO,
	}	COMMAND_CONTROL_ID;

	CString message("");
	CString mainInstruction("Do you want to change related data in the database?");
	CString title(_T("Confirm to edit the system database"));

	CTaskDialog taskDialog(message, mainInstruction, title, !TDCBF_CANCEL_BUTTON);

	taskDialog.AddCommandControl(CMD_YES, _T("Yes, not only change system information but also change database."));
	taskDialog.AddCommandControl(CMD_NO, _T("No, just change system information of module."));

	taskDialog.SetDefaultCommandControl(CMD_YES);

	taskDialog.SetMainIcon(IDI_QUESTION);

	taskDialog.SetDialogWidth(300);

	taskDialog.SetFooterText(_T("All data of database is associate with system name."));
	taskDialog.SetFooterIcon(TD_WARNING_ICON);

	INT nResult = -1;
	switch (taskDialog.DoModal())
	{
	case CMD_YES:	nResult = 1;	break;
	case CMD_NO:	nResult = 0;	break;
	}

	return nResult;
}

const INT CMainFrame::ConfirmReboot()
{
	/// <summary>
	///	사용자에게 System을 재부팅할 것인지 문의한다.
	/// </summary>
	/// <returns>
	/// TRUE	:	Reboot the system.
	/// FALSE	:	cancel this method.
	/// </returns>

	typedef enum _RETURN_CODE
	{
		ret_cancel = -1,
		ret_no = 0,
		ret_yes = 1,
	}	RETURN_CODE;


	typedef enum _COMMAND_CONTROL_ID
	{
		CMD_YES = 1000,
		CMD_NO,
	}	COMMAND_CONTROL_ID;

	CString message("Would you reboot this system now?");
	CString mainInstruction("Some properties like 'system name' take effect after rebooting the system.");
	CString title(_T("Confirm to reboot the system property"));

	CTaskDialog taskDialog(message, mainInstruction, title, !TDCBF_CANCEL_BUTTON);

	taskDialog.AddCommandControl(CMD_YES, _T("Yes, reboot the current system now."));
	taskDialog.AddCommandControl(CMD_NO, _T("No, cancel apply changes properties."));

	taskDialog.SetDefaultCommandControl(CMD_YES);

	taskDialog.SetMainIcon(IDI_QUESTION);

	taskDialog.SetDialogWidth(350);

	INT nResult = FALSE;
	switch (taskDialog.DoModal())
	{
	case CMD_YES:	nResult = TRUE;	break;
	}

	return nResult;
}

const BOOL CMainFrame::CheckControlGranted()
{
	CString strCaption(_T("System Control"));
	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();

	const BOOL bGranted =
		(TRUE == pConfigDB->IsControlGranted()) ||
		(TRUE == RequestElevatedPermissions(strCaption) && TRUE == pConfigDB->IsControlGranted());

	if (FALSE == bGranted)
	{
		ShowRequireElevatedPermissions(strCaption);
		return FALSE;
	}

	return TRUE;
}

const BOOL CMainFrame::ChangePanelButtonImage(const INT nPanelID, const INT nIndex, const BOOL bLargeImage)
{
	CMFCRibbonButton* pButton = static_cast<CMFCRibbonButton*>(m_wndRibbonBar.FindByID(nPanelID));

	if (pButton->GetImageIndex(bLargeImage) != nIndex)
	{
		pButton->SetImageIndex(nIndex, bLargeImage);
		pButton->Redraw();
	}

	return TRUE;
}

const BOOL CMainFrame::EditSingleSystemConfiguration(const DWORD dwAddress)
{
	CDeviceMotionGate* pDevice = NULL;
	CString strTitle;
	if (FALSE == CDeviceList::GetInstance()->GetMotiongateByAddress(dwAddress, pDevice))
	{
		return FALSE;
	}

	BOOL bResult = FALSE;

	pDevice->EnterEditMode();
	 
	CDlgDeviceTotalConfiguration dlg;
	dlg.SetMotiongateCtrl(pDevice);
	dlg.SetNoticeConfig(FALSE);
	dlg.SetTotalConfig(FALSE); 

	strTitle.Format(_T("Tooling Configuration"));
	dlg.SetTitleText(strTitle);
	dlg.SetConfigMethod(require_device_modify);

	/*CSheetConfigDeviceSingle dlg;
	CString strTitle;
	dlg.SetMotiongateCtrl(pDevice);*/
	
	if (IDOK == dlg.DoModal())
	{
		//ConfirmModifyDatabase -> 'X' 버튼을 누르면 아무것도 안하고 끝낸다.
		INT nModifyDatabase = 1; //ConfirmModifyDatabase(); 기존.
		
		//BOOL bConfirmReboot = ConfirmReboot();
		if (-1 == nModifyDatabase ||
			FALSE == ConfirmReboot())
		{
			pDevice->ExitEditMode(FALSE);
			return FALSE;
		}

		// pDevice->ExitEditMode(); 기존..

		if (nModifyDatabase)
		{ 
			/*db에 적용할 경우*/
		    			
			// 1-1. 변경할 이름이 db에 있을경우 db에 해당 항목 내용 변경 실시
			// 1-2. 변경할 이름이 기존 db에 없을경우 db에 신규 등록
			
			CDeviceList::GetInstance()->RemoveDeviceIDKey(pDevice->GetAddress());

			if (TRUE == pDevice->IsDataBaseExist(pDevice->GetEditSystemName()))
			{ // 1-1. 변경할 이름이 db에 있을경우 해당 항목 내용 변경 실시
				pDevice->UpdateChangeProject();
			}
			else
			{ 
			  //1 - 2. 변경할 이름이 기존 db에 없을경우 db에 신규 등록
				pDevice->InsertNewProjectDatabase();
			}
			pDevice->ExitEditMode();
			
		}
		else
		{ // 우선 여기 들어올일 없음..
			/*db에 적용 하지 않을경우.. 굳이 이경우를 생각할 필요..?*/
			
			if (TRUE == pDevice->IsDataBaseExist(pDevice->GetEditSystemName()))
			{
				pDevice->ExitEditMode();
			}
			else
			{
				pDevice->InitDatabase();
				pDevice->InsertNewProjectDatabase();
			}
			
		}

//		pDevice->ExitEditMode();

		if (FALSE == pDevice->CompareMotiongateInfo())
		{
			pDevice->ChangeSystemInformation();
		}
		else
		{
			pDevice->RebootSystem();
		}
		
		bResult = TRUE;
	}
	else
	{
		pDevice->ExitEditMode(FALSE);
	}

	return bResult;
}





const BOOL CMainFrame::DoMGFactoryReset(const DWORD dwAddress, const BOOL bAct)
{
	CDeviceMotionGate* pDevice = NULL;
	BOOL bResult = FALSE;

	
	CString message("Would you reboot this system now?");
	CString mainInstruction("EZI-MOTIONGATE Factory Reset take effect after rebooting the system.");
	CString title(_T("Confirm to reboot the system"));

	CTaskDialog taskDialog(message, mainInstruction, title, TDCBF_CANCEL_BUTTON);

	taskDialog.AddCommandControl(IDYES, _T("Yes, reboot the current system now."));
	taskDialog.AddCommandControl(IDNO, _T("No, cancel factory reset"));

	taskDialog.SetDefaultCommandControl(IDYES);

	taskDialog.SetMainIcon(IDI_QUESTION);

	taskDialog.SetDialogWidth(350);


	if (FALSE == CDeviceList::GetInstance()->GetMotiongateByAddress(dwAddress, pDevice))
	{
		AfxMessageBox(_T("Read Address Fail"));
		bResult = FALSE;
	}

	switch (taskDialog.DoModal())
	{
	case IDYES:
	{
		if (bAct)
		{

			pDevice->DoMGFactoryReset();

			if (FALSE == pDevice->CompareMotiongateInfo())
			{
				pDevice->ChangeSystemInformation();
			}
			else
			{
				pDevice->RebootSystem();
			}

			bResult = TRUE;
		}
	}	break;
	}

	

	return bResult;
}

void CMainFrame::OnBnClickedBtnSystemOption()
{
	CDlgSystemOption dlg;
	dlg.DoModal();
}

void CMainFrame::OnUpdateBtnSystemOption(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}
// IDP_BTN_SELECT

const BOOL CMainFrame::CreateModBusServer()
{
	CModbusServer* pServer = CModbusServer::GetInstance();
	if (NULL == pServer)
	{
		return FALSE;
	}
	else
	{
		pServer->Listen(502);
	}

	return TRUE;
}


void CMainFrame::OnBnClickedBtnDeviceSelectRestoration()
{
	
}


void CMainFrame::OnUpdateBtnDeviceSelectRestoration(CCmdUI* pCmdUI)
{

}

void CMainFrame::OnBtnDeviceRestorationSelect()
{
	// CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl(); 권한 수정..
	CDeviceMotionGate* pDevice = GetSelectedMotionGateControl();
	if (NULL == pDevice)
	{
		return;
	}

	pDevice->PauseAutoBackup();

	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();

	CString strCaption(_T("Selected Restoration"));
	BOOL bAutoBackup = TRUE;

	if ((TRUE == pConfigDB->IsDeviceRestorationGranted()) ||
		(TRUE == RequestElevatedPermissions(strCaption) &&
			TRUE == pConfigDB->IsDeviceRestorationGranted()))
	{
		//CSheetRestoration propSheet;
		CSheetSelectRestoration propSheet;
		propSheet.SetCaption(_T("System Selected Restoration"));
		propSheet.SetAddress(pDevice->GetAddress());

		if (IDOK == propSheet.DoModal())
		{
			bAutoBackup = FALSE;
			pDevice->SetProjectName(pDevice->GetSystemName());
			pDevice->SelectBackupID(propSheet.GetProjectBackupID());
			//pDevice->EnableAutoHoming(propSheet.IsAutoHomingModeEnabled());
			pDevice->EnableAutoHoming(FALSE); // 그전에 갑자기 장치가 죽어서 Enable Homing 가동 상태로 끝나있을수도 있다.
			
			pDevice->KeepStopAutoBackup(TRUE);

			CDlgRestoreHomingAlarm alarm;
			alarm.SetMotionGate(pDevice);
			
			// 추후 진행 사항
			for (INT nAxis = 1; nAxis < 9; nAxis++)
			{
				if (pDevice->IsMotorConnected(nAxis))
				{
					pDevice->MoveStop(nAxis);
				}
			}

			//pDevice->StartRestoration(); // 추후 날려야함..

			if (IDOK == alarm.DoModal())
			{
				CDlgHomingControlPanel homing;
				homing.SetMotionGate(pDevice);

				if (IDOK == homing.DoModal())
				{

				}
			}
			
			pDevice->KeepStopAutoBackup(FALSE);

			pDevice->EnableAutoRestoration(TRUE); // Auto Backup 금지..
		}
	}
	else
	{
		pDevice->PauseAutoBackup(FALSE);

		ShowRequireElevatedPermissions(strCaption);
	}

	pDevice->PauseAutoBackup(FALSE);
}


void CMainFrame::OnUpdateBtnDeviceRestorationSelect(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;

	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();

	if (pDevice)
	{
		bEnable = (pDevice->IsMotiongateControllable() && pDevice->IsMotorConnectionContinuity());
	}

	pCmdUI->Enable(bEnable);
}

void CMainFrame::OnBnClickedBtnDeviceReboot()
{
	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();
	CString strCaption(_T("Reboot System"));

	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();
	pDevice->PauseAutoBackup();

	if ((TRUE == pConfigDB->IsDeviceBackupGranted()) ||
		(TRUE == RequestElevatedPermissions(strCaption) &&
			TRUE == pConfigDB->IsDeviceBackupGranted()))
	{
		CString message("");
		CString mainInstruction("Would you reboot this system now?");
		CString title(_T("Confirm to reboot the system"));

		CTaskDialog taskDialog(message, mainInstruction, title, TDCBF_CANCEL_BUTTON);

		taskDialog.AddCommandControl(IDYES, _T("Yes"));
		taskDialog.AddCommandControl(IDNO, _T("No"));

		taskDialog.SetDefaultCommandControl(IDYES);

		taskDialog.SetMainIcon(IDI_QUESTION);

		taskDialog.SetDialogWidth(350);

		if (FALSE == CheckControlGranted())
		{
			return;
		}

		CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();
		if (NULL == pDevice)
		{
			return;
		}

		switch (taskDialog.DoModal())
		{
		case IDYES:
		{
			pDevice->RebootSystem();
		}
		break;

		case IDNO:
		default:
			break;
		}
	}
	else
	{
		pDevice->PauseAutoBackup(FALSE);

		ShowRequireElevatedPermissions(strCaption);
	}
}


void CMainFrame::OnUpdateBtnDeviceReboot(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;

	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();

	if (pDevice)
	{
		bEnable = (pDevice->IsMotiongateControllable() && pDevice->IsMotorConnectionContinuity());
	}

	pCmdUI->Enable(bEnable);
}


void CMainFrame::OnBnClickedBtnMGDeviceFactoryReset()
{
	CString strContent("");
	CString strMainInstruction("Do you want to proceed with EZI-MOTIONGATE Factory Reset?");
	CString strTitle("Confirm to apply");

	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();

	if (NULL == pDevice)
	{
		return;
	}

	pDevice->PauseAutoBackup();

	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();

	CString strCaption(_T("MotionGate Factory Reset"));

	/*
	if ((TRUE == pConfigDB->IsDeviceFactoryResetGranted()) ||
		(TRUE == RequestElevatedPermissions(strCaption) &&
			TRUE == pConfigDB->IsDeviceFactoryResetGranted()))
	{
		CTaskDialog taskDialog(strContent, strMainInstruction, strTitle, TDCBF_CANCEL_BUTTON, TDF_ENABLE_HYPERLINKS | TDF_USE_COMMAND_LINKS | TDF_ALLOW_DIALOG_CANCELLATION);

		taskDialog.AddCommandControl(IDYES, _T("Yes"));
		taskDialog.AddCommandControl(IDNO, _T("No"));

		taskDialog.SetDefaultCommandControl(IDYES);

		taskDialog.SetMainIcon(AfxGetApp()->LoadStandardIcon(IDI_QUESTION));

		taskDialog.SetDialogWidth(300);

		switch (taskDialog.DoModal())
		{
		case IDYES:
		{
			if (FALSE == DoMGFactoryReset(GetSelectedDeviceAddress(), TRUE))
			{

			}
		}
		break;

		case IDNO:
		default:
			break;
		}
	}
	else
	{
		pDevice->PauseAutoBackup(FALSE);

		ShowRequireElevatedPermissions(strCaption);
	}
	
	*/

	DoMGFactoryReset(GetSelectedDeviceAddress(), TRUE);

	pDevice->PauseAutoBackup(FALSE);
}


void CMainFrame::OnUpdateBtnMGDeviceFactoryReset(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;

	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();

	if (pDevice)
	{
		bEnable = (
			pDevice->IsMotiongateControllable() &&
			FALSE == pDevice->IsBareMotorConnected()
			);
	}

	pCmdUI->Enable(bEnable);
}


void CMainFrame::SetSelectedAddress(DWORD dwAddress)
{
	m_dwSelectedAddress = dwAddress;
}


DWORD CMainFrame::GetSelectedAddress()
{
	return m_dwSelectedAddress;
}


void CMainFrame::OnBnClickedInformBackup()
{
	OnBnClickedBtnDeviceBackup();
}

void CMainFrame::OnBnClickedInformBackupManage()
{
	OnBnClickedBtnDeviceBackupData();
}


void CMainFrame::OnBnClickedInformRestore()
{
	OnBtnDeviceRestorationSelect();
}


void CMainFrame::OnBnClickedInformComapre()
{
	OnBnClickedBtnDeviceRestoration();
}


void CMainFrame::OnBnClickedInformAssign()
{
	OnBnClickedBtnDeviceIdAssignment();
}


void CMainFrame::OnBnClickedInformReboot()
{
	OnBnClickedBtnDeviceReboot();
}


void CMainFrame::OnBnClickedInformFactoryReset()
{
	OnBnClickedBtnMGDeviceFactoryReset();
}


void CMainFrame::OnBnClickedInformMotorReset()
{
	OnBnClickedBtnDeviceFactoryReset();
}


void CMainFrame::OnBnClickedInformBackupSchedule()
{
	OnBnClickedBtnBackupSchedule();
}


void CMainFrame::OnBnClickedInformConfig()
{
	OnBnClickedMenuConfiguration();
}

void CMainFrame::OnBnClickedInformSystemRun()
{
	OnBnClickedPanelRun();
}

void CMainFrame::OnUpdateToolBar(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateOperationToolBar(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateOutlookToolBar(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}


void CMainFrame::OnBnClickedOperationWarning()
{
	m_logStatus = LOG_WARNING;

	m_paneOperationLog.SetCurrentLogStatus(m_logStatus);
}


void CMainFrame::OnBnClickedOperationError()
{
	m_logStatus = LOG_ERROR;
	m_paneOperationLog.SetCurrentLogStatus(m_logStatus);
}


void CMainFrame::OnBnClickedOperationAll()
{//Tool Bar - ALL 버튼 클릭
	m_LogCheck.bLogAll = TRUE;
	m_LogCheck.bSelect = FALSE;

	m_logStatus = LOG_ALL;

	m_paneOperationLog.SetCurrentLogStatus(m_logStatus);
}

void CMainFrame::OnBnClickedOperationFilter()
{// Tool Bar - Select 버튼 클릭
	m_LogCheck.bLogAll = FALSE;
	m_LogCheck.bSelect = TRUE;

	m_logStatus = LOG_SELECT;
	m_paneOperationLog.SetCurrentLogStatus(m_logStatus);
}

void CMainFrame::OnTreeClickedOperationLog()
{
	if (m_LogCheck.bSelect) m_paneOperationLog.SetCurrentLogStatus(m_logStatus);
}


void CMainFrame::OnBnClickedOperationFocusName()
{
	m_logStatus = LOG_NAME;
	m_paneOperationLog.SetCurrentLogStatus(m_logStatus);
}


void CMainFrame::OnBnClickedOperationFocusAddress()
{
	m_logStatus = LOG_IP;
	m_paneOperationLog.SetCurrentLogStatus(m_logStatus);
}



void CMainFrame::OnBnClickedOperationFocusMix()
{
	m_logStatus = LOG_MIX;
	m_paneOperationLog.SetCurrentLogStatus(m_logStatus);
}

void CMainFrame::OnBnClickedStatusCheckName()
{
	INT a = 0;
	/*m_wndOutlookDevice.SetFocusName(TRUE);
	m_wndOutlookDevice.SetFocusIP(FALSE);*/
}


void CMainFrame::OnBnClickedStatusAddress()
{
	INT a = 0;
	/*m_wndOutlookDevice.SetFocusIP(TRUE);
	m_wndOutlookDevice.SetFocusName(FALSE);*/
}





const BOOL CMainFrame::GetLogInformationAll(LPVOID pDatabaseRecordData, INT& nRowid, CString& strInform, CString& strIP, CString& strID, CString& strTime)
{
	INT nResult = -1;
	CDataBaseRecordData* pRecord = static_cast<CDataBaseRecordData*>(pDatabaseRecordData);
	/*CString strInform;
	CString strIP;
	CString strID;*/
	CString strQuery;

	//if (ExecuteQuery(strQuery) && SeekToBegin())
	if (1)
	{
		//CDataBaseRecordData* pRecord = SeekToNext();

		//if (FALSE == pRecord->GetIntergerData(_T("rowid"), nRowid))
		//{
		//	return -1;
		//}

	/*	if (FALSE == pRecord->GetTextData(_T("TimeStamp"), strTime) ||
			FALSE == pRecord->GetTextData(_T("Information"), strInform) ||
			FALSE == pRecord->GetTextData(_T("Device_Name"), strID) ||
			FALSE == pRecord->GetTextData(_T("Device_IP"), strIP))
		{
			return -1;
		}*/

		//CDataBaseLog* pLog = CDataBaseLog::GetInstance();

		//CDataBaseLog* pLog = m_lstRecord.GetAt(m_cursor);
	}
	return TRUE;
}



BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	UINT id = (UINT)LOWORD(wParam);
	

	return CMDIFrameWndEx::OnCommand(wParam, lParam);
}


void CMainFrame::UpdateOperationLog(PST_OPERAION_MESSAGE pMsg, CString strMessage)
{
	CDeviceMotionGate* pDevice = pMsg->pDevice;
	BOOL bEnable = FALSE;
	INT nLevel;
	switch (pMsg->nLevel)
	{
	case msgAll:
	case msgNone:
	case msgNormal:
	{
		bEnable = TRUE;
		nLevel = STS_LOG_NORMAL;
	} break;
	case msgError: 
	{
		bEnable = TRUE;
		nLevel = STS_LOG_ERROR;
	} break;
	case msgWarning:
	{
		bEnable = TRUE;
		nLevel = STS_LOG_WARNING;
	} break; 
	case msgInternal:
	case msgSequence:
	default:
		break;
	}
	
	if (bEnable)
	{
		pDevice->ActiveOperationLog(nLevel, strMessage);
	}
}

void CMainFrame::OnBnClickedBtnToggleBackup()
{
	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();
	if (NULL == pDevice)
	{
		return;
	}

	pDevice->EnableAutoRestoration(!pDevice->IsAutoRestorationEnabled());
}

void CMainFrame::OnBnClickedAlarmReset()
{
	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();
	if (NULL == pDevice)
	{
		return;
	}

	pDevice->ResetMotionGateAlarm();
}

void CMainFrame::OnUpdateBackupSchedule(CCmdUI* pCmdUI)
{
	return;
	// BOOL bBackupAutoSecControl, BOOL bBackupToggle, BOOL bBdakupTime, BOOL bToggleEnable
	bool bEnableToggleButton = false;
	bool bShowToggleBtn = false;
	bool bEnableTextAutoSec = false;
	bool bEnbaleTextTime = false;
	
	CDeviceMotionGate* pDevice = GetSelectedMotionGateControl();
	if (pDevice == NULL)
	{
		return;
	}
	if (1)
	{ // Date Time 저장.
		CString strText;

		if (pDevice)
		{
			if (pDevice->IsMotiongateConnected() && pDevice->IsAutoRestorationEnabled())
			{
				COleDateTime dateTime;
				if (pDevice->GetAutoBackupDatetime(dateTime))
				{
					strText = dateTime.Format(_T("%c"));
				}

				bEnbaleTextTime = true;
			}
			else
			{
				strText = _T("Stopped.");
			}
		}

		m_pWndMotionManager->SetBackupDateTime(strText);
	}
	
	if (1)
	{ // Auto Sec 저장.
		CString strText;

		if (pDevice)
		{
			if (pDevice->IsMotiongateConnected() && pDevice->IsAutoRestorationEnabled())
			{
				const DOUBLE dbLeftSecond = pDevice->GetAutoBackupLeftSeconds();

				if (86400 <= dbLeftSecond)	// 60 * 60 * 24
				{
					strText.Format(_T("It starts in %d day(s)"), static_cast<INT>(dbLeftSecond) / 86400);
				}
				else if (3600 <= dbLeftSecond)	// 60 * 60
				{
					strText.Format(_T("It starts in %d Hour(s)"), static_cast<INT>(dbLeftSecond) / 3600);
				}
				else if (60 <= dbLeftSecond)
				{
					strText.Format(_T("It starts in %d minute(s)"), static_cast<INT>(dbLeftSecond) / 60);
				}
				else if (0 < dbLeftSecond)
				{
					strText.Format(_T("It starts in %d second(s)"), static_cast<INT>(dbLeftSecond));
				}
			}
		}
		bEnableTextAutoSec = true; // 원래 제어부분이 없었음..
		m_pWndMotionManager->SetBackupAutoSec(strText);
	}

	if (1)
	{ // Toggle
		BOOL bEnable = FALSE;

		BOOL  bRun = FALSE;

		if (pDevice && pDevice->IsMotiongateConnected())
		{
			if (pDevice->IsAutoBackupScheduleExist())
			{
				bRun = pDevice->IsAutoRestorationEnabled();
				CString strName = pDevice->GetSystemName();
				bEnable = TRUE;
			}
		}

		bEnableToggleButton = bRun; // 실제 Run상태인지 아닌지
		bShowToggleBtn = bEnable; // Display Enable
		CString strText(bRun ? _T("ON") : _T("OFF"));
		m_pWndMotionManager->SetBackupToggle(bEnableToggleButton); // 버튼 활성화 실시.
	}
	
	DWORD dwAddress = pDevice->GetAddress();
	if (m_pDeviceAddress != dwAddress)
	{
		BOOL bChcek = TRUE;
	}

	if (m_pDeviceAddress != pDevice->GetAddress() ||
		m_bEnableTextAutoSec != bEnableTextAutoSec ||
		m_bEnableToggleButton != bEnableToggleButton ||
		m_bEnbaleTextTime != bEnbaleTextTime ||
		m_bShowToggleBtn != bShowToggleBtn)
	{
		m_pDeviceAddress = pDevice->GetAddress();
		m_bEnableToggleButton = bEnableToggleButton;
		m_bEnableTextAutoSec = bEnableTextAutoSec;
		m_bEnbaleTextTime = bEnbaleTextTime;
		m_bShowToggleBtn = bShowToggleBtn;
		
		m_pWndMotionManager->SetVisibleBackupDate(bEnableTextAutoSec, bShowToggleBtn, bEnbaleTextTime);
	}
}

void CMainFrame::OnUpdateSelcetedDeviceBackupSchedule()
{
	// BOOL bBackupAutoSecControl, BOOL bBackupToggle, BOOL bBdakupTime, BOOL bToggleEnable
	bool bEnableToggleButton = false;
	bool bShowToggleBtn = false;
	bool bEnableTextAutoSec = false;
	bool bEnbaleTextTime = false;
	bool bDeviceError = false;

	CDeviceMotionGate* pDevice = GetSelectedMotionGateControl();
	if (pDevice == NULL)
	{
		return;
	}
	if (1)
	{ // Date Time 저장.
		CString strText;

		if (pDevice)
		{
			if (pDevice->IsMotiongateConnected() && 
				pDevice->IsAutoRestorationEnabled() &&
				pDevice->IsAutoBackupScheduleExist())
			{
				COleDateTime dateTime;
				if (pDevice->GetAutoBackupDatetime(dateTime))
				{
					strText = dateTime.Format(_T("%c"));
				}

				bEnbaleTextTime = true;
			}
			else
			{
				strText = _T("Stopped.");
			}
		}

		m_pWndMotionManager->SetBackupDateTime(strText);
	}

	if (1)
	{ // Auto Sec 저장.
		CString strText;
		BOOL bExist = FALSE;
		if (pDevice)
		{
			if (pDevice->IsMotiongateConnected() &&
				pDevice->IsAutoRestorationEnabled() &&
				pDevice->IsAutoBackupScheduleExist())
			{
				const DOUBLE dbLeftSecond = pDevice->GetAutoBackupLeftSeconds();

				if (86400 <= dbLeftSecond)	// 60 * 60 * 24
				{
					strText.Format(_T("It starts in %d day(s)"), static_cast<INT>(dbLeftSecond) / 86400);
				}
				else if (3600 <= dbLeftSecond)	// 60 * 60
				{
					strText.Format(_T("It starts in %d Hour(s)"), static_cast<INT>(dbLeftSecond) / 3600);
				}
				else if (60 <= dbLeftSecond)
				{
					strText.Format(_T("It starts in %d minute(s)"), static_cast<INT>(dbLeftSecond) / 60);
				}
				else if (0 < dbLeftSecond)
				{
					strText.Format(_T("It starts in %d second(s)"), static_cast<INT>(dbLeftSecond));
				}

				bExist = TRUE;
			}

			else
			{

			}
		}
		bEnableTextAutoSec = bExist; // 원래 제어부분이 없었음..
		m_pWndMotionManager->SetBackupAutoSec(strText);
	}

	if (1)
	{ // Toggle
		BOOL bEnable = FALSE;

		BOOL  bRun = FALSE;

		if (pDevice && pDevice->IsMotiongateConnected())
		{
			if (pDevice->IsAutoBackupScheduleExist())
			{
				bRun = pDevice->IsAutoRestorationEnabled();
				CString strName = pDevice->GetSystemName();
				bEnable = TRUE;
			}
		}

		bEnableToggleButton = bRun; // 실제 Run상태인지 아닌지
		bShowToggleBtn = bEnable; // Display Enable
		CString strText(bRun ? _T("ON") : _T("OFF"));
		m_pWndMotionManager->SetBackupToggle(bEnableToggleButton); // 버튼 활성화 실시.
	}

	if (1)
	{ // Error Check
		if (pDevice && pDevice->IsMotiongateConnected())
		{
			bDeviceError = pDevice->IsAutoBackupError();
		}
	}

	if (m_pDeviceAddress != pDevice->GetAddress() ||
		m_bEnableTextAutoSec != bEnableTextAutoSec ||
		m_bEnableToggleButton != bEnableToggleButton ||
		m_bEnbaleTextTime != bEnbaleTextTime ||
		m_bShowToggleBtn != bShowToggleBtn ||
		m_bSystemErrorDetected != bDeviceError)
	{
		m_pDeviceAddress = pDevice->GetAddress();
		m_bEnableToggleButton = bEnableToggleButton;
		m_bEnableTextAutoSec = bEnableTextAutoSec;
		m_bEnbaleTextTime = bEnbaleTextTime;
		m_bShowToggleBtn = bShowToggleBtn;
		m_bSystemErrorDetected = bDeviceError;

		m_pWndMotionManager->SetVisibleBackupDate(bEnableTextAutoSec, bShowToggleBtn, bEnbaleTextTime);
	}
}



void CMainFrame::OnUpdateBtnSystemBackupManage(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}


void CMainFrame::OnUpdateBtnSystemCompare(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::SetSystemCompare(BOOL bCtrl)
{
	m_bSystemCompare = bCtrl;
}


const BOOL CMainFrame::IsSystemCompare()
{
	return m_bSystemCompare;
}

void CMainFrame::SetSystemCompareProjectName(CString strName)
{
	m_strCompareSystemName = strName;
}


CString CMainFrame::GetSystemCompareProjectName()
{
	return m_strCompareSystemName;
}

void CMainFrame::OnBtnSystemCompare()
{

	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();

	CString strCaption(_T("Restoration"));

	if ((TRUE == pConfigDB->IsDeviceRestorationGranted()) ||
		(TRUE == RequestElevatedPermissions(strCaption) &&
			TRUE == pConfigDB->IsDeviceRestorationGranted()))
	{
		CDlgSystemManage dlgSystemCompare;
		dlgSystemCompare.SetTitle(_T("System Compare"));
		dlgSystemCompare.SetCompareMode(TRUE);
		
		if (IDOK == dlgSystemCompare.DoModal())
		{
			CString strSelectedSystemName = dlgSystemCompare.GetSystemName();
			SetSystemCompare(TRUE);
			SetSystemCompareProjectName(strSelectedSystemName);
			
			if (IsNominalIDExist(strSelectedSystemName))
			{ // Nominal ID 존재할경우
				CSheetRestoration propSheet;
				propSheet.SetCaption(_T("System Compare"));
				propSheet.SetProjectName(strSelectedSystemName);
				INT nID = propSheet.GetNominalBackupID();
				if (IDOK == propSheet.DoModal())
				{

				}
			}

			else
			{
				CString message("You have to backup as nominal first.");
				CString mainInstruction("Nominal backup data does not exist.");
				CString title(_T("Compare backup data with nominal"));

				CTaskDialog taskDialog(message, mainInstruction, title, TDCBF_CLOSE_BUTTON);

				taskDialog.AddCommandControl(IDYES, _T("OK, I understand."));

				taskDialog.SetMainIcon(TD_ERROR_ICON);

				taskDialog.SetDialogWidth(300);

				taskDialog.DoModal();
			}
			
		}

		SetSystemCompare(FALSE);
	}
	else
	{
		ShowRequireElevatedPermissions(strCaption);
	}
}

void CMainFrame::OnBtnSystemBackupManage()
{
	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();

	CString strCaption(_T("Backup"));

	if ((TRUE == pConfigDB->IsDeviceBackupGranted()) ||
		(TRUE == RequestElevatedPermissions(strCaption) &&
			TRUE == pConfigDB->IsDeviceBackupGranted()))
	{
		CDlgSystemManage dlgSystemBackup;
		dlgSystemBackup.SetTitle(_T("System Backup Manage"));
		dlgSystemBackup.SetCompareMode(FALSE);

		if (IDOK == dlgSystemBackup.DoModal())
		{
			CString strSeletecName = dlgSystemBackup.GetSystemName();
			if (_T("") == strSeletecName)
			{
				return;
			}

			if (true)
			{
				CDlgManageBackup dlg;
				dlg.SetSystemControl(TRUE);
				dlg.SetSystemControlDeviceName(strSeletecName);

				if (IDOK == dlg.DoModal())
				{
					POSITION pos = dlg.GetDeleteItemStartPosition();

					CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();
					CString strProjectName(strSeletecName);

					if (pos)
					{
						if (pProjectDB->OpenDataBase())
						{
							pProjectDB->Begin();
							while (pos)
							{
								INT nBackupID = dlg.GetNextDeleteItem(pos);
								pProjectDB->DeleteBackupData(nBackupID, strProjectName);
								pProjectDB->DeleteBackupList(nBackupID, strProjectName);
							}

							pProjectDB->Commit();

							pProjectDB->DoVacuum();

							pProjectDB->CloseDataBase();
						}
					}

					/*if (dlg.IsNominalSelectionChanged())
					{
						pProjectDB->ChangeNominalBackupID(dlg.GetSelectedNominalID(), strProjectName);
					}*/

					if (FALSE == dlg.IsExistLastestBackupID())
					{
						pProjectDB->RemoveLastestBackupID(strProjectName);
					}
				}
			}

		}
	}

	else
	{
		ShowRequireElevatedPermissions(strCaption);
	}
}


BOOL CMainFrame::IsNominalIDExist(CString strName)
{
	INT nNominalID = -1;
	CString strNominalTime = _T("");
	
	CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();
	pProjectDB->GetNominalBackupInfo(strName, &nNominalID, &strNominalTime);

	return nNominalID != -1;
}


BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR* p_data = (NMHDR*)lParam;
	if (p_data->code == IDR_PANE_DEVICE)
	{
		TOOLTIPTEXT* pTooltip = (TOOLTIPTEXT*)lParam;
		INT nIndex = pTooltip->hdr.idFrom;
		CString str;
		str.Format(_T("tooltip (%d) detected \r\n"), nIndex);
		TRACE(str);
	}


	return CMDIFrameWndEx::OnNotify(wParam, lParam, pResult);
}


void CMainFrame::SaveLogData()
{
	
}


void CMainFrame::OnClose()
{
	CTime time = CTime::GetCurrentTime();
	CString strTime;
	strTime.Format(_T("%04d-%02d-%02d  %02d:%02d:%02d"),
		time.GetYear(), time.GetMonth(), time.GetDay(),
		time.GetHour(), time.GetMinute(), time.GetSecond());

#if defined(LOG_START)
	CDataBaseLog::GetInstance()->InsertLog(0, _T("Program Terminated"));
	m_paneOperationLog.SaveLogList(0, strTime, _T("SYSTEM"), L"", _T("Program Terminated"), CTime::GetCurrentTime());
#endif

	//m_paneOperationLog.SaveLogData();

	if (!ProjectDBVacuum())	AfxMessageBox(_T("Project db _ Vacuum Fail"));
	if(!ConfigDBVacuum()) AfxMessageBox(_T("ConfigDBVacuum db _ Vacuum Fail"));
	if (!LogDBVacuum()) AfxMessageBox(_T("LogDBVacuum db _ Vacuum Fail"));

	CMDIFrameWndEx::OnClose();
}


void CMainFrame::OnBtnSystemBackup()
{
	if (FALSE == CheckControlGranted())
	{
		return;
	}

	CDlgSystemBackup dlg;
	if (dlg.DoModal())
	{

	}
	
}


void CMainFrame::OnUpdateBtnSystemBackup(CCmdUI* pCmdUI)
{
	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	const BOOL bEnable = pDeviceList->IsMonitoringEnabled();

	pCmdUI->Enable(bEnable);
}


void CMainFrame::TestLogSpeed()
{
	OPERATIONLOG_INFORMATION stInform;
	CTime time = CTime::GetCurrentTime();
	
	stInform.nLevel = STS_LOG_NORMAL;
	stInform.strTime.Format(_T("%04d-%02d-%02d  %02d:%02d:%02d"),
		time.GetYear(), time.GetMonth(), time.GetDay(),
		time.GetHour(), time.GetMinute(), time.GetSecond());
	stInform.strDeviceName = _T("Test Log");
	stInform.strDeviceIP = _T("192.0.0.1");
	stInform.strInform = _T("TEST....");
	m_paneOperationLog.SaveLogList(stInform.nLevel, stInform.strTime, stInform.strDeviceName, stInform.strDeviceIP, stInform.strInform, time);
}




const BOOL CMainFrame::ProjectDBVacuum()
{
	BOOL bResult = FALSE;
	CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();

	if (pProjectDB->OpenDataBase())
	{
		pProjectDB->Begin();

		bResult = pProjectDB->DoVacuum();

		pProjectDB->CloseDataBase();
	}

	return bResult;
}


const BOOL CMainFrame::ConfigDBVacuum()
{
	BOOL bResult = FALSE;
	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();
	if (pConfigDB->OpenDataBase())
	{
		pConfigDB->Begin();

		bResult = pConfigDB->DoVacuum();

		pConfigDB->CloseDataBase();
	}

	return bResult;
	
}


const BOOL CMainFrame::LogDBVacuum()
{
	BOOL bResult = FALSE;
	CDataBaseLog* pLogDB = CDataBaseLog::GetInstance();
	if (pLogDB->OpenDataBase())
	{
		pLogDB->Begin();

		bResult = pLogDB->DoVacuum();

		pLogDB->CloseDataBase();
	}

	return bResult;
}


const BOOL CMainFrame::NoticeSingleSystemConfiguration(const DWORD dwAddress)

{
	CDeviceMotionGate* pDevice = NULL;
	CString strTitle;
	if (FALSE == CDeviceList::GetInstance()->GetMotiongateByAddress(dwAddress, pDevice))
	{
		return FALSE;
	}

	BOOL bResult = FALSE;

	pDevice->EnterEditMode();

	CDlgDeviceTotalConfiguration dlg;
	dlg.SetMotiongateCtrl(pDevice);
	dlg.SetNoticeConfig(TRUE);
	dlg.SetTotalConfig(FALSE); 
	
	strTitle.Format(_T("Tooling Configuration"));
	dlg.SetTitleText(strTitle);
	dlg.SetConfigMethod(require_notice_modify);

	/*CSheetConfigDeviceSingle dlg;
	CString strTitle;
	dlg.SetMotiongateCtrl(pDevice);*/
	INT nModifyDatabase = 1;
	if (IDOK == dlg.DoModal())
	{
		if (TRUE == pDevice->MatchToDevice())
		{ // Motor의 개수를 db에 맞출수 없다.
			NoticeNoChangeValue();
			pDevice->ExitEditMode(FALSE);
			return FALSE;
		}

		else if (FALSE == pDevice->CompareChangeDatabase())
		{ // Cofiguration 화면에서 설정한 값들과 database 값들을 비교하여 다를경우 db파일 변경 및 reboot를 물어본다.
			
			if (TRUE == pDevice->CompareNameDatabase())
			{ // 다른이름으로 바꿀경우.. Motor 수량은 무시한다.

			}
			else
			{
				nModifyDatabase = ConfirmModifyDatabase();
				//ConfirmModifyDatabase -> 'X' 버튼을 누르면 아무것도 안하고 끝낸다.
				if (-1 == nModifyDatabase ||
					FALSE == ConfirmReboot())
				{
					pDevice->ExitEditMode(FALSE);
					return FALSE;
				}
			}
		}

		else if(TRUE == pDevice->CompareChangeDatabase())
		{ // Configuration 화면에서 설정한 값(Edit)들과 database 값들을 비교하여 같을경우 굳이 database 변경할지 안물어 봐도 된다.
			if (FALSE == ConfirmReboot())
			{
				pDevice->ExitEditMode(FALSE);
				return FALSE;
			}
		}
		
		pDevice->ExitEditMode();

		if (nModifyDatabase)
		{
			// ModBus 관련된 DeviceID 데이터 삭제 부분.. 현재는 사용 X
			CDeviceList::GetInstance()->RemoveDeviceIDKey(pDevice->GetAddress()); 
			pDevice->UpdateDatabase();
		}
		

		if (FALSE == pDevice->CompareMotiongateInfo())
		{
			pDevice->ChangeSystemInformation();
		}
		else
		{
			pDevice->RebootSystem();
		}

		bResult = TRUE;
	}
	else
	{
		pDevice->ExitEditMode(FALSE);
	}

	return bResult;
}

const BOOL CMainFrame::NoticeSingleSystemCompareResult(const DWORD dwAddress)
{
	BOOL bResult = FALSE;
	CDeviceMotionGate* pDevice = NULL;
	CString strTitle;
	if (FALSE == CDeviceList::GetInstance()->GetMotiongateByAddress(dwAddress, pDevice))
	{
		return FALSE;
	}
	
	bResult = pDevice->CompareNoticeInfo();

	return bResult;
}


void CMainFrame::OnBnClickedStatusIPSearching()
{
	int a = 0;
	
}

void CMainFrame::OnTreeLoadDevice()
{
	m_wndOutlookDevice.LoadDeviceInformation();
}

void CMainFrame::OnBtnSystemHistory()
{
	if (CDataBaseConfig::GetInstance()->IsAdministrator())
	{
		
	}
	else
	{
		if (RequestAdministratorPermissions(_T("System History")))
		{
		}
		else return;
	}

	if (IDOK == m_wndSystemHistory.DoModal())
	{

	} 
	
	m_wndSystemHistory.DestroyWindow();
}


void CMainFrame::OnUpdateBtnSystemHistory(CCmdUI* pCmdUI)
{
	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	const BOOL bEnable = pDeviceList->IsMonitoringEnabled();

	//pCmdUI->Enable(bEnable);
}





const BOOL CMainFrame::RequestAdministratorPermissions(LPCTSTR lpszCaption)
{
	CString message("");
	CString mainInstruction("This task requires current user to have elevated 'Administrator 'permissions.");
	CString title(lpszCaption ? lpszCaption : _T("Require permissions"));
	
	CTaskDialog taskDialog(message, mainInstruction, title, !TDCBF_CANCEL_BUTTON);

	if (CDataBaseConfig::GetInstance()->IsDefaultOperator())
	{
		taskDialog.AddCommandControl(IDYES, _T("Log in with a ID that has the appropriate permissions."), TRUE, TRUE);
	}
	else
	{
		taskDialog.AddCommandControl(IDYES, _T("Re Log in with a another ID that has the appropriate permissions."), TRUE, TRUE);
	}

	taskDialog.AddCommandControl(IDNO, L"No, cancel the task.");

	taskDialog.SetMainIcon(TD_SHIELD_ICON);

	taskDialog.SetDialogWidth(350);

	// Change the default command button control
	taskDialog.SetDefaultCommandControl(IDYES);

	if (IDYES == taskDialog.DoModal())
	{
		if (DoLoginSequence(TRUE))
		{
			if (CDataBaseConfig::GetInstance()->IsAdministrator())
			{
				return TRUE;
			}

			else ShowRequireElevatedPermissions(lpszCaption);
		}
	}

	return FALSE;
}


void CMainFrame::CheckDeviceMotionGate()
{
	CDeviceMotionGate* pDevice = GetSelectedMotiongateCtrl();

	if (m_pDevice != pDevice)
	{
		m_pDevice = pDevice;
		m_pWndMotionManager->SetDeviceMotionGate(m_pDevice);
	}
}


void CMainFrame::OnOutlookTreeClicked()
{
	if (m_pWndMotionManager != NULL && GetOutlookVisible())
	{
		CheckDeviceMotionGate();
	}
}


CDeviceMotionGate* CMainFrame::GetSelectedMotionGateControl()
{
	return m_pDevice;
}


const BOOL CMainFrame::GetOutlookVisible()
{
	return m_wndOutlookDevice.IsWindowVisible();
}




