// CPageRestorationSelectedMethod.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageRestorationSelectedMethod.h"

#include "afxdialogex.h"

#include "CSheetSelectRestoration.h"

#include "CDataBaseManager.h"



#define RGB_BK RGB(240, 240, 240)
#define RGB_WHITE RGB(255, 255, 255) 
// CPageRestorationSelectedMethod


IMPLEMENT_DYNAMIC(CPageRestorationSelectedMethod, CMFCPropertyPage)

CPageRestorationSelectedMethod::CPageRestorationSelectedMethod()
	: CMFCPropertyPageEx(IDP_RESTORATION_SELECT_METHOD)
{

}

CPageRestorationSelectedMethod::~CPageRestorationSelectedMethod()
{
}

//
//BEGIN_MESSAGE_MAP(CPageRestorationSelectedMethod, CMFCPropertyPage)
//END_MESSAGE_MAP()
//

BEGIN_MESSAGE_MAP(CPageRestorationSelectedMethod, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_BTN_BACK, &CPageRestorationSelectedMethod::OnBnClickedBtnBack)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CPageRestorationSelectedMethod::OnBnClickedBtnNext)
	ON_BN_CLICKED(IDC_SEL_METHOD1, &CPageRestorationSelectedMethod::OnBnClickedSelMethod1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MAIN, &CPageRestorationSelectedMethod::OnLvnItemchangedListMain)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_MAIN_NOMINAL, &CPageRestorationSelectedMethod::OnBnClickedBtnMainNominal)
	ON_BN_CLICKED(IDC_BTN_RECENT, &CPageRestorationSelectedMethod::OnBnClickedBtnRecent)
END_MESSAGE_MAP()

void CPageRestorationSelectedMethod::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}



// CPageRestorationSelectedMethod message handlers


BOOL CPageRestorationSelectedMethod::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	m_btnPrev.SubclassDlgItem(IDC_BTN_BACK, this);
	m_btnPrev.SetImage(IDB_BACK);

	m_btnNext.SubclassDlgItem(IDC_BTN_NEXT, this);
	m_btnNext.SetImage(IDB_NEXT);

	m_btnMethod1.SubclassDlgItem(IDC_SEL_METHOD1, this);
	m_btnMethod1.SetImage(IDB_UNCHECK);
	m_btnMethod1.SetCheck(BST_CHECKED);

	m_btnMethod2.SubclassDlgItem(IDC_SEL_METHOD2, this);
	m_btnMethod2.SetImage(IDB_MAP1_OFF);

	m_btnMethod3.SubclassDlgItem(IDC_SEL_METHOD3, this);
	m_btnMethod3.SetImage(IDB_MAP2_OFF);

	m_nSelMethod = CSheetSelectRestoration::method_direct_restore;

	m_bToggle = false;
	m_bRestoreEnable = false;
	m_alarm.nErrorCount = 0;
	
	InitializeListCtrl();

	LoadProjectList();

	InitControlTextSize();

	return TRUE;
}


void CPageRestorationSelectedMethod::OnBnClickedBtnBack()
{
	ActivatePrevPage();
}


void CPageRestorationSelectedMethod::OnBnClickedBtnNext()
{
	if (m_bRestoreEnable)
	{
		ActivateNextPage();
	}
	else
	{
		//m_RestorationAlarm.DoModal();
		ShowDisableAlarm();
	}
	
}


void CPageRestorationSelectedMethod::OnBnClickedSelMethod1()
{
	GetDlgItem(IDC_BTN_NEXT)->EnableWindow((-1 == m_nNominalID) && (false == m_bToggle) ? FALSE : TRUE);

	if (!m_bToggle) // Nominal 일때..
	{
		m_btnMethod1.SetImage(IDB_CHECK);
		m_bToggle = true;
		m_wndList.EnableWindow(FALSE);

		if (-1 == m_nNominalID)
		{
			return;
		}

		const INT nCount = m_wndList.GetItemCount();
		for (INT nIndex = 0; nCount; nIndex++)
		{
			if (m_wndList.GetItemData(nIndex) == m_nNominalID)
			{
				m_wndList.SelectSingleItem(nIndex);
				m_wndList.SetFocus();
				break;
			}

		}
	}
	else
	{
		m_btnMethod1.SetImage(IDB_UNCHECK);
		m_bToggle = false;
		m_wndList.EnableWindow(TRUE);

	}

	m_btnMethod1.Invalidate(FALSE);

	m_wndList.SetTextBkColor(m_wndList.OnGetCellTextColor(0, 0));


	if (0 != m_nSelMethod)
	{
		m_btnMethod1.SetImage(IDB_CHECK);
		m_btnMethod1.Invalidate(FALSE);

		m_btnMethod2.SetImage(IDB_MAP1_OFF);
		m_btnMethod2.Invalidate(FALSE);

		m_btnMethod3.SetImage(IDB_MAP2_OFF);
		m_btnMethod3.Invalidate(FALSE);

		m_nSelMethod = 0;

		ChangeProjectInfo(m_nSelMethod);
	}
}

