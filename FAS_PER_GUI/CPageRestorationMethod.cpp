// CPageRestorationMethod.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageRestorationMethod.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "CSheetRestoration.h"

#include "CDataBaseManager.h"
#include "CDeviceList.h"

// CPageRestorationMethod dialog

IMPLEMENT_DYNAMIC(CPageRestorationMethod, CMFCPropertyPageEx)

CPageRestorationMethod::CPageRestorationMethod(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPageEx(IDP_RESTORATION_METHOD)
{
	m_NominalData = m_SelectedData = { 0, };
	m_bRequestFromRibbon = FALSE;
}

CPageRestorationMethod::~CPageRestorationMethod()
{
	//KillTimer(TIMER_NEXT_BUTTON);
}

void CPageRestorationMethod::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageRestorationMethod, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_BTN_BACK, &CPageRestorationMethod::OnBnClickedBtnBack)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CPageRestorationMethod::OnBnClickedBtnNext)
	ON_BN_CLICKED(IDC_SEL_METHOD1, &CPageRestorationMethod::OnBnClickedSelMethod1)
	ON_BN_CLICKED(IDC_SEL_METHOD2, &CPageRestorationMethod::OnBnClickedSelMethod2)
	ON_BN_CLICKED(IDC_SEL_METHOD3, &CPageRestorationMethod::OnBnClickedSelMethod3)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MAIN, &CPageRestorationMethod::OnLvnItemchangedListMain)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_MAIN_NOMINAL, &CPageRestorationMethod::OnBnClickedBtnMainNominal)
	ON_BN_CLICKED(IDC_BTN_RECENT, &CPageRestorationMethod::OnBnClickedBtnRecent)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPageRestorationMethod message handlers


BOOL CPageRestorationMethod::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	m_btnPrev.SubclassDlgItem(IDC_BTN_BACK, this);
	m_btnPrev.SetImage(IDB_BACK);

	m_btnNext.SubclassDlgItem(IDC_BTN_NEXT, this);
	m_btnNext.SetImage(IDB_NEXT);

	m_btnMethod1.SubclassDlgItem(IDC_SEL_METHOD1, this);
	m_btnMethod1.SetImage(IDB_MAP0_ON);
	m_btnMethod1.SetCheck(BST_CHECKED);
	m_btnMethod1.ShowWindow(SW_HIDE);

	m_btnMethod2.SubclassDlgItem(IDC_SEL_METHOD2, this);
	m_btnMethod2.SetImage(IDB_MAP1_OFF);

	m_btnMethod3.SubclassDlgItem(IDC_SEL_METHOD3, this);
	m_btnMethod3.SetImage(IDB_MAP2_OFF);

	m_bEnableCompare = FALSE;

	InitializeStaticControl();

	InitializeListCtrl();

	LoadProjectList();

	InitComapreButton();

	OnBnClickedSelMethod2();

	InitControlTextSize();

	//SetTimer(TIMER_NEXT_BUTTON, 100, NULL);

	return TRUE;
}


void CPageRestorationMethod::OnBnClickedBtnBack()
{
	ActivatePrevPage();
}


void CPageRestorationMethod::OnBnClickedBtnNext()
{
	if (TRUE == m_bEnableCompare)
	{
		ActivateNextPage();
	}
	else
	{
		ShowDisableAlarm();
	}
	
}


void CPageRestorationMethod::OnBnClickedSelMethod1()
{
	if (0 != m_nSelMethod)
	{
		m_btnMethod1.SetImage(IDB_MAP0_ON);
		m_btnMethod1.Invalidate(FALSE);

		m_btnMethod2.SetImage(IDB_MAP1_OFF);
		m_btnMethod2.Invalidate(FALSE);

		m_btnMethod3.SetImage(IDB_MAP2_OFF);
		m_btnMethod3.Invalidate(FALSE);

		//m_nSelMethod = 0;
		m_nSelMethod = sts_selected_files;

		ChangeProjectInfo(m_nSelMethod);
	}
}

void CPageRestorationMethod::OnBnClickedSelMethod2()
{
	if (1 != m_nSelMethod)
	{
		m_btnMethod1.SetImage(IDB_MAP0_OFF);
		m_btnMethod1.Invalidate(FALSE);

		m_btnMethod2.SetImage(IDB_MAP1_ON);
		m_btnMethod2.Invalidate(FALSE);

		m_btnMethod3.SetImage(IDB_MAP2_OFF);
		m_btnMethod3.Invalidate(FALSE);

		m_nSelMethod = sts_selected_files;

		ChangeProjectInfo(m_nSelMethod);
	}
}

