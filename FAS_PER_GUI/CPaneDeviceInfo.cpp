// CPaneDeviceInfo.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPaneDeviceInfo.h"

#include "MainFrm.h"

#include "Define_Message.h"

#include "CDataBaseManager.h"

#define PERIOD_POLLING			100
#define PERIOD_SCAN_DEVICE		0

// CPaneDeviceInfo

IMPLEMENT_DYNAMIC(CPaneDeviceInfo, CDockablePane)

CPaneDeviceInfo::CPaneDeviceInfo()
{

}

CPaneDeviceInfo::~CPaneDeviceInfo()
{

}


BEGIN_MESSAGE_MAP(CPaneDeviceInfo, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &CPaneDeviceInfo::OnTvnSelChangedTree)
	ON_COMMAND(IDC_CHK_NAME, &CPaneDeviceInfo::OnChkName)
	ON_COMMAND(IDC_CHK_ADDRESS, CPaneDeviceInfo::OnChkAddress)
	ON_UPDATE_COMMAND_UI(IDC_CHK_NAME, &CPaneDeviceInfo::OnUpdateChkName)
	ON_UPDATE_COMMAND_UI(IDC_CHK_ADDRESS, &CPaneDeviceInfo::OnUpdateChkAddress)
	ON_WM_TIMER()
	ON_NOTIFY(NM_CLICK, IDC_TREE, &CPaneDeviceInfo::OnNMClickTree)
	ON_NOTIFY(NM_RCLICK, IDC_TREE, &CPaneDeviceInfo::OnNMRClickTree)
END_MESSAGE_MAP()


// CPaneDeviceInfo 메시지 처리기
int CPaneDeviceInfo::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_toolbar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PANE_TOOLBAR);
	m_toolbar.LoadToolBar(IDR_PANE_DEVICE, 0, 0, TRUE /* 잠금 */);
	m_toolbar.SetPaneStyle(m_toolbar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_toolbar.SetPaneStyle(m_toolbar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_toolbar.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_toolbar.SetRouteCommandsViaFrame(FALSE);

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

	this->LoadDeviceInformation();

	return 0;
}


void CPaneDeviceInfo::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	INT nTop = 0, nLeft = 0;

	const int cyTlb = m_toolbar.CalcFixedLayout(FALSE, TRUE).cy;
	m_toolbar.SetWindowPos(&wndTop, nLeft, nTop, cx - nLeft, cyTlb, SWP_NOZORDER);
	nTop += cyTlb;

	m_wndTree.SetWindowPos(&wndTop, nLeft, nTop, cx - nLeft, cy - nTop, SWP_NOZORDER);
}

void CPaneDeviceInfo::LoadDeviceInformation()
{
	m_wndTree.DeleteAllItems();
	
	m_hSelItem = NULL;

	CDeviceList* pDeviceList = CDeviceList::GetInstance();

	

	const INT nCount = pDeviceList->GetRegisteredDeviceCount();
	for (INT nIndex = 0; nIndex < nCount; nIndex++)
	{
		CDeviceMotionGate* pDevice = pDeviceList->GetMotiongateByPriority(nIndex);
		if (NULL == pDevice)
		{
			continue;
		}

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
		strTemp.Format(_T("Motor Count : %d"), pDevice->GetMotorCount());
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
}

void CPaneDeviceInfo::OnTvnSelChangedTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = reinterpret_cast<NM_TREEVIEW*>(pNMHDR);
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
}

void CPaneDeviceInfo::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_POLLING:
	{
		SetTimer(nIDEvent, PERIOD_POLLING, NULL);

		this->DisplayMotiongateStatus();
	}	break;
	}
	CDockablePane::OnTimer(nIDEvent);
}

const UINT CPaneDeviceInfo::GetDeviceCount(const BOOL bLock)
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

void CPaneDeviceInfo::GetDeviceStatus(CDeviceMotionGate* pDevice, CString& strStatus)
{
	if (CDeviceList::display_address == CDeviceList::GetInstance()->GetDisplayMethod())
	{
		pDevice->GetAddress(strStatus);
	}
	else
	{
		strStatus = pDevice->GetSystemName();
	}

	strStatus += _T(" - ");

	pDevice->GetSimpleStatus(strStatus);
}

