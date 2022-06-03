// COutlookDevice.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "COutlookDevice.h"

#include "MainFrm.h"

#include "Define_Message.h"

#include "CDataBaseManager.h"

#include "CDlgNetworkSearch.h"

// COutlookDevice

#define PERIOD_POLLING			100
#define PERIOD_SCAN_DEVICE		0

IMPLEMENT_DYNAMIC(COutlookDevice, CWnd)

COutlookDevice::COutlookDevice()
{

}

COutlookDevice::~COutlookDevice()
{
}


BEGIN_MESSAGE_MAP(COutlookDevice, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &COutlookDevice::OnTvnSelChangedTree)
	ON_COMMAND(IDC_CHK_NAME, &COutlookDevice::OnChkName)
	ON_COMMAND(IDC_CHK_ADDRESS, COutlookDevice::OnChkAddress)
	ON_UPDATE_COMMAND_UI(IDC_CHK_NAME, &COutlookDevice::OnUpdateChkName)
	ON_UPDATE_COMMAND_UI(IDC_CHK_ADDRESS, &COutlookDevice::OnUpdateChkAddress)
	ON_WM_TIMER()
	ON_NOTIFY(NM_CLICK, IDC_TREE, &COutlookDevice::OnNMClickTree)
	ON_NOTIFY(NM_RCLICK, IDC_TREE, &COutlookDevice::OnNMRClickTree)
	ON_COMMAND(ID_OPERATION_RUN, &COutlookDevice::OnCmdSystemRun)
	ON_COMMAND(ID_OPERATION_STOP, &COutlookDevice::OnCmdSystemStop)
	ON_COMMAND(ID_OPERATION_REBOOT, &COutlookDevice::OnCmdSystemReboot)
	ON_COMMAND(ID_SYSTEM_SCHEDULE, &COutlookDevice::OnCmdSystemShedule)
	ON_COMMAND(ID_INFORM_ALARM_RST, &COutlookDevice::OnCmdSystemAlarmReset)
	ON_COMMAND(ID_PANEL_CONFIG, &COutlookDevice::OnCmdDeviceConfig) 
	ON_COMMAND(ID_PANEL_RESET, &COutlookDevice::OnCmdDeviceReset)
	ON_COMMAND(ID_DEVICE_STOP, &COutlookDevice::OnCmdDeviceStop)
	ON_COMMAND(ID_DEVICE_BACKUP, &COutlookDevice::OnCmdDeviceBackup)
	ON_COMMAND(ID_DEVICE_RESTORE, &COutlookDevice::OnCmdDeviceRestoration)
	ON_COMMAND(ID_DEVICE_ASSIGNMENT, &COutlookDevice::OnCmdDeviceAssignment)
	ON_COMMAND(ID_DEVICE_FACTORY_RESET, &COutlookDevice::OnCmdDeviceFactoryReset)
	ON_COMMAND(ID_DEVICE_OPERATION, &COutlookDevice::OnCmdDeviceOperation)
	//ON_COMMAND(IDC_CHK_STATUS_NAME, &CMainFrame::OnBnClickedStatusCheckName)
	//ON_COMMAND(IDC_CHK_STATUS_ADDRESS, &CMainFrame::OnBnClickedStatusAddress)
	ON_COMMAND(IDC_CHK_STATUS_NAME, &COutlookDevice::SetFocusName)
	ON_COMMAND(IDC_CHK_STATUS_ADDRESS, &COutlookDevice::SetFocusIP)
	ON_COMMAND(IDC_CHK_STATUS_MIX, &COutlookDevice::SetFocusNone)
	ON_COMMAND(IDC_CHK_STATUS_SEARCH, &COutlookDevice::SetIPSearching)
	
END_MESSAGE_MAP()