void CPageRestorationMethod::OnBnClickedSelMethod3()
{
	if (2 != m_nSelMethod)
	{
		m_btnMethod1.SetImage(IDB_MAP0_OFF);
		m_btnMethod1.Invalidate(FALSE);

		m_btnMethod2.SetImage(IDB_MAP1_OFF);
		m_btnMethod2.Invalidate(FALSE);

		m_btnMethod3.SetImage(IDB_MAP2_ON);
		m_btnMethod3.Invalidate(FALSE);

		m_nSelMethod = sts_selected_device;

		ChangeProjectInfo(m_nSelMethod);
	}
}


void CPageRestorationMethod::OnLvnItemchangedListMain(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	*pResult = 0;

	if (pNMLV->uNewState & LVIS_SELECTED || pNMLV->uOldState & LVIS_SELECTED)
	{
		this->ChangeProjectInfo(m_nSelMethod);
	}
}

void CPageRestorationMethod::ChangeProjectInfo(const INT nMethod)
{
	CSheetRestoration* pParent = static_cast<CSheetRestoration*>(GetParent());
	CDeviceMotionGate* pDevice = NULL;
	CWnd* pWnd = GetDlgItem(IDC_BTN_NEXT);

	CString strName;
	m_bEnableCompare = FALSE;


	POSITION pos = m_wndList.GetFirstSelectedItemPosition();
	if (pos)
	{
		const INT nIndex = m_wndList.GetNextSelectedItem(pos);
		
		m_SelectedData.nBackupID = m_wndList.GetItemData(nIndex);
		m_SelectedData.strTimeStamp = m_wndList.GetItemText(nIndex, 0);
		m_SelectedData.strComment = m_wndList.GetItemText(nIndex, 1);
		
		if (sts_selected_device == m_nSelMethod)
		{
			if (CDeviceList::GetInstance()->GetMotiongateByAddress(pParent->GetAddress(), pDevice))
			{
				if (IsEnableCompareCurrentSystem(pDevice, m_SelectedData.nBackupID))
				{
					pParent->SetProjectInfo(
						m_wndList.GetItemData(nIndex),
						m_wndList.GetItemText(nIndex, 0),
						m_wndList.GetItemText(nIndex, 1));
					
					m_bEnableCompare = TRUE;
				}

				else
				{
					pParent->SetProjectInfo();
				}
			}
			else
			{
				ASSERT(0);
			}
		
		}
		else if (sts_selected_files == m_nSelMethod)
		{
			if (TRUE == IsEnableCompareFiles(pParent->GetProjectName(), m_NominalData.nBackupID, m_SelectedData.nBackupID))
			{
				pParent->SetProjectInfo(
					m_wndList.GetItemData(nIndex),
					m_wndList.GetItemText(nIndex, 0),
					m_wndList.GetItemText(nIndex, 1));

				m_bEnableCompare = TRUE;
			}

			else
			{
				pParent->SetProjectInfo();
			}
		}

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
	
	ChangeTextInfo(nMethod);
	pParent->SetMethod(nMethod);
}

void CPageRestorationMethod::InitializeListCtrl()
{
	m_wndList.SubclassDlgItem(IDC_LIST_MAIN, this);
	m_wndList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SINGLEROW | LVS_EX_DOUBLEBUFFER);
	SetWindowTheme(m_wndList.m_hWnd, _T("Explorer"), NULL);

	INT nIndex = 0;
	m_wndList.InsertColumn(nIndex++, _T("Time Stamp"), 0, 150);
	m_wndList.InsertColumn(nIndex++, _T("Comment"), 0, 100);
	m_wndList.InsertColumn(nIndex++, _T("State"), 0, 500);

	m_wndList.DeleteAllItems();

	m_wndList.LoadHeaderItemInfo();

	
}

void CPageRestorationMethod::LoadProjectList()
{
	CString strSystemName;
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	
	if (pMainWnd->IsSystemCompare())
	{ // Ribbon Bar에서 실행할 경우
		strSystemName = pMainWnd->GetSystemCompareProjectName();
	}
	else
	{
		CSheetRestoration* pParent = static_cast<CSheetRestoration*>(GetParent());
		CDeviceMotionGate* pDevice = pParent->GetMotiongateCtrl();
		strSystemName = pDevice->GetSystemName();
	}
	
	CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();
	
	/*
	if (1)
	{ // 기존
		if (pProjectDB->GetProjectBackupList(strSystemName) && pProjectDB->SeekToBegin())
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
		}

		m_nRestoredID = pProjectDB->GetLatestRestoredID(strSystemName);
		GetDlgItem(IDC_BTN_RECENT)->EnableWindow(-1 != m_nRestoredID ? TRUE : FALSE);

		m_nNominalID = pProjectDB->GetNominalBackupID(strSystemName);
		GetDlgItem(IDC_BTN_MAIN_NOMINAL)->EnableWindow(-1 != m_nNominalID ? TRUE : FALSE);

		if (-1 != m_nNominalID)
		{
			SetNominalData(m_nNominalID);
		}
	}
	*/
	
	if (1)
	{ // 수정
		m_nRestoredID = pProjectDB->GetLatestRestoredID(strSystemName);
		GetDlgItem(IDC_BTN_RECENT)->EnableWindow(-1 != m_nRestoredID ? TRUE : FALSE);

		m_nNominalID = pProjectDB->GetNominalBackupID(strSystemName);
		GetDlgItem(IDC_BTN_MAIN_NOMINAL)->EnableWindow(-1 != m_nNominalID ? TRUE : FALSE);

		if (pProjectDB->GetProjectBackupList(strSystemName) && pProjectDB->SeekToBegin())
		{
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
				{ // Nominal인경우
					pData->GetTextData(_T("TimeStamp"), strTemp);
					iItem = m_wndList.InsertItem(0, strTemp);
					m_NominalData.strTimeStamp = strTemp;

					pData->GetTextData(_T("Backup_Name"), strTemp);
					m_wndList.SetItemText(iItem, 1, strTemp);
					m_NominalData.strComment = strTemp;
				}
												
				m_wndList.SetItemData(iItem, nBackupID);

				if (nBackupID == m_nNominalID)
				{
					//SetNominalData(m_nNominalID);
					SetNominalData(iItem);
					m_wndList.SetItemText(iItem, 2, _T("Nominal Data"));
				}

				if (m_nRestoredID == nBackupID)
				{
					m_wndList.SetItemText(iItem, 2, _T("Recently Restored Data"));
				}
			}
		}

		
	}
	
}

void CPageRestorationMethod::OnDestroy()
{
	m_wndList.SaveHeaderItemInfo();

	CMFCPropertyPage::OnDestroy();
}


