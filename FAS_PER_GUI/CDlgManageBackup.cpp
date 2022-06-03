// CDlgManageBackup.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgManageBackup.h"
#include "afxdialogex.h"

#include "CDeviceList.h"
#include "CDataBaseManager.h"
// CDlgManageBackup dialog

IMPLEMENT_DYNAMIC(CDlgManageBackup, CDialogEx)

CDlgManageBackup::CDlgManageBackup(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MANAGE_BACKUP, pParent)
{
	m_dwAddress = 0;
	m_nCurNominalID = 0;
	m_nNewNominalID = 0;
	m_nRestoredID = 0;
	m_bSystemControl = FALSE;
	m_strSystemName = _T("");
}

CDlgManageBackup::~CDlgManageBackup()
{
}

void CDlgManageBackup::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgManageBackup, CDialogEx)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_RECENT, &CDlgManageBackup::OnBnClickedBtnRecent)
	ON_BN_CLICKED(IDC_BTN_MAIN_NOMINAL, &CDlgManageBackup::OnBnClickedBtnMainNominal)
	ON_BN_CLICKED(IDOK, &CDlgManageBackup::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgManageBackup::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_CHANGE_NOMINAL, &CDlgManageBackup::OnBnClickedBtnChangeNominal)
	ON_BN_CLICKED(IDC_BTN_DELETE, &CDlgManageBackup::OnBnClickedBtnDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MAIN, &CDlgManageBackup::OnLvnItemchangedListMain)
END_MESSAGE_MAP()


// CDlgManageBackup message handlers


BOOL CDlgManageBackup::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_BACKUP_MANAGE));
	this->SetIcon(hIcon, FALSE);
	
	m_btnPrev.SubclassDlgItem(IDC_BTN_BACK, this);
	m_btnPrev.SetImage(IDB_BACK);

	m_btnNext.SubclassDlgItem(IDC_BTN_NEXT, this);
	m_btnNext.SetImage(IDB_NEXT);

	if (1)
	{
		if (1)
		{
			COLORREF m_colorBkName = RGB(0x0a, 0x0a, 0x50);
			COLORREF m_colorTxtName = RGB(0xc8, 0xc8, 0xc8);

			const INT nResID[] = {
				IDC_ST_NOMINAL_CAPTION,
				IDC_ST_LATEST_CAPTION,
				NULL,
			};

			for (INT nIndex = 0; nResID[nIndex]; nIndex++)
			{
				if (m_wndCaption[nIndex].SubclassDlgItem(nResID[nIndex], this))
				{
					m_wndCaption[nIndex].SetBkColor(m_colorBkName);
					m_wndCaption[nIndex].SetTextColor(m_colorTxtName);
				}
			}
		}

		if (1)
		{
			COLORREF m_colorBkName = RGB(0xf0, 0xf0, 0xf0);
			COLORREF m_colorTxtName = RGB(0x00, 0x00, 0x00);

			const INT nResID[] = {
				IDC_ST_NOMINAL,
				IDC_ST_LATEST,
				NULL,
			};

			for (INT nIndex = 0; nResID[nIndex]; nIndex++)
			{
				if (m_wndText[nIndex].SubclassDlgItem(nResID[nIndex], this))
				{
					m_wndText[nIndex].SetBkColor(m_colorBkName);
					m_wndText[nIndex].SetTextColor(m_colorTxtName);
				}
			}
		}
	}

	InitializeListCtrl();

	this->LoadProjectList();

	InitControlTextSize();

	return TRUE;
}

void CDlgManageBackup::SetAddress(const DWORD dwAddress)
{
	m_dwAddress = dwAddress;
}

const DWORD CDlgManageBackup::GetAddress()
{
	return m_dwAddress;
}

void CDlgManageBackup::InitializeListCtrl()
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

