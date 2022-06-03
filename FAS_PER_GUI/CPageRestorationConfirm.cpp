// CPageRestorationConfirm.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageRestorationConfirm.h"
#include "afxdialogex.h"

#include "CSheetRestoration.h"
#include "CSheetSelectRestoration.h"

#include "CDeviceList.h"
#include "CDataBaseManager.h"

// CPageRestorationConfirm dialog

IMPLEMENT_DYNAMIC(CPageRestorationConfirm, CMFCPropertyPageEx)

CPageRestorationConfirm::CPageRestorationConfirm(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPageEx(IDP_RESTORATION_CONFIRM)
{
	m_bCompareType = true;
}

CPageRestorationConfirm::~CPageRestorationConfirm()
{
}

void CPageRestorationConfirm::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageRestorationConfirm, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_BTN_BACK, &CPageRestorationConfirm::OnBnClickedBtnBack)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CPageRestorationConfirm::OnBnClickedBtnNext)
	ON_BN_CLICKED(IDC_BTN_HOMING, &CPageRestorationConfirm::OnBnClickedBtnHoming)
END_MESSAGE_MAP()


// CPageRestorationConfirm message handlers

BOOL CPageRestorationConfirm::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	if (1)
	{
		COLORREF m_colorBkName = RGB(0x0a, 0x0a, 0x50);
		COLORREF m_colorTxtName = RGB(0xc8, 0xc8, 0xc8);

		const INT nResID[] = {
			IDC_ST_ADDRESS_CAPTION,
			IDC_ST_NAME_CAPTION,
			IDC_ST_COUNT_CAPTION,			
			IDC_ST_COMMENT_CAPTION,
			IDC_ST_TIMESTAMP_CAPTION,
			IDC_ST_NOMINAL_CAPTION,
			IDC_ST_LATEST_CAPTION,
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
			IDC_ST_ADDRESS,
			IDC_ST_NAME,
			IDC_ST_COUNT,
			IDC_ST_COMMENT,
			IDC_ST_TIMESTAMP,
			IDC_ST_NOMINAL,
			IDC_ST_LATEST,
			NULL,
		};

		for (INT nIndex = 0; nResID[nIndex]; nIndex++)
		{
			m_wndText[nIndex].SubclassDlgItem(nResID[nIndex], this);
			m_wndText[nIndex].SetBkColor(m_colorBkName);
			m_wndText[nIndex].SetTextColor(m_colorTxtName);
		}
	}

	m_btnBack.SubclassDlgItem(IDC_BTN_BACK, this);
	m_btnBack.SetImage(IDB_BACK);

	m_btnStart.SubclassDlgItem(IDC_BTN_NEXT, this);
	m_btnStart.SetImage(IDB_START);

	m_btnHome.SubclassDlgItem(IDC_BTN_HOMING, this);
	
	if (m_bCompareType)
	{
		m_btnHome.SetImage(static_cast<CSheetRestoration*>(GetParent())->IsAutoHomingModeEnabled() ? IDB_HOMING_ON : IDB_HOMING_OFF);
	}
	else 
	{
		m_btnHome.SetImage(static_cast<CSheetSelectRestoration*>(GetParent())->IsAutoHomingModeEnabled() ? IDB_HOMING_ON : IDB_HOMING_OFF);
	}

	InitControlTextSize();

	return TRUE;
}

