// CDlgSchedule.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgSchedule.h"

#include "CDataBaseManager.h"

// CDlgSchedule

IMPLEMENT_DYNCREATE(CDlgSchedule, CDlgDevice)

CDlgSchedule::CDlgSchedule()
	: CDlgDevice(IDD_DEVICE_SCHEDULE)
{

}

CDlgSchedule::~CDlgSchedule()
{

}

void CDlgSchedule::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgSchedule, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHK_SUN, &CDlgSchedule::OnBnClickedChkSun)
	ON_BN_CLICKED(IDC_CHK_MON, &CDlgSchedule::OnBnClickedChkMon)
	ON_BN_CLICKED(IDC_CHK_TUE, &CDlgSchedule::OnBnClickedChkTue)
	ON_BN_CLICKED(IDC_CHK_WED, &CDlgSchedule::OnBnClickedChkWed)
	ON_BN_CLICKED(IDC_CHK_THU, &CDlgSchedule::OnBnClickedChkThu)
	ON_BN_CLICKED(IDC_CHK_FRI, &CDlgSchedule::OnBnClickedChkFri)
	ON_BN_CLICKED(IDC_CHK_SAT, &CDlgSchedule::OnBnClickedChkSat)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_OK, &CDlgSchedule::OnBnClickedBtnOk)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, &CDlgSchedule::OnLvnItemchangedList)
	ON_BN_CLICKED(IDOK, &CDlgSchedule::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSchedule diagnostics

#ifdef _DEBUG
void CDlgSchedule::AssertValid() const
{
	CDialog::AssertValid();
}

#ifndef _WIN32_WCE
void CDlgSchedule::Dump(CDumpContext& dc) const
{
	CDialog::Dump(dc);
}
#endif
#endif //_DEBUG


// CDlgSchedule message handlers

//void CDlgSchedule::OnInitialUpdate()
//{
//	CDlgDevice::OnInitialUpdate();
//
//	/*if (1)
//	{
//		static_cast<CButton*>(GetDlgItem(IDC_SEL_NEW))->SetCheck(BST_CHECKED);
//	}
//
//	static_cast<CDateTimeCtrl*>(GetDlgItem(IDC_TIME_PICKER))->SetFormat(_T("tt, hh:mm:00"));
//
//	SetTimer(TIMER_INITIALIZE, ELAPSE_TIMER_INITIALIZE, NULL);*/
//}

void CDlgSchedule::IntializeListCtrl()
{
	m_wndList.SubclassDlgItem(IDC_LIST, this);

	m_wndList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_DOUBLEBUFFER);

	INT nIndex = 0;
	m_wndList.InsertColumn(nIndex++, _T("Time"), 0, 100);
	m_wndList.InsertColumn(nIndex++, _T("Date"), 0, 100);
	m_wndList.InsertColumn(nIndex++, _T("Repeat"), 0, 100);

	m_wndList.LoadHeaderItemInfo();

	SetWindowTheme(m_wndList.m_hWnd, _T("Explorer"), NULL);

	m_wndList.SetColumnWidth(0, 100);
	m_wndList.SetColumnWidth(1, 100);
	m_wndList.SetColumnWidth(2, 130);

	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	if (pDevice)
	{
		CString strSystemName(pDevice->GetSystemName());
		CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();
		if (pProjectDB->LoadBackupReservation(strSystemName) &&
			pProjectDB->SeekToBegin())
		{
			CDataBaseRecordData* pData = NULL;
			while (pData = pProjectDB->SeekToNext())
			{
				CString strDateTime, strRepeat;
				DWORD dwRepeat = 0;

				if (pData->GetTextData(_T("Schedule_DateTime"), strDateTime) &&
					pData->GetTextData(_T("Week_Code"), strRepeat))
				{
					COleDateTime oleDateTime;
					oleDateTime.ParseDateTime(strDateTime);

					//	Insert Time
					const INT nItemIndex = m_wndList.InsertItem(m_wndList.GetItemCount(), oleDateTime.Format(_T("%X")));

					//	Convert Repeat
					if (-1 != strRepeat.Find(_T("A"))) dwRepeat |= 0x01;
					if (-1 != strRepeat.Find(_T("B"))) dwRepeat |= 0x02;
					if (-1 != strRepeat.Find(_T("C"))) dwRepeat |= 0x04;
					if (-1 != strRepeat.Find(_T("D"))) dwRepeat |= 0x08;
					if (-1 != strRepeat.Find(_T("E"))) dwRepeat |= 0x10;
					if (-1 != strRepeat.Find(_T("F"))) dwRepeat |= 0x20;
					if (-1 != strRepeat.Find(_T("G"))) dwRepeat |= 0x40;

					ConvertRepeatCodeToString(dwRepeat, strRepeat);

					//	Insert Date
					m_wndList.SetItemText(nItemIndex, 1, dwRepeat ? _T("-") : oleDateTime.Format(_T("%Y-%m-%d")));

					////	Insert Repeat
					m_wndList.SetItemText(nItemIndex, 2, strRepeat);
					m_wndList.SetItemData(nItemIndex, dwRepeat);
				}
			}
		}
	}

	InitControlTextSize();
}

