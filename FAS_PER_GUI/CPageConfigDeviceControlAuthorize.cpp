// CPageConfigDeviceControlAuthorize.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageConfigDeviceControlAuthorize.h"
#include "CSheetConfig.h"
#include "CDataBaseManager.h"
#include "MainFrm.h"
#include "CDeviceList.h"
#include "CDlgSystemPassword.h"

// CPageConfigDeviceControlAuthorize

IMPLEMENT_DYNAMIC(CPageConfigDeviceControlAuthorize, CMFCPropertyPage)

CPageConfigDeviceControlAuthorize::CPageConfigDeviceControlAuthorize()
	:CMFCPropertyPage(IDP_CONFIG_DEVICE_CONTROL_AUTHORIZE)
{
	memset(&m_Admin, 0, sizeof(WORD));
	memset(&m_Engineer, 0, sizeof(WORD));
	memset(&m_Operator, 0, sizeof(WORD));
	memset(&m_Guest, 0, sizeof(WORD));

	m_AdminPassword.strName = _T("Administrator");
	m_EngineerPassword.strName = _T("Engineer");
	m_OperatorPassword.strName = _T("Operator");
	m_GuestPassword.strName = _T("Guest");
}

CPageConfigDeviceControlAuthorize::~CPageConfigDeviceControlAuthorize()
{
}


BEGIN_MESSAGE_MAP(CPageConfigDeviceControlAuthorize, CMFCPropertyPage)
END_MESSAGE_MAP()



// CPageConfigDeviceControlAuthorize 메시지 처리기




CPageConfigDeviceControlAuthorize* CPageConfigDeviceControlAuthorize::GetInstance()
{
	static CPageConfigDeviceControlAuthorize _instance;
	return &_instance;
}


void CPageConfigDeviceControlAuthorize::OnOK()
{
	if (FALSE == SaveLifeCountData())
	{
		ASSERT(0);
	}

	if (FALSE == SaveControlDataBase())
	{
		ASSERT(0);
	}

	if (FALSE == SavePassword())
	{
		ASSERT(0);
	}

	CMFCPropertyPage::OnOK();
}


BOOL CPageConfigDeviceControlAuthorize::SaveLifeCountData()
{
	CString strStandard;
	CString strCycle;
	
	CDataBaseConfig* pProjectDB = CDataBaseConfig::GetInstance();
	CDeviceMotionGate* pDevice = NULL;
	
	// 일괄 수정 필요.
	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	GetDlgItemText(IDC_EDIT_LIFECOUNT_STANDARD, strStandard);
	GetDlgItemText(IDC_EDIT_LIFECOUNT_CYCLE, strCycle);

	for (INT i = 0; i < pDeviceList->GetAddressCount(); i++)
	{
		const DWORD dwAddress = pDeviceList->GetAddressByIndex(i);
		CDeviceMotionGate* pDevice = NULL;
		pDeviceList->GetMotiongateByAddress(dwAddress, pDevice);

		if (pProjectDB->SetLifeCountStandardAlarmValue(_ttoi(strStandard)))
		{
			pDevice->SetLifeCountStandard((DWORD)_ttoi(strStandard));
		}
		else
		{
			AfxMessageBox(_T("SetLifeCountStandardAlarmValue Funtion is Fail"));
			return FALSE;
		}

		if (pProjectDB->SetLifeCountCycleAlarmValue(_ttoi(strCycle)))
		{
			pDevice->SetLifeCountCycle((DWORD)_ttoi(strCycle));
		}
		else
		{
			AfxMessageBox(_T("SetLifeCountStandardAlarmValue Funtion is Fail"));
			return FALSE;
		}
	}
	/*CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	pDevice = pFrame->GetSelectedMotiongateCtrl();*/

	return TRUE;
}


void CPageConfigDeviceControlAuthorize::LoadConfigurationValue()
{
	CDataBaseConfig* pProjectDB = CDataBaseConfig::GetInstance();
	pProjectDB->LoadTotalSystemInform(); // 다시한번 읽어들임.

	INT nStandard = pProjectDB->GetLifeCountStandardValue();
	INT nCycle = pProjectDB->GetLifeCountCycleValue();

	CString strStandard;
	CString strCycle;

	strStandard.Format(_T("%d"), nStandard);
	strCycle.Format(_T("%d"), nCycle);
	
	SetDlgItemText(IDC_EDIT_LIFECOUNT_STANDARD, strStandard);
	SetDlgItemText(IDC_EDIT_LIFECOUNT_CYCLE, strCycle);
}