// COutlookDevice message handlers
int COutlookDevice::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	/**/
	if (1)
	{
		m_toolbar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PANE_TOOLBAR);
		m_toolbar.LoadToolBar(IDR_PANE_DEVICE, 0, 0, TRUE /* 잠금 */);
		m_toolbar.EnableTextLabels();
		m_toolbar.SetPaneStyle(m_toolbar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
		m_toolbar.SetPaneStyle(m_toolbar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
		m_toolbar.SetOwner(this);

		// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
		m_toolbar.SetRouteCommandsViaFrame(FALSE);
	}
	
	
	if (FALSE == m_wndTree.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | TVS_SHOWSELALWAYS | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_DISABLEDRAGDROP | TVS_FULLROWSELECT, CRect(10, 10, 150, 150), this, IDC_TREE))
	{
		return FALSE;
	}

	m_hSelItem = NULL;

	SetWindowTheme(m_wndTree.m_hWnd, _T("Explorer"), NULL);

	CImageList imgList;
	if (imgList.Create(IDB_DEVICE_STATUS, 16, 1, RGB(0xff, 0xff, 0xff)))
	{
		m_wndTree.SetImageList(&imgList, TVSIL_NORMAL);
		imgList.Detach();
	}

	if (true)
	{
		CFont font;
		LOGFONT lf = { 0, };
		afxGlobalData.fontRegular.GetLogFont(&lf);

		lf.lfHeight = -15;
		font.CreateFontIndirect(&lf);
		
		m_wndTree.SetFont(&font);
		font.Detach();
	}

	m_bClicked = m_bFocusName = m_bFocusIP = FALSE;

	this->LoadDeviceInformation();

	return 0;
}

BOOL COutlookDevice::Create(const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::Create(nullptr, _T(""), WS_CHILD | WS_VISIBLE, rect, pParentWnd, nID);
}



void COutlookDevice::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	INT nTop = 0, nLeft = 0;

	const int cyTlb = m_toolbar.CalcFixedLayout(FALSE, TRUE).cy;
m_toolbar.SetWindowPos(&wndTop, nLeft, nTop, cx - nLeft, cyTlb, SWP_NOZORDER);
	nTop += cyTlb;

	m_wndTree.SetWindowPos(&wndTop, nLeft, nTop, cx - nLeft, cy - nTop, SWP_NOZORDER);
}

void COutlookDevice::LoadDeviceInformation()
{
	m_wndTree.DeleteAllItems();

	m_hSelItem = NULL;

	CDeviceList* pDeviceList = CDeviceList::GetInstance();

	const INT nCount = pDeviceList->GetAddressCount();
	for (INT nIndex = 0; nIndex < nCount; nIndex++)
	{
		const DWORD dwAddress = pDeviceList->GetAddressByIndex(nIndex);
		CDeviceMotionGate* pDevice = NULL;
		pDeviceList->GetMotiongateByAddress(dwAddress, pDevice);

		CString strTemp;

		this->GetDeviceStatus(pDevice, strTemp);
		strTemp += _T("Stopped");

		HTREEITEM hParent = m_wndTree.InsertItem(strTemp, 0, 0);

		if (NULL == m_hSelItem)
		{
			m_hSelItem = hParent;
		}

		TREE_ITEM_DATA_FLAG data;
		data.orgValue = 0;

		//	Setting Motiongate information
		data.info_motiongate.dwDeviceAddress = pDevice->GetAddress();
		m_wndTree.SetItemData(hParent, data.info_motiongate.dwDeviceAddress);

		//	Setting Connection information
		data.info_connection.btItemType = typeConnection;
		strTemp.Format(_T("Motor Count : Unknown"));
		HTREEITEM hConnection = m_wndTree.InsertItem(strTemp, 0, 0, hParent);

		m_wndTree.SetItemData(hConnection, data.info_connection.orgValue);
	}

	//	For default selection event
	m_wndTree.SetFocus();
	if (this->GetDeviceCount())
	{
		SetTimer(TIMER_POLLING, PERIOD_POLLING, NULL);
	}
	else
	{
		KillTimer(TIMER_POLLING);
	}
	
	// KKB
	((CMainFrame*)AfxGetMainWnd())->Invalidate(false);
}


void COutlookDevice::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_POLLING:
	{
		SetTimer(nIDEvent, PERIOD_POLLING, NULL);

		this->DisplayMotiongateStatus();
	}	break;
	}

	CWnd::OnTimer(nIDEvent);
}

const UINT COutlookDevice::GetDeviceCount(const BOOL bLock)
{
	UINT iCount = 0;
	HTREEITEM hItem = m_wndTree.GetFirstVisibleItem();
	while (hItem)
	{
		iCount++;
		hItem = m_wndTree.GetNextSiblingItem(hItem);
	}

	return iCount;
}

