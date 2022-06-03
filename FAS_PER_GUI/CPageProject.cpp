// CPageProject.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageProject.h"
#include "afxdialogex.h"

#include "CDeviceList.h"
#include "CSheetBackup.h"
#include "MainFrm.h"
#include "CDataBaseManager.h"
#include "CSheetRestoration.h"
// CPageProject dialog

IMPLEMENT_DYNAMIC(CPageProject, CMFCPropertyPageEx)

CPageProject::CPageProject(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPageEx(IDP_PROJECT)
{
	m_bSystemCtrl = FALSE;

}

CPageProject::~CPageProject()
{
}

void CPageProject::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageProject, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CPageProject::OnBnClickedBtnNext)
END_MESSAGE_MAP()


// CPageProject message handlers


void CPageProject::OnBnClickedBtnNext()
{
	ActivateNextPage();
}


BOOL CPageProject::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	m_btnNext.SubclassDlgItem(IDC_BTN_NEXT, this);
	m_btnNext.SetImage(IDB_NEXT);


	if (1)
	{
		COLORREF m_colorBkName = RGB(0x0a, 0x0a, 0x50);
		COLORREF m_colorTxtName = RGB(0xc8, 0xc8, 0xc8);

		const INT nResID[] = {
			IDC_ST_ADDRESS_CAPTION,
			IDC_ST_NAME_CAPTION,			
			IDC_ST_COUNT_CAPTION,
			NULL
		};

		for (INT nIndex = 0; nResID[nIndex]; nIndex++)
		{
			m_wndColor[nIndex].SubclassDlgItem(nResID[nIndex], this);
			m_wndColor[nIndex].SetBkColor(m_colorBkName);
			m_wndColor[nIndex].SetTextColor(m_colorTxtName);
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
			NULL,
		};

		for (INT nIndex = 0; nResID[nIndex]; nIndex++)
		{
			const INT nRelIndex = nIndex + 3;
			m_wndColor[nRelIndex].SubclassDlgItem(nResID[nIndex], this);
			m_wndColor[nRelIndex].SetBkColor(m_colorBkName);
			m_wndColor[nRelIndex].SetTextColor(m_colorTxtName);
		}
	}

	if (IsSystemCtrl())
	{
		CString str = _T("");
		CString strPrjName = ((CMainFrame*)AfxGetMainWnd())->GetSystemCompareProjectName();
		SetDlgItemText(IDC_ST_ADDRESS, str);
		SetDlgItemText(IDC_ST_COUNT, str);
		SetDlgItemText(IDC_ST_NAME, strPrjName);

		LoadSystemControlImage(strPrjName);
	}
	else
	{
		CDeviceMotionGate* pDevice = NULL;
		if (CDeviceList::GetInstance()->GetMotiongateByAddress(static_cast<CSheetBackup*>(GetParent())->GetAddress(), pDevice))
		{
			CString strAddress;
			pDevice->GetAddress(strAddress);
			SetDlgItemText(IDC_ST_ADDRESS, strAddress);

			SetDlgItemText(IDC_ST_NAME, pDevice->GetSystemName());

			CString strMotorCount;
			strMotorCount.Format(_T("%d"), pDevice->GetMotorCount());
			SetDlgItemText(IDC_ST_COUNT, strMotorCount);

			m_wndViewer.SubclassDlgItem(IDC_ST_PROJECT, this);
			m_wndViewer.LoadMainImage(pDevice->GetSystemImage());
		}


		CRect rect;
		m_wndViewer.GetClientRect(rect);

		INT nWidth = rect.Width();
		INT nHeight = rect.Height();
	}

	InitControlTextSize();

	return TRUE;
}


void CPageProject::SetSystemControl(BOOL bCtrl)
{
	m_bSystemCtrl = bCtrl;
}


BOOL CPageProject::IsSystemCtrl()
{
	return m_bSystemCtrl;
}


void CPageProject::LoadSystemControlImage(CString strName)
{
	CString str;
	CDataBaseProject* pProject = CDataBaseProject::GetInstance();

	if (pProject->OpenDataBase())
	{
		if (pProject->GetDeviceList())
		{
			while (CDataBaseRecordData* pRecord = pProject->SeekToNext())
			{
				CDeviceMotionGate* pDevice = new CDeviceMotionGate;
				if (pDevice->LoadFromDatabase(pRecord))
				{
					if (pDevice->GetDatabaseSystemName() == strName)
					{
						m_wndViewer.SubclassDlgItem(IDC_ST_PROJECT, this);
						m_wndViewer.EnableWindow(FALSE);

						m_wndViewer.LoadMainImage(pDevice->GetSystemImage());

						// etc
						CSheetRestoration* pParent = static_cast<CSheetRestoration*>(GetParent());
						pParent->SetDataBaseMotorCount(pDevice->GetDatabaseMotorCount());
						str.Format(_T("%d"), pDevice->GetDatabaseMotorCount());
						SetDlgItemText(IDC_ST_COUNT, str);
					}
				}

				else
				{
					delete pDevice;
					ASSERT(0);
				}
			}
		}

		pProject->CloseDataBase();
	}
}


void CPageProject::InitControlTextSize()
{
	if (true)
	{
		LOGFONT lf = { 0, };
		afxGlobalData.fontRegular.GetLogFont(&lf);

		lf.lfHeight = -15;
		m_BigFont.CreateFontIndirect(&lf);

		GetDlgItem(IDC_ST_ADDRESS_CAPTION)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_ADDRESS)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_NAME_CAPTION)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_NAME)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_COUNT_CAPTION)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_COUNT)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}
}
