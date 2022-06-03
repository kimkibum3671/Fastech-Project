// CPageRestorationSelect.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageRestorationSelect.h"
#include "afxdialogex.h"

#include "CSheetRestoration.h"

#include "CDataBaseManager.h"

// CPageRestorationSelect dialog

IMPLEMENT_DYNAMIC(CPageRestorationSelect, CMFCPropertyPageEx)

CPageRestorationSelect::CPageRestorationSelect(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPageEx(IDD_RESTORATION_COMPARE_SELECTI)
{
	m_nNominalID = -1;
	m_nRestoredID = -1;
	m_nFirstSelectedID = -1;
}

CPageRestorationSelect::~CPageRestorationSelect()
{
}

void CPageRestorationSelect::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageRestorationSelect, CMFCPropertyPage)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_BACK, &CPageRestorationSelect::OnBnClickedBtnBack)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CPageRestorationSelect::OnBnClickedBtnNext)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MAIN, &CPageRestorationSelect::OnLvnItemchangedListMain)
	ON_BN_CLICKED(IDC_BTN_MAIN_NOMINAL, &CPageRestorationSelect::OnBnClickedBtnMainNominal)
	ON_BN_CLICKED(IDC_BTN_RECENT, &CPageRestorationSelect::OnBnClickedBtnRecent)
END_MESSAGE_MAP()

// CPageRestorationSelect message handlers

BOOL CPageRestorationSelect::OnInitDialog()
{
	CMFCPropertyPageEx::OnInitDialog();

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
				IDC_ST_FIRST_CAPTION,
				NULL,
			};

			for (INT nIndex = 0; nResID[nIndex]; nIndex++)
			{
				m_wndCaption[nIndex].SubclassDlgItem(nResID[nIndex], this);
				m_wndCaption[nIndex].SetBkColor(m_colorBkName);
				m_wndCaption[nIndex].SetTextColor(m_colorTxtName);
			}
		}

		if (1)
		{
			COLORREF m_colorBkName = RGB(0xf0, 0xf0, 0xf0);
			COLORREF m_colorTxtName = RGB(0x00, 0x00, 0x00);

			const INT nResID[] = {
				IDC_ST_NOMINAL,
				IDC_ST_LATEST,
				IDC_ST_FIRST,
				NULL,
			};

			for (INT nIndex = 0; nResID[nIndex]; nIndex++)
			{
				m_wndText[nIndex].SubclassDlgItem(nResID[nIndex], this);
				m_wndText[nIndex].SetBkColor(m_colorBkName);
				m_wndText[nIndex].SetTextColor(m_colorTxtName);
			}
		}
	}

	InitializeListCtrl();

	return TRUE;
}


BOOL CPageRestorationSelect::OnSetActive()
{
	CSheetRestoration* pParent = static_cast<CSheetRestoration*>(GetParent());
	m_nFirstSelectedID = pParent->GetProjectBackupID();

	SetDlgItemText( IDC_ST_NOMINAL, (-1 != m_nNominalID) ? pParent->GetNominalTimeStamp() : _T("-"));
	SetDlgItemText(IDC_ST_LATEST, ( -1 != m_nRestoredID) ? pParent->GetLatestRestoredTimeStamp() : _T("-"));
	SetDlgItemText(IDC_ST_FIRST, pParent->GetProjectTimeStamp());

	DisplaySelectedBackupInfo();

	return CMFCPropertyPageEx::OnSetActive();
}


void CPageRestorationSelect::InitializeListCtrl()
{
	m_wndList.SubclassDlgItem(IDC_LIST_MAIN, this);
	m_wndList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SINGLEROW | LVS_EX_DOUBLEBUFFER);
	SetWindowTheme(m_wndList.m_hWnd, _T("Explorer"), NULL);

	INT nIndex = 0;
	m_wndList.InsertColumn(nIndex++, _T("Time Stamp"), 0, 150);
	m_wndList.InsertColumn(nIndex++, _T("Comment"), 0, 100);

	m_wndList.DeleteAllItems();

	m_wndList.LoadHeaderItemInfo();

	this->LoadProjectList();
}

