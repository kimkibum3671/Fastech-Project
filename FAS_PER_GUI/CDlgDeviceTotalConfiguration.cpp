// CDlgDeviceTotalConfiguration.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgDeviceTotalConfiguration.h"

#include "CDataBaseManager.h"
#include "CDeviceList.h"

#include "afxdialogex.h"


// CDlgDeviceTotalConfiguration 대화 상자

IMPLEMENT_DYNAMIC(CDlgDeviceTotalConfiguration, CDialogEx)

CDlgDeviceTotalConfiguration::CDlgDeviceTotalConfiguration(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CONFIG_DEVICE_TOTAL, pParent)
{
	m_pDevice = NULL;

	m_bOverlapDeviceID = FALSE;

	m_bProgramStart = TRUE;

	m_nHomingTimeout = 60;

	m_bTotalConfig = FALSE;

	m_bNoticeConfig = FALSE;

	m_nSelectedMethod = -1;
}

CDlgDeviceTotalConfiguration::~CDlgDeviceTotalConfiguration()
{
	/*if (m_bmpImage.GetSafeHandle())
	{
		m_bmpImage.DeleteObject();
	}*/
}

void CDlgDeviceTotalConfiguration::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgDeviceTotalConfiguration, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT_SYSTEM, &CDlgDeviceTotalConfiguration::OnEnChangeEditSystem)
	ON_EN_CHANGE(IDC_EDIT_DEVICE_ID, &CDlgDeviceTotalConfiguration::OnEnChangeEditDeviceId)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_MOTOR, &CDlgDeviceTotalConfiguration::OnCbnSelchangeComboMotor)
	ON_EN_CHANGE(IDC_EDIT_TIMEOUT, &CDlgDeviceTotalConfiguration::OnEnChangeEditTimeout)
	ON_BN_CLICKED(IDOK, &CDlgDeviceTotalConfiguration::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgDeviceTotalConfiguration 메시지 처리기


void CDlgDeviceTotalConfiguration::SetMotiongateCtrl(CDeviceMotionGate* pDevice)
{
	m_pDevice = pDevice;
}


BOOL CDlgDeviceTotalConfiguration::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_CONFIG));
	this->SetIcon(hIcon, FALSE);


	/*CPageConfigDeviceAddress 부분*/
	if (1)
	{
		if (require_notice_modify == GetConfigMethod())
		{
			//m_strSystemName = m_pDevice->GetDatabaseSystemName();
			m_strSystemName = m_pDevice->GetSystemName();
		}
		else if (require_system_modify == GetConfigMethod())
		{
			m_strSystemName = m_pDevice->GetEditSystemName();
		}
		else if (require_device_modify == GetConfigMethod())
		{
			m_strSystemName = m_pDevice->GetSystemName();
		}
		
		m_strDeviceID.Format(_T("%d"), m_pDevice->GetEditDeviceID());

		SetDlgItemText(IDC_EDIT_SYSTEM, m_strSystemName);
		SetDlgItemText(IDC_EDIT_DEVICE_ID, m_strDeviceID);

		CreateNamingWizard();
		((CButton*)GetDlgItem(IDC_CHECK_ACTIVE_WIZARD))->SetCheck(FALSE);
	}

	/*CPageConfigDeviceMotor 부분*/
	if (1)
	{ //여기서 MotionGate FactoryReset 진행된 상태에서 Notice에서 호출되는 경우 생각해야함..
		
		if (require_notice_modify == GetConfigMethod())
		{
			m_nMotorCount = m_pDevice->GetMotorCount();
		}
		else if (require_system_modify == GetConfigMethod())
		{
			m_nMotorCount = m_pDevice->GetEditMotorCount();
		}
		else if (require_device_modify == GetConfigMethod())
		{
			m_nMotorCount = m_pDevice->GetMotorCount();
		}

		CreateConboMotorNum();
	}

	/*CPageConfigDeviceOrigin 부분*/
	if (1)
	{ // Control HIDE
		(GetDlgItem(IDC_EDIT_TIMEOUT))->EnableWindow(FALSE);
		SetDlgItemInt(IDC_EDIT_TIMEOUT, m_pDevice->GetEditHomingTimeout(), FALSE);
	}

	/*CPageConfigDeviceImage 부분*/
	if (1) 
	{

		if (m_wndViewer.SubclassDlgItem(IDC_ST_INFORM_IMAGE, this))
		{
			m_pDevice->GetSystemImageBitmap(&m_bmpImage);
			m_wndViewer.LoadMainImage(m_pDevice->GetSystemImage());
		}
	}

	/*Naming Wizard*/
	if (1)
	{
		ShowNamingWizard(FALSE, m_bProgramStart);
	}
	
	/*CSheetConfigDeviceSingle 부분*/
	if (m_pDevice)
	{
		BOOL bEnableControl = TRUE;
		CString strTitle, strOkButton;
		//SetWindowText(GetTitleText());
		

		switch (GetConfigMethod())
		{
		case require_notice_modify:
		{
			strOkButton.Format(_T("Apply"));
			bEnableControl = FALSE;
		}	break;

		case require_system_insert:
		{
			strOkButton.Format(_T("Insert"));
		}	break;
		case require_system_modify:
		case require_device_modify:
		{
			strOkButton.Format(_T("Change"));
		}	break;

		default:
		{
			strOkButton.Format(_T("Ok"));
		}	break;
		}

		SetDlgItemText(IDOK, strOkButton);
		//SetControlEnableWindow(bEnableControl);
	}

	InitControlTextSize();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgDeviceTotalConfiguration::CreateNamingWizard()
{
	CComboBox* pCmdMainZone = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_MAIN_ZONE));
	CComboBox* pCmdSubZone = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_SUB_ZONE));
	CComboBox* pCmdStation = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_STATION));
	CComboBox* pCmdRobot = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_ROBOT));
	CComboBox* pCmdTool = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_TOOL));

	CString strTemp = _T("-");

	if (1)
	{
		pCmdMainZone->AddString(strTemp);
		pCmdSubZone->AddString(strTemp);
		pCmdStation->AddString(strTemp);
		pCmdRobot->AddString(strTemp);
		pCmdTool->AddString(strTemp);
	}

	for (INT nIndex = _T('A'); nIndex <= _T('Z'); nIndex++)
	{
		strTemp.Format(_T("%c"), nIndex);
		pCmdMainZone->AddString(strTemp);
		pCmdSubZone->AddString(strTemp);
	}

	pCmdMainZone->SetCurSel(0);
	pCmdSubZone->SetCurSel(0);

	for (INT nIndex = 0; nIndex < 1000; nIndex++)
	{
		strTemp.Format(_T("%03d"), nIndex);
		pCmdStation->AddString(strTemp);
	}

	pCmdStation->SetCurSel(0);

	for (INT nIndex = 0; nIndex < 100; nIndex++)
	{
		strTemp.Format(_T("%02d"), nIndex);
		pCmdRobot->AddString(strTemp);
		pCmdTool->AddString(strTemp);
	}

	pCmdRobot->SetCurSel(0);
	pCmdTool->SetCurSel(0);
}


