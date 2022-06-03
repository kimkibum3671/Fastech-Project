// CPageConfigDeviceDatabase.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageConfigDeviceDatabase.h"
#include "afxdialogex.h"

#include "CDeviceList.h"
#include "CDataBaseManager.h"

#include "CSheetConfigDeviceSingle.h"
#include "CDlgDeviceTotalConfiguration.h"

// CPageConfigDeviceDatabase dialog

IMPLEMENT_DYNAMIC(CPageConfigDeviceDatabase, CMFCPropertyPage)

CPageConfigDeviceDatabase::CPageConfigDeviceDatabase(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDP_CONFIG_DEVICE_DATABASE)
{

}

CPageConfigDeviceDatabase::~CPageConfigDeviceDatabase()
{
}

void CPageConfigDeviceDatabase::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageConfigDeviceDatabase, CMFCPropertyPage)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_ADD, &CPageConfigDeviceDatabase::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_MOD, &CPageConfigDeviceDatabase::OnBnClickedBtnMod)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DEVICE, &CPageConfigDeviceDatabase::OnLvnItemchangedListDevice)
	ON_BN_CLICKED(IDC_BTN_DEL, &CPageConfigDeviceDatabase::OnBnClickedBtnDel)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DEVICE, &CPageConfigDeviceDatabase::OnNMDblclkListDevice)
END_MESSAGE_MAP()


// CPageConfigDeviceDatabase message handlers


BOOL CPageConfigDeviceDatabase::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	if (1)	// Initialize ListCtrl
	{
		m_listCtrl.SubclassDlgItem(IDC_LIST_DEVICE, this);
		m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
		HRESULT result = ::SetWindowTheme(m_listCtrl.m_hWnd, _T("Explorer"), NULL);

		m_listCtrl.InsertColumn(0, _T("Device Name"));
		//m_listCtrl.InsertColumn(1, _T("Device ID"));
		m_listCtrl.InsertColumn(1, _T("Motor Count"));
		//m_listCtrl.InsertColumn(2, _T("Homing Timeout"));
		m_listCtrl.InsertColumn(2, _T("Nominal Info."));
		m_listCtrl.InsertColumn(3, _T("Latest Info."));

		CDataBaseProject* pProject = CDataBaseProject::GetInstance();
		if (pProject->OpenDataBase())
		{
			if (pProject->GetDeviceList()) // 등록 Project 가 존재한다면...
			{
				while (CDataBaseRecordData* pRecord = pProject->SeekToNext())
				{
					CDeviceMotionGate* pDevice = new CDeviceMotionGate;
					if (pDevice->LoadFromDatabase(pRecord))
					{
						if (!(pDevice->GetDeletedStatus()))
						{
							SetListItem(pDevice);
						}
					}
					else
					{
						//	뭔가 문제가 있다.
						delete pDevice;
						ASSERT(0);
					}
				}
			}

			pProject->CloseDataBase();

			const INT nCount = m_listCtrl.GetItemCount(); // 등록 아이템 개수를 반환한다.
			for (INT nIndex = 0; nIndex < nCount; nIndex++)
			{
				CString strDevice(m_listCtrl.GetItemText(nIndex, 0));
				INT nID = 0;
				CString strTimeStamp;
				if (pProject->GetNominalBackupInfo(strDevice, &nID, &strTimeStamp) &&
					-1 != nID )
				{
					m_listCtrl.SetItemText(nIndex, 2, strTimeStamp);
				}

				if (pProject->GetLatestRestoredInfo(strDevice, &nID, &strTimeStamp) &&
					-1 != nID )
				{
					m_listCtrl.SetItemText(nIndex, 3, strTimeStamp);
				}
			}
		}

		//	Load HeadSize
		INT nWidth1, nWidth2;
		m_listCtrl.SetColumnWidth(0, LVSCW_AUTOSIZE);
		nWidth1 = m_listCtrl.GetColumnWidth(0);
		m_listCtrl.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
		nWidth2 = m_listCtrl.GetColumnWidth(0);
		m_listCtrl.SetColumnWidth(0, max(nWidth1, nWidth2) + 30);

		m_listCtrl.SetColumnWidth(1, LVSCW_AUTOSIZE);
		nWidth1 = m_listCtrl.GetColumnWidth(1);
		m_listCtrl.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
		nWidth2 = m_listCtrl.GetColumnWidth(1);
		m_listCtrl.SetColumnWidth(1, max(nWidth1, nWidth2) + 30);

		m_listCtrl.SetColumnWidth(2, LVSCW_AUTOSIZE);
		nWidth1 = m_listCtrl.GetColumnWidth(1);
		m_listCtrl.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);
		nWidth2 = m_listCtrl.GetColumnWidth(2);
		m_listCtrl.SetColumnWidth(2, max(nWidth1, nWidth2) + 30);

		m_listCtrl.SetColumnWidth(3, LVSCW_AUTOSIZE);
		nWidth1 = m_listCtrl.GetColumnWidth(1);
		m_listCtrl.SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER);
		nWidth2 = m_listCtrl.GetColumnWidth(2);
		m_listCtrl.SetColumnWidth(3, max(nWidth1, nWidth2) + 30);

		m_listCtrl.SetColumnWidth(4, LVSCW_AUTOSIZE);
		nWidth1 = m_listCtrl.GetColumnWidth(1);
		m_listCtrl.SetColumnWidth(4, LVSCW_AUTOSIZE_USEHEADER);
		nWidth2 = m_listCtrl.GetColumnWidth(2);
		m_listCtrl.SetColumnWidth(4, max(nWidth1, nWidth2) - 30);

		m_listCtrl.SetColumnWidth(5, LVSCW_AUTOSIZE);
		nWidth1 = m_listCtrl.GetColumnWidth(1);
		m_listCtrl.SetColumnWidth(5, LVSCW_AUTOSIZE_USEHEADER);
		nWidth2 = m_listCtrl.GetColumnWidth(2);
		m_listCtrl.SetColumnWidth(5, max(nWidth1, nWidth2) - 30);
	}

	m_listCtrl.LoadHeaderItemInfo();

	InitControlTextSize();

	return TRUE;
}