void CPageRestorationSelect::LoadProjectList()
{
	CSheetRestoration* pParent = static_cast<CSheetRestoration*>(GetParent());
	CDeviceMotionGate* pDevice = pParent->GetMotiongateCtrl();

	CString strSystemName(pDevice->GetSystemName());

	CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();	

	m_wndList.RemoveAll();

	if (pProjectDB->GetProjectBackupList(strSystemName) && pProjectDB->SeekToBegin())
	{
		INT nItem = 0;
		CString strTemp;
		while (CDataBaseRecordData* pData = pProjectDB->SeekToNext())
		{
			pData->GetTextData(_T("TimeStamp"), strTemp);
			const INT nIndex = m_wndList.InsertItem(nItem, strTemp);

			pData->GetTextData(_T("Backup_Name"), strTemp);
			m_wndList.SetItemText(nIndex, 1, strTemp);

			INT nBackupID = -1;
			pData->GetIntergerData(_T("Backup_ID"), nBackupID);
			m_wndList.SetItemData(nIndex, nBackupID);

			nItem++;
		}
	}

	m_nRestoredID = pParent->GetLatestRestoredID();
	GetDlgItem(IDC_BTN_RECENT)->EnableWindow(-1 != m_nRestoredID ? TRUE : FALSE);

	m_nNominalID = pParent->GetNominalBackupID();
	GetDlgItem(IDC_BTN_MAIN_NOMINAL)->EnableWindow(-1 != m_nNominalID ? TRUE : FALSE);
	
	if (-1 != m_nNominalID)
	{
		SetNominalData(m_nNominalID);
	}
}


void CPageRestorationSelect::OnDestroy()
{
	m_wndList.SaveHeaderItemInfo();

	CMFCPropertyPageEx::OnDestroy();
}


void CPageRestorationSelect::OnBnClickedBtnBack()
{
	ActivatePrevPage();
}


void CPageRestorationSelect::OnBnClickedBtnNext()
{
	ActivateNextPage();
}


void CPageRestorationSelect::OnLvnItemchangedListMain(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	*pResult = 0;

	DisplaySelectedBackupInfo();
}


void CPageRestorationSelect::DisplaySelectedBackupInfo()
{
	CSheetRestoration* pParent = static_cast<CSheetRestoration*>(GetParent());

	CString strCmpBackupID;

	POSITION pos = m_wndList.GetFirstSelectedItemPosition();
	INT nIndex = -1;

	if (pos)
	{
		nIndex = m_wndList.GetNextSelectedItem(pos);
		
		const INT nBackupID = m_wndList.GetItemData(nIndex);

		COLORREF m_colorBkOff = RGB(0xc8, 0xc8, 0xc8);
		COLORREF m_colorBkGreen = RGB(0x00, 0xc8, 0x00);
		COLORREF m_colorBkRed = RGB(0xc8, 0x00, 0x00);

		m_wndText[0].SetBkColor(m_nNominalID == nBackupID ? m_colorBkGreen : m_colorBkOff);
		m_wndText[1].SetBkColor(m_nRestoredID == nBackupID ? m_colorBkGreen : m_colorBkOff);
		m_wndText[2].SetBkColor(m_nFirstSelectedID == nBackupID ? m_colorBkRed : m_colorBkOff);

		pParent->SetCompareBackupID(nBackupID, m_wndList.GetItemText(nIndex, 0) );
	}
	else
	{
		pParent->SetCompareBackupID();
	}

	BOOL bEnable = FALSE;
	if (-1 != nIndex)
	{
		bEnable = (strCmpBackupID != pParent->GetProjectBackupID() &&
			m_nFirstSelectedID != nIndex);
	}

	GetDlgItem(IDC_BTN_NEXT)->EnableWindow(bEnable);
}

void CPageRestorationSelect::OnBnClickedBtnMainNominal()
{
	if (-1 == m_nNominalID)
	{
		return;
	}

	const INT nCount = m_wndList.GetItemCount();
	for (INT nIndex = 0; nIndex < nCount; nIndex++)
	{
		if (m_wndList.GetItemData(nIndex) == m_nNominalID )
		{
			m_wndList.SelectSingleItem(nIndex);
			m_wndList.SetFocus();
			break;
		}
	}
}


void CPageRestorationSelect::OnBnClickedBtnRecent()
{
	if (-1 ==m_nRestoredID)
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


void CPageRestorationSelect::SetNominalData(const INT nSelectIndex)
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