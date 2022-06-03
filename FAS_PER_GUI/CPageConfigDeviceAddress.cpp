// CPageConfigDeviceAddress.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageConfigDeviceAddress.h"
#include "afxdialogex.h"

#include "CSheetConfigDevice.h"
#include "CDataBaseManager.h"
//#include "CDeviceList.h"

// CPageConfigDeviceAddress dialog

IMPLEMENT_DYNAMIC(CPageConfigDeviceAddress, CPageConfig)

CPageConfigDeviceAddress::CPageConfigDeviceAddress(CWnd* pParent /*=nullptr*/)
	: CPageConfig(IDP_CONFIG_DEVICE_ADDRESS)
{
	m_bOverlapDeviceID = FALSE;
}


CPageConfigDeviceAddress::~CPageConfigDeviceAddress()
{
}


void CPageConfigDeviceAddress::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageConfigDeviceAddress, CMFCPropertyPage)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT_SYSTEM, &CPageConfigDeviceAddress::OnEnChangeEditSystem)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_GENERATE, &CPageConfigDeviceAddress::OnBnClickedBtnGenerate)
	ON_EN_CHANGE(IDC_EDIT_DEVICE_ID, &CPageConfigDeviceAddress::OnEnChangeEditDeviceId)
END_MESSAGE_MAP()


// CPageConfigDeviceAddress message handlers
BOOL CPageConfigDeviceAddress::OnInitDialog()
{
	CMFCPropertyPageEx::OnInitDialog();

	CSheetConfigDevice* pParent = dynamic_cast<CSheetConfigDevice*>(GetParent());
		
	m_strSystemName = pParent->GetEditSystemName();
	m_strDeviceID.Format(_T("%d"), pParent->GetEditDeviceID());
	
	SetDlgItemText(IDC_EDIT_SYSTEM, m_strSystemName);
	SetDlgItemText(IDC_EDIT_DEVICE_ID, m_strDeviceID);
	
	CreateNamingWizard();

	return TRUE;
}

void CPageConfigDeviceAddress::OnEnChangeEditSystem()
{
	const INT nDelimeter = 20;

	CWnd* pWnd = GetDlgItem(IDC_EDIT_SYSTEM);

	pWnd->GetWindowText(m_strSystemName);

	ShowWizardButtonOK();
}


void CPageConfigDeviceAddress::OnEnChangeEditDeviceId()
{
	INT nDeviceID = -1;
	
	CWnd* pWnd = GetDlgItem(IDC_EDIT_DEVICE_ID);

	pWnd->GetWindowText(m_strDeviceID);

	
	
	ShowWizardButtonOK();

	/*else
	{
		AfxMessageBox(_T("Please enter values from 0 to 255"));
		pWnd->SetWindowText(L"");
	}*/
}


const INT CPageConfigDeviceAddress::CheckSystemName()
{
	return m_nCheckName;
}


void CPageConfigDeviceAddress::OnOK()
{
	CSheetConfigDevice* pParent = dynamic_cast<CSheetConfigDevice*>(GetParent());

	ASSERT(pParent);
	pParent->SetEditSystemName(m_strSystemName);
	pParent->SetEditDeviceID(_ttoi(m_strDeviceID));
	CPageConfig::OnOK();
}

void CPageConfigDeviceAddress::ShowWizardButtonOK()
{
	SetTimer(TIMER_SHOW_WIZARD_BUTTON, 100, NULL);
}

void CPageConfigDeviceAddress::OnTimer(UINT_PTR nIDEvent)
{	
	switch (nIDEvent)
	{
	case TIMER_SHOW_WIZARD_BUTTON:
	{
		KillTimer(nIDEvent);

		const INT nLength = m_strSystemName.GetLength();

		INT nCmdShow = SW_SHOWNORMAL;
		if (0 == nLength || 20 < nLength)
		{
			nCmdShow = SW_HIDE;
		}
		else
		{
			//	
		}

		dynamic_cast<CSheetConfigDevice*>(GetParent())->ShowWizardButton(IDOK, nCmdShow);

		/*
			bool bContinue = true;
		
		const INT nLength = m_strSystemName.GetLength();
		
		CString str = m_strDeviceID == L"" ? L"None" : m_strDeviceID;

		for (int i = 0; i < str.GetLength(); i++) // 숫자로만 구성되어있는지 확인!!
		{
			bContinue &= isdigit(str.GetAt(i)) ? true : false;
		}
		
		if (bContinue)
		{
			BOOL bOverlapDeivceID = FALSE;
			CString strResult = L"";
			const INT nDeviceID = _ttoi(m_strDeviceID);
			
			CDataBaseProject* pProject = CDataBaseProject::GetInstance();
			bOverlapDeivceID = pProject->CheckOverlapDeviceID(nDeviceID, strResult, m_strSystemName); // device id 중복 여부 확인.
			
			const INT nCmdShow = !(0 == nLength || 20 < nLength) && ((0 <= nDeviceID) && (256 > nDeviceID)) && bOverlapDeivceID ? SW_SHOWNORMAL : SW_HIDE;

			dynamic_cast<CSheetConfigDevice*>(GetParent())->ShowWizardButton(IDOK, nCmdShow);
		}

		else
		{
			dynamic_cast<CSheetConfigDevice*>(GetParent())->ShowWizardButton(IDOK, SW_HIDE);
		}

		*/
	}	break;
	}
	CPageConfig::OnTimer(nIDEvent);
}