BOOL CPageRestorationConfirm::OnSetActive()
{
	if (m_bCompareType)
	{
		CSheetRestoration* pParent = static_cast<CSheetRestoration*>(GetParent());
		CDeviceMotionGate* pDevice = NULL;

		if (CDeviceList::GetInstance()->GetMotiongateByAddress(pParent->GetAddress(), pDevice))
		{
			CString strAddress;
			pDevice->GetAddress(strAddress);
			SetDlgItemText(IDC_ST_ADDRESS, strAddress);

			SetDlgItemText(IDC_ST_NAME, pDevice->GetSystemName());

			CString strMotorCount;
			strMotorCount.Format(_T("%d"), pDevice->GetMotorCount());
			SetDlgItemText(IDC_ST_COUNT, strMotorCount);
		}

		SetDlgItemText(IDC_ST_TIMESTAMP, pParent->GetProjectTimeStamp());
		SetDlgItemText(IDC_ST_COMMENT, pParent->GetProjectComment());

		if (1)
		{
			CColorStaticST* pWnd = static_cast<CColorStaticST*>(GetDlgItem(IDC_ST_NOMINAL));
			if (pParent->IsProjectNominalData())
			{
				pWnd->SetWindowText(_T("Yes"));
				pWnd->SetBkColor(RGB(0xc8, 0xc8, 0x00));
			}
			else
			{
				pWnd->SetWindowText(_T("No"));
				pWnd->SetBkColor(RGB(0xf0, 0xf0, 0xf0));
			}
		}
		if (1)
		{
			CColorStaticST* pWnd = static_cast<CColorStaticST*>(GetDlgItem(IDC_ST_LATEST));
			if (pParent->IsBackupEqualRestoredID())
			{
				pWnd->SetWindowText(_T("Yes"));
				pWnd->SetBkColor(RGB(0xc8, 0xc8, 0x00));
				//pWnd->SetBkColor(RGB(0xf0, 0xf0, 0xf0));
			}
			else
			{
				pWnd->SetWindowText(_T("No"));
				pWnd->SetBkColor(RGB(0xf0, 0xf0, 0xf0));
				//pWnd->SetBkColor(RGB(0xc8, 0xc8, 0x00));
			}
		}

		if (pParent->IsAutoHomingModeEnabled())
		{
			m_btnHome.SetImage(IDB_HOMING_ON);
			m_btnHome.SetWindowText(_T("Enabled"));
		}
		else
		{
			m_btnHome.SetImage(IDB_HOMING_OFF);
			m_btnHome.SetWindowText(_T("Disabled"));
		}
	}
	
	else
	{
		CSheetSelectRestoration* pParent = static_cast<CSheetSelectRestoration*>(GetParent());
		CDeviceMotionGate* pDevice = NULL;

		if (CDeviceList::GetInstance()->GetMotiongateByAddress(pParent->GetAddress(), pDevice))
		{
			CString strAddress;
			pDevice->GetAddress(strAddress);
			SetDlgItemText(IDC_ST_ADDRESS, strAddress);

			SetDlgItemText(IDC_ST_NAME, pDevice->GetSystemName());

			CString strMotorCount;
			strMotorCount.Format(_T("%d"), pDevice->GetMotorCount());
			SetDlgItemText(IDC_ST_COUNT, strMotorCount);
		}

		SetDlgItemText(IDC_ST_TIMESTAMP, pParent->GetProjectTimeStamp());
		SetDlgItemText(IDC_ST_COMMENT, pParent->GetProjectComment());

		if (1)
		{
			CColorStaticST* pWnd = static_cast<CColorStaticST*>(GetDlgItem(IDC_ST_NOMINAL));
			if (pParent->IsProjectNominalData())
			{
				pWnd->SetWindowText(_T("Yes"));
				pWnd->SetBkColor(RGB(0xc8, 0xc8, 0x00));
			}
			else
			{
				pWnd->SetWindowText(_T("No"));
				pWnd->SetBkColor(RGB(0xf0, 0xf0, 0xf0));
			}
		}
		if (1)
		{
			CColorStaticST* pWnd = static_cast<CColorStaticST*>(GetDlgItem(IDC_ST_LATEST));
			if (pParent->IsBackupEqualRestoredID())
			{
				pWnd->SetWindowText(_T("Yes"));
				pWnd->SetBkColor(RGB(0xc8, 0xc8, 0x00));
				//pWnd->SetBkColor(RGB(0xf0, 0xf0, 0xf0));
			}
			else
			{
				pWnd->SetWindowText(_T("No"));
				pWnd->SetBkColor(RGB(0xf0, 0xf0, 0xf0));
				//pWnd->SetBkColor(RGB(0xc8, 0xc8, 0x00));
			}
		}

		if (pParent->IsAutoHomingModeEnabled())
		{
			m_btnHome.SetImage(IDB_HOMING_ON);
			m_btnHome.SetWindowText(_T("Enabled"));
		}
		else
		{
			m_btnHome.SetImage(IDB_HOMING_OFF);
			m_btnHome.SetWindowText(_T("Disabled"));
		}

		/*
		if (1)
		{ // KKB
			CString strError;
			
			if (1)
			{
				CDataBaseProject* pProject = CDataBaseProject::GetInstance();
				BOOL bConnected = TRUE;
				
				if (pProject->OpenDataBase())
				{
					CString strName = pDevice->GetSystemName();
					INT nBackupID = pParent->GetProjectBackupID();
					INT nIndex = -1;
					INT nMotorCount = -1;
					if (pProject->GetBackupFileMotorIndex(strName, nBackupID))
					{
						while (CDataBaseRecordData* pRecord = pProject->SeekToNext())
						{
							if (LoadBackupFileMotorIndex(pRecord, nIndex))
							{
								if (!pDevice->IsMotorConnected(nIndex))
								{
									strError.Format(_T("Motor Index (Restoration Information) ¡Á Motor Index (Project Information)"));
									bConnected = FALSE;
									break;
								}
							}
						}
					}

					if (bConnected)
					{
						if (pProject->GetBackupMotorCount(strName, nBackupID, nMotorCount))
						{
							if (pDevice->GetMotorCount() != nMotorCount)
							{
								strError.Format(_T("Motor Count (Restoration Information) ¡Á Motor Count (Project Information)"));
								bConnected = FALSE;
							}
						}
						else
						{
							strError.Format(_T("[estoration Information] File open error."));
							bConnected = FALSE;
						}
					}
				}

				pProject->CloseDataBase();
				
				SetDlgItemText(IDC_STATIC_NOTICE_ERROR, strError);
				GetDlgItem(IDC_STATIC_NOTICE)->ShowWindow(!bConnected);
				m_btnStart.EnableWindow(bConnected);
			}
		}
		*/
		
	}

	m_btnHome.Invalidate(FALSE);
	return CMFCPropertyPage::OnSetActive();
}