void CDlgDeviceTotalConfiguration::OnEnChangeEditSystem()
{	const INT nDelimeter = 20;

	CWnd* pWnd = GetDlgItem(IDC_EDIT_SYSTEM);

	//pWnd->GetWindowText(m_strSystemName);

	ShowNamingWizard(FALSE, m_bProgramStart);
	ShowWizardButtonOK();
}


void CDlgDeviceTotalConfiguration::OnEnChangeEditDeviceId()
{
	INT nDeviceID = -1;
	
	CWnd* pWnd = GetDlgItem(IDC_EDIT_DEVICE_ID);

	pWnd->GetWindowText(m_strDeviceID);

	ShowWizardButtonOK();
}


void CDlgDeviceTotalConfiguration::ShowWizardButtonOK()
{
	SetTimer(TIMER_SHOW_WIZARD_BUTTON, 100, NULL);
}


void CDlgDeviceTotalConfiguration::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_SHOW_WIZARD_BUTTON:
	{
		KillTimer(nIDEvent);
		
		INT nCmdShow = SW_SHOWNORMAL;

		CString strEditName;
		
		GetDlgItemText(IDC_EDIT_SYSTEM, strEditName);

		const INT nLength = strEditName.GetLength();

		if (0 == nLength || 20 < nLength)
		{
			nCmdShow = SW_HIDE;
		}
		else
		{
			if (TRUE == IsChangeDeviceInformation())
			{

			}
			
			else
			{
				nCmdShow = SW_HIDE;
			}
		}
		
		//ShowNamingWizard(nLength == 0 ? false : true, m_bProgramStart);
		(GetDlgItem(IDOK))->ShowWindow(nCmdShow);
		m_bProgramStart = FALSE;

	}	break;


	default:
		break;
	}


	CDialogEx::OnTimer(nIDEvent);
}


