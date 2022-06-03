// CSheetConfig.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CSheetConfig.h"

#include "CDeviceList.h"

#include "CDataBaseManager.h"

// CSheetConfig

IMPLEMENT_DYNAMIC(CSheetConfig, CSheetConfigDevice)

CSheetConfig::CSheetConfig()
{
	SetLook(CMFCPropertySheet::PropSheetLook_Tree, 250);

	SetIconsList(IDB_TREE, 16, RGB(0xff, 0xff, 0xff));

	m_bShowWizardButton = TRUE;

	/*
	기존
	CMFCPropertySheetCategoryInfo* pCategoryDevice = AddTreeCategory(_T("Device"), 0, 1);*/
	CMFCPropertySheetCategoryInfo* pCategoryDevice = AddTreeCategory(_T("Tooling"), 0, 1);
	//AddPageToTree(pCategoryDevice, &m_pageDeviceInterface, -1, 2);	
	AddPageToTree(pCategoryDevice, &m_pageDeviceDatabase, -1, 2);	
	
	/* Delete Project 관리부분
	AddPageToTree(pCategoryDevice, &m_pageProjectManagement,-1, 2);
	*/
	
	
	CMFCPropertySheetCategoryInfo* pCategoryNetwork = AddTreeCategory(_T("Network"), 0, 1);
	AddPageToTree(pCategoryNetwork, &m_pageNetworkGeneral, -1, 2);


	//CMFCPropertySheetCategoryInfo* pCategoryProjectManage = AddTreeCategory(_T("Device List (Delete Project)"), )

	/*CMFCPropertySheetCategoryInfo* pCategoryInterface = AddTreeCategory(_T("Interface"), 0, 1, pCategoryDevice);
	AddPageToTree(pCategoryInterface, &m_pageDeviceInterface, -1, 2);
	CMFCPropertySheetCategoryInfo* pCategorySocket = AddTreeCategory(_T("Socket"), 0, 1, pCategoryDevice);
	AddPageToTree(pCategorySocket, &m_pageDeviceSocket, -1, 2);
	CMFCPropertySheetCategoryInfo* pCategorySerial = AddTreeCategory(_T("Serial"), 0, 1, pCategoryDevice);
	AddPageToTree(pCategorySerial, &m_pageDeviceAddress, -1, 2);
	AddPageToTree(pCategorySerial, &m_pageDeviceMotor, -1, 2);
	AddPageToTree(pCategorySerial, &m_pageDeviceOrigin, -1, 2);
	AddPageToTree(pCategorySerial, &m_pageDeviceImage, -1, 2);*/

//	CMFCPropertySheetCategoryInfo* pCategoryUser = AddTreeCategory(_T("User"), 0, 1);
//	AddPageToTree(pCategoryUser, &m_pageUserList);

	CMFCPropertySheetCategoryInfo* pCategoryOthers = AddTreeCategory(_T("Others"), 0, 1);
	// 기존 이미지 제어 사진
	//AddPageToTree(pCategoryOthers, &m_pageDefaultImg, -1, 2);
	AddPageToTree(pCategoryOthers, &m_pageDeviceOthers, -1, 2);
	AddPageToTree(pCategoryOthers, &m_pageDeviceLifeCount, -1, 2);

	CPageConfigDevicePLC m_pageDevicePLC = CPageConfigDevicePLC::GetInstance();

	/* PLC 통신 Address 부분
	CMFCPropertySheetCategoryInfo* pCategoryPLC = AddTreeCategory(_T("PLC Setting"), 0, 1);
	AddPageToTree(pCategoryPLC, &m_pageDevicePLC, -1, 2);
	*/

	/*CMFCPropertySheetCategoryInfo* pCategoryTest = AddTreeCategory(_T("Test Setting"), 0, 1);
	AddPageToTree(pCategoryTest, &m_pageDeviceTest, -1, 2);*/
}

CSheetConfig::~CSheetConfig()
{

}


BEGIN_MESSAGE_MAP(CSheetConfig, CMFCPropertySheet)
END_MESSAGE_MAP()



// CSheetConfig message handlers
void CSheetConfig::Initialize(CDeviceMotionGate* pDevice)
{
	CSheetConfigDevice::Initialize(pDevice);

	ShowWizardButton(ID_APPLY_NOW, SW_HIDE);

	RecalculateWizardButton();
}