void CPageConfigDeviceDatabase::OnDestroy()
{
	m_listCtrl.SaveHeaderItemInfo();

	CMFCPropertyPage::OnDestroy();
}

void CPageConfigDeviceDatabase::SetListItem(LPVOID pDeviceMotiongate, const INT nIndex)
{
	CDeviceMotionGate* pDevice = static_cast<CDeviceMotionGate*>(pDeviceMotiongate);
	
	CString strTemp;

 	INT nItem = 0;
	
	if (-1 == nIndex)
	{
		const INT nCount = m_listCtrl.GetItemCount();
		nItem = m_listCtrl.InsertItem(nCount, pDevice->GetDatabaseSystemName());
	}
	else
	{
		nItem = nIndex;
		
		m_listCtrl.SetItemText(nItem, 0, pDevice->GetDatabaseSystemName());
	}
	
	/* Device ID 값을 ListCtrl에 저장..
	strTemp.Format(_T("%d"), pDevice->GetDatabaseDeviceID());
	m_listCtrl.SetItemText(nItem, 1, strTemp);
	*/

	strTemp.Format(_T("%d"), pDevice->GetDatabaseMotorCount());
	m_listCtrl.SetItemText(nItem, 1, strTemp);

	/*strTemp.Format(_T("%d"), pDevice->GetDatabaseHomingTimeout());
	m_listCtrl.SetItemText(nItem, 2, strTemp);*/

	m_listCtrl.SetItemData(nItem, reinterpret_cast<DWORD_PTR>(pDevice));
}


void CPageConfigDeviceDatabase::OnBnClickedBtnAdd()
{
	CDeviceMotionGate* pDevice = new CDeviceMotionGate;

	ShowEditPropertySheet(pDevice, -1, TRUE);
}