void COutlookDevice::GetDeviceStatus(CDeviceMotionGate* pDevice, CString& strStatus, BOOL bFocusName, BOOL bFocusIP)
{
	if (pDevice->IsMotiongateConnected() ||
		_tcslen(pDevice->GetSystemName()) )
	{
		strStatus = pDevice->GetSystemName();
	}
	else
	{
		if (0 != (CString(pDevice->GetProjectName()).GetLength()))
		{
			strStatus = pDevice->GetProjectName();
		}
		else
		{
			pDevice->GetAddress(strStatus);
		}
		
	}

	if (bFocusName)
	{
		CString strName = pDevice->GetSystemName();
		strStatus = (strName.GetLength() == 0 ? _T("Unknown.") : strName);
	}

	if (bFocusIP)
	{
		pDevice->GetAddress(strStatus);
	}

	strStatus += _T(" - ");

	pDevice->GetSimpleStatus(strStatus);
}

const ULONGLONG COutlookDevice::GetSelectedItemData()
{
	TREE_ITEM_DATA_FLAG result;
	result.orgValue = 0;

	if (m_hSelItem)
	{
		HTREEITEM hItem = m_hSelItem;
		HTREEITEM hParent = m_wndTree.GetParentItem(hItem);
		if (hParent)
		{
			result.info_motiongate.reserved = m_wndTree.GetItemData(hItem);
			hItem = hParent;
		}

		result.info_motiongate.dwDeviceAddress = m_wndTree.GetItemData(hItem);
	}

	return result.orgValue;
}

afx_msg void COutlookDevice::OnUpdateChkName(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(CDeviceList::GetInstance()->GetDisplayMethod() == CDeviceList::display_name);
}

afx_msg void COutlookDevice::OnUpdateChkAddress(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(CDeviceList::GetInstance()->GetDisplayMethod() == CDeviceList::display_address);
}

void COutlookDevice::OnChkName()
{
	CDeviceList::GetInstance()->SetDisplayMethod(CDeviceList::display_name);
}

void COutlookDevice::OnChkAddress()
{
	CDeviceList::GetInstance()->SetDisplayMethod(CDeviceList::display_address);
}

void COutlookDevice::DisplayMotiongateStatus()
{
	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	pDeviceList->LockMutex();

	HTREEITEM hItem = m_wndTree.GetFirstVisibleItem();

	while (hItem)
	{
		const DWORD dwAddress = m_wndTree.GetItemData(hItem);

		INT nNewDeviceImage = 5;

		CDeviceMotionGate* pDevice = NULL;
		if (pDeviceList->GetMotiongateByAddress(dwAddress, pDevice))
		{
			CString strDeviceStatus;
			this->GetDeviceStatus(pDevice, strDeviceStatus, m_bFocusName, m_bFocusIP);
			if (true)
			{
				if (m_wndTree.GetItemText(hItem) != strDeviceStatus)
				{
					//strDeviceStatus.Remove('.');
					m_wndTree.SetItemText(hItem, strDeviceStatus);
				}

				//	Check Connection Status

				HTREEITEM hMotorCount = m_wndTree.GetChildItem(hItem);
				nNewDeviceImage = DisplayConnectedMotorInformation(pDevice, hItem, hMotorCount);

				//	Change MotorCount Image
				INT nCurImage, nCurSelImage;

				if (m_wndTree.GetItemImage(hMotorCount, nCurImage, nCurSelImage) && nCurImage != nNewDeviceImage)
				{
					m_wndTree.SetItemImage(hMotorCount, nNewDeviceImage, nNewDeviceImage);
				}

				//	Check Motor Status
				const INT nNewMotorImage = this->DisplayConnectedMotorStatus(pDevice, hMotorCount);
			/*	if (nNewDeviceImage > nNewMotorImage)
				{
					nNewDeviceImage = nNewMotorImage;
				}*/

				//	Change Motiongate Image
				if (pDevice->IsMotiongateConnected())
				{
					if (FALSE == pDevice->IsDeviceInformationValid() ||
						pDevice->IsTeachingPendantOnLine())
					{
						nNewDeviceImage = 1;
					}
				}

				if (m_wndTree.GetItemImage(hItem, nCurImage, nCurSelImage) && nCurImage != nNewDeviceImage)
				{
					m_wndTree.SetItemImage(hItem, nNewDeviceImage, nNewDeviceImage);
				}
			}
		
		}

		hItem = m_wndTree.GetNextSiblingItem(hItem);
	}

	pDeviceList->UnlockMutex();
}

