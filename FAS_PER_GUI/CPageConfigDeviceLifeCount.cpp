// CPageConfigDeviceLifeCount.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageConfigDeviceLifeCount.h"
#include "CDataBaseManager.h"

// CPageConfigDeviceLifeCount

IMPLEMENT_DYNAMIC(CPageConfigDeviceLifeCount, CMFCPropertyPage)

CPageConfigDeviceLifeCount::CPageConfigDeviceLifeCount()
	:CMFCPropertyPage(IDP_CONFIG_DEVICE_LIFE_COUNT)
{

}

CPageConfigDeviceLifeCount::~CPageConfigDeviceLifeCount()
{
}


BEGIN_MESSAGE_MAP(CPageConfigDeviceLifeCount, CMFCPropertyPage)
END_MESSAGE_MAP()



// CPageConfigDeviceLifeCount 메시지 처리기


BOOL CPageConfigDeviceLifeCount::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	LoadConfigurationValue();

	InitControlTextSize();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CPageConfigDeviceLifeCount::LoadConfigurationValue()
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


const BOOL CPageConfigDeviceLifeCount::SaveLifeCountData()
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

	return TRUE;
}


void CPageConfigDeviceLifeCount::OnOK()
{
	if (FALSE == SaveLifeCountData())
	{
		ASSERT(0);
	}

	CMFCPropertyPage::OnOK();
}





void CPageConfigDeviceLifeCount::InitControlTextSize()
{
	if (true)
	{
		LOGFONT lf = { 0, };
		afxGlobalData.fontRegular.GetLogFont(&lf);

		lf.lfHeight = -15;
		m_BigFont.CreateFontIndirect(&lf);

		GetDlgItem(IDC_STATIC_LIFE_COUNT)->SetFont(&m_BigFont);
		GetDlgItem(IDC_EDIT_LIFECOUNT_STAND)->SetFont(&m_BigFont);
		GetDlgItem(IDC_EDIT_LIFECOUNT_PERIOD2)->SetFont(&m_BigFont);
		GetDlgItem(IDC_EDIT_LIFECOUNT_STANDARD)->SetFont(&m_BigFont);
		GetDlgItem(IDC_EDIT_LIFECOUNT_CYCLE)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}

	/*if (true)
	{
	m_BigFont.CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, TEXT_STYLE);

	GetDlgItem(IDC_STATIC_LIFE_COUNT)->SetFont(&m_BigFont);
	GetDlgItem(IDC_EDIT_LIFECOUNT_STAND)->SetFont(&m_BigFont);
	GetDlgItem(IDC_EDIT_LIFECOUNT_PERIOD2)->SetFont(&m_BigFont);
	GetDlgItem(IDC_EDIT_LIFECOUNT_STANDARD)->SetFont(&m_BigFont);
	GetDlgItem(IDC_EDIT_LIFECOUNT_CYCLE)->SetFont(&m_BigFont);

	m_BigFont.Detach();
	}*/
	
}