void CPageConfigDeviceDatabase::OnBnClickedBtnMod()
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if (pos)
	{
		const INT nItem = m_listCtrl.GetNextSelectedItem(pos);
		CDeviceMotionGate* pDevice = reinterpret_cast<CDeviceMotionGate*>(m_listCtrl.GetItemData(nItem));
		ShowEditPropertySheet(pDevice, nItem, FALSE);
	}
}

const BOOL CPageConfigDeviceDatabase::IsValidSystemName(LPCTSTR lpszSystemName, LPVOID pDeviceMotiongate)
{
	const INT nCount = m_listCtrl.GetItemCount();
	for (INT nIndex = 0; nCount > nIndex; nIndex++)
	{
		if (lpszSystemName == m_listCtrl.GetItemText(nIndex, 0) &&
			reinterpret_cast<DWORD_PTR>(pDeviceMotiongate) != m_listCtrl.GetItemData(nIndex) )
		{
			return FALSE;
		}
	}

	return TRUE;
}


const BOOL CPageConfigDeviceDatabase::IsValidDeviceID(const INT nDeviceID, LPVOID pDeviceMotiongate)
{
	CString strDeviceID;

	strDeviceID.Format(_T("%d"), nDeviceID);

	const INT nCount = m_listCtrl.GetItemCount();
	for (INT nIndex = 0; nIndex < nCount; nIndex++)
	{
		if (strDeviceID == m_listCtrl.GetItemText(nIndex, 1) &&
			reinterpret_cast<DWORD_PTR>(pDeviceMotiongate) != m_listCtrl.GetItemData(nIndex))
		{
			return FALSE;
		}
	}
	return TRUE;
}


void CPageConfigDeviceDatabase::ShowInvalidSystemName()
{
	CString strContent(_T(""));
	CString strMainInstruction("System alias ( name or device id) is already used in the system.");
	CString strTitle( _T("Failed to add or modify device information."));

	CTaskDialog taskDialog(strContent, strMainInstruction, strTitle, TDCBF_CANCEL_BUTTON | TDCBF_CLOSE_BUTTON);

	taskDialog.AddCommandControl(IDYES, _T("OK, I understand."));

	taskDialog.SetMainIcon(TD_ERROR_ICON);

	taskDialog.SetDialogWidth(300);

	taskDialog.DoModal();
}


void CPageConfigDeviceDatabase::OnLvnItemchangedListDevice(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	INT nSelectionCount = 0;

	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	while (pos)
	{
		nSelectionCount++;
		m_listCtrl.GetNextSelectedItem(pos);
	}

	GetDlgItem(IDC_BTN_MOD)->EnableWindow( 1 == nSelectionCount);
	GetDlgItem(IDC_BTN_DEL)->EnableWindow( nSelectionCount);
}

void CPageConfigDeviceDatabase::ShowEditPropertySheet(LPVOID pDeviceMotiongate, const INT nItem, const BOOL bInsert)
{
	CDeviceMotionGate* pDevice = static_cast<CDeviceMotionGate*>(pDeviceMotiongate);

	//CSheetConfigDeviceSingle dlg;
	CString strTitle;
	INT nRequire;
	CDlgDeviceTotalConfiguration dlg;

	dlg.SetMotiongateCtrl(pDevice);
	dlg.SetTotalConfig(TRUE); 
	dlg.SetNoticeConfig(FALSE);

	for (INT i = 0; i < m_listCtrl.GetItemCount(); i++)
	{
		dlg.SetTotalConfigurationList(m_listCtrl.GetItemText(i, 0));
	}
	
	if (bInsert)
	{
		strTitle.Format(_T("Insert Tooling"));
		nRequire = require_system_insert;
	}
	else
	{
		strTitle.Format(_T("Modify Tooling"));
		nRequire = require_system_modify;
	}
	
	dlg.SetTitleText(strTitle);
	dlg.SetConfigMethod(nRequire);
	
	CWnd* pParent = GetParent();
	pParent->PostMessageW(WM_SHOWWINDOW, FALSE, SW_PARENTCLOSING);

	BOOL bExit = FALSE;
	while (FALSE == bExit)
	{
		pDevice->EnterEditMode();
		CBitmap* pBitmap = pDevice->GetSystemEditImage();

		if (IDOK == dlg.DoModal())
		{
			CBitmap* pBitmap = pDevice->GetSystemEditImage();
			pDevice->ExitEditMode(); // 여기서 EditControl에 설정한 데이터들을 m_stDatabaseDeviceInfo에 저장한다.
			/* Device ID 살렸을때.. Device ID 의 중복 검사를 실시한다.
			if (IsValidSystemName(pDevice->GetSystemName(), pDevice) && IsValidDeviceID(pDevice->GetDatabaseDeviceID(), pDevice))
			{	// No Error
				SetListItem(pDevice, nItem);
			}
			else
			{
				ShowInvalidSystemName();
			}
			*/
			if (IsValidSystemName(pDevice->GetSystemName(), pDevice))
			{	
				SetListItem(pDevice, nItem);
			}
			
			bExit = TRUE;
		}
		else
		{
			if (-1 == nItem)
			{	//	신규 등록을 위해서 메모리를 생성했는데 등록을 취소하였으므로 메모리를 반환한다.
				delete pDevice;
			}
			
			bExit = TRUE;
		}
	}

	pParent->PostMessageW(WM_SHOWWINDOW, TRUE, SW_PARENTOPENING);
}