void CDlgSchedule::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_INITIALIZE:
	{
		KillTimer(nIDEvent);

		CDeviceMotionGate* pDevice = GetMotiongateCtrl();
		if (NULL == pDevice)
		{
			break;
		}

		CString strCaption;
		strCaption.Format(_T("%s - Auto Backup Schdule"), pDevice->GetSystemName());
		this->SetWindowTextW(strCaption);

		IntializeListCtrl();
	}	break;
	}
	CDlgDevice::OnTimer(nIDEvent);
}

void CDlgSchedule::DisplayRepeatCondition()
{
	BOOL bEnable = TRUE;

	const INT nResID[] = {
		IDC_CHK_SUN,
		IDC_CHK_MON,
		IDC_CHK_TUE,
		IDC_CHK_WED,
		IDC_CHK_THU,
		IDC_CHK_FRI,
		IDC_CHK_SAT,
		NULL,
	};

	for (INT nIndex = 0; nResID[nIndex]; nIndex++)
	{
		if (BST_CHECKED == static_cast<CButton*>(GetDlgItem(nResID[nIndex]))->GetCheck())
		{
			bEnable = FALSE;
			break;
		}
	}

	static_cast<CButton*>(GetDlgItem(IDC_DATE_PICKER))->EnableWindow(bEnable);
}


void CDlgSchedule::OnDestroy()
{
	CDlgDevice::OnDestroy();
}

void CDlgSchedule::OnBnClickedChkSun()
{
	DisplayRepeatCondition();
}


void CDlgSchedule::OnBnClickedChkMon()
{
	DisplayRepeatCondition();
}


void CDlgSchedule::OnBnClickedChkTue()
{
	DisplayRepeatCondition();
}


void CDlgSchedule::OnBnClickedChkWed()
{
	DisplayRepeatCondition();
}


void CDlgSchedule::OnBnClickedChkThu()
{
	DisplayRepeatCondition();
}


void CDlgSchedule::OnBnClickedChkFri()
{
	DisplayRepeatCondition();
}


void CDlgSchedule::OnBnClickedChkSat()
{
	DisplayRepeatCondition();
}