BOOL CPageConfigDeviceControlAuthorize::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();
	
	LOGFONT lf = { 0, };
	afxGlobalData.fontRegular.GetLogFont(&lf);

	lf.lfHeight = -15;
	m_BigFont.CreateFontIndirect(&lf);

	LoadConfigurationValue();

	LoadFromDataBase();

	InitButtonCtrl();

	SetSettingButtonImage();

	GetLoadImage();

	InitControlTextSize();

	m_BigFont.Detach();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CPageConfigDeviceControlAuthorize::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	UINT id = (UINT)LOWORD(wParam);
	
	switch (id)
	{
	case IDC_BUTTON_ADMIN_DEV0:
	case IDC_BUTTON_ADMIN_DEV1:
	case IDC_BUTTON_ADMIN_DEV2:
	case IDC_BUTTON_ADMIN_DEV3:
	case IDC_BUTTON_ADMIN_DEV4:
	case IDC_BUTTON_ADMIN_DEV5:
	case IDC_BUTTON_ADMIN_DEV6:
	case IDC_BUTTON_ADMIN_DEV7:
	case IDC_BUTTON_ADMIN_DEV8:
	{
		CString str;
		INT nID = id - IDC_BUTTON_ADMIN_DEV0;
		
		/*m_Admin.wData ^= (1 << nID);

		GetDlgItemText(id, str);
		SetButtonImage(id, str == _T("OFF"));*/

	}	break;
	case IDC_BUTTON_ENGINEER_DEV0:
	case IDC_BUTTON_ENGINEER_DEV1:
	case IDC_BUTTON_ENGINEER_DEV2:
	case IDC_BUTTON_ENGINEER_DEV3:
	case IDC_BUTTON_ENGINEER_DEV4:
	case IDC_BUTTON_ENGINEER_DEV5:
	case IDC_BUTTON_ENGINEER_DEV6:
	case IDC_BUTTON_ENGINEER_DEV7:
	case IDC_BUTTON_ENGINEER_DEV8:
	{
		CString str;
		INT nID = id - IDC_BUTTON_ENGINEER_DEV0;

		m_Engineer.wData ^= (1 << nID);

		GetDlgItemText(id, str);
		SetButtonImage(id, str == _T("OFF"));

	}	break;
	case IDC_BUTTON_OPERATOR_DEV0:
	case IDC_BUTTON_OPERATOR_DEV1:
	case IDC_BUTTON_OPERATOR_DEV2:
	case IDC_BUTTON_OPERATOR_DEV3:
	case IDC_BUTTON_OPERATOR_DEV4:
	case IDC_BUTTON_OPERATOR_DEV5:
	case IDC_BUTTON_OPERATOR_DEV6:
	case IDC_BUTTON_OPERATOR_DEV7:
	case IDC_BUTTON_OPERATOR_DEV8:
	{
		CString str;
		INT nID = id - IDC_BUTTON_OPERATOR_DEV0;

		m_Operator.wData ^= (1 << nID);

		GetDlgItemText(id, str);
		SetButtonImage(id, str == _T("OFF"));

	}	break;

	case IDC_BUTTON_GUEST_DEV0:
	case IDC_BUTTON_GUEST_DEV1:
	case IDC_BUTTON_GUEST_DEV2:
	case IDC_BUTTON_GUEST_DEV3:
	case IDC_BUTTON_GUEST_DEV4:
	case IDC_BUTTON_GUEST_DEV5:
	case IDC_BUTTON_GUEST_DEV6:
	case IDC_BUTTON_GUEST_DEV7:
	case IDC_BUTTON_GUEST_DEV8:
	{
		CString str;
		INT nID = id - IDC_BUTTON_GUEST_DEV0;

		m_Guest.wData ^= (1 << nID);

		GetDlgItemText(id, str);
		SetButtonImage(id, str == _T("OFF"));

	}	break;

	case IDC_BUTTON_ADMIN_DEV9 :
	case IDC_BUTTON_ENGINEER_DEV9:
	case IDC_BUTTON_OPERATOR_DEV9:
	{
		ChangeSystemPassword(id);
	}	break;


	//case IDC_BUTTON_SETCONFIG:
	//{
	//	/*if (FALSE == SaveDataBase())
	//	{
	//		ASSERT(0);
	//	}*/
	//	;
	//}	break;

	default:
		break;
	}

	//return CMFCPropertyPage::OnCommand(wParam, lParam);
	return TRUE;
}