void CPageConfigDeviceDatabase::OnBnClickedBtnDel()
{
	//CString message("Are you sure to delete a selected item?");
	CString mainInstruction("Are you sure to delete a selected item?");
	CString message("All data related to the selected device will be deleted. \nWould you like to proceed?");
	CString title("Confirm to action.");

	CTaskDialog taskDialog(message, mainInstruction, title, !TDCBF_CANCEL_BUTTON, !(TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON));

	taskDialog.AddCommandControl(IDYES, L"Yes.");
	taskDialog.AddCommandControl(IDNO, L"No.");

	taskDialog.SetMainIcon(IDI_EXCLAMATION);

	// Change the default command button control
	taskDialog.SetDefaultCommandControl(IDYES);
	taskDialog.SetDialogWidth(message.GetLength() * 3);

	if (IDYES == taskDialog.DoModal())
	{
		CList<INT, INT> lstDelete;

		if (1)
		{	//	역순으로 삭제하기 위한 정렬
			POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
			while (pos)
			{
				INT nIndex = m_listCtrl.GetNextSelectedItem(pos);
				CString strItem = m_listCtrl.GetItemText(nIndex, 0);
				
				lstDelete.AddHead(nIndex);
				
				m_BackupDelList.AddTail(strItem);
				/*
				if (true)
				{
					CString message("");
					CString mainInstruction("Are you sure to delete a selected item Backup Data?");
					CString title("Confirm to action");

					CTaskDialog taskDlg(message, mainInstruction, title, TDCBF_CANCEL_BUTTON);
					taskDlg.AddCommandControl(IDYES, L"Yes.");
					taskDlg.AddCommandControl(IDNO, L"No.");

					taskDlg.SetMainIcon(IDI_QUESTION);

					// Change the default command button control
					taskDlg.SetDefaultCommandControl(IDYES);

					if (IDYES == taskDlg.DoModal())
					{
						m_BackupDelList.AddTail(m_listCtrl.GetItemText(m_listCtrl.GetNextSelectedItem(pos), 0));
					}

				}
				*/
			}
		}

		if (1)
		{	// 삭제 실시
			POSITION pos = lstDelete.GetHeadPosition();
			while (pos)
			{
				const INT nIndex = lstDelete.GetNext(pos);
				delete reinterpret_cast<CDeviceMotionGate*>(m_listCtrl.GetItemData(nIndex));
				m_listCtrl.DeleteItem(nIndex);
			}
		}

		lstDelete.RemoveAll();
	}
}