void CDlgSchedule::OnBnClickedBtnOk()
{
	const INT nResID[] = {
		IDC_SEL_NEW,
		IDC_SEL_MOD,
		IDC_SEL_DEL,
		NULL
	};


	CTime timeDate, timeClock;
	CDateTimeCtrl* pDatePicker = static_cast<CDateTimeCtrl*>(GetDlgItem(IDC_DATE_PICKER));
	pDatePicker->GetTime(timeDate);

	CDateTimeCtrl* pTimePicker = static_cast<CDateTimeCtrl*>(GetDlgItem(IDC_TIME_PICKER));
	pTimePicker->GetTime(timeClock);

	DWORD dwRepeatCode = this->GetRepeatCode();

	const BOOL bDataTime = pDatePicker->IsWindowEnabled();
	INT nIndex = bDataTime ? this->FindBackupReservation(timeDate.GetTime(), timeClock.GetTime()) : this->FindBackupReservation(timeClock.GetTime());

	const INT nResult = GetRadioIndex(nResID);
	switch (nResult)
	{
	case IDC_SEL_NEW:
	{
		if (-1 == nIndex)
		{
			nIndex = m_wndList.InsertItem(m_wndList.GetItemCount(), timeClock.Format(_T("%H:%M:00")));
			m_wndList.SetItemText(nIndex, 1, bDataTime ? timeDate.Format(_T("%Y-%m-%d")) : _T("-"));
		}
		else if (FALSE == bDataTime)
		{
			dwRepeatCode |= m_wndList.GetItemData(nIndex);
			m_wndList.SetItemText(nIndex, 0, timeClock.Format(_T("%H:%M:00")));
		}
		else
		{
			break;
		}

		CString strRepeatCode;
		this->ConvertRepeatCodeToString(dwRepeatCode, strRepeatCode);

		m_wndList.SetItemText(nIndex, 2, strRepeatCode);
		m_wndList.SetItemData(nIndex, dwRepeatCode);
		m_wndList.SelectSingleItem(nIndex);

		m_wndList.SetFocus();
	}	break;

	case IDC_SEL_MOD:
	{
		POSITION pos = m_wndList.GetFirstSelectedItemPosition();
		const INT nItemIndex = m_wndList.GetNextSelectedItem(pos);
		DWORD dwRepeatCode = bDataTime ? 0 : this->GetRepeatCode();

		CString strRepeatCode;
		ConvertRepeatCodeToString(dwRepeatCode, strRepeatCode);

		m_wndList.SetItemText(nItemIndex, 0, timeClock.Format(_T("%H:%M:00")));
		m_wndList.SetItemText(nItemIndex, 1, bDataTime ? timeDate.Format(_T("%Y-%m-%d")) : _T("-"));
		m_wndList.SetItemText(nItemIndex, 2, strRepeatCode);
		m_wndList.SetItemData(nItemIndex, dwRepeatCode);

		if (-1 != nIndex && nItemIndex != nIndex)
		{
			m_wndList.DeleteItem(nIndex);
		}

		m_wndList.SelectSingleItem(nItemIndex);
	}	break;

	case IDC_SEL_DEL:
	{
		CList<INT, INT> lstDelete;
		POSITION pos = m_wndList.GetFirstSelectedItemPosition();
		while (pos)
		{
			lstDelete.AddHead(m_wndList.GetNextSelectedItem(pos));
		}

		while (lstDelete.GetCount())
		{
			m_wndList.DeleteItem(lstDelete.GetHead());
			lstDelete.RemoveHead();
		}
	}	break;
	}

}


void CDlgSchedule::OnLvnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	const INT nItemCount = 3;
	BOOL	bEnable[nItemCount] = { FALSE, };
	INT		nSelectRadioIndex = -1;

	DWORD dwRepeatCode = 0;

	const INT nSelCount = m_wndList.GetSelectedCount();
	switch (nSelCount)
	{
	case 0:
	{
		bEnable[0] = TRUE;
		nSelectRadioIndex = 0;
	}	break;

	case 1:
	{
		bEnable[0] = TRUE;
		bEnable[1] = TRUE;
		bEnable[2] = TRUE;
		nSelectRadioIndex = 1;

		POSITION pos = m_wndList.GetFirstSelectedItemPosition();
		if (pos)
		{
			const INT nItemIndex = m_wndList.GetNextSelectedItem(pos);
			dwRepeatCode = m_wndList.GetItemData(nItemIndex);

			COleDateTime dateTime1, dateTime2, today(COleDateTime::GetCurrentTime());
			dateTime1.ParseDateTime(m_wndList.GetItemText(nItemIndex, 0));
			dateTime2.SetDateTime(today.GetYear(), today.GetMonth(), today.GetDay(), dateTime1.GetHour(), dateTime1.GetMinute(), dateTime1.GetSecond());
			TRACE(_T("%d-%d-%d %d:%d:%d\n"), dateTime1.GetYear(), dateTime1.GetMonth(), dateTime1.GetDay(), today.GetHour(), today.GetMinute(), today.GetSecond());
			static_cast<CDateTimeCtrl*>(GetDlgItem(IDC_TIME_PICKER))->SetTime(dateTime2);

			if (0 == dwRepeatCode)
			{
				dateTime1.ParseDateTime(m_wndList.GetItemText(nItemIndex, 1));
				dateTime2.SetDateTime(dateTime1.GetYear(), dateTime1.GetMonth(), dateTime1.GetDay(), today.GetHour(), today.GetMinute(), today.GetSecond());
				static_cast<CDateTimeCtrl*>(GetDlgItem(IDC_DATE_PICKER))->SetTime(dateTime2);
			}
		}

	}	break;

	default:
	{
		bEnable[2] = TRUE;
		nSelectRadioIndex = 2;
	}	break;
	}

	SetRepeatCode(dwRepeatCode);
	EnableEditControls((1 < nSelCount) ? FALSE : TRUE);

	const INT nResID[nItemCount] = {
		IDC_SEL_NEW,
		IDC_SEL_MOD,
		IDC_SEL_DEL
	};

	for (INT nIndex = 0; nIndex < nItemCount; nIndex++)
	{
		CButton* pBtn = static_cast<CButton*>(GetDlgItem(nResID[nIndex]));
		if (pBtn->IsWindowEnabled() != bEnable[nIndex])
		{
			pBtn->EnableWindow(bEnable[nIndex]);
		}
	}

	if (-1 != nSelectRadioIndex)
	{
		SetRadioIndex(nResID, nSelectRadioIndex, nItemCount);
	}

	*pResult = 0;
}


