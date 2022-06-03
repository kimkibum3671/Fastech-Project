// CPageFactoryResetSelect.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageFactoryResetSelect.h"
#include "afxdialogex.h"

#include "CSheetFactoryReset.h"

#include "CDeviceList.h"

// CPageFactoryResetSelect dialog

IMPLEMENT_DYNAMIC(CPageFactoryResetSelect, CMFCPropertyPageEx)

CPageFactoryResetSelect::CPageFactoryResetSelect(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPageEx(IDP_FACTORY_RESET_SELECT)
{

}

CPageFactoryResetSelect::~CPageFactoryResetSelect()
{
}

void CPageFactoryResetSelect::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageFactoryResetSelect, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_BTN_MOTOR1, &CPageFactoryResetSelect::OnBnClickedBtnMotor1)
	ON_BN_CLICKED(IDC_BTN_MOTOR2, &CPageFactoryResetSelect::OnBnClickedBtnMotor2)
	ON_BN_CLICKED(IDC_BTN_MOTOR3, &CPageFactoryResetSelect::OnBnClickedBtnMotor3)
	ON_BN_CLICKED(IDC_BTN_MOTOR4, &CPageFactoryResetSelect::OnBnClickedBtnMotor4)
	ON_BN_CLICKED(IDC_BTN_MOTOR5, &CPageFactoryResetSelect::OnBnClickedBtnMotor5)
	ON_BN_CLICKED(IDC_BTN_MOTOR6, &CPageFactoryResetSelect::OnBnClickedBtnMotor6)
	ON_BN_CLICKED(IDC_BTN_MOTOR7, &CPageFactoryResetSelect::OnBnClickedBtnMotor7)
	ON_BN_CLICKED(IDC_BTN_MOTOR8, &CPageFactoryResetSelect::OnBnClickedBtnMotor8)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CPageFactoryResetSelect::OnBnClickedBtnNext)
	ON_BN_CLICKED(IDC_BTN_BACK, &CPageFactoryResetSelect::OnBnClickedBtnBack)
END_MESSAGE_MAP()


// CPageFactoryResetSelect message handlers

BOOL CPageFactoryResetSelect::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	CSheetFactoryReset* pParent = static_cast<CSheetFactoryReset*>(GetParent());

	CDeviceMotionGate* pDevice = NULL;
	if (FALSE == CDeviceList::GetInstance()->GetMotiongateByAddress(pParent->GetAddress(), pDevice))
	{
		return FALSE;
	}

	if (1)
	{
		const INT nResID[] = {
			IDC_BTN_MOTOR1,
			IDC_BTN_MOTOR2,
			IDC_BTN_MOTOR3,
			IDC_BTN_MOTOR4,
			IDC_BTN_MOTOR5,
			IDC_BTN_MOTOR6,
			IDC_BTN_MOTOR7,
			IDC_BTN_MOTOR8,
			NULL
		};

		for (INT nIndex = 0; nResID[nIndex]; nIndex++)
		{
			m_btnMotor[nIndex].SubclassDlgItem(nResID[nIndex], this);
			m_btnMotor[nIndex].SetImageEx(IDB_FLAG_SMALL_OFF);

			m_btnMotor[nIndex].EnableWindow(pDevice->IsMotorConnected(nIndex + 1));
		}
	}

	m_btnBack.SubclassDlgItem(IDC_BTN_BACK, this);
	m_btnBack.SetImage(IDB_BACK);

	m_btnNext.SubclassDlgItem(IDC_BTN_NEXT, this);
	m_btnNext.SetImage(IDB_NEXT);	

	m_wndViewer.SubclassDlgItem(IDC_ST_PROJECT, this);
	m_wndViewer.LoadMainImage(pDevice->GetSystemImage());

	return TRUE;
}


void CPageFactoryResetSelect::OnBnClickedBtnMotor1()
{
	this->OnMotorSelected(IDC_BTN_MOTOR1);
}


void CPageFactoryResetSelect::OnBnClickedBtnMotor2()
{
	this->OnMotorSelected(IDC_BTN_MOTOR2);
}


void CPageFactoryResetSelect::OnBnClickedBtnMotor3()
{
	this->OnMotorSelected(IDC_BTN_MOTOR3);
}


void CPageFactoryResetSelect::OnBnClickedBtnMotor4()
{
	this->OnMotorSelected(IDC_BTN_MOTOR4);
}


void CPageFactoryResetSelect::OnBnClickedBtnMotor5()
{
	this->OnMotorSelected(IDC_BTN_MOTOR5);
}


void CPageFactoryResetSelect::OnBnClickedBtnMotor6()
{
	this->OnMotorSelected(IDC_BTN_MOTOR6);
}


void CPageFactoryResetSelect::OnBnClickedBtnMotor7()
{
	this->OnMotorSelected(IDC_BTN_MOTOR7);
}


void CPageFactoryResetSelect::OnBnClickedBtnMotor8()
{
	this->OnMotorSelected(IDC_BTN_MOTOR8);
}


void CPageFactoryResetSelect::OnMotorSelected(const INT nResID)
{
	for (INT nIndex = 0; nIndex < 8; nIndex++)
	{
		INT nNewImageID = IDB_FLAG_SMALL_OFF;
		if (m_btnMotor[nIndex].GetDlgCtrlID() == nResID)
		{
			nNewImageID = IDB_FLAG_SMALL_ON;
			static_cast<CSheetFactoryReset*>(GetParent())->SetSelectedMotorID(nIndex + 1);
		}
		
		UINT nCurImageID = 0;
		m_btnMotor[nIndex].GetImageEx(&nCurImageID);
		if (nCurImageID != nNewImageID)
		{
			m_btnMotor[nIndex].SetImageEx(nNewImageID);
			m_btnMotor[nIndex].Invalidate(FALSE);
		}
	}

	GetDlgItem(IDC_BTN_NEXT)->EnableWindow();
}


void CPageFactoryResetSelect::OnBnClickedBtnNext()
{
	ActivateNextPage();
}


void CPageFactoryResetSelect::OnBnClickedBtnBack()
{
	ActivatePrevPage();
}
