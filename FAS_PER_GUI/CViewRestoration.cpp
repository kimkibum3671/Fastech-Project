// CViewRestoration.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CViewRestoration.h"

#include "CDocDevice.h"

#include "CDeviceMotionGate.h"

#include "CDataBaseManager.h"

#include "CDeviceList.h"

#include "MainFrm.h"

// CViewRestoration

IMPLEMENT_DYNCREATE(CViewRestoration, CViewDevice)

CViewRestoration::CViewRestoration()
	: CViewDevice(IDD_RESTORATION)
{
	m_bAutoHoming = TRUE;
}

CViewRestoration::~CViewRestoration()
{
}

void CViewRestoration::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CViewRestoration, CFormView)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_START, &CViewRestoration::OnBnClickedBtnStart)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MAIN, &CViewRestoration::OnLvnItemchangedListMain)
	ON_BN_CLICKED(IDC_BTN_HOMING, &CViewRestoration::OnBnClickedBtnHoming)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CViewRestoration diagnostics

#ifdef _DEBUG
void CViewRestoration::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CViewRestoration::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CViewRestoration message handlers


void CViewRestoration::OnInitialUpdate()
{
	CViewDevice::OnInitialUpdate();

	if (m_wndNameRegister.SubclassDlgItem(IDC_ST_NAME_REGISTER, this))
	{

	}

	if (m_wndNameDevice.SubclassDlgItem(IDC_ST_NAME_DEVICE, this))
	{

	}

	CComboBox* pCmbCtrl = static_cast<CComboBox*>(GetDlgItem(IDC_CMB_COMPARE));
	pCmbCtrl->AddString(_T("Nothing - Start restoration"));
	pCmbCtrl->AddString(_T("Nominal backup data"));
	pCmbCtrl->AddString(_T("Current Motiongate data"));
	pCmbCtrl->SetCurSel(0);	// 추후에 이 부분도 보완이 필요할 것이다.

	SetTimer(TIMER_INITIALIZE, ELAPSE_TIMER_INITIALIZE, NULL);
}


void CViewRestoration::OnTimer(UINT_PTR nIDEvent)
{
	/*switch (nIDEvent)
	{
	case TIMER_INITIALIZE:
	{
		KillTimer(nIDEvent);

		CDeviceMotionGate* pDevice = GetMotiongateCtrl();
		if (NULL == pDevice)
		{
			break;
		}

		m_strProjectName = pDevice->GetSystemName();
		CString strDeviceName(pDevice->GetRealName());

		m_wndNameRegister.SetWindowText(m_strProjectName);
		m_wndNameDevice.SetWindowText(strDeviceName);

		if (m_strProjectName != strDeviceName)
		{
			GetDlgItem(IDC_ST_GROUP_DEVICE)->ShowWindow(SW_SHOW);
			m_wndNameDevice.ShowWindow(SW_SHOW);
		}

		DestroyDeviceWindow(this);

		m_bAutoRestoration = pDevice->IsAutoRestorationEnabled();
		pDevice->EnableAutoRestoration(FALSE);

		if (pDevice->IsMotiongateConnected() && FALSE == CheckNameMatched(pDevice))
		{

		}

		InitializeListCtrl();

		SetTimer(TIMER_POLLING, ELAPSE_TIMER_POLLING, NULL);
	}	break;

	case TIMER_POLLING:
	{
		CDeviceMotionGate* pDevice = GetMotiongateCtrl();
		if (NULL == pDevice || FALSE == pDevice->IsMotiongateConnected())
		{
			GetParent()->DestroyWindow();
			return;
		}

		CDocDevice* pDoc = static_cast<CDocDevice*>(GetDocument());
		pDoc->CheckDeviceTitle();

		this->DisplayNames(pDevice);

		SetTimer(nIDEvent, ELAPSE_TIMER_POLLING, NULL);
	}	break;
	}*/

	CViewDevice::OnTimer(nIDEvent);
}

void CViewRestoration::DisplayNames(CDeviceMotionGate* pDevice)
{
	/*CString strDeviceName(pDevice->GetRealName()), strRegisterName(pDevice->GetSystemName());

	COLORREF m_colorBkName = RGB(0x0a, 0x0a, 0x50);
	COLORREF m_colorTxtName = RGB(0xc8, 0xc8, 0xc8);
	if (strDeviceName != strRegisterName)
	{
		if (CTime::GetCurrentTime().GetSecond() % 2)
		{
			m_colorTxtName = RGB(0xc8, 0x00, 0x00);
			m_colorBkName = RGB(0x00, 0x00, 0x00);
		}
		else
		{
			m_colorTxtName = RGB(0x00, 0x00, 0x00);
			m_colorBkName = RGB(0xc8, 0x00, 0x00);
		}
	}

	if (m_wndNameRegister.GetBkColor() != m_colorBkName)
	{
		m_wndNameRegister.SetBkColor(m_colorBkName);
	}

	if (m_wndNameRegister.GetTextColor() != m_colorTxtName)
	{
		m_wndNameRegister.SetTextColor(m_colorTxtName);
	}

	if (m_wndNameDevice.GetBkColor() != m_colorBkName)
	{
		m_wndNameDevice.SetBkColor(m_colorBkName);
	}

	if (m_wndNameDevice.GetTextColor() != m_colorTxtName)
	{
		m_wndNameDevice.SetTextColor(m_colorTxtName);
	}*/
}