void COutlookDevice::ReloadConnectedMotorStatus(CDeviceMotionGate* pDevice, const HTREEITEM hMotiongate)
{
	//	1. Erase Current motor status
	HTREEITEM hMotorInfo = m_wndTree.GetChildItem(hMotiongate);
	HTREEITEM hMotor = m_wndTree.GetNextSiblingItem(hMotorInfo);

	while (hMotor)
	{
		HTREEITEM hDelete = hMotor;
		hMotor = m_wndTree.GetNextSiblingItem(hMotor);
		m_wndTree.DeleteItem(hDelete);
	}

	//	2. Insert Motor Status
	INFO_MOTOR dataMotor;
	dataMotor.orgValue = 0;
	dataMotor.btItemType = typeMotor;
	dataMotor.wStatus = 4;

	CString strTemp;
	for (INT nIndex = 0; nIndex <= 8; nIndex++)
	{
		/*if (pDevice->IsMotorConnected(nIndex))
		{
			dataMotor.btDeviceID = nIndex;
			strTemp.Format(_T("Motor #%d"), nIndex);
			HTREEITEM hItem = m_wndTree.InsertItem(strTemp, 0, 0, hMotiongate);
			m_wndTree.SetItemData(hItem, dataMotor.orgValue);
		}*/
		WORD dwAxisOrganization = pDevice->GetOriginAxisOrganization();
		
		if (TRUE == ((dwAxisOrganization >> nIndex) & 0x01))
		{
			dataMotor.btDeviceID = nIndex;
			strTemp.Format(_T("Motor #%d"), nIndex);
			HTREEITEM hItem = m_wndTree.InsertItem(strTemp, 0, 0, hMotiongate);
			m_wndTree.SetItemData(hItem, dataMotor.orgValue);
		}
	}
}


const INT COutlookDevice::DisplayConnectedMotorInformation(CDeviceMotionGate* pDevice, const HTREEITEM hParent, const HTREEITEM hMotorCount)
{
	INT nResult = CDeviceMotionGate::motor_status_ok;
	INT nState = pDevice->GetStatus();
	
	switch (nState)
	{
	case CDeviceMotionGate::state_stop:
	{
		nResult = CDeviceMotionGate::motor_status_unknown;
	}	break;

	case CDeviceMotionGate::state_connect:
	{
		INT nCode = pDevice->GetOpCode();
		switch (pDevice->GetOpCode())
		{
		case CDeviceMotionGate::opCode_disconenct:
		case CDeviceMotionGate::opCode_no_response:
		{
			//nResult = CDeviceMotionGate::motor_status_error;
			nResult = CDeviceMotionGate::motor_status_unknown;
			
		}	break;

		case CDeviceMotionGate::opCode_MotiongateBusy:
		{
			nResult = CDeviceMotionGate::motor_status_caution;
		}	break;

		case CDeviceMotionGate::opCode_read_motiongate_info:
		case CDeviceMotionGate::opCode_scan_device:
		{
			nResult = CDeviceMotionGate::motor_status_initialize;
		}	break;
		}
	}	break;

	case CDeviceMotionGate::state_motion_gate_get_system_status_all:
	case CDeviceMotionGate::state_motion_gate_command:
	{
		const INT nMotorRegCount = pDevice->GetMotorCount();
		const INT nMotorCount = pDevice->GetConnectedMotorCount();
		
		const DWORD dwConnectedInfo = pDevice->GetConnectedMotorStatus();
		static INT nAxisOrganizatioin = 0;


		INFO_CONNECTON flagConnection;
		flagConnection.orgValue = m_wndTree.GetItemData(hMotorCount);

		//	Connected Motor Count
		CString strTemp;
		strTemp.Format(_T("Motor Count : %d / %d"), nMotorCount, nMotorRegCount);

		if (strTemp != m_wndTree.GetItemText(hMotorCount) ||
			flagConnection.wConnection != static_cast<WORD>(dwConnectedInfo) ||
			nAxisOrganizatioin != pDevice->GetOriginAxisOrganization())
		{
			m_wndTree.SetItemText(hMotorCount, strTemp);

			this->ReloadConnectedMotorStatus(pDevice, hParent);
			flagConnection.wConnection = static_cast<WORD>(dwConnectedInfo);

			nAxisOrganizatioin = pDevice->GetOriginAxisOrganization();

			if (flagConnection.wConnection & 0x1)
			{
				flagConnection.status = 1;
			}
			else
			{
				flagConnection.status = 0;

				INT nCount = 0;
				for (INT nIndex = 1; nIndex < 16; nIndex++)
				{
					if ((flagConnection.wConnection >> nIndex) & 0x1)
					{
						nCount++;
					}
					else if (nCount != nMotorCount)
					{
						flagConnection.status = 2;
						break;
					}
				}
			}

			m_wndTree.SetItemData(hMotorCount, flagConnection.orgValue);

		}
		else if (nMotorCount != nMotorRegCount)
		{
			nResult = 2;	//	모터 수량 불일치
		}
		else
		{
			switch (flagConnection.status)
			{
			case 0:	// 이상무
			{

			}	break;

			case 1:	// 0 번 모터 발견
			{
				nResult = CDeviceMotionGate::motor_status_caution;
			}	break;

			case 2:	//	연결된 Motor가 연속적이지 않음
			{
				nResult = CDeviceMotionGate::motor_status_warning;
			}	break;
			}
		}

	}	break;

	case CDeviceMotionGate::state_backup:
	case CDeviceMotionGate::state_restoration:
	case CDeviceMotionGate::state_factory_reset:
	case CDeviceMotionGate::state_assign_motor_id:
	case CDeviceMotionGate::state_home_all:
	{
		nResult = 1;
	}	break;
	}

	return nResult;
}