void CPageConfigDeviceAddress::CreateNamingWizard()
{
	CComboBox* pCmbMainZone = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_MAIN_ZONE));
	CComboBox* pCmbSubZone = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_SUB_ZONE));
	CComboBox* pCmbStation = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_STATION));
	CComboBox* pCmbRobot = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_ROBOT));
	CComboBox* pCmbTool = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_TOOL));

	CString strTemp;

	for (INT nIndex = _T('A'); nIndex <= _T('Z'); nIndex++)
	{
		strTemp.Format(_T("%c"), nIndex);
		pCmbMainZone->AddString(strTemp);
		pCmbSubZone->AddString(strTemp);
	}

	pCmbMainZone->SetCurSel(0);
	pCmbSubZone->SetCurSel(0);

	for (INT nIndex = 0; nIndex < 1000; nIndex++)
	{
		strTemp.Format(_T("%03d"), nIndex + 1);
		pCmbStation->AddString(strTemp);
	}

	pCmbStation->SetCurSel(0);

	for (INT nIndex = 0; nIndex < 100; nIndex++)
	{
		strTemp.Format(_T("%02d"), nIndex + 1);
		pCmbRobot->AddString(strTemp);
		pCmbTool->AddString(strTemp);
	}

	pCmbRobot->SetCurSel(0);
	pCmbTool->SetCurSel(0);
}


void CPageConfigDeviceAddress::OnBnClickedBtnGenerate()
{
	CComboBox* pCmbMainZone = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_MAIN_ZONE));
	CComboBox* pCmbSubZone = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_SUB_ZONE));
	CComboBox* pCmbStation = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_STATION));
	CComboBox* pCmbRobot = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_ROBOT));
	CComboBox* pCmbTool = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_TOOL));

	BOOL bAutoComplete = TRUE;

	CString strTemp, strProjectName;
	GetDlgItemText(IDC_CMB_MAIN_ZONE, strProjectName);

	/*GetDlgItemText(IDC_EDIT_SEPERATOR1, strTemp);
	strProjectName += strTemp;*/

	GetDlgItemText(IDC_CMB_SUB_ZONE, strTemp);
	strProjectName += strTemp;

	/*GetDlgItemText(IDC_EDIT_SEPERATOR2, strTemp);
	strProjectName += strTemp;*/

	GetDlgItemText(IDC_CMB_STATION, strTemp);
	if (bAutoComplete)
	{
		strTemp.Format(_T("%03d"), _ttoi(strTemp));
		SetDlgItemText(IDC_CMB_STATION, strTemp);
	}
	strProjectName += strTemp;

	/*GetDlgItemText(IDC_EDIT_SEPERATOR3, strTemp);
	strProjectName += strTemp;*/

	strProjectName += _T("R");

	GetDlgItemText(IDC_CMB_ROBOT, strTemp);
	if (bAutoComplete)
	{
		strTemp.Format(_T("%02d"), _ttoi(strTemp));
		SetDlgItemText(IDC_CMB_ROBOT, strTemp);
	}
	strProjectName += strTemp;

	/*GetDlgItemText(IDC_EDIT_SEPERATOR4, strTemp);
	strProjectName += strTemp;*/

	strProjectName += _T("B");

	GetDlgItemText(IDC_CMB_TOOL, strTemp);
	if (bAutoComplete)
	{
		strTemp.Format(_T("%02d"), _ttoi(strTemp));
		SetDlgItemText(IDC_CMB_TOOL, strTemp);
	}
	strProjectName += strTemp;

	strProjectName.MakeUpper();

	SetDlgItemText(IDC_EDIT_SYSTEM, strProjectName);
}