void CViewRestoration::ResizeWindows()
{
	//	Resize를 할 수 있는 Window 크기 정의
	CRect rtWindow;
	GetClientRect(rtWindow);

	if (rtWindow.right < 732)
	{
		rtWindow.right = 732;
	}

	if (rtWindow.bottom < 364)
	{
		rtWindow.bottom = 364;
	}

	if (1)
	{
		CWnd* pWnd = GetDlgItem(IDC_ST_MAIN);
		if (pWnd)
		{
			//	Resize Main List
			CRect rtTarget;
			pWnd->GetWindowRect(&rtTarget);
			ScreenToClient(rtTarget);
			const INT nGap = rtTarget.left;
			rtTarget.right = rtWindow.right - nGap;
			rtTarget.bottom = rtWindow.bottom - nGap;
			pWnd->SetWindowPos(&wndTop, rtTarget.left, rtTarget.top, rtTarget.Width(), rtTarget.Height(), SWP_NOZORDER | SWP_NOMOVE);
		}

		pWnd = GetDlgItem(IDC_LIST_MAIN);
		if (pWnd)
		{
			//	Resize Main List
			CRect rtTarget;
			pWnd->GetWindowRect(&rtTarget);
			ScreenToClient(rtTarget);
			const INT nGap = rtTarget.left;
			rtTarget.right = rtWindow.right - nGap;
			rtTarget.bottom = rtWindow.bottom - nGap;
			pWnd->SetWindowPos(&wndTop, rtTarget.left, rtTarget.top, rtTarget.Width(), rtTarget.Height(), SWP_NOZORDER | SWP_NOMOVE);
		}
	}
}

void CViewRestoration::OnSize(UINT nType, int cx, int cy)
{
	CViewDevice::OnSize(nType, cx, cy);

	ResizeWindows();
}


void CViewRestoration::OnBnClickedBtnStart()
{
	switch (static_cast<CComboBox*>(GetDlgItem(IDC_CMB_COMPARE))->GetCurSel())
	{
	case compare_nothing:
	{
		CDeviceMotionGate* pDevice = GetMotiongateCtrl();
		if (NULL == pDevice)
		{
			return;
		}

		if (FALSE == pDevice->IsMotorConnectionContinuity( m_bAutoRestoration ? static_cast<CDocDevice*>(GetDocument())->GetSelectMotorIndex() + 1 : 0 ))
		{
			//	수량이 안맞거나 중간에 비어있는 모터가 존재한다.
			return;
		}

		if (GetDlgItem(IDC_ST_NAME_DEVICE)->IsWindowVisible())
		{
			//	이름을 일치시키는데 대한 경고
		}

		POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
		if (pos)
		{
			//	최종 확인 후 진행
			pDevice->SetProjectName(m_strProjectName);
			pDevice->SelectBackupID(static_cast<INT>(m_listCtrl.GetItemData(m_listCtrl.GetNextSelectedItem(pos))));
			pDevice->EnableAutoHoming( m_bAutoHoming );
			
			if (m_bAutoRestoration)
			{
				if (pDevice->StartRestoration() && pDevice->StartAssignID())
				{
					DestroyDeviceWindow();
				}
			}
			else
			{
				if (pDevice->StartRestoration())
				{
					DestroyDeviceWindow();
				}
			}
			
		}
	}	break;
	}
}

void CViewRestoration::InitializeListCtrl()
{
	m_listCtrl.SubclassDlgItem(IDC_LIST_MAIN, this);
	m_listCtrl.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

	INT nIndex = 0;
	m_listCtrl.InsertColumn(nIndex++, _T("Time Stamp"), 0, 150);
	m_listCtrl.InsertColumn(nIndex++, _T("Comment"), 0, 100);

	m_listCtrl.DeleteAllItems();

	m_listCtrl.LoadHeaderItemInfo();

	this->LoadProjectList(m_strProjectName);
}

void CViewRestoration::LoadProjectList(LPCTSTR lpszProjectName)
{
	m_listCtrl.DeleteAllItems();

	CDataBaseProject* pProject = CDataBaseProject::GetInstance();

	if (pProject->GetProjectBackupList(lpszProjectName))
	{
		pProject->SeekToBegin();

		while (CDataBaseRecordData* pData = pProject->SeekToNext())
		{
			CString strTemp;
			pData->GetTextData(_T("TimeStamp"), strTemp);
			const INT nIndex = m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), strTemp);

			pData->GetTextData(_T("Backup_Name"), strTemp);
			m_listCtrl.SetItemText(nIndex, 1, strTemp);

			INT nItemIndex = -1;
			pData->GetIntergerData(_T("Backup_ID"), nItemIndex);
			m_listCtrl.SetItemData(nIndex, nItemIndex);
		}
	}
}


void CViewRestoration::OnLvnItemchangedListMain(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	*pResult = 0;

	GetDlgItem(IDC_BTN_START)->EnableWindow(m_listCtrl.GetFirstSelectedItemPosition() ? TRUE : FALSE);
}


void CViewRestoration::OnBnClickedBtnHoming()
{
	m_bAutoHoming = !m_bAutoHoming;

	SetDlgItemText(IDC_BTN_HOMING, m_bAutoHoming ? _T("Enabled") : _T("Disabled"));
}


void CViewRestoration::OnDestroy()
{
	m_listCtrl.SaveHeaderItemInfo();

	CViewDevice::OnDestroy();

	if (m_bAutoRestoration)
	{
		CDocDevice* pDoc = static_cast<CDocDevice*>(GetDocument());
		::PostMessage(AfxGetMainWnd()->m_hWnd, CMainFrame::UM_SHOW_ASSIGNMENT_VIEW, pDoc->GetAddress(), pDoc->GetSelectMotorIndex() );
	}
}

void CViewRestoration::PrepareDestroy()
{
	m_bAutoRestoration = FALSE;
}