void CPageRestorationSelectedMethod::OnLvnItemchangedListMain(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	*pResult = 0;

	if (pNMLV->uNewState & LVIS_SELECTED || pNMLV->uOldState & LVIS_SELECTED)
	{
		this->ChangeProjectInfo(m_nSelMethod);
	}
	
}

void CPageRestorationSelectedMethod::ChangeProjectInfo(const INT nMethod)
{
	CSheetSelectRestoration* pParent = static_cast<CSheetSelectRestoration*>(GetParent());
	CDeviceMotionGate* pDevice = NULL;

	CWnd* pWnd = GetDlgItem(IDC_BTN_NEXT);
	

	POSITION pos = m_wndList.GetFirstSelectedItemPosition();

	
	/*
	if (pos)
	{
		const INT nIndex = m_wndList.GetNextSelectedItem(pos);

		INT nBackupID = m_wndList.GetItemData(nIndex);

		if (CDeviceList::GetInstance()->GetMotiongateByAddress(pParent->GetAddress(), pDevice))
		{
			if (CheckEnableRestoration(pDevice, nBackupID))
			{
				pParent->SetProjectInfo(
					m_wndList.GetItemData(nIndex),
					m_wndList.GetItemText(nIndex, 0),
					m_wndList.GetItemText(nIndex, 1));

				if (FALSE == pWnd->IsWindowEnabled())
				{
					pWnd->EnableWindow();
				}
			}

			else
			{
				pParent->SetProjectInfo();

				if (pWnd->IsWindowEnabled())
				{
					pWnd->EnableWindow(FALSE);
				}
			}
		}
	}
	*/
	if (pos)
	{
		/*const INT nIndex = m_wndList.GetNextSelectedItem(pos);
				
		pParent->SetProjectInfo(
			m_wndList.GetItemData(nIndex),
			m_wndList.GetItemText(nIndex, 0),
			m_wndList.GetItemText(nIndex, 1));

		if (FALSE == pWnd->IsWindowEnabled())
		{
			pWnd->EnableWindow();
		}*/
		const INT nIndex = m_wndList.GetNextSelectedItem(pos);

		INT nBackupID = m_wndList.GetItemData(nIndex);

		if (CDeviceList::GetInstance()->GetMotiongateByAddress(pParent->GetAddress(), pDevice))
		{
			if (CheckEnableRestoration(pDevice, nBackupID))
			{
				pParent->SetProjectInfo(
					m_wndList.GetItemData(nIndex),
					m_wndList.GetItemText(nIndex, 0),
					m_wndList.GetItemText(nIndex, 1));
			}
			else
			{
				pParent->SetProjectInfo();
			}

			if (FALSE == pWnd->IsWindowEnabled())
			{
				pWnd->EnableWindow();
			}
		
		}
	}
	else
	{
		pParent->SetProjectInfo();

		if (pWnd->IsWindowEnabled())
		{
			pWnd->EnableWindow(FALSE);
		}
	}

	pParent->SetMethod(m_nSelMethod);
}

