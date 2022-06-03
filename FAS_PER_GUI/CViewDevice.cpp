// CViewDevice.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CViewDevice.h"

#include "CDeviceList.h"
#include "CDocDevice.h"

#include "MainFrm.h"

IMPLEMENT_DYNAMIC(CViewDevice, CFormView)

// CViewDevice
CViewDevice::CViewDevice(LPCTSTR lpszTemplateName) : CFormView(lpszTemplateName)
{
	
}

CViewDevice::CViewDevice(UINT nIDTemplate) : CFormView(nIDTemplate)
{

}

CViewDevice::~CViewDevice()
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	if (pDevice)
	{
		pDevice->PauseAutoBackup(FALSE);
	}
}

void CViewDevice::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CViewDevice, CFormView)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CViewDevice diagnostics

#ifdef _DEBUG
void CViewDevice::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CViewDevice::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

// CViewDevice message handlers
CDeviceMotionGate* CViewDevice::GetMotiongateCtrl()
{
	CDeviceMotionGate* pDevice = NULL;
	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	pDeviceList->GetMotiongateByAddress(static_cast<CDocDevice*>(GetDocument())->GetAddress(), pDevice);

	return pDevice;
}

void CViewDevice::DestroyDeviceWindow(CFormView* pExceptionView)
{
	static_cast<CMainFrame*>(AfxGetMainWnd())->DestroyDeviceWindow(pExceptionView);
}

const BOOL CViewDevice::CheckNameMatched(CDeviceMotionGate* pDevice)
{
	/*LPCTSTR lpszResult = pDevice->GetSystemName();
	CString strDeviceName(pDevice->GetRealName());

	const BOOL bResult = strDeviceName == lpszResult;
	if (FALSE == bResult)
	{
		CString strCaption, strMessage;
		strCaption.LoadString(IDS_CAPTION_WARNING);
		strMessage.LoadString(IDS_MESSAGE_VALID_NAME);
		MessageBox(strMessage, strCaption, MB_ICONWARNING);
	}

	return bResult;*/

	return TRUE;
}

void CViewDevice::PrepareDestroy()
{

}

void CViewDevice::SetRadioIndex(const INT* pResArray, const INT nIndex, const INT cbArray)
{
	for (INT i = 0; (-1 == cbArray) ? pResArray[i] : i < cbArray; i++)
	{
		CButton* pBtn = static_cast<CButton*>(GetDlgItem(pResArray[i]));
		const BOOL bEnable = i == nIndex;
		if (bEnable != pBtn->GetCheck())
		{
			pBtn->SetCheck(bEnable);
		}
	}
}

const INT CViewDevice::GetRadioIndex(const INT* pResArray, const INT cbArray)
{
	for (INT i = 0; ( -1 == cbArray ) ? pResArray[i] : i < cbArray; i++)
	{
		if (BST_CHECKED == static_cast<CButton*>(GetDlgItem(pResArray[i]))->GetCheck())
		{
			return pResArray[i];
		}
	}

	return -1;
}


void CViewDevice::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
}


void CViewDevice::PostNcDestroy()
{
	CFormView::PostNcDestroy();
}


void CViewDevice::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 0:
	{
		CDeviceMotionGate* pDevice = GetMotiongateCtrl();
		if (pDevice)
		{
			pDevice->PauseAutoBackup();
		}
	}	break;
	}

	CFormView::OnTimer(nIDEvent);
}