void CDlgManageBackup::LoadProjectList()
{
	CDeviceMotionGate* pDevice = NULL;
	if (FALSE == CDeviceList::GetInstance()->GetMotiongateByAddress(m_dwAddress, pDevice) && FALSE == IsSystemControl())
	{
		return;
	}
	
	m_lstItemID.RemoveAll();

	BOOL bCheck = IsSystemControl();
	CString strName = GetSystemControlDeviceName();
	CString strSystemName(IsSystemControl() ? GetSystemControlDeviceName() : pDevice->GetSystemName());

	CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();	

	m_nNewNominalID = m_nCurNominalID = pProjectDB->GetNominalBackupID(strSystemName);
	if (-1 == m_nNewNominalID)
	{
		m_wndText[0].SetBkColor(RGB(0xff, 0x00, 0x00));
	}
	else
	{
		GetDlgItem(IDC_BTN_MAIN_NOMINAL)->EnableWindow();
	}

	m_nRestoredID = pProjectDB->GetLatestRestoredID(strSystemName);

	if (-1 == m_nRestoredID)
	{
		m_wndText[1].SetBkColor(RGB(0xff, 0x00, 0x00));
	}
	else
	{
		GetDlgItem(IDC_BTN_RECENT)->EnableWindow();
	}

	/*
	 // 수정
	if (pProjectDB->GetProjectBackupList(strSystemName) && pProjectDB->SeekToBegin())
	{
		INT nIndex = 0;
		CString strTemp;
		
		while (CDataBaseRecordData* pData = pProjectDB->SeekToNext())
		{
			INT nBackupID = -1;
			BOOL bNominal = FALSE;
			
			pData->GetIntergerData(_T("Backup_ID"), nBackupID);
			m_wndList.SetItemData(nIndex, nBackupID); // 기존 db 순서대로 저장.
			m_lstItemID.AddTail(nBackupID);

			bNominal = (m_nNewNominalID == nBackupID) ? TRUE : FALSE;

			if (bNominal)
			{
				SetDlgItemText(IDC_ST_NOMINAL, strTemp);
				SetNominalData(0);

				pData->GetTextData(_T("TimeStamp"), strTemp);
				m_wndList.InsertItem(0, strTemp);

				pData->GetTextData(_T("Backup_Name"), strTemp);
				m_wndList.SetItemText(0, 1, strTemp);
			}
			nIndex++;
		} // Nominal 부분 추출 및 저장

		pProjectDB->SeekToBegin();

		nIndex = (-1 != m_nNewNominalID) ? 1 : 0;

		while (CDataBaseRecordData* pData = pProjectDB->SeekToNext())
		{
			INT nNominalID = -1;
			INT nBackupID = -1;
			BOOL bNominal = FALSE;

			pData->GetIntergerData(_T("Backup_ID"), nBackupID);
			
			bNominal = (m_nNewNominalID == nBackupID) ? TRUE : FALSE;

			if (!bNominal)
			{
				pData->GetTextData(_T("TimeStamp"), strTemp);
				m_wndList.InsertItem(nIndex, strTemp);

				pData->GetTextData(_T("Backup_Name"), strTemp);
				m_wndList.SetItemText(nIndex, 1, strTemp);

				nIndex++;
			}
		}
	}
	

	 //기존..
	if (pProjectDB->GetProjectBackupList(strSystemName) && pProjectDB->SeekToBegin())
	{
		INT nIndex = 0;
		CString strTemp;
		while (CDataBaseRecordData* pData = pProjectDB->SeekToNext())
		{
			pData->GetTextData(_T("TimeStamp"), strTemp);
			m_wndList.InsertItem(nIndex, strTemp);

			INT nBackupID = -1;
			pData->GetIntergerData(_T("Backup_ID"), nBackupID);
			m_wndList.SetItemData(nIndex, nBackupID);
			m_lstItemID.AddTail(nBackupID);

			if (m_nNewNominalID == nBackupID)
			{
				SetDlgItemText(IDC_ST_NOMINAL, strTemp);
				SetNominalData(nIndex);
			}

			if (m_nRestoredID == nBackupID)
			{
				SetDlgItemText(IDC_ST_LATEST, strTemp);
			}

			pData->GetTextData(_T("Backup_Name"), strTemp);
			m_wndList.SetItemText(nIndex, 1, strTemp);

			nIndex++;
		}
	}
	*/
	
	if (pProjectDB->GetProjectBackupList(strSystemName) && pProjectDB->SeekToBegin())
	{
		CString strTemp;
		while (CDataBaseRecordData* pData = pProjectDB->SeekToNext())
		{
			INT nBackupID = -1;
			pData->GetIntergerData(_T("Backup_ID"), nBackupID);

			pData->GetTextData(_T("TimeStamp"), strTemp);
			INT iItem = 0;
			if (m_nNewNominalID != nBackupID)
			{
				iItem = m_wndList.InsertItem(m_wndList.GetItemCount(), strTemp);
				m_lstItemID.AddTail(nBackupID);
			}
			else // if (m_nNewNominalID == nBackupID)
			{
				iItem = m_wndList.InsertItem(0, strTemp);
				m_lstItemID.AddHead(nBackupID);
			}

			m_wndList.SetItemData(iItem, nBackupID);

			if (m_nNewNominalID == nBackupID)
			{
				SetDlgItemText(IDC_ST_NOMINAL, strTemp);
				SetNominalData(iItem);
				m_wndList.SetItemText(iItem, 2, _T("Nominal Data"));
			}

			if (m_nRestoredID == nBackupID)
			{
				SetDlgItemText(IDC_ST_LATEST, strTemp);
				m_wndList.SetItemText(iItem, 2, _T("Recently Restored Data"));
			}

			pData->GetTextData(_T("Backup_Name"), strTemp);
			m_wndList.SetItemText(iItem, 1, strTemp);
		}
	}

	m_nItemCount = m_wndList.GetItemCount();
}