BOOL CDlgDeviceTotalConfiguration::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT id = (UINT)LOWORD(wParam);
	ASSERT(GetDlgItem(id));

	switch (id)
	{
	case IDC_CHECK_ACTIVE_WIZARD:
	{
		BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_ACTIVE_WIZARD))->GetCheck();
		ShowNamingWizard(bCheck, m_bProgramStart);
		EnableGenerateBtn();
	}	break;

	case IDC_BTN_GENERATE:
	{
		OnBtnClickedGenerate();
		((CButton*)GetDlgItem(IDC_CHECK_ACTIVE_WIZARD))->SetCheck(FALSE);
	}	break;

	case IDC_BTN_DEFAULT:
	{
		OnBtnClickedDefault();
	}	break;

	case IDC_BTN_LOAD:
	{
		OnBtnClickedLoad();
	}	break;

	case IDC_BTN_SAVE:
	{
		OnBtnClickedSave();
	}	break;

	
	case IDC_CMB_MAIN_ZONE :
	case IDC_CMB_SUB_ZONE:
	case IDC_CMB_STATION:
	case IDC_CMB_ROBOT:
	case IDC_CMB_TOOL:
	{
		/*BOOL bEnable = ((CComboBox*)GetDlgItem(id))->GetCurSel() == 0 ? FALSE : TRUE;
		(GetDlgItem(IDC_BTN_GENERATE))->EnableWindow(bEnable);*/
		EnableGenerateBtn();
	}
		break;

	case IDOK:
	{
		GetDlgItemText(IDC_EDIT_SYSTEM, m_strSystemName);
		/*if (bEnable)
		{
			m_strSystemName = strEditName;
		}*/
		m_pDevice->SetEditSystemName(m_strSystemName);
		m_pDevice->SetEditDeviceID(_ttoi(m_strDeviceID));
		m_pDevice->SetEditMotorCount(m_nMotorCount);
		m_pDevice->SetEditHomingTimeout(m_nHomingTimeout = 60);
		m_pDevice->SetEditSystemImage(m_bmpImage);
	}	break;

	default:
		break;
	}

	return CDialogEx::OnCommand(wParam, lParam);
}


void CDlgDeviceTotalConfiguration::OnBtnClickedGenerate()
{
	CComboBox* pCmdMainZone = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_MAIN_ZONE));
	CComboBox* pCmdSubZone = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_SUB_ZONE));
	CComboBox* pCmdStation = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_STATION));
	CComboBox* pCmdRobot = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_ROBOT));
	CComboBox* pCmdTool = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_TOOL));

	BOOL bAutoComplete = TRUE;

	CString strTemp, strProjectName;
	GetDlgItemText(IDC_CMB_MAIN_ZONE, strProjectName);

	GetDlgItemText(IDC_CMB_SUB_ZONE, strTemp);
	strProjectName += strTemp;

	GetDlgItemText(IDC_CMB_STATION, strTemp);
	if (bAutoComplete)
	{
		strTemp.Format(_T("%03d"), _ttoi(strTemp));
		SetDlgItemText(IDC_CMB_STATION, strTemp);
	}
	strProjectName += strTemp;
	
	strProjectName += _T("R");

	GetDlgItemText(IDC_CMB_ROBOT, strTemp);
	if (bAutoComplete)
	{
		strTemp.Format(_T("%02d"), _ttoi(strTemp));
		SetDlgItemText(IDC_CMB_ROBOT, strTemp);
	}
	strProjectName += strTemp;

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