void CDlgSchedule::EnableEditControls(const BOOL bEnable)
{
	const INT nResID[] = {
		IDC_CHK_SUN,
		IDC_CHK_MON,
		IDC_CHK_TUE,
		IDC_CHK_WED,
		IDC_CHK_THU,
		IDC_CHK_FRI,
		IDC_CHK_SAT,
		IDC_DATE_PICKER,
		IDC_TIME_PICKER,
		NULL,
	};

	for (INT i = 0; nResID[i]; i++)
	{
		CWnd* pWnd = GetDlgItem(nResID[i]);
		if (pWnd->IsWindowEnabled() != bEnable)
		{
			pWnd->EnableWindow(bEnable);
		}
	}

	if (bEnable)
	{
		DisplayRepeatCondition();
	}
}

const INT CDlgSchedule::FindBackupReservation(COleDateTime timeClock)
{
	COleDateTime timeDate;
	timeDate.SetTime(timeClock.GetHour(), timeClock.GetMinute(), 0);
	return FindBackupReservation(timeDate, timeClock);
}

const INT CDlgSchedule::FindBackupReservation(COleDateTime timeDate, COleDateTime timeClock)
{
	const INT nItemCount = m_wndList.GetItemCount();

	COleDateTime time;
	time.SetDateTime(timeDate.GetYear(), timeDate.GetMonth(), timeDate.GetDay(), timeClock.GetHour(), timeClock.GetMinute(), 0);

	for (INT i = 0; i < nItemCount; i++)
	{
		BOOL bRepeat = TRUE;
		CString strDate(m_wndList.GetItemText(i, 1));
		if (3 < strDate.GetLength())
		{
			bRepeat = FALSE;
		}

		CString strTime(m_wndList.GetItemText(i, 0));
		COleDateTime timeItem;
		timeItem.ParseDateTime(bRepeat ? strTime : strDate + _T(" ") + strTime);

		if (timeItem == time)
		{
			return i;
		}
	}

	return -1;
}

void CDlgSchedule::SetRepeatCode(const DWORD dwRepeatCode)
{
	const INT nResID[] = {
		IDC_CHK_SUN,
		IDC_CHK_MON,
		IDC_CHK_TUE,
		IDC_CHK_WED,
		IDC_CHK_THU,
		IDC_CHK_FRI,
		IDC_CHK_SAT,
		NULL,
	};

	for (INT i = 0; i < nResID[i]; i++)
	{
		const INT nChecked = (dwRepeatCode & (0x1 << i)) ? BST_CHECKED : BST_UNCHECKED;

		CButton* pBtn = static_cast<CButton*>(GetDlgItem(nResID[i]));
		if (nChecked != (BST_CHECKED == pBtn->GetCheck()))
		{
			pBtn->SetCheck(nChecked);
		}
	}
}

const DWORD CDlgSchedule::GetRepeatCode()
{
	DWORD dwResult = 0;

	const INT nResID[] = {
		IDC_CHK_SUN,
		IDC_CHK_MON,
		IDC_CHK_TUE,
		IDC_CHK_WED,
		IDC_CHK_THU,
		IDC_CHK_FRI,
		IDC_CHK_SAT,
		NULL,
	};

	for (INT i = 0; i < nResID[i]; i++)
	{
		if (BST_CHECKED == static_cast<CButton*>(GetDlgItem(nResID[i]))->GetCheck())
		{
			dwResult |= (0x1 << i);
		}
	}

	return dwResult;
}