BOOL CSheetConfig::OnInitDialog()
{
	BOOL bResult = CSheetConfigDevice::OnInitDialog();

	SetTitle(_T("Total System Configuration"));

	m_BigFont.CreateFont(FONT_SIZE, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, _T("Arial"));

	if (1)
	{// 굳이 이렇게 해야하나..
		INT nHeight, nWidth;
		CRect rect, rectOk, rectCancel;
		
		this->GetClientRect(rect);
		
		CWnd* pWnd = GetDlgItem(IDCANCEL);
		if (pWnd->GetSafeHwnd())
		{
			pWnd->GetClientRect(rectCancel);
			pWnd->SetWindowPos(NULL, rect.Width() - (rectCancel.Width() + 5) , rect.Height() - (rectCancel.Height() + 5), rectCancel.Width(), rectCancel.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
			pWnd->SetWindowText(_T("Cancel"));
			pWnd->SetFont(&m_BigFont);
		}
		
		pWnd = GetDlgItem(IDOK);
		if (pWnd->GetSafeHwnd())
		{
			pWnd->GetClientRect(rectOk);
			pWnd->SetWindowPos(NULL, rect.Width() - (rectCancel.Width() + rectOk.Width() + 10), rect.Height() - (rectCancel.Height() + 5), rectCancel.Width(), rectCancel.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
			pWnd->SetWindowText(_T("Ok"));
			pWnd->SetFont(&m_BigFont);
		}
	
		ShowWizardButton(ID_APPLY_NOW, SW_HIDE);
	}

	m_wndTree.SetFont(&m_BigFont);

	return bResult;
}

CDeviceMotionGate* CSheetConfig::GetMotiongateCtrl()
{
	//	Serial의 Motiongate Ctrl을 가져오면 된다.
	return CDeviceList::GetInstance()->GetMotiongateForSerial();
}

CDeviceMotionGate* CSheetConfig::ScanAddressFromSocket(const DWORD dwAddress)
{
	return NULL;// m_pageDeviceSocket.ScanAddress(dwAddress);
}

CDeviceMotionGate* CSheetConfig::ScanNameFromSocket(LPCTSTR lpszSystemName)
{
	return NULL;// m_pageDeviceSocket.ScanName(lpszSystemName);
}

void CSheetConfig::SetEditAddress(const DWORD dwAddrss, CMFCPropertyPage* pPage)
{
	CDataBaseConfig::GetInstance()->SetSerialAddress(dwAddrss);
}

const DWORD	CSheetConfig::GetEditAddress(CMFCPropertyPage* pPage)
{
	return CDataBaseConfig::GetInstance()->GetSerialAddress();
}

void CSheetConfig::SetEditSystemName(LPCTSTR lpszSystemName, CMFCPropertyPage* pPage)
{
	CDataBaseConfig::GetInstance()->SetSerialName(lpszSystemName);
}

LPCTSTR CSheetConfig::GetEditSystemName(CMFCPropertyPage* pPage)
{
	return CDataBaseConfig::GetInstance()->GetSerialName();
}

void CSheetConfig::SetEditMotorCount(const INT nCount, CMFCPropertyPage* pPage)
{
	CDataBaseConfig::GetInstance()->SetSerialMotorCount(nCount);
}

const INT CSheetConfig::GetEditMotorCount(CMFCPropertyPage* pPage)
{
	return CDataBaseConfig::GetInstance()->GetSerialMotorCount();
}

void CSheetConfig::SetEditHomingTimeout(const INT nTimeout, const BOOL bSecond, CMFCPropertyPage* pPage)
{
	CDataBaseConfig::GetInstance()->SetSerialHomingTimeout(nTimeout, bSecond);
}

const INT CSheetConfig::GetEditHomingTimeout(const BOOL bSecond, CMFCPropertyPage* pPage)
{
	return CDataBaseConfig::GetInstance()->GetSerialHomingTimeout(bSecond);
}

HBITMAP	CSheetConfig::SetEditSystemImage(HBITMAP hBitmap, CMFCPropertyPage* pPage )
{
	HBITMAP hReturn = NULL;


	if( pPage == &m_pageDefaultImg)
	{
		hReturn = CDataBaseConfig::GetInstance()->SetNoImage(hBitmap);
	}
//	else if (pPage == &m_pageDeviceImage)
//	{
//		hReturn = CDataBaseConfig::GetInstance()->SetSerialSystemImage(hBitmap);
//	}

	return hReturn;
}

HBITMAP	CSheetConfig::GetEditSystemImage(CMFCPropertyPage* pPage )
{
	HBITMAP hReturn = NULL;

	if (pPage == &m_pageDefaultImg)
	{
		hReturn = CDataBaseConfig::GetInstance()->GetNoImage();
	}
//	else if (pPage == &m_pageDeviceImage)
//	{
//		hReturn = CDataBaseConfig::GetInstance()->GetSerialSystemImage();
//	}

	return hReturn;
}