void CPageRestorationMethod::OnBnClickedBtnMainNominal()
{
	if( -1 == m_nNominalID )
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


void CPageRestorationMethod::OnBnClickedBtnRecent()
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



void CPageRestorationMethod::SetNominalData(const INT nSelectIndex)
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

const BOOL CPageRestorationMethod::IsEnableCompareFiles(CString strName, INT nNominalID, INT nSelectedID)
{
	CDataBaseProject* pProject = CDataBaseProject::GetInstance();

	CString str;
	BOOL bResult = TRUE;
	INT nIndex = 0;

	InitAlarmStruct();

	if (pProject->OpenDataBase())
	{ // Nominal Backup Data Motor Index 가져오는 부분.
		if (pProject->GetBackupFileMotorIndex(strName, nNominalID))
		{
			while (CDataBaseRecordData* pRecord = pProject->SeekToNext())
			{
				if (LoadBackupFileMotorIndex(pRecord, nIndex))
				{
					m_alarm.dwNominal += (0x01 << nIndex);
				}
			}
		}

		pProject->CloseDataBase();
	}
	else
	{
		ASSERT(0);
	}

	if (pProject->OpenDataBase())
	{ // Selected Backup Data Motor Index 가져오는 부분.
		if (pProject->GetBackupFileMotorIndex(strName, nSelectedID))
		{
			while (CDataBaseRecordData* pRecord = pProject->SeekToNext())
			{
				if (LoadBackupFileMotorIndex(pRecord, nIndex))
				{
					m_alarm.dwSelect += (0x01 << nIndex);
				}
			}
		}

		pProject->CloseDataBase();
	}
	else
	{
		ASSERT(0);
	}

	for (INT nMotor = 1; nMotor < 9; nMotor++)
	{
		BOOL bNominalExist = (m_alarm.dwNominal >> nMotor) & 0x01;
		BOOL bSelectExist = (m_alarm.dwSelect >> nMotor) & 0x01;
		
		if (TRUE == bSelectExist)
		{
			if (FALSE == bNominalExist)
			{
				str.Format(_T("there is no nominal data for 'Motor#%d'"), nMotor);
				SetIndexError(str);
				bResult &= FALSE;
			}
		}

		if (TRUE == bNominalExist)
		{
			if (FALSE == bSelectExist)
			{
				str.Format(_T("there is no selected data for 'Motor#%d'"), nMotor);
				SetIndexError(str);

				bResult &= FALSE;
			}
		}
	}

	return bResult;
}

const BOOL CPageRestorationMethod::IsEnableCompareCurrentSystem(CDeviceMotionGate* pDevice, INT nBackupID)
{
	CDataBaseProject* pProject = CDataBaseProject::GetInstance();

	CString strError, strIndex, strCount, strEndPoint;
	BOOL bEnable = TRUE;

	InitAlarmStruct();

	if (pProject->OpenDataBase())
	{
		CString strName = pDevice->GetSystemName();
		INT nIndex = -1;
		INT nMotorCount = -1;
		INT nMotorEndPoint = -1;

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
					
					dwFileMotor += (0x01 << nIndex);

					if (!pDevice->IsMotorConnected(nIndex))
					{ // 실제 모터에는 연결되어있지 않음
						//nFileIndex[nFileCount] = nIndex;
						str.Format(_T("'Motor#%d' to compare could not be found."), nIndex);
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

		
	}

	pProject->CloseDataBase();

	//SetRestorationEnableStatus(strIndex, strCount, strEndPoint, strError);
	return bEnable;
}

void CPageRestorationMethod::InitAlarmStruct()
{
	m_alarm.nErrorCount = 0;
	//memset(m_alarm.strError, 0, sizeof(CString) * 100);
	for (INT i = 0; i < 8; i++)
	{
		m_alarm.strError[i] = _T("");
	}

	m_alarm.dwNominal = 0x00;
	m_alarm.dwSelect = 0x00;
}

const BOOL CPageRestorationMethod::LoadBackupFileMotorIndex(LPVOID pDatabaseRecordData, INT& nIndex)
{
	CDataBaseRecordData* pRecord = static_cast<CDataBaseRecordData*>(pDatabaseRecordData);

	if (FALSE == pRecord->GetIntergerData(_T("Motor_ID"), nIndex))
	{
		return FALSE;
	}

	return TRUE;
}

void CPageRestorationMethod::SetIndexError(CString strError)
{
	m_alarm.strError[m_alarm.nErrorCount++] = strError;
}

void CPageRestorationMethod::InitComapreButton()
{

	CSheetRestoration* pParent = static_cast<CSheetRestoration*>(GetParent());
	CDeviceMotionGate* pDevice = pParent->GetMotiongateCtrl();
	CButton* pButton;
	BOOL bEnable = FALSE;
	
	if (1) // Compare
	{
		if (NULL != pDevice)
		{ // 연결된상태이며 Master 모드가 아닐 경우만 장치 비교 동작을 진행 할 수 있다.
			bEnable = (pDevice->IsRun() && pDevice->IsMotiongateControllable());
			m_btnMethod3.EnableWindow(bEnable);
		}
		else
		{ // 전체 프로젝트 파일 비교 모드는 장치 비교 동작을 진행할 수 없다.
			m_btnMethod3.ShowWindow(bEnable);
		}
	}
}


void CPageRestorationMethod::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_NEXT_BUTTON:
	{
		BOOL bComparing = CheckingComparing();

		//m_btnNext.EnableWindow(!bComparing);
		m_wndList.EnableWindow(!bComparing);
		m_btnMethod2.EnableWindow(!bComparing);
		m_btnMethod3.EnableWindow(!bComparing);
		
		if (bComparing)
		{
			GetDlgItem(IDC_BTN_RECENT)->EnableWindow(!bComparing);
			GetDlgItem(IDC_BTN_MAIN_NOMINAL)->EnableWindow(!bComparing);
			SetDlgItemText(IDC_STATIC_STATUS, _T("Comparing..."));
		}
		
		else
		{
			GetDlgItem(IDC_BTN_RECENT)->EnableWindow(-1 != m_nRestoredID ? TRUE : FALSE);
			GetDlgItem(IDC_BTN_MAIN_NOMINAL)->EnableWindow(-1 != m_nNominalID ? TRUE : FALSE);
			SetDlgItemText(IDC_STATIC_STATUS, _T("Ready."));
		}

	} break;
	default:
		break;
	}
	CMFCPropertyPageEx::OnTimer(nIDEvent);
}


const BOOL CPageRestorationMethod::CheckingComparing()
{
	CSheetRestoration* pParent = static_cast<CSheetRestoration*>(GetParent());
	CDeviceMotionGate* pDevice = pParent->GetMotiongateCtrl();

	return pDevice->IsBackupState();
}