void CDlgSchedule::ConvertRepeatCodeToString(const DWORD dwRepeatCode, CString& strRepeatCode)
{
	strRepeatCode.Empty();

	TCHAR* strWeekCode = _T("SMTWTFS");
	TCHAR chEmptyWeekCode = _T('-');

	const INT nStrLen = _tcslen(strWeekCode);
	for (INT i = 0; i < nStrLen; i++)
	{

		strRepeatCode += (dwRepeatCode & (0x1 << i)) ? strWeekCode[i] : chEmptyWeekCode;
		strRepeatCode += _T(" ");
	}
}


BOOL CDlgSchedule::OnInitDialog()
{
	CDlgDevice::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_BACKUP_SCHEDULE));
	this->SetIcon(hIcon, FALSE);  

	if (1)
	{
		static_cast<CButton*>(GetDlgItem(IDC_SEL_NEW))->SetCheck(BST_CHECKED);
	}

	static_cast<CDateTimeCtrl*>(GetDlgItem(IDC_TIME_PICKER))->SetFormat(_T("tt, hh:mm:00"));

	SetTimer(TIMER_INITIALIZE, ELAPSE_TIMER_INITIALIZE, NULL);

	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgSchedule::OnBnClickedOk()
{
	m_wndList.SaveHeaderItemInfo();

	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	if (NULL == pDevice)
	{
		return;
	}

	CString strSystemName(pDevice->GetSystemName());

	BOOL bResult = TRUE;
	CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();

	if (pProjectDB->OpenDataBase())
	{
		if (pProjectDB->Begin() &&
			pProjectDB->DeleteBackupReservationDataAll(strSystemName))
		{
			const INT nCount = m_wndList.GetItemCount();
			for (INT nIndex = 0; nIndex < nCount; nIndex++)
			{
				CString strTime(m_wndList.GetItemText(nIndex, 0));
				CString strDate(m_wndList.GetItemText(nIndex, 1));
				const DWORD dwWeekCode = m_wndList.GetItemData(nIndex);
				CString strWeekCode;
				if (dwWeekCode & 0x01) strWeekCode += _T("A");
				if (dwWeekCode & 0x02) strWeekCode += _T("B");
				if (dwWeekCode & 0x04) strWeekCode += _T("C");
				if (dwWeekCode & 0x08) strWeekCode += _T("D");
				if (dwWeekCode & 0x10) strWeekCode += _T("E");
				if (dwWeekCode & 0x20) strWeekCode += _T("F");
				if (dwWeekCode & 0x40) strWeekCode += _T("G");

				COleDateTime dateTime;
				dateTime.ParseDateTime(dwWeekCode ? strTime : strTime + _T(" ") + strDate);

				if (FALSE == pProjectDB->InsertBackupReservationData(strSystemName, dateTime, strWeekCode))
				{
					bResult = FALSE;
					break;
				}
			}
		}
		else
		{
			bResult = FALSE;
		}

		if (bResult)
		{
			pProjectDB->Commit();
			pProjectDB->CloseDataBase();
			pDevice->CheckAutoBackupSchedule();
		}
		else
		{
			pProjectDB->CancelCommit();
			pProjectDB->CloseDataBase();
		}
	}

	CDialog::OnOK();
}



void CDlgSchedule::InitControlTextSize()
{
	if (true)
	{
		LOGFONT lf = { 0, };
		afxGlobalData.fontRegular.GetLogFont(&lf);

		lf.lfHeight = -15;
		m_BigFont.CreateFontIndirect(&lf);

		GetDlgItem(IDC_LIST)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_GROUP_TIME)->SetFont(&m_BigFont);
		GetDlgItem(IDC_DATE_PICKER)->SetFont(&m_BigFont);
		GetDlgItem(IDC_TIME_PICKER)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_GROUP_DAY)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_GROUP_CONTROL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_OK)->SetFont(&m_BigFont);
		GetDlgItem(IDOK)->SetFont(&m_BigFont);
		GetDlgItem(IDCANCEL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_SEL_NEW)->SetFont(&m_BigFont);
		GetDlgItem(IDC_SEL_MOD)->SetFont(&m_BigFont);
		GetDlgItem(IDC_SEL_DEL)->SetFont(&m_BigFont);

		for (INT i = 0; i < 7; i++)
		{
			GetDlgItem(IDC_CHK_MON + i)->SetFont(&m_BigFont);
		}

		m_BigFont.Detach();
	}

}
