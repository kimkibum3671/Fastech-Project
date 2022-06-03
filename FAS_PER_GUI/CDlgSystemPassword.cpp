// CDlgSystemPassword.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgSystemPassword.h"
#include "afxdialogex.h"
#include "CDataBaseManager.h"
#include "CPageConfigDeviceControlAuthorize.h"

// CDlgSystemPassword 대화 상자

IMPLEMENT_DYNAMIC(CDlgSystemPassword, CDialogEx)

CDlgSystemPassword::CDlgSystemPassword(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDP_CONFIG_DEVICE_PASSWORD, pParent)
{
	m_pParent = pParent;
}

CDlgSystemPassword::~CDlgSystemPassword()
{
}

void CDlgSystemPassword::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSystemPassword, CDialogEx)
END_MESSAGE_MAP()


// CDlgSystemPassword 메시지 처리기


BOOL CDlgSystemPassword::GetLoadImage(CString strName)
{
	CDataBaseConfig* pConfig = CDataBaseConfig::GetInstance();
	if (FALSE == pConfig->LoadUserImage(strName, &m_Image))
	{
		return FALSE;
	}

	m_wndImage.SubclassDlgItem(IDC_STATIC_USER, this);
	m_wndImage.LoadMainImage(&m_Image);
	
	SetDlgItemText(IDC_STATIC_USER_NAME, m_strName);

	return TRUE;
}


BOOL CDlgSystemPassword::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetLoadImage(GetController());

	GetLoadPassword(GetController());

	GetLoadChangedPassword(GetChangedPassword());

	InitControlTextSize();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgSystemPassword::SetController(CString strName)
{
	m_strName = strName;
}

CString CDlgSystemPassword::GetController()
{
	return m_strName;
}


const BOOL CDlgSystemPassword::GetLoadPassword(CString strName)
{
	CString strPassword;
	
	CDataBaseConfig* pConfig = CDataBaseConfig::GetInstance();
		
	if (FALSE == pConfig->LoadUserPassword(strName, strPassword))
	{
		return FALSE;
	}

	SetDlgItemText(IDC_EDIT_CURRENT_PW, strPassword);
	GetDlgItem(IDC_EDIT_CURRENT_PW)->EnableWindow(FALSE);
	
	return TRUE;
}


void CDlgSystemPassword::GetChangePassword()
{
	PASSWORD_NODE node;
	CPageConfigDeviceControlAuthorize* pConfigOthers = (CPageConfigDeviceControlAuthorize*)m_pParent;
	
	node.strName = GetController();
	GetDlgItemText(IDC_EDIT_CHANGE_PW, node.strPassword);
	node.bChange = TRUE;
	
	pConfigOthers->SetAuthorizedPassword(node);
}


void CDlgSystemPassword::OnOK()
{
	GetChangePassword();
	CDialogEx::OnOK();
}


void CDlgSystemPassword::SetChangedPassword(CString strPassword)
{
	m_strChangedPassword = strPassword;
}


CString CDlgSystemPassword::GetChangedPassword()
{
	return m_strChangedPassword;
}


void CDlgSystemPassword::GetLoadChangedPassword(CString strPassword)
{ // 비밀번호 변경후 다시 로딩할 경우..
	SetDlgItemText(IDC_EDIT_CHANGE_PW, strPassword);
}


void CDlgSystemPassword::InitControlTextSize()
{
	if (true)
	{
		LOGFONT lf = { 0, };
		afxGlobalData.fontRegular.GetLogFont(&lf);

		lf.lfHeight = -18;
		m_BigFont.CreateFontIndirect(&lf);

		GetDlgItem(IDC_STATIC_GROUP_USER)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_USER)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_USER_NAME)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_CURRENT_PW)->SetFont(&m_BigFont);
		GetDlgItem(IDC_EDIT_CURRENT_PW)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_CHANGE_PW)->SetFont(&m_BigFont);
		GetDlgItem(IDC_EDIT_CHANGE_PW)->SetFont(&m_BigFont);
		GetDlgItem(IDOK)->SetFont(&m_BigFont);
		GetDlgItem(IDCANCEL)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}

	/*if (true)
	{
		m_BigFont.CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, TEXT_STYLE);

		GetDlgItem(IDC_STATIC_GROUP_USER)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_USER)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_USER_NAME)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_CURRENT_PW)->SetFont(&m_BigFont);
		GetDlgItem(IDC_EDIT_CURRENT_PW)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_CHANGE_PW)->SetFont(&m_BigFont);
		GetDlgItem(IDC_EDIT_CHANGE_PW)->SetFont(&m_BigFont);
		GetDlgItem(IDOK)->SetFont(&m_BigFont);
		GetDlgItem(IDCANCEL)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}*/
	

}