const BOOL CPageRestorationSelectedMethod::CheckEnableRestoration(CDeviceMotionGate* pDevice, INT nBackupID)
{
	CDataBaseProject* pProject = CDataBaseProject::GetInstance();
	
	CString strError,strIndex, strCount, strEndPoint;
	BOOL bEnable = TRUE;

	if (pProject->OpenDataBase())
	{
		CString strName = pDevice->GetSystemName();
		INT nIndex = -1;
		INT nMotorCount = -1;
		INT nMotorEndPoint = -1;
		
		InitAlarmStruct();
		
		if (pProject->GetBackupFileMotorIndex(strName, nBackupID))
		{// 인덱스 비교
			
			DWORD dwFileMotor = 0x00;
			DWORD dwToolMotor = pDevice->GetConnectedMotorStatus();
			CString strBackupIndex;
			CString strMotorIndex;
			CString str;

			while (CDataBaseRecordData* pRecord = pProject->SeekToNext())
			{
				if (LoadBackupFileMotorIndex(pRecord, nIndex))
				{
					/*if (!pDevice->IsMotorConnected(nIndex)) bEnable = FALSE;
					str.Format(_T("(#%d)"), nIndex);
					strBackupIndex += str;
					nFileIndex[nFileCount] = nIndex;
					nFileCount++;*/
					dwFileMotor += (0x01 << nIndex);
					
					if (!pDevice->IsMotorConnected(nIndex))
					{ // 실제 모터에는 연결되어있지 않음
						//nFileIndex[nFileCount] = nIndex;
						//str.Format(_T("'Motor#%d' is exist in the backup data, but not in the device."), nIndex);
						str.Format(_T("'Motor#%d' to restore could not be found."), nIndex);
						SetIndexError(str);
						bEnable = FALSE;
					}
				}
			}
			
			for (INT nMotor = 0; nMotor < 9; nMotor++)
			{
				if (pDevice->IsMotorConnected(nMotor))
				{
					BOOL bExist = (dwFileMotor >> nMotor) & 0x01;
					if (bExist == FALSE)
					{
						//str.Format(_T("'Motor#%d' is exist in the device, but not in the backup data."), nMotor);
						str.Format(_T("there is no backup data for 'Motor#%d'"), nMotor);
						SetIndexError(str);
						bEnable = FALSE;
					}
				}
			}

		}
		else
		{
			strError.Format(_T("[Restoration Information] database file open error. \n"));
			SetIndexError(strError);
			bEnable = FALSE;
		}
		
		/*
		if (TRUE) // bEnable
		{
			if (pProject->GetBackupMotorEndNumber(strName, nBackupID, nMotorEndPoint))
			{// 끝번호 비교
				INT nEndPoint = 0;
				for (INT i = 0; i < 9; i++)
				{
					if (pDevice->IsMotorConnected(i))
					{
						nEndPoint = i;
					}
				}

				if (!pDevice->IsMotorConnected(nMotorEndPoint))
				{
					strCount.Format(_T("Restored Data Last Index : Motor[#%d] ≠ Tooling Data Last Index : Motor[#%d] \n"), nMotorEndPoint, nEndPoint);
					bEnable = FALSE;
					SetIndexError(strCount);
				}
			}
			else
			{
				strError.Format(_T("[Restoration Information] database file open error. \n"));
				SetIndexError(strError);
				bEnable = FALSE;
			}


			if (pProject->GetBackupMotorCount(strName, nBackupID, nMotorCount))
			{// 개수 비교
				if (pDevice->GetMotorCount() != nMotorCount)
				{
					strEndPoint.Format(_T("Restored Data [Motor Count : %d] ≠ Tooling Data [Motor Count : %d] \n"), nMotorCount, pDevice->GetConnectedMotorCount());
					SetIndexError(strEndPoint);
					bEnable = FALSE;
				}
			}
			else
			{
				strError.Format(_T("[Restoration Information] database file open error. \n"));
				SetIndexError(strError);
				bEnable = FALSE;
			}
		}
		*/
		
	}

	pProject->CloseDataBase();

	m_bRestoreEnable = bEnable;

	//SetRestorationEnableStatus(strIndex, strCount, strEndPoint, strError);
	return bEnable;
}


void CPageRestorationSelectedMethod::InitializeListCtrl()
{
	m_wndList.SubclassDlgItem(IDC_LIST_MAIN, this);
	m_wndList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SINGLEROW | LVS_EX_DOUBLEBUFFER);
	SetWindowTheme(m_wndList.m_hWnd, _T("Explorer"), NULL);

	INT nIndex = 0;
	m_wndList.InsertColumn(nIndex++, _T("Time Stamp"), 0, 150);
	m_wndList.InsertColumn(nIndex++, _T("Comment"), 0, 100);


	m_wndList.DeleteAllItems();

	m_wndList.LoadHeaderItemInfo();
}