const ULONGLONG CPaneDeviceInfo::GetSelectedItemData()
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

afx_msg void CPaneDeviceInfo::OnUpdateChkName(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(CDeviceList::GetInstance()->GetDisplayMethod() == CDeviceList::display_name);
}

afx_msg void CPaneDeviceInfo::OnUpdateChkAddress(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(CDeviceList::GetInstance()->GetDisplayMethod() == CDeviceList::display_address);
}

void CPaneDeviceInfo::OnChkName()
{
	CDeviceList::GetInstance()->SetDisplayMethod(CDeviceList::display_name);
}

void CPaneDeviceInfo::OnChkAddress()
{
	CDeviceList::GetInstance()->SetDisplayMethod(CDeviceList::display_address);
}

void CPaneDeviceInfo::DisplayMotiongateStatus()
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
			this->GetDeviceStatus(pDevice, strDeviceStatus);

			if (m_wndTree.GetItemText(hItem) != strDeviceStatus)
			{
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
			if (nNewDeviceImage > nNewMotorImage)
			{
				nNewDeviceImage = nNewMotorImage;
			}

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

		hItem = m_wndTree.GetNextSiblingItem(hItem);
	}

	pDeviceList->UnlockMutex();
}

void CPaneDeviceInfo::ReloadConnectedMotorStatus(CDeviceMotionGate* pDevice, const HTREEITEM hMotiongate)
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
		if (pDevice->IsMotorConnected(nIndex))
		{
			dataMotor.btDeviceID = nIndex;
			strTemp.Format(_T("Motor #%d"), nIndex);
			HTREEITEM hItem = m_wndTree.InsertItem(strTemp, 0, 0, hMotiongate);
			m_wndTree.SetItemData(hItem, dataMotor.orgValue);
		}
	}
}