void CPageRestorationConfirm::OnBnClickedBtnBack()
{
	ActivatePrevPage();
}


void CPageRestorationConfirm::OnBnClickedBtnNext()
{
	OnOK();
}


void CPageRestorationConfirm::OnBnClickedBtnHoming()
{
	if (m_bCompareType)
	{
		CSheetRestoration* pParent = static_cast<CSheetRestoration*>(GetParent());
	}
	else
	{
		CSheetSelectRestoration* pParent = static_cast<CSheetSelectRestoration*>(GetParent());
	}
	CSheetSelectRestoration* pParent = static_cast<CSheetSelectRestoration*>(GetParent());
	const BOOL bEnable = !pParent->IsAutoHomingModeEnabled();
	pParent->EnableAutoHomingMode(bEnable);

	if (bEnable)
	{
		m_btnHome.SetImage(IDB_HOMING_ON);
		m_btnHome.SetWindowText(_T("Enabled"));
	}
	else
	{
		m_btnHome.SetImage(IDB_HOMING_OFF);
		m_btnHome.SetWindowText(_T("Disabled"));
	}

	m_btnHome.Invalidate(FALSE);
}

const BOOL CPageRestorationConfirm::LoadBackupFileMotorIndex(LPVOID pDatabaseRecordData, INT& nIndex)
{
	CDataBaseRecordData* pRecord = static_cast<CDataBaseRecordData*>(pDatabaseRecordData);

	if (FALSE == pRecord->GetIntergerData(_T("Motor_ID"), nIndex))
	{
		return FALSE;
	}

	return TRUE;
}



void CPageRestorationConfirm::InitControlTextSize()
{
	if (true)
	{
		LOGFONT lf = { 0, };
		afxGlobalData.fontRegular.GetLogFont(&lf);

		lf.lfHeight = -15;
		m_BigFont.CreateFontIndirect(&lf);

		GetDlgItem(IDC_STATIC_PROJECT_INFO)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_ADDRESS_CAPTION)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_ADDRESS)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_NAME_CAPTION)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_NAME)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_COUNT_CAPTION)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_COUNT)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_RESTORE_INFO)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_TIMESTAMP_CAPTION)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_TIMESTAMP)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_COMMENT_CAPTION)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_COMMENT)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_NOMINAL_CAPTION)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_NOMINAL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_LATEST_CAPTION)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_LATEST)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}

	/*if (true)
	{
		m_BigFont.CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, TEXT_STYLE);

	GetDlgItem(IDC_STATIC_PROJECT_INFO)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_ADDRESS_CAPTION)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_ADDRESS)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_NAME_CAPTION)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_NAME)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_COUNT_CAPTION)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_COUNT)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_RESTORE_INFO)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_TIMESTAMP_CAPTION)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_TIMESTAMP)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_COMMENT_CAPTION)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_COMMENT)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_NOMINAL_CAPTION)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_NOMINAL)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_LATEST_CAPTION)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_LATEST)->SetFont(&m_BigFont);

	m_BigFont.Detach();
	}*/

}