void CDlgDeviceTotalConfiguration::CreateConboMotorNum()
{
	CComboBox* pCmdMotor = static_cast<CComboBox*> (GetDlgItem(IDC_COMBO_MOTOR));
	CString strTemp;
	for (INT nIndex = 1; nIndex < 9; nIndex++)
	{
		strTemp.Format(_T("%d"), nIndex);
		pCmdMotor->AddString(strTemp);
	}
	
	pCmdMotor->SetCurSel(m_nMotorCount - 1);
}


void CDlgDeviceTotalConfiguration::OnCbnSelchangeComboMotor()
{
	INT nIndex = ((CComboBox*)GetDlgItem(IDC_COMBO_MOTOR))->GetCurSel();
	m_nMotorCount = nIndex + 1;

	SetTimer(TIMER_SHOW_WIZARD_BUTTON, 100, NULL);
}

void CDlgDeviceTotalConfiguration::OnEnChangeEditTimeout()
{
	m_nHomingTimeout = GetDlgItemInt(IDC_EDIT_TIMEOUT, FALSE);
}


HBITMAP CDlgDeviceTotalConfiguration::GetEditSystemImage()
{
	ASSERT(m_pDevice);
	CString str;
	m_pDevice->GetAddress(str);
	CBitmap* pBitmap = m_pDevice->GetEditSystemImage();
	
	return *pBitmap;
}


void CDlgDeviceTotalConfiguration::OnBtnClickedDefault()
{
	CDataBaseConfig::GetInstance()->LoadNoImage(&m_bmpImage);
	m_wndViewer.LoadMainImage(&m_bmpImage);
}


void CDlgDeviceTotalConfiguration::OnBtnClickedLoad()
{
	CString strFilter(_T("Image Files|*.BMP;*.GIF;*.JPG;*.PNG;*.TIF||"));
	CFileDialog dlg(TRUE, NULL, NULL, OFN_EXPLORER, strFilter);

	if (IDOK == dlg.DoModal())
	{
		CImage image;
		if (S_OK == image.Load(dlg.GetPathName()))
		{
			if (m_bmpImage.GetSafeHandle())
			{
				m_bmpImage.Detach();
			}

			m_bmpImage.Attach(image.Detach());
			m_wndViewer.LoadMainImage(&m_bmpImage);
		}
	}
}


void CDlgDeviceTotalConfiguration::OnBtnClickedSave()
{
	// TODO: 여기에 구현 코드 추가.
}