void CDlgManageBackup::OnDestroy()
{
	m_wndList.SaveHeaderItemInfo();

	CDialogEx::OnDestroy();
}


void CDlgManageBackup::OnBnClickedBtnRecent()
{
	if (-1 == m_nRestoredID)
	{
		return;
	}

	const INT nCount = m_wndList.GetItemCount();
	for (INT nIndex = 0; nIndex < nCount; nIndex++)
	{
		if (m_wndList.GetItemData(nIndex) == m_nRestoredID)
		{
			m_wndList.SelectSingleItem(nIndex);
			m_wndList.SetFocus();
			break;
		}
	}
}


void CDlgManageBackup::OnBnClickedBtnMainNominal()
{
	if (-1 == m_nNewNominalID)
	{
		return;
	}

	/*else
	{
		m_wndList.SelectSingleItem(0);
		m_wndList.SetFocus();
		
	}*/

	const INT nCount = m_wndList.GetItemCount();
	for (INT nIndex = 0; nIndex < nCount; nIndex++)
	{
		if (m_wndList.GetItemData(nIndex) == m_nNewNominalID)
		{
			m_wndList.SelectSingleItem(nIndex);
			m_wndList.SetFocus();
			break;
		}
	}
}


void CDlgManageBackup::OnBnClickedOk()
{
	if (m_nItemCount == m_wndList.GetItemCount() &&
		m_nCurNominalID == m_nNewNominalID)
	{
		// Edit된 것이 없다면 그냥 닫어라
		CDialogEx::OnCancel();
		return;
	}

	const BOOL bNominalWarning = (-1 != m_nCurNominalID && -1 == m_nNewNominalID);

	CString strContent("");
	CString strMainInstruction("Do you apply this result to system?");
	CString strTitle("Confirm to apply");

	if (bNominalWarning)
	{
		strMainInstruction += _T("\n\n!!! Nominal data will be deleted !!!");
	}

	CTaskDialog taskDialog(strContent, strMainInstruction, strTitle, TDCBF_CANCEL_BUTTON, TDF_ENABLE_HYPERLINKS | TDF_USE_COMMAND_LINKS | TDF_ALLOW_DIALOG_CANCELLATION);

	taskDialog.AddCommandControl(IDYES, _T("Yes, Apply this edited result."));
	taskDialog.AddCommandControl(IDNO, _T("No, Don't apply this."));

	taskDialog.SetDefaultCommandControl(IDYES);

	if (bNominalWarning)
	{
		taskDialog.SetMainIcon(TD_WARNING_ICON);
	}
	else
	{
		taskDialog.SetMainIcon(AfxGetApp()->LoadStandardIcon(IDI_QUESTION));
	}
	
	taskDialog.SetDialogWidth(300);

	switch (taskDialog.DoModal())
	{
	case IDYES:
		break;
	
	case IDNO:
		CDialogEx::OnCancel();

	default:
		return;
		break;
	}

	POSITION pos = m_lstItemID.GetHeadPosition();
	while (pos)
	{
		POSITION posCurrent = pos;

		LVFINDINFO info = { 0, };
		info.flags = LVFI_PARAM;
		info.lParam = m_lstItemID.GetNext(pos);

		const INT nIndex = m_wndList.FindItem(&info);
		if (-1 != nIndex)
		{
			m_lstItemID.RemoveAt(posCurrent);
		}
	}

	CDialogEx::OnOK();
}


void CDlgManageBackup::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


void CDlgManageBackup::OnBnClickedBtnChangeNominal()
{
	POSITION pos = m_wndList.GetFirstSelectedItemPosition();
	if(pos)
	{
		INT nPos = m_wndList.GetNextSelectedItem(pos);
		m_nNewNominalID = m_wndList.GetItemData(nPos);
		SetDlgItemText(IDC_ST_NOMINAL, m_wndList.GetItemText(nPos/*m_nNewNominalID*/, 0) );

		CWnd* pWnd = GetDlgItem(IDC_BTN_MAIN_NOMINAL);
		if (FALSE == pWnd->IsWindowEnabled())
		{
			pWnd->EnableWindow();
		}

		GetDlgItem(IDC_BTN_CHANGE_NOMINAL)->EnableWindow(FALSE);
		m_wndText[0].SetBkColor(RGB(0xf0, 0xf0, 0xf0));

		SetNominalData(nPos/*m_nNewNominalID*/);
		m_wndList.Invalidate(FALSE);
	}
}