void CPageConfigDeviceControlAuthorize::LoadFromDataBase()
{
	CDataBaseConfig* pConfig = CDataBaseConfig::GetInstance();
	
	m_Admin.wData = (pConfig->GetAuthorizeControl(_T("Administrator")));

	m_Engineer.wData = (pConfig->GetAuthorizeControl(_T("Engineer")));

	m_Operator.wData = (pConfig->GetAuthorizeControl(_T("Operator")));

	m_Guest.wData = (pConfig->GetAuthorizeControl(_T("Guest")));
}

void CPageConfigDeviceControlAuthorize::InitButtonCtrl()
{
	BOOL bEnable = FALSE;
	for (INT i = 0; i < BTN_RANGE_MAX; i++)
	{
		bEnable = m_Admin.wData >> i & 1;
		SetButtonImage(IDC_BUTTON_ADMIN_DEV0 + i, bEnable);

		bEnable = m_Engineer.wData >> i & 1;
		SetButtonImage(IDC_BUTTON_ENGINEER_DEV0 + i, bEnable);

		bEnable = m_Operator.wData >> i & 1;
		SetButtonImage(IDC_BUTTON_OPERATOR_DEV0 + i, bEnable);

		bEnable = m_Guest.wData >> i & 1;
		SetButtonImage(IDC_BUTTON_GUEST_DEV0 + i, bEnable);
	}
	
}

void CPageConfigDeviceControlAuthorize::SetSettingButtonImage()
{
	INT nPassWord[] =
	{
		IDC_BUTTON_ADMIN_DEV9,
		IDC_BUTTON_ENGINEER_DEV9,
		IDC_BUTTON_OPERATOR_DEV9,
		IDC_BUTTON_GUEST_DEV9,
	};

	CMFCButton* pBtn;

	for (INT nID = 0; nID < 4; nID++)
	{
		pBtn = static_cast<CMFCButtonEx*>(GetDlgItem(nPassWord[nID]));
		pBtn->SetImage(IDB_BITMAP_PASSWORD);
		pBtn->SetFont(&m_BigFont);
	}
}


const BOOL CPageConfigDeviceControlAuthorize::SaveControlDataBase()
{
	CDataBaseConfig* pConfig = CDataBaseConfig::GetInstance();

	if (FALSE == pConfig->SetAuthorizeControl(_T("Administrator"), m_Admin)) return FALSE;
	
	if (FALSE == pConfig->SetAuthorizeControl(_T("Engineer"), m_Engineer)) return FALSE;

	if (FALSE == pConfig->SetAuthorizeControl(_T("Operator"), m_Operator)) return FALSE;

	if (FALSE == pConfig->SetAuthorizeControl(_T("Guest"), m_Guest)) return FALSE;

	return TRUE;
}


void CPageConfigDeviceControlAuthorize::GetLoadImage()
{
	CDataBaseConfig* pConfig = CDataBaseConfig::GetInstance();
	if (FALSE == pConfig->LoadUserImage(_T("Guest"), &m_bmpGuest) ||
		FALSE == pConfig->LoadUserImage(_T("Operator"), &m_bmpOperator) ||
		FALSE == pConfig->LoadUserImage(_T("Engineer"), &m_bmpEngineer) ||
		FALSE == pConfig->LoadUserImage(_T("Administrator"), &m_bmpAdministrator))
	{
		return;
	}

	if (true)
	{
		m_wndAdminImage[0].SubclassDlgItem(IDC_STATIC_ADMIN_DEV0, this);
		m_wndAdminImage[1].SubclassDlgItem(IDC_STATIC_ADMIN_DEV1, this);

		m_wndEngineerImage[0].SubclassDlgItem(IDC_STATIC_ENGINEER_DEV0, this);
		//m_wndEngineerImage[1].SubclassDlgItem(IDC_STATIC_ENGINEER_DEV1, this);

		m_wndOperatorImage[0].SubclassDlgItem(IDC_STATIC_OPERATOR_DEV0, this);
		//m_wndOperatorImage[1].SubclassDlgItem(IDC_STATIC_OPERATOR_DEV1, this);

		m_wndGuestImage[0].SubclassDlgItem(IDC_STATIC_GUEST_DEV0, this);
		//m_wndGuestImage[1].SubclassDlgItem(IDC_STATIC_GUEST_DEV1, this);
	}

	if (true)
	{
		m_wndAdminImage[0].LoadMainImage(&m_bmpAdministrator);
		//m_wndAdminImage[1].LoadMainImage(&m_bmpAdministrator);

		m_wndEngineerImage[0].LoadMainImage(&m_bmpEngineer);
		//m_wndEngineerImage[1].LoadMainImage(&m_bmpEngineer);

		m_wndOperatorImage[0].LoadMainImage(&m_bmpOperator);
		//m_wndOperatorImage[1].LoadMainImage(&m_bmpOperator);

		m_wndGuestImage[0].LoadMainImage(&m_bmpGuest);
		//m_wndGuestImage[1].LoadMainImage(&m_bmpGuest);
	}

}