void CPageConfigDeviceDatabase::OnOK()
{
	CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();
	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	/*
	// Backup 관련 진행 사항..
	if (pProjectDB->OpenDataBase())
	{
		pProjectDB->Begin();

		pProjectDB->SetDeleteProjectAll(true); // [Project] Delete 항목 전체 1로 설정.
		pProjectDB->UpdateRestoreBackupListTable(NULL, true, false); // [Delete Backup List]에 전체 리스트 등록..
		pProjectDB->DeleteBackupListTable();
			
		const INT nCount = m_listCtrl.GetItemCount();
		for(INT nIndex = 0; nCount > nIndex; nIndex++)
		{
			CDeviceMotionGate* pDevice = reinterpret_cast<CDeviceMotionGate*>(m_listCtrl.GetItemData(nIndex));
			pDevice->UpdateDeleteValue(false); // Boolean Check
			pDevice->CopyList(_T("Restore_Backup_List"), _T("Backup_List")); // [Restore_Backup_List] 현재 삭제되지 않은 항목 삭제. 
			pDevice->UpdateRestoreBackupListTable(); // Delete Backup List에 현재 삭제되지 않은 Project Name 에 대한 정보들을 삭제.
			pDevice->SaveToDatabase(nIndex);
				
		}
		
		pProjectDB->Commit();
		pProjectDB->CloseDataBase();
		
	}
	*/
	
	//DeleteBackupData

	if (pProjectDB->OpenDataBase())
	{ // 삭제 대상 장치 백업 데이터 삭제 실시.
		INT	nCount = 0;
		BOOL bDelete = FALSE;
		CString afxMessage;
		pProjectDB->Begin();
		
		if (1)
		{	// 삭제 실시
			POSITION pos = m_BackupDelList.GetHeadPosition();
			
			while (pos)
			{
				CDeviceMotionGate* pDevice = NULL;
				const CString strName = m_BackupDelList.GetNext(pos);
				bDelete = (pProjectDB->DeleteBackupListItem(strName) & 
						   pProjectDB->DeleteBackupDataItem(strName) &
						   pProjectDB->DeleteBackupReservationDataAll(strName));
				
				if (TRUE == pDeviceList->GetMotiongateByName(strName, pDevice))
				{
					pDevice->InitDatabase();
				}
			}
			
			m_BackupDelList.RemoveAll();
		}
		
		pProjectDB->Commit();

		pProjectDB->DoVacuum();

		pProjectDB->CloseDataBase();
	}

	if (pProjectDB->OpenDataBase())
	{
		pProjectDB->Begin();
		pProjectDB->DeleteProjectTable();
		const INT nCount = m_listCtrl.GetItemCount();
		for (INT nIndex = 0; nCount > nIndex; nIndex++)
		{
			CDeviceMotionGate* pDevice = reinterpret_cast<CDeviceMotionGate*>(m_listCtrl.GetItemData(nIndex));
			pDevice->SaveToDatabase(nIndex);
		}

		pProjectDB->Commit();
		pProjectDB->CloseDataBase();
	}

	CMFCPropertyPage::OnOK();
}


void CPageConfigDeviceDatabase::OnNMDblclkListDevice(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if (pos)
	{
		const INT nItem = m_listCtrl.GetNextSelectedItem(pos);
		CDeviceMotionGate* pDevice = reinterpret_cast<CDeviceMotionGate*>(m_listCtrl.GetItemData(nItem));
		ShowEditPropertySheet(pDevice, nItem, FALSE);
	}
}



void CPageConfigDeviceDatabase::InitControlTextSize()
{
	if (true)
	{
		LOGFONT lf = { 0, };
		afxGlobalData.fontRegular.GetLogFont(&lf);

		lf.lfHeight = -15;
		m_BigFont.CreateFontIndirect(&lf);

		GetDlgItem(IDC_BTN_UP)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_DOWN)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_ADD)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_MOD)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_DEL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_LIST_DEVICE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_WARNING)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_WARNING)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}

	/*if (true)
	{
		m_BigFont.CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, TEXT_STYLE);

		GetDlgItem(IDC_BTN_UP)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_DOWN)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_ADD)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_MOD)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_DEL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_LIST_DEVICE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_WARNING)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_WARNING)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}*/
	
}