void CDlgDeviceTotalConfiguration::ShowNamingWizard(INT nShow, BOOL bProgramStart)
{
	CComboBox* pCmdMainZone = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_MAIN_ZONE));
	CComboBox* pCmdSubZone = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_SUB_ZONE));
	CComboBox* pCmdStation = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_STATION));
	CComboBox* pCmdRobot = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_ROBOT));
	CComboBox* pCmdTool = static_cast<CComboBox*> (GetDlgItem(IDC_CMB_TOOL));
	
	//Naming Wizard 부분
	GetDlgItem(IDC_BTN_GENERATE)->EnableWindow(bProgramStart ? FALSE: nShow);
	pCmdMainZone->EnableWindow(bProgramStart ? FALSE : nShow);
	pCmdSubZone->EnableWindow(bProgramStart ? FALSE : nShow);
	pCmdStation->EnableWindow(bProgramStart ? FALSE : nShow);
	pCmdRobot->EnableWindow(bProgramStart ? FALSE : nShow);
	pCmdTool->EnableWindow(bProgramStart ? FALSE : nShow);

	// 사용자 Naiming 부분
	GetDlgItem(IDC_EDIT_SYSTEM)->EnableWindow(bProgramStart ? TRUE : !nShow);
	
	if (1) 
	{
		CString strEditName;
		CString strCheck;
		CString strMainZone, strSubZone, strStation, strRobot, strTool;
		CString strCheckStation;
		CString strCheckRobot;
		CString strCheckTool;

		INT nIndex;
		INT nIndexMainZone;
		INT nIndexSubZone;
		INT nIndexStation;
		INT nIndexRobot;
		INT nIndexTool;

		BOOL bRule = FALSE;

		GetDlgItemText(IDC_EDIT_SYSTEM, strEditName);
		GetDlgItemText(IDC_CMB_MAIN_ZONE, strMainZone);
		GetDlgItemText(IDC_CMB_SUB_ZONE, strSubZone);
		GetDlgItemText(IDC_CMB_STATION, strStation);
		GetDlgItemText(IDC_CMB_ROBOT, strRobot);
		GetDlgItemText(IDC_CMB_TOOL, strTool);

		strEditName.MakeUpper();
		INT nLength = strEditName.GetLength();
		
		if (11 == strEditName.GetLength())
		{
			strCheck = strEditName.Left(1);
			if (_T("A") <= strCheck && _T("Z") >= strCheck)
			{
				nIndexMainZone = strCheck.GetAt(0) - 'A';

				strCheck = strEditName.Mid(1, 1);
				if (_T("A") <= strCheck && _T("Z") >= strCheck)
				{
					nIndexSubZone = strCheck.GetAt(0) - 'A';

					strCheck = strEditName.Mid(2, strEditName.GetLength() - 2);
					nIndex = strCheck.Find(_T("R"));
					if (-1 != nIndex && nIndex == 3) // IDC_CMB_ROBOT 찾기 && Station 번호가 무조건 3자리(nIndex)어야한다.
					{
						strCheckStation = strCheck.Left(nIndex);
						nIndexStation = _ttoi(strCheckStation);

						if (0 <= nIndexStation && 999 >= nIndexStation)
						{
							strCheck = strCheck.Mid(nIndex + 1, strCheck.GetLength() - (nIndex + 1));

							nIndex = strCheck.Find(_T("B"));
							strCheckRobot = strCheck.Left(nIndex);
							nIndexRobot = _ttoi(strCheckRobot);

							if ((0 <= nIndexRobot && 99 >= nIndexRobot) && (nIndex == 2)) // IDC_CMD_BOARD 찾기 && Robot 번호가 무조건 2자리(Index)어야 한다.
							{
								strCheck = strCheck.Mid(nIndex + 1, strCheck.GetLength() - (nIndex + 1));
								nIndex = strCheck.GetLength();
								nIndexTool = _ttoi(strCheck);
								if (0 <= nIndexTool && 99 >= nIndexTool && nIndex == 2)
								{
									pCmdMainZone->SetCurSel(nIndexMainZone + 1);
									pCmdSubZone->SetCurSel(nIndexSubZone + 1);
									pCmdStation->SetCurSel(nIndexStation + 1);
									pCmdRobot->SetCurSel(nIndexRobot + 1);
									pCmdTool->SetCurSel(nIndexTool + 1);
									bRule = TRUE;
								}
							}
						}
					}
				}
			}
		}
		

		if (!bRule)
		{
			pCmdMainZone->SetCurSel(0);
			pCmdSubZone->SetCurSel(0);
			pCmdStation->SetCurSel(0);
			pCmdRobot->SetCurSel(0);
			pCmdTool->SetCurSel(0);
		}
		
	}

}


void CDlgDeviceTotalConfiguration::SetTotalConfig(BOOL bTotal)
{// System Configuration에서 호출 여부..
	m_bTotalConfig = bTotal;
}