void CDlgManageBackup::OnBnClickedBtnDelete()
{
	CList<DWORD, DWORD> lstDelete;
	POSITION pos = m_wndList.GetFirstSelectedItemPosition();
	while(pos)
	{
		const INT nIndex = m_wndList.GetItemData(m_wndList.GetNextSelectedItem(pos));
		lstDelete.AddTail(nIndex);
	}

	while( lstDelete.GetCount() )
	{
		LVFINDINFO info = { 0, };
		info.flags = LVFI_PARAM;
		info.lParam = lstDelete.RemoveHead();

		const INT nIndex = m_wndList.FindItem( &info );
		if (-1 != nIndex)
		{
			m_wndList.DeleteItem(nIndex);

			if (static_cast<INT>(info.lParam) == m_nNewNominalID)
			{
				m_nNewNominalID = -1;
				SetDlgItemText(IDC_ST_NOMINAL, _T("None") );
				
				m_wndText[0].SetBkColor( RGB( 0xff, 0x00, 0x00 ) );
				GetDlgItem(IDC_BTN_MAIN_NOMINAL)->EnableWindow(FALSE);

				SetNominalData(-1);
			}

			if (static_cast<INT>(info.lParam) == m_nRestoredID)
			{
				m_nRestoredID = -1;

				SetDlgItemText(IDC_ST_LATEST, _T("None"));

				m_wndText[1].SetBkColor(RGB(0xff, 0x00, 0x00));
				GetDlgItem(IDC_BTN_RECENT)->EnableWindow(FALSE);
			}
		}
	}
}

POSITION CDlgManageBackup::GetDeleteItemStartPosition()
{
	return m_lstItemID.GetHeadPosition();
}

const INT CDlgManageBackup::GetNextDeleteItem(POSITION& position)
{
	return m_lstItemID.GetNext(position);
}

const BOOL CDlgManageBackup::IsNominalSelectionChanged()
{ // 사용 X
	return m_nCurNominalID != m_nNewNominalID;
}

const INT CDlgManageBackup::GetSelectedNominalID()
{
	return m_nNewNominalID;
}

const BOOL	CDlgManageBackup::IsExistLastestBackupID()
{
	return -1 != m_nRestoredID;
}


void CDlgManageBackup::OnLvnItemchangedListMain(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	*pResult = 0;

	BOOL bSetNominal = FALSE;
	BOOL bDelete = FALSE;

	switch (m_wndList.GetSelectedCount())
	{
	case 0:
	{
		//	Do nothing
	}	break;

	case 1:
	{
		POSITION pos = m_wndList.GetFirstSelectedItemPosition();
		if (m_nNewNominalID != static_cast<INT>(m_wndList.GetItemData(m_wndList.GetNextSelectedItem(pos))))
		{
			bSetNominal = TRUE;
		}
	}
	default:
	{
		bDelete = TRUE;
	}	break;
	}

	if (1)
	{
		CWnd* pWnd = GetDlgItem(IDC_BTN_CHANGE_NOMINAL);
		if (pWnd->IsWindowEnabled() != bSetNominal )
		{	
			pWnd->EnableWindow(bSetNominal);
		}
	}

	if (1)
	{
		CWnd* pWnd = GetDlgItem(IDC_BTN_DELETE);
		if (pWnd->IsWindowEnabled() != bDelete)
		{
			pWnd->EnableWindow(bDelete);
		}
	}
}

void CDlgManageBackup::SetNominalData(const INT nSelectIndex)
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


void CDlgManageBackup::SetSystemControl(BOOL bCtrl)
{
	m_bSystemControl = bCtrl;
}


BOOL CDlgManageBackup::IsSystemControl()
{
	return m_bSystemControl;
}


void CDlgManageBackup::SetSystemControlDeviceName(CString str)
{
	m_strSystemName = str;
}


CString CDlgManageBackup::GetSystemControlDeviceName()
{
	return m_strSystemName;
}


void CDlgManageBackup::InitControlTextSize()
{
	if (true)
	{
		LOGFONT lf = { 0, };
		afxGlobalData.fontRegular.GetLogFont(&lf);

		lf.lfHeight = -15;
		m_BigFont.CreateFontIndirect(&lf);

		GetDlgItem(IDC_ST_NOMINAL_CAPTION)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_NOMINAL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_LATEST_CAPTION)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_LATEST)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_DELETE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_LIST_MAIN)->SetFont(&m_BigFont);
		GetDlgItem(IDOK)->SetFont(&m_BigFont);
		GetDlgItem(IDCANCEL)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}

	/*if (true)
	{
		m_BigFont.CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, TEXT_STYLE);

		GetDlgItem(IDC_ST_NOMINAL_CAPTION)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_NOMINAL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_LATEST_CAPTION)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_LATEST)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_DELETE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_LIST_MAIN)->SetFont(&m_BigFont);
		GetDlgItem(IDOK)->SetFont(&m_BigFont);
		GetDlgItem(IDCANCEL)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}*/
	
}