void CPageRestorationSelectedMethod::LoadProjectList()
{
	CSheetSelectRestoration* pParent = static_cast<CSheetSelectRestoration*>(GetParent());
	CDeviceMotionGate* pDevice = pParent->GetMotiongateCtrl();

	CString strSystemName(pDevice->GetSystemName());

	CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();


	m_nRestoredID = pProjectDB->GetLatestRestoredID(strSystemName);
	GetDlgItem(IDC_BTN_RECENT)->EnableWindow(-1 != m_nRestoredID ? TRUE : FALSE);

	m_nNominalID = pProjectDB->GetNominalBackupID(strSystemName);
	GetDlgItem(IDC_BTN_MAIN_NOMINAL)->EnableWindow(-1 != m_nNominalID ? TRUE : FALSE);
	GetDlgItem(IDC_SEL_METHOD1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SEL_METHOD1)->EnableWindow(-1 != m_nNominalID ? TRUE : FALSE);
	
	INT nNominalIndex = 0;
	
	
	/*if (pProjectDB->GetProjectBackupList(strSystemName) && pProjectDB->SeekToBegin())
	{
		INT nIndex = 0;
		CString strTemp;
		while (CDataBaseRecordData* pData = pProjectDB->SeekToNext())
		{
			
			pData->GetTextData(_T("TimeStamp"), strTemp);
			m_wndList.InsertItem(nIndex, strTemp);

			pData->GetTextData(_T("Backup_Name"), strTemp);
			m_wndList.SetItemText(nIndex, 1, strTemp);

			INT nBackupID = -1;
			pData->GetIntergerData(_T("Backup_ID"), nBackupID);
			m_wndList.SetItemData(nIndex, nBackupID);

			nIndex++;
		}
	}*/
	

	
	if (pProjectDB->GetProjectBackupList(strSystemName) && pProjectDB->SeekToBegin())
	{ // 수정중
		INT nIndex = 0;
		CString strTemp;
		
		while (CDataBaseRecordData* pData = pProjectDB->SeekToNext())
		{
			INT iItem = 0;

			INT nBackupID = -1;
			pData->GetIntergerData(_T("Backup_ID"), nBackupID);
			
			if (m_nNominalID != nBackupID)
			{
				pData->GetTextData(_T("TimeStamp"), strTemp);
				iItem = m_wndList.InsertItem(m_wndList.GetItemCount(), strTemp);
				
				pData->GetTextData(_T("Backup_Name"), strTemp);
				m_wndList.SetItemText(iItem, 1, strTemp);
			}
			else
			{
				pData->GetTextData(_T("TimeStamp"), strTemp);
				iItem = m_wndList.InsertItem(0, strTemp);
				
				pData->GetTextData(_T("Backup_Name"), strTemp);
				m_wndList.SetItemText(iItem, 1, strTemp);
			}
			
			m_wndList.SetItemData(iItem, nBackupID);

			if (nBackupID == m_nNominalID)
			{
				//SetNominalData(0);
				SetNominalData(iItem);
			}
		}
	}
	
	
	
	
	

	//if (-1 != m_nNominalID)
	//{
	//	//SetNominalData(0);
	//	SetNominalData(m_nNominalID);
	//}
}

void CPageRestorationSelectedMethod::OnDestroy()
{
	m_wndList.SaveHeaderItemInfo();

	CMFCPropertyPage::OnDestroy();
}


void CPageRestorationSelectedMethod::OnBnClickedBtnMainNominal()
{
	if (-1 == m_nNominalID)
	{
		return;
	}

	const INT nCount = m_wndList.GetItemCount();
	for (INT nIndex = 0; nCount; nIndex++)
	{
		if (m_wndList.GetItemData(nIndex) == m_nNominalID)
		{
			m_wndList.SelectSingleItem(nIndex);
			m_wndList.SetFocus();
			break;
		}

	}
}


void CPageRestorationSelectedMethod::OnBnClickedBtnRecent()
{
	if (-1 == m_nRestoredID)
	{
		return;
	}

	const INT nCount = m_wndList.GetItemCount();
	for (INT nIndex = 0; nCount; nIndex++)
	{
		if (m_wndList.GetItemData(nIndex) == m_nRestoredID)
		{
			m_wndList.SelectSingleItem(nIndex);
			m_wndList.SetFocus();
			break;
		}
	}
}


// CPageRestorationSelectedMethod 메시지 처리기


//BOOL CPageRestorationSelectedMethod::OnCommand(WPARAM wParam, LPARAM lParam)
//{
//	UINT id = (UINT)LOWORD(wParam);
//
//	switch (id)
//	{
//	default:
//		break;
//	}
//
//	return CMFCPropertyPageEx::OnCommand(wParam, lParam);
//}


bool CPageRestorationSelectedMethod::GetCheckState(int nIcon)
{
	bool bResult = false;
	bResult = ((CButton*)GetDlgItem(nIcon))->GetCheck() == BST_CHECKED ?  true :  false;
	return bResult;
}


void CPageRestorationSelectedMethod::SetNominalData(const INT nSelectIndex)
{
	const INT nCount = m_wndList.GetItemCount();
	for (INT nIndex = 0; nIndex < nCount; nIndex++)
	{
		if (nIndex == nSelectIndex)
		{
			m_wndList.SetItemBkColor(nIndex, RGB(0x00, 0xff, 0x00));
		}
		else
		{
			m_wndList.RemoveItemBkColor(nIndex);
		}
	}
}