void CPageConfigDeviceControlAuthorize::SetButtonImage(INT nID, BOOL bCheck)
{
	CMFCButton* pBtn = static_cast<CMFCButtonEx*>(GetDlgItem(nID));
	pBtn->SetImage(bCheck ? IDB_CHECK : IDB_UNCHECK);
	pBtn->SetWindowText(bCheck ? _T("ON") : _T("OFF"));
	pBtn->SetFont(&m_BigFont);
}


BOOL CPageConfigDeviceControlAuthorize::ChangeSystemPassword(INT nID)
{
	PASSWORD_NODE node;
	
	switch (nID)
	{
	case IDC_BUTTON_ADMIN_DEV9:
	{
		node = m_AdminPassword;
	}	break;
	case IDC_BUTTON_ENGINEER_DEV9:
	{
		node = m_EngineerPassword;
	}	break;
	case IDC_BUTTON_OPERATOR_DEV9:
	{
		node = m_OperatorPassword;
	}	break;

	case IDC_BUTTON_GUEST_DEV9:
	{
		node = m_GuestPassword;
	}	break;

	default:
		break;
	}

	if (_T("") == node.strName)
	{
		return FALSE;
	}

	CDlgSystemPassword dlg(this);
	dlg.SetController(node.strName);
	dlg.SetChangedPassword(node.strPassword);// 반일 비밀번호 변경후 다시 수정하고 싶으면 진행한다.

	if (IDOK == dlg.DoModal())
	{
				
	}
	else
	{

	}

	return 0;
}


void CPageConfigDeviceControlAuthorize::SetAuthorizedPassword(PASSWORD_NODE node)
{
	PASSWORD_NODE nodeTest = node;

	if (node.strName == _T("Administrator")) m_AdminPassword = node; //m_AdminPassword.strPassword = node.strPassword;

	if (node.strName == _T("Engineer"))  m_EngineerPassword = node;// m_EngineerPassword.strPassword = node.strPassword;

	if (node.strName == _T("Operator")) m_OperatorPassword = node; //m_OperatorPassword.strPassword = node.strPassword;

	if (node.strName == _T("Guest")) m_GuestPassword = node; //m_GuestPassword.strPassword = node.strPassword;
}


BOOL CPageConfigDeviceControlAuthorize::SavePassword()
{
	if (FALSE == SavePasswordToDataBase(m_AdminPassword)) return FALSE;
	
	if (FALSE == SavePasswordToDataBase(m_EngineerPassword)) return FALSE;

	if (FALSE == SavePasswordToDataBase(m_OperatorPassword)) return FALSE;

	if (FALSE == SavePasswordToDataBase(m_GuestPassword)) return FALSE;

	return TRUE;
}


const BOOL CPageConfigDeviceControlAuthorize::SavePasswordToDataBase(PASSWORD_NODE node)
{
	if (node.bChange)
	{
		CDataBaseConfig* pProjectDB = CDataBaseConfig::GetInstance();

		if (FALSE == pProjectDB->ChangePassword(node)) return FALSE;
	}

	return TRUE;
}




void CPageConfigDeviceControlAuthorize::InitControlTextSize()
{
	GetDlgItem(IDC_STATIC_ADMIN)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_ENGINEER)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_OPERATOR)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_GUEST)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_BACKUP)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_BACKUP_MANAGE)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_RESTORE)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_COMPARE)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_CONFIG)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_SCHEDULE)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_ASSIGN)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_RESET)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_SETTING)->SetFont(&m_BigFont);
}
