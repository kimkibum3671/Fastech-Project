// CPageConfigDeviceMotor.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageConfigDeviceMotor.h"
#include "afxdialogex.h"

#include "CSheetConfigDevice.h"

// CPageConfigDeviceMotor dialog

IMPLEMENT_DYNAMIC(CPageConfigDeviceMotor, CPageConfig)

CPageConfigDeviceMotor::CPageConfigDeviceMotor(CWnd* pParent /*=nullptr*/)
	: CPageConfig(IDP_CONFIG_DEVICE_MOTOR)
{
	
}

CPageConfigDeviceMotor::~CPageConfigDeviceMotor()
{
}

void CPageConfigDeviceMotor::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageConfigDeviceMotor, CMFCPropertyPage)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO_MOTOR1, &CPageConfigDeviceMotor::OnBnClickedRadioMotor1)
	ON_BN_CLICKED(IDC_RADIO_MOTOR2, &CPageConfigDeviceMotor::OnBnClickedRadioMotor2)
	ON_BN_CLICKED(IDC_RADIO_MOTOR3, &CPageConfigDeviceMotor::OnBnClickedRadioMotor3)
	ON_BN_CLICKED(IDC_RADIO_MOTOR4, &CPageConfigDeviceMotor::OnBnClickedRadioMotor4)
	ON_BN_CLICKED(IDC_RADIO_MOTOR5, &CPageConfigDeviceMotor::OnBnClickedRadioMotor5)
	ON_BN_CLICKED(IDC_RADIO_MOTOR6, &CPageConfigDeviceMotor::OnBnClickedRadioMotor6)
	ON_BN_CLICKED(IDC_RADIO_MOTOR7, &CPageConfigDeviceMotor::OnBnClickedRadioMotor7)
	ON_BN_CLICKED(IDC_RADIO_MOTOR8, &CPageConfigDeviceMotor::OnBnClickedRadioMotor8)
END_MESSAGE_MAP()


// CPageConfigDeviceMotor message handlers


BOOL CPageConfigDeviceMotor::OnInitDialog()
{
	CMFCPropertyPageEx::OnInitDialog();
	
	const INT nResID[] = {
		IDC_RADIO_MOTOR1,
		IDC_RADIO_MOTOR2,
		IDC_RADIO_MOTOR3,
		IDC_RADIO_MOTOR4,
		IDC_RADIO_MOTOR5,
		IDC_RADIO_MOTOR6,
		IDC_RADIO_MOTOR7,
		IDC_RADIO_MOTOR8,
		NULL
	};

	CSheetConfigDevice* pParent = dynamic_cast<CSheetConfigDevice*>(GetParent());

	m_nMotorCount = pParent->GetEditMotorCount();

	const INT nMotorIndex = m_nMotorCount - 1;
	for (INT nIndex = 0; nResID[nIndex]; nIndex++)
	{
		if (nMotorIndex == nIndex)
		{
			
			static_cast<CButton*>(GetDlgItem(nResID[nIndex]))->SetCheck(BST_CHECKED);
		}
		else
		{
			static_cast<CButton*>(GetDlgItem(nResID[nIndex]))->SetCheck(BST_UNCHECKED);
		}
	}
	
	return TRUE;
}


void CPageConfigDeviceMotor::OnBnClickedRadioMotor1()
{
	this->ChangeMotorCount(1);
}


void CPageConfigDeviceMotor::OnBnClickedRadioMotor2()
{
	this->ChangeMotorCount(2);
}


void CPageConfigDeviceMotor::OnBnClickedRadioMotor3()
{
	this->ChangeMotorCount(3);
}


void CPageConfigDeviceMotor::OnBnClickedRadioMotor4()
{
	this->ChangeMotorCount(4);
}


void CPageConfigDeviceMotor::OnBnClickedRadioMotor5()
{
	this->ChangeMotorCount(5);
}


void CPageConfigDeviceMotor::OnBnClickedRadioMotor6()
{
	this->ChangeMotorCount(6);
}


void CPageConfigDeviceMotor::OnBnClickedRadioMotor7()
{
	this->ChangeMotorCount(7);
}


void CPageConfigDeviceMotor::OnBnClickedRadioMotor8()
{
	this->ChangeMotorCount(8);
}

void CPageConfigDeviceMotor::ChangeMotorCount(const INT nCount)
{
	m_nMotorCount = nCount;
}


void CPageConfigDeviceMotor::OnOK()
{
	CSheetConfigDevice* pParent = dynamic_cast<CSheetConfigDevice*>(GetParent());
	pParent->SetEditMotorCount(m_nMotorCount);

	CPageConfig::OnOK();
}