const INT CPaneDeviceInfo::DisplayConnectedMotorInformation(CDeviceMotionGate* pDevice, const HTREEITEM hParent, const HTREEITEM hMotorCount)
{
	INT nResult = 5;

	switch (pDevice->GetStatus())
	{
	case CDeviceMotionGate::state_stop:
	{
		nResult = 0;
	}	break;

	case CDeviceMotionGate::state_connect:
	{
		switch (pDevice->GetOpCode())
		{
		case CDeviceMotionGate::opCode_disconenct:
		case CDeviceMotionGate::opCode_no_response:
		{
			nResult = 3;
		}	break;

		case CDeviceMotionGate::opCode_MotiongateBusy:
		{
			nResult = 1;
		}	break;

		case CDeviceMotionGate::opCode_read_motiongate_info:
		case CDeviceMotionGate::opCode_scan_device:
		{
			nResult = 4;
		}	break;
		}
	}	break;

	case CDeviceMotionGate::state_motion_gate_get_system_status_all:
	case CDeviceMotionGate::state_motion_gate_command:
	{
		const INT nMotorRegCount = pDevice->GetMotorCount();
		const INT nMotorCount = pDevice->GetConnectedMotorCount();

		const DWORD dwConnectedInfo = pDevice->GetConnectedMotorStatus();

		INFO_CONNECTON flagConnection;
		flagConnection.orgValue = m_wndTree.GetItemData(hMotorCount);

		if (flagConnection.wConnection != static_cast<WORD>(dwConnectedInfo))
		{
			this->ReloadConnectedMotorStatus(pDevice, hParent);
			flagConnection.wConnection = static_cast<WORD>(dwConnectedInfo);

			//	Connected Motor Count
			CString strTemp;
			strTemp.Format(_T("Motor Count : %d / %d"), nMotorCount, nMotorRegCount);
			m_wndTree.SetItemText(hMotorCount, strTemp);

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
			nResult = 1;
		}	break;

		case 2:	//	연결된 Motor가 연속적이지 않음
		{
			nResult = 2;
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


const INT CPaneDeviceInfo::DisplayConnectedMotorStatus(CDeviceMotionGate* pDevice, const HTREEITEM hMotorCount)
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

void CPaneDeviceInfo::OnNMClickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	CPoint ptGlobal;			GetCursorPos(&ptGlobal);
	CPoint ptTree(ptGlobal);	m_wndTree.ScreenToClient(&ptTree);

	m_hSelItem = m_wndTree.HitTest(ptTree);
}

void CPaneDeviceInfo::OnNMRClickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 1;

	CPoint ptGlobal;			GetCursorPos(&ptGlobal);
	CPoint ptTree(ptGlobal);	m_wndTree.ScreenToClient(&ptTree);

	m_hSelItem = m_wndTree.HitTest(ptTree);
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
}

void CPaneDeviceInfo::PopupDeviceControlMenu(CPoint pt)
{
	CMenu menu;
	if( FALSE == menu.CreatePopupMenu() )
	{
		return;
	}

	const INT nControlIndex = ( NULL == m_wndTree.GetParentItem(m_hSelItem) ) ? 0 : 1;
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

const BOOL CPaneDeviceInfo::GetSystemControlMenu(CMenu* pMenu)
{
	CDeviceList* pList = CDeviceList::GetInstance();
	CDeviceMotionGate* pDevice = NULL;	

	TREE_ITEM_DATA_FLAG result;
	result.orgValue = GetSelectedItemData();
	if (FALSE == pList->GetMotiongateByAddress(result.info_motiongate.dwDeviceAddress, pDevice) )
	{
		return FALSE;
	}

	if (NULL == pDevice)
	{
		return FALSE;
	}

	const BOOL bMonitoringEnable = pList->IsMonitoringEnabled();

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
		}
		else
		{
			nFlag |= MF_GRAYED;
		}

		pMenu->AppendMenu(nFlag, reinterpret_cast<UINT_PTR>(menuOperation.Detach()), _T("Operation Mode"));		
	}
	
	pMenu->AppendMenu(MF_SEPARATOR);

	if (1)
	{	//	Automatic backup schedule
		pMenu->AppendMenu(MF_STRING, ID_SYSTEM_SCHEDULE, _T("Automatic Backup Schedule ..."));	
	}
	
	pMenu->AppendMenu(MF_SEPARATOR);

	if (1)
	{	//	Automatic backup schedule
		pMenu->AppendMenu(MF_STRING, ID_PANEL_CONFIG, _T("Configuration ..."));
	}

	return TRUE;
}

const BOOL CPaneDeviceInfo::GetDeviceControlMenu(CMenu* pMenu)
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
	const BOOL bBusy = pDevice->IsBusy();

	INT nDefaultFlag = bControlEnable ? MF_STRING : MF_STRING | MF_GRAYED;
	
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
			FALSE == pDevice->IsMotorConnectionContinuity() )
		{
			uFlag |= MF_GRAYED;
		}

		pMenu->AppendMenu(uFlag, ID_DEVICE_BACKUP, _T("Backup ..."));
	}

	if (1)
	{	//	Restore
		UINT uFlag = nDefaultFlag;
		if (bBusy ||
			bTeachingPendant ||
			FALSE == pDevice->IsMotorConnectionContinuity() )
		{
			uFlag |= MF_GRAYED;
		}

		pMenu->AppendMenu(uFlag, ID_DEVICE_RESTORE, _T("Restore ..."));
	}

	pMenu->AppendMenu(MF_SEPARATOR);

	if (1)
	{	//	ID Assignment
		UINT uFlag = nDefaultFlag;
		if (bBusy ||
			bTeachingPendant ||
			FALSE == pDevice->IsBareMotorConnected() )
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
			TRUE == pDevice->IsBareMotorConnected() )
		{
			uFlag |= MF_GRAYED;
		}

		pMenu->AppendMenu(uFlag, ID_DEVICE_FACTORY_RESET, _T("Factory Reset ..."));
	}

	pMenu->AppendMenu(MF_SEPARATOR);

	if (1)
	{	//	Show motion test
		UINT uFlag = nDefaultFlag;
		if (bBusy)
		{
			uFlag |= MF_GRAYED;
		}

		pMenu->AppendMenu(uFlag, ID_DEVICE_OPERATION, _T("Motion Test..."));
	}
	
	return TRUE;
}