const INT COutlookDevice::DisplayConnectedMotorStatus(CDeviceMotionGate* pDevice, const HTREEITEM hMotorCount)
{
	INT nResult = 5;

	//	1. Find Next Sibliing Item
	HTREEITEM hItem = hMotorCount;
	while (1)
	{
		hItem = m_wndTree.GetNextSiblingItem(hItem);
		if (NULL == hItem)
		{
			break;
		}

		INFO_MOTOR dataMotor;
		dataMotor.orgValue = m_wndTree.GetItemData(hItem);

		INT nNewIndex = pDevice->GetMotorsTotalStatus(dataMotor.btDeviceID);

		INT nCurImage, nCurSelImage;
		if (m_wndTree.GetItemImage(hItem, nCurImage, nCurSelImage) && nCurImage != nNewIndex)
		{
			m_wndTree.SetItemImage(hItem, nNewIndex, nNewIndex);
		}

		if (nResult > nNewIndex)
		{
			nResult = nNewIndex;
		}
	}

	return nResult;
}


void COutlookDevice::OnTvnSelChangedTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = reinterpret_cast<NM_TREEVIEW*>(pNMHDR);
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;

	if (!IsTreeClicked())
	{
		CTime cTime = CTime::GetCurrentTime();
		CString strTime, strData;
		strTime.Format(_T(" [%02d월 %02d일  / %02d시 : %02d분 : %02d초]"),
			cTime.GetMonth(), cTime.GetDay(),
			cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond());

		strData.Format(_T("%s : [Not Clicked] OnTvnSelChangedTree Detected!! \r\n"), strTime);
		TRACE(strData);
	}

	if (hItem != NULL && TRUE == IsTreeClicked())
	{
		SendCommandMessageToMainFrame(ID_OPERATION_CHANGED);
		SetTreeClicked(FALSE);
	}
}

void COutlookDevice::OnNMClickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	CPoint ptGlobal;			GetCursorPos(&ptGlobal);
	CPoint ptTree(ptGlobal);	m_wndTree.ScreenToClient(&ptTree);

	HTREEITEM hItem = m_wndTree.HitTest(ptTree);

	
	if (NULL == hItem)
	{
		return;
	}

	m_hSelItem = hItem;
	
	SetTreeClicked(TRUE);
	
	SendCommandMessageToMainFrame(ID_OPERATION_SELECT);

}

void COutlookDevice::OnNMRClickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 1;

	CPoint ptGlobal;			GetCursorPos(&ptGlobal);
	CPoint ptTree(ptGlobal);	m_wndTree.ScreenToClient(&ptTree);

	HTREEITEM hItem = m_wndTree.HitTest(ptTree);

	
	if (hItem)
	{
		m_hSelItem = hItem;
	}

	if (NULL == m_hSelItem)
	{
		return;
	}

	m_wndTree.SelectItem(m_hSelItem);

	if (m_hSelItem)
	{
		*pResult = 1;

		PopupDeviceControlMenu(ptGlobal);
	}
	else
	{
		*pResult = 0;
	}

	SetTreeClicked(TRUE);
}

void COutlookDevice::PopupDeviceControlMenu(CPoint pt)
{
	CMenu menu;
	if (FALSE == menu.CreatePopupMenu())
	{
		return;
	}

	const INT nControlIndex = (NULL == m_wndTree.GetParentItem(m_hSelItem)) ? 0 : 1;
	switch (nControlIndex)
	{
	case 0:
	{
		GetSystemControlMenu(&menu);
	}	break;

	case 1:
	{
		GetDeviceControlMenu(&menu);
	}	break;
	}

	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, this);
	menu.DestroyMenu();
}