const BOOL CDlgDeviceTotalConfiguration::IsTotalConfig()
{
	return m_bTotalConfig;
}


void CDlgDeviceTotalConfiguration::EnableGenerateBtn()
{
	BOOL bEnable = TRUE;
	for (INT i = 0; i < 5; i++) bEnable &= ((CComboBox*)GetDlgItem(IDC_CMB_MAIN_ZONE + i))->GetCurSel() == 0 ? FALSE : TRUE;
	bEnable &= ((CButton*)GetDlgItem(IDC_CHECK_ACTIVE_WIZARD))->GetCheck();

	GetDlgItem(IDC_BTN_GENERATE)->EnableWindow(bEnable);
}


void CDlgDeviceTotalConfiguration::SetTitleText(CString str)
{
	m_strTitle = str;
}


CString CDlgDeviceTotalConfiguration::GetTitleText()
{
	return m_strTitle;
}


void CDlgDeviceTotalConfiguration::SetConfigMethod(INT nState)
{
	m_nSelectedMethod = nState;
}


INT CDlgDeviceTotalConfiguration::GetConfigMethod()
{
	return m_nSelectedMethod;
}


void CDlgDeviceTotalConfiguration::SetControlEnableWindow(BOOL bEnable)
{
	GetDlgItem(IDC_EDIT_SYSTEM)->EnableWindow(bEnable);
	//GetDlgItem(IDC_COMBO_MOTOR)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_ACTIVE_WIZARD)->EnableWindow(bEnable);
	GetDlgItem(IDC_CMB_MAIN_ZONE)->EnableWindow(bEnable);
	GetDlgItem(IDC_CMB_SUB_ZONE)->EnableWindow(bEnable);
	GetDlgItem(IDC_CMB_STATION)->EnableWindow(bEnable);
	GetDlgItem(IDC_CMB_ROBOT)->EnableWindow(bEnable);
	GetDlgItem(IDC_CMB_TOOL)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTN_GENERATE)->EnableWindow(bEnable);
}


const BOOL CDlgDeviceTotalConfiguration::IsNoticeConfig()
{
	return m_bNoticeConfig;
}


void CDlgDeviceTotalConfiguration::SetNoticeConfig(BOOL bNotice)
{
	m_bNoticeConfig = bNotice;
}


const BOOL CDlgDeviceTotalConfiguration::IsInformChange(CString strEditName)
{
	BOOL bResult = TRUE;
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
					if (pDevice->GetDatabaseSystemName() == strEditName)
					{
						bResult = FALSE;
						
						delete pDevice;
						break;
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
	}

	pProject->CloseDataBase();
	
	return bResult;
}


const BOOL CDlgDeviceTotalConfiguration::IsChangeDeviceInformation()
{
	CString strEditName;
	GetDlgItemText(IDC_EDIT_SYSTEM, strEditName);
	INT nIndex = GetEditMotorIndex();

	//BOOL bEnable = TRUE;

	if (-1 == GetEditMotorIndex())
	{
		return FALSE;
	}

	if (require_system_modify == GetConfigMethod())
	{ // 통합 설정화면에서 선택 항목 Modify 진행할 경우
		/*
			프로젝트명 수정시 Tooling List에 해당 프로젝트명이 없어야한다.
		*/
		if (m_strSystemName != strEditName)
		{
			if (TRUE == IsExistInTotalConfigurationList(strEditName))
			{ //Edit이름이 Tooling List에 존재하는 경우
				return FALSE;
			}
		}
	}

	else if (require_system_insert == GetConfigMethod())
	{ // 통합 설정화면에서 항목 신규 등록할 경우
		
		/*
			※ 신규 등록하고자 할때 이미 db에 있는경우.. 등록 할 수 없도록 제어.
				1. EditControl의 이름이 db(or 통합 설정 화면 ListControl)에 정의되어 있지 않아야 한다.
		*/
		if (TRUE == IsExistInTotalConfigurationList(strEditName))
		{//Edit이름이 Total Configuration 파일에 존재하는 경우
			return FALSE;
		}
		
	}

	else if (require_notice_modify == GetConfigMethod())
	{ // 알림 화면을 통한 설정화면 수정할 경우
		if (m_strSystemName != strEditName)
		{
			if (TRUE == IsDeviceExist(strEditName))
			{ // 현재 DeviceList에 같은 이름의 장치가 등록되어있어 사용중 or 사용 준비중에 있다면 해당 이름으로 변경할 수 없다.
				return FALSE;
			}
		}
	}

	else if (require_device_modify == GetConfigMethod())
	{ // 연결된 장치에 대한 설정값 변경할 경우
		if (m_strSystemName != strEditName)
		{
			if (TRUE == IsDeviceExist(strEditName))
			{ // 현재 DeviceList에 같은 이름의 장치가 등록되어있어 사용중 or 사용 준비중에 있다면 해당 이름으로 변경할 수 없다.
				return FALSE;
			}
		}
	}
		

	return TRUE;
}