const BOOL CPageRestorationSelectedMethod::LoadBackupFileMotorIndex(LPVOID pDatabaseRecordData, INT& nIndex)
{
	CDataBaseRecordData* pRecord = static_cast<CDataBaseRecordData*>(pDatabaseRecordData);

	if (FALSE == pRecord->GetIntergerData(_T("Motor_ID"), nIndex))
	{
		return FALSE;
	}

	return TRUE;
}


void CPageRestorationSelectedMethod::SetRestorationEnableStatus(CString strIndexError, CString strCountError, CString strEndPointError, CString strDbError)
{
	SetRestorationAlarmStatus(strIndexError, strCountError, strEndPointError, strDbError);
	m_RestorationAlarm.SetRestorationEnableStatus(strIndexError, strCountError, strEndPointError, strDbError);
}


void CPageRestorationSelectedMethod::ShowDisableAlarm()
{
	CString strContent("");
	CString strMainInstruction("Can not execute restoration!");
	CString strTitle("Restoration Alarm");
	CString strMessage;
	INT nWidth = 0;
	
	CTaskDialog taskDlg(strContent, strMainInstruction, strTitle, TDCBF_OK_BUTTON);

	for (INT i = 0; i < m_alarm.nErrorCount; i++)
	{
		taskDlg.AddCommandControl(i + 1, m_alarm.strError[i], FALSE);
		//taskDlg.SetCommandControlOptions(i + 1, FALSE);
		
		if (nWidth < m_alarm.strError[i].GetLength()) nWidth = m_alarm.strError[i].GetLength();
	}

	taskDlg.SetMainIcon(TD_ERROR_ICON);
	
	//taskDlg.AddCommandControl(1, _T("OK, I understand"));


	taskDlg.SetDialogWidth(nWidth * 6);
	taskDlg.DoModal();
	
}


void CPageRestorationSelectedMethod::SetRestorationAlarmStatus(CString strIndexError, CString strCountError, CString strEndPointError, CString strDbError)
{
	INT nCnt = 0;

	if (strIndexError != _T(""))
	{
		m_alarm.strError[nCnt] = m_alarm.strIndexError = strIndexError;
		nCnt++;
	}

	if (strCountError != _T(""))
	{
		m_alarm.strError[nCnt] = m_alarm.strCountError = strCountError;
		nCnt++;
	}

	if (strEndPointError != _T(""))
	{
		m_alarm.strError[nCnt] = m_alarm.strCountError = strEndPointError;
		nCnt++;
	}

	if (strDbError != _T(""))
	{
		m_alarm.strError[nCnt] = m_alarm.strDBError = strDbError;
		nCnt++;
	}

	m_alarm.nErrorCount = nCnt;
}


void CPageRestorationSelectedMethod::SetIndexError(CString strError)
{
	m_alarm.strError[m_alarm.nErrorCount++] = strError;
}


void CPageRestorationSelectedMethod::InitAlarmStruct()
{
	m_alarm.nErrorCount = 0;
	//memset(m_alarm.strError, 0, sizeof(CString) * 100);
	for (INT i = 0; i < 100; i++)
	{
		m_alarm.strError[i] = _T("");
	}
	m_alarm.strIndexError = _T("");
	m_alarm.strCountError = _T("");
	m_alarm.strDBError = _T("");
	m_alarm.strEndPointError = _T("");
}


void CPageRestorationSelectedMethod::InitControlTextSize()
{
	if (true)
	{
		LOGFONT lf = { 0, };
		afxGlobalData.fontRegular.GetLogFont(&lf);

		lf.lfHeight = -15;
		m_BigFont.CreateFontIndirect(&lf);

		GetDlgItem(IDC_BTN_RECENT)->SetFont(&m_BigFont);
		GetDlgItem(IDC_LIST_MAIN)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_BACK)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_NEXT)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}

	/*if (true)
	{
		m_BigFont.CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, TEXT_STYLE);

	GetDlgItem(IDC_BTN_RECENT)->SetFont(&m_BigFont);
	GetDlgItem(IDC_LIST_MAIN)->SetFont(&m_BigFont);
	GetDlgItem(IDC_BTN_BACK)->SetFont(&m_BigFont);
	GetDlgItem(IDC_BTN_NEXT)->SetFont(&m_BigFont);

	m_BigFont.Detach();
	}*/

}