const BOOL COutlookDevice::GetSystemControlMenu(CMenu* pMenu)
{
	CDeviceList* pList = CDeviceList::GetInstance();
	CDeviceMotionGate* pDevice = NULL;

	BOOL bSystemRun = pList->IsStartMonitoring();
	
	TREE_ITEM_DATA_FLAG result;
	result.orgValue = GetSelectedItemData();
	if (FALSE == pList->GetMotiongateByAddress(result.info_motiongate.dwDeviceAddress, pDevice))
	{
		return FALSE;
	}

	if (NULL == pDevice)
	{
		return FALSE;
	}

	const BOOL bMonitoringEnable = pList->IsMonitoringEnabled();
	const BOOL bMasterConnected = pDevice->GetMasterConnectStatus();
	const BOOL bMotorExist = pDevice->IsMotionGateMotorExist();

	if (1)
	{	//	Device name or address
		CString strDeviceName(pDevice->GetSystemName());
		if (strDeviceName.IsEmpty())
		{
			pDevice->GetAddress(strDeviceName);
		}
		
		CString strSystemName;
		strSystemName.Format(_T("System - %s"),strDeviceName);

		pMenu->AppendMenu(MF_STRING, IDC_STATIC, strSystemName);

		pMenu->AppendMenu(MF_SEPARATOR);
	}

	if (1)
	{	//	Operation
		INT nFlag = MF_POPUP;
		CMenu menuOperation;
		menuOperation.CreatePopupMenu();
		if (bMonitoringEnable)
		{
			menuOperation.AppendMenu(
				pDevice->IsRunning() ? MF_STRING | MF_CHECKED : MF_STRING,
				ID_OPERATION_RUN, _T("Run"));

			menuOperation.AppendMenu(
				pDevice->IsRunning() ? MF_STRING : MF_STRING | MF_CHECKED,
				ID_OPERATION_STOP, _T("Stop"));

			/* Reboot Hide
			menuOperation.AppendMenu(MF_SEPARATOR);

			menuOperation.AppendMenu(
				pDevice->IsRunning() ? MF_STRING : MF_STRING | MF_DISABLED,
				ID_OPERATION_REBOOT, _T("Reboot"));*/
		}
		else
		{
			nFlag |= MF_GRAYED;
		}

		pMenu->AppendMenu(nFlag, reinterpret_cast<UINT_PTR>(menuOperation.Detach()), _T("Operation Mode"));
		
		pMenu->AppendMenu(MF_SEPARATOR);
	}	

	INT nDefaultFlag = CString(pDevice->GetSystemName()).GetLength() ? MF_STRING : MF_STRING | MF_GRAYED;

	if (1)
	{	//	Configuration
		CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();
		BOOL bEnable = pConfigDB->IsDeviceConfigGranted() & bSystemRun & !bMasterConnected;
		pMenu->AppendMenu(bEnable ? nDefaultFlag : MF_GRAYED, ID_PANEL_CONFIG, _T("Configuration"));
	}

	if (1)
	{	//	Automatic backup schedule
		CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();
		BOOL bEnable = pConfigDB->IsDeviceBackupScheduleGranted() & bSystemRun & bMotorExist;
		pMenu->AppendMenu(bEnable ? nDefaultFlag : MF_GRAYED, ID_SYSTEM_SCHEDULE, _T("Automatic Backup Schedule"));

		//pMenu->AppendMenu(MF_SEPARATOR);
	}	

	if (1)
	{	//	Alarm Reset
		CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();
		BOOL bEnable = pConfigDB->IsControlGranted() & bSystemRun & bMotorExist;
		pMenu->AppendMenu(bEnable ? nDefaultFlag : MF_GRAYED, ID_INFORM_ALARM_RST, _T("Tooling Alarm Reset"));
	}

	if (1)
	{
		CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();
		BOOL bEnable = pConfigDB->IsDeviceIdResetGranted() & bSystemRun & !bMasterConnected;
		//pMenu->AppendMenu(nDefaultFlag | (bEnable ? 0 : MF_GRAYED), ID_PANEL_RESET, _T("MotionGate FactoryReset..."));
		pMenu->AppendMenu( bEnable ? nDefaultFlag : MF_GRAYED, ID_PANEL_RESET, _T("MotionGate FactoryReset"));
	}

	return TRUE;
}