void CPageRestorationMethod::InitializeStaticControl()
{
	COLORREF colorBkName = RGB(0x0a, 0x0a, 0x50);
	COLORREF colorTxtName = RGB(0xc8, 0xc8, 0xc8);

	const INT nResID[] = {
		IDC_ST_STANDARD,
		IDC_STATIC_STANDARD,
		IDC_ST_TARGET,
		IDC_STATIC_TARGET,
		IDC_ST_STATUS,
		IDC_STATIC_STATUS
	};

	for (INT nIndex = 0; nIndex < 6; nIndex++)
	{
		m_wndColor[nIndex].SubclassDlgItem(nResID[nIndex], this);
		m_wndColor[nIndex].SetBkColor(nIndex % 2 == 0 ? RGB(0x0a, 0x0a, 0x50) : RGB(0xff, 0xd4, 0x00));
		m_wndColor[nIndex].SetTextColor(nIndex % 2 == 0 ? RGB(0xc8, 0xc8, 0xc8) : RGB(0, 0, 0));
	}
}


void CPageRestorationMethod::ChangeTextInfo(INT nMethod)
{
	CString str;

	str.Format(_T("[ %s ]"), m_NominalData.strTimeStamp);
	SetDlgItemText(IDC_STATIC_NOMINAL, str);

	if (1 == nMethod)
	{

		m_SelectedData.strTimeStamp == _T("") ? str.Format(_T("None.")) : str.Format(_T("[ %s ]"), m_SelectedData.strTimeStamp);
		
		SetDlgItemText(IDC_STATIC_SELECTED, str);
	}

	else if (2 == nMethod)
	{
		/*str.Format(_T("Current System"));
		SetDlgItemText(IDC_STATIC_STANDARD, str);*/

		m_SelectedData.strTimeStamp == _T("") ? str.Format(_T("None.")) : str.Format(_T("[ %s ]"), m_SelectedData.strTimeStamp);
		//str.Format(_T("Selected Data / Time : %s"), m_SelectedData.strComment, m_SelectedData.strTimeStamp);
		SetDlgItemText(IDC_STATIC_SELECTED, str);
	}
}

void CPageRestorationMethod::ShowDisableAlarm()
{
	CString strContent("");
	CString strMainInstruction("Can not execute comparing!");
	CString strTitle("Comparing Alarm");
	CString strMessage;
	INT nWidth = 0;

	CTaskDialog taskDlg(strContent, strMainInstruction, strTitle, TDCBF_OK_BUTTON);

	for (INT i = 0; i < m_alarm.nErrorCount; i++)
	{
		taskDlg.AddCommandControl(i + 1, m_alarm.strError[i], FALSE);

		if (nWidth < m_alarm.strError[i].GetLength()) nWidth = m_alarm.strError[i].GetLength();
	}

	taskDlg.SetMainIcon(TD_ERROR_ICON);

	taskDlg.SetDialogWidth(nWidth * 6);
	taskDlg.DoModal();
}


void CPageRestorationMethod::SetNominalID(INT nBackupID)
{
	m_NominalData.nBackupID = nBackupID;
}


void CPageRestorationMethod::SetDevice(CDeviceMotionGate* pDevice)
{
	m_pDevice = pDevice;
}


const BOOL CPageRestorationMethod::IsRequestFromRibbon()
{
	return m_bRequestFromRibbon;
}


void CPageRestorationMethod::SetRequestRibbon(BOOL bRequest)
{
	m_bRequestFromRibbon = bRequest;
}


void CPageRestorationMethod::InitControlTextSize()
{
	if (true)
	{
		LOGFONT lf = { 0, };
		afxGlobalData.fontRegular.GetLogFont(&lf);

		lf.lfHeight = -15;
		m_BigFont.CreateFontIndirect(&lf);

		GetDlgItem(IDC_SEL_METHOD2)->SetFont(&m_BigFont);
		GetDlgItem(IDC_SEL_METHOD3)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_INFO)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_STANDARD)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_TARGET)->SetFont(&m_BigFont);
		GetDlgItem(IDC_LIST_MAIN)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_BACK)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_NEXT)->SetFont(&m_BigFont);

		lf.lfHeight = -13;
		GetDlgItem(IDC_STATIC_NOMINAL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_SELECTED)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}

	/*if (true)
	{
	m_BigFont.CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, TEXT_STYLE);

	GetDlgItem(IDC_SEL_METHOD2)->SetFont(&m_BigFont);
	GetDlgItem(IDC_SEL_METHOD3)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_INFO)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_STANDARD)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_NOMINAL)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_TARGET)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_SELECTED)->SetFont(&m_BigFont);
	GetDlgItem(IDC_LIST_MAIN)->SetFont(&m_BigFont);
	GetDlgItem(IDC_BTN_BACK)->SetFont(&m_BigFont);
	GetDlgItem(IDC_BTN_NEXT)->SetFont(&m_BigFont);

	m_BigFont.Detach();
	}*/
	
}