const BOOL CDlgDeviceTotalConfiguration::IsDeviceExist(CString strName)
{
	CDeviceMotionGate* pDevice = NULL;
	CDeviceList* pDeviceList = CDeviceList::GetInstance();

	
	pDeviceList->GetMotiongateByName(strName, pDevice);
	
	if (pDevice != NULL) return TRUE;

	return FALSE;
}


void CDlgDeviceTotalConfiguration::SetTotalConfigurationList(CString strName)
{
	m_configList.AddTail(strName);
}


const BOOL CDlgDeviceTotalConfiguration::IsExistInTotalConfigurationList(CString strName)
{
	POSITION pos = m_configList.GetHeadPosition();

	while (pos)
	{
		CString str = m_configList.GetNext(pos);
		if (str == strName)
		{
			return TRUE;
		}
	}
	
	return FALSE;
}


INT CDlgDeviceTotalConfiguration::GetEditMotorIndex()
{
	INT nIndex = ((CComboBox*)GetDlgItem(IDC_COMBO_MOTOR))->GetCurSel();
	return nIndex;
}


void CDlgDeviceTotalConfiguration::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}


void CDlgDeviceTotalConfiguration::InitControlTextSize()
{
	if (true)
	{
		LOGFONT lf = { 0, };
		afxGlobalData.fontRegular.GetLogFont(&lf);

		lf.lfHeight = -15;
		m_BigFont.CreateFontIndirect(&lf);

		GetDlgItem(IDC_BTN_DEFAULT)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_LOAD)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_GENERATE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_GROUP_WIZARD)->SetFont(&m_BigFont);
		GetDlgItem(IDC_CHECK_ACTIVE_WIZARD)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_MAIN_ZONE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_SUB_ZONE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_STATION)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_ROBOT)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_TOOL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_R)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_B)->SetFont(&m_BigFont);
		GetDlgItem(IDC_CMB_MAIN_ZONE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_CMB_SUB_ZONE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_CMB_STATION)->SetFont(&m_BigFont);
		GetDlgItem(IDC_CMB_ROBOT)->SetFont(&m_BigFont);
		GetDlgItem(IDC_CMB_TOOL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_SYSTEM)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_MOTOR)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_TIMEOUT)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_ID)->SetFont(&m_BigFont);
		GetDlgItem(IDC_EDIT_SYSTEM)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_MAX_CHARACTERS)->SetFont(&m_BigFont);
		GetDlgItem(IDC_COMBO_MOTOR)->SetFont(&m_BigFont);
		GetDlgItem(IDC_EDIT_TIMEOUT)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_SEC)->SetFont(&m_BigFont);
		GetDlgItem(IDC_EDIT_DEVICE_ID)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_VALUE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_RECOMMEND)->SetFont(&m_BigFont);
		GetDlgItem(IDOK)->SetFont(&m_BigFont);
		GetDlgItem(IDCANCEL)->SetFont(&m_BigFont);


		m_BigFont.Detach();
	}

}