const BOOL COutlookDevice::GetDeviceControlMenu(CMenu* pMenu)
{
	CDataBaseConfig* pConfig = CDataBaseConfig::GetInstance();
	CDeviceList* pList = CDeviceList::GetInstance();
	CDeviceMotionGate* pDevice = NULL;

	TREE_ITEM_DATA_FLAG result;
	result.orgValue = GetSelectedItemData();
	if (FALSE == pList->GetMotiongateByAddress(result.info_motiongate.dwDeviceAddress, pDevice))
	{
		return FALSE;
	}

	if (NULL == pDevice)
	{
		return FALSE;
	}

	const BOOL bControlEnable = (pList->IsMonitoringEnabled() && pDevice->IsMotiongateConnected() && pDevice->IsRunning());
	const BOOL bTeachingPendant = pDevice->IsTeachingPendantOnLine();
	const BOOL bMaster = pDevice->GetMasterConnectStatus();
	const BOOL bBusy = pDevice->IsBusy();

	INT nDefaultFlag = bControlEnable ? MF_STRING : MF_STRING | MF_GRAYED;

	if (1)
	{	//	Device name or address
		CString strDeviceName(pDevice->GetSystemName());
		if (strDeviceName.IsEmpty())
		{
			pDevice->GetAddress(strDeviceName);
		}

		CString strSystemName;
		strSystemName.Format(_T("System - %s"), strDeviceName);		

		pMenu->AppendMenu(MF_STRING, IDC_STATIC, strSystemName);

		pMenu->AppendMenu(MF_SEPARATOR);
	}

	if (1)
	{	//	Backup
		UINT uFlag = nDefaultFlag;
		if (FALSE == bBusy ||
			FALSE == pDevice->IsGrantedToStop() ||
			bTeachingPendant)
		{
			uFlag |= MF_GRAYED;
		}

		pMenu->AppendMenu(uFlag, ID_DEVICE_STOP, _T("Stop Execution"));
	}

	pMenu->AppendMenu(MF_SEPARATOR);

	if (1)
	{	//	Backup
		UINT uFlag = nDefaultFlag;
		if (bBusy ||
			bTeachingPendant ||
			FALSE == pDevice->IsMotorConnectionContinuity())
		{
			uFlag |= MF_GRAYED;
		}

		pMenu->AppendMenu(uFlag, ID_DEVICE_BACKUP, _T("Backup"));
	}

	if (1)
	{	//	Restore
		UINT uFlag = nDefaultFlag;
		if (bBusy ||
			bTeachingPendant ||
			bMaster ||
			FALSE == pDevice->IsMotorConnectionContinuity())
		{
			uFlag |= MF_GRAYED;
		}

		pMenu->AppendMenu(uFlag, ID_DEVICE_RESTORE, _T("Restore"));
	}

	pMenu->AppendMenu(MF_SEPARATOR);

	if (1)
	{	//	ID Assignment
		UINT uFlag = nDefaultFlag;
		if (bBusy ||
			bTeachingPendant ||
			bMaster ||
			FALSE == pDevice->IsBareMotorConnected())
		{
			uFlag |= MF_GRAYED;
		}

		pMenu->AppendMenu(uFlag, ID_DEVICE_ASSIGNMENT, _T("ID Assignment ..."));
	}

	if (1)
	{	//	Factory Reset
		UINT uFlag = nDefaultFlag;
		if (bBusy ||
			bTeachingPendant ||
			bMaster ||
			TRUE == pDevice->IsBareMotorConnected())
		{
			uFlag |= MF_GRAYED;
		}

		pMenu->AppendMenu(uFlag, ID_DEVICE_FACTORY_RESET, _T("Factory Reset"));
	}

	pMenu->AppendMenu(MF_SEPARATOR);

	//if (1)
	//{	//	Show motion test
	//	UINT uFlag = nDefaultFlag;
	//	if (bBusy)
	//	{
	//		uFlag |= MF_GRAYED;
	//	}

	//	pMenu->AppendMenu(uFlag, ID_DEVICE_OPERATION, _T("Motion Test..."));
	//}

	return TRUE;
}

void COutlookDevice::SendCommandMessageToMainFrame(WPARAM wParam, LPARAM lParam)
{
	AfxGetMainWnd()->SendMessage(WM_COMMAND, wParam, lParam);
}

void COutlookDevice::OnCmdSystemRun()
{
	SendCommandMessageToMainFrame(ID_OPERATION_RUN);
}

void COutlookDevice::OnCmdSystemStop()
{
	SendCommandMessageToMainFrame(ID_OPERATION_STOP);
}

