// CPageAssignSelect.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageAssignSelect.h"
#include "afxdialogex.h"

#include "CSheetAssignID.h"

#include "CDeviceList.h"

// CPageAssignSelect dialog

IMPLEMENT_DYNAMIC(CPageAssignSelect, CMFCPropertyPageEx)

CPageAssignSelect::CPageAssignSelect(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPageEx(IDP_ASSIGN_SELECT)
{

}

CPageAssignSelect::~CPageAssignSelect()
{
}

void CPageAssignSelect::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageAssignSelect, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_BTN_BACK, &CPageAssignSelect::OnBnClickedBtnBack)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CPageAssignSelect::OnBnClickedBtnNext)
	ON_BN_CLICKED(IDC_BTN_MOTOR1, &CPageAssignSelect::OnBnClickedBtnMotor1)
	ON_BN_CLICKED(IDC_BTN_MOTOR2, &CPageAssignSelect::OnBnClickedBtnMotor2)
	ON_BN_CLICKED(IDC_BTN_MOTOR3, &CPageAssignSelect::OnBnClickedBtnMotor3)
	ON_BN_CLICKED(IDC_BTN_MOTOR4, &CPageAssignSelect::OnBnClickedBtnMotor4)
	ON_BN_CLICKED(IDC_BTN_MOTOR5, &CPageAssignSelect::OnBnClickedBtnMotor5)
	ON_BN_CLICKED(IDC_BTN_MOTOR6, &CPageAssignSelect::OnBnClickedBtnMotor6)
	ON_BN_CLICKED(IDC_BTN_MOTOR7, &CPageAssignSelect::OnBnClickedBtnMotor7)
	ON_BN_CLICKED(IDC_BTN_MOTOR8, &CPageAssignSelect::OnBnClickedBtnMotor8)
END_MESSAGE_MAP()


// CPageAssignSelect message handlers
BOOL CPageAssignSelect::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	CSheetAssignID* pParent = static_cast<CSheetAssignID*>(GetParent());

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

			m_btnMotor[nIndex].EnableWindow(FALSE == pDevice->IsMotorConnected(nIndex + 1));
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


void CPageAssignSelect::OnBnClickedBtnBack()
{
	ActivatePrevPage();
}


void CPageAssignSelect::OnBnClickedBtnNext()
{
	ActivateNextPage();
}

void CPageAssignSelect::OnBnClickedBtnMotor1()
{
	this->OnMotorSelected(IDC_BTN_MOTOR1);
}


void CPageAssignSelect::OnBnClickedBtnMotor2()
{
	this->OnMotorSelected(IDC_BTN_MOTOR2);
}


void CPageAssignSelect::OnBnClickedBtnMotor3()
{
	this->OnMotorSelected(IDC_BTN_MOTOR3);
}


void CPageAssignSelect::OnBnClickedBtnMotor4()
{
	this->OnMotorSelected(IDC_BTN_MOTOR4);
}


void CPageAssignSelect::OnBnClickedBtnMotor5()
{
	this->OnMotorSelected(IDC_BTN_MOTOR5);
}


void CPageAssignSelect::OnBnClickedBtnMotor6()
{
	this->OnMotorSelected(IDC_BTN_MOTOR6);
}


void CPageAssignSelect::OnBnClickedBtnMotor7()
{
	this->OnMotorSelected(IDC_BTN_MOTOR7);
}


void CPageAssignSelect::OnBnClickedBtnMotor8()
{
	this->OnMotorSelected(IDC_BTN_MOTOR8);
}

void CPageAssignSelect::OnMotorSelected(const INT nResID)
{
	for (INT nIndex = 0; nIndex < 8; nIndex++)
	{
		INT nNewImageID = IDB_FLAG_SMALL_OFF;
		if (m_btnMotor[nIndex].GetDlgCtrlID() == nResID)
		{
			nNewImageID = IDB_FLAG_SMALL_ON;
			static_cast<CSheetAssignID*>(GetParent())->SetSelectedMotorID(nIndex + 1);
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