void COutlookDevice::OnCmdSystemReboot()
{
	SendCommandMessageToMainFrame(ID_OPERATION_REBOOT);
}

void COutlookDevice::OnCmdSystemShedule()
{
	SendCommandMessageToMainFrame(ID_SYSTEM_SCHEDULE);
}

void COutlookDevice::OnCmdSystemAlarmReset()
{
	SendCommandMessageToMainFrame(ID_INFORM_ALARM_RST);
}

void COutlookDevice::OnCmdDeviceConfig()
{
	SendCommandMessageToMainFrame(ID_DEVICE_CONFIG);
}

void COutlookDevice::OnCmdDeviceStop()
{
	SendCommandMessageToMainFrame(ID_DEVICE_STOP);
}

void COutlookDevice::OnCmdDeviceBackup()
{
	SendCommandMessageToMainFrame(ID_DEVICE_BACKUP);
}

void COutlookDevice::OnCmdDeviceRestoration()
{
	SendCommandMessageToMainFrame(ID_DEVICE_RESTORE);
}

void COutlookDevice::OnCmdDeviceAssignment()
{
	SendCommandMessageToMainFrame(ID_DEVICE_ASSIGNMENT);
}

void COutlookDevice::OnCmdDeviceFactoryReset()
{
	SendCommandMessageToMainFrame(ID_DEVICE_FACTORY_RESET);
}

void COutlookDevice::OnCmdDeviceOperation()
{
	SendCommandMessageToMainFrame(ID_DEVICE_OPERATION);
}

void COutlookDevice::OnCmdDeviceReset()
{
	SendCommandMessageToMainFrame(ID_INFORM_FACTORY_RESET);
}


//BOOL COutlookDevice::OnCommand(WPARAM wParam, LPARAM lParam)
//{
//
//	return CWnd::OnCommand(wParam, lParam);
//}


void COutlookDevice::SetFocusName(/*BOOL bFocus*/)
{
	BOOL bFocus = TRUE;
	m_bFocusName = bFocus;
	m_bFocusIP = !bFocus;
}


void COutlookDevice::SetFocusIP(/*BOOL bFocus*/)
{
	BOOL bFocus = TRUE;
	m_bFocusName = !bFocus;
	m_bFocusIP = bFocus;
}


void COutlookDevice::SetFocusNone()
{
	m_bFocusName = m_bFocusIP = FALSE;
}


BOOL COutlookDevice::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR* p_data = (NMHDR*)lParam;
	INT nForm = p_data->idFrom;
	UINT nCode = p_data->code;
	WORD dwToolbar = LOWORD(wParam);

	if (p_data->code == IDR_PANE_DEVICE)
	{
		TOOLTIPTEXT* pTooltip = (TOOLTIPTEXT*)lParam;
		INT nIndex = pTooltip->hdr.idFrom;
		CString str;
		str.Format(_T("tooltip (%d) detected \r\n"), nIndex);
		TRACE(str);
	}

	return CWnd::OnNotify(wParam, lParam, pResult);
}


void COutlookDevice::SetIPSearching()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	if (FALSE == pFrame->CheckControlGranted())
	{
		return;
	}

	CDeviceList* pList = CDeviceList::GetInstance();
	if (!pList->IsStartMonitoring())
	{ // System Stop 경우...
		CDlgNetworkSearch dlg;
		dlg.SetOutlookSearch(TRUE);

		if (IDOK == dlg.DoModal())
		{
			CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();
			const INT nCurInterface = pConfigDB->GetInterface();

			if (pConfigDB->GetInterface() == nCurInterface)
			{
				SendCommandMessageToMainFrame(ID_OPERATION_LOAD_TREE);
			}
		}
		else
		{
			
		}

	}
	else
	{ // System Run 경우...

		CString message("If you want to use 'IP Searching', please stop system monitoring.");
		CString mainInstruction("Failed to execute 'IP Searching'.");
		CString title(_T("System monitoring stop is required."));

		CTaskDialog taskDialog(message, mainInstruction, title, !(TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON));

		taskDialog.AddCommandControl(IDYES, _T("OK, I understand."));

		taskDialog.SetMainIcon(TD_ERROR_ICON);

		taskDialog.SetDialogWidth(300);

		taskDialog.DoModal();
	}


}




const BOOL COutlookDevice::IsTreeClicked()
{
	return m_bClicked;
}


void COutlookDevice::SetTreeClicked(BOOL bClicked)
{
	m_bClicked = bClicked;
}
