// CDocDevice.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDocDevice.h"

#include "CDeviceList.h"

// CDocDevice

IMPLEMENT_DYNCREATE(CDocDevice, CDocument)

CDocDevice::CDocDevice()
{
	m_nSelectedMotorIndex = -1;
}

BOOL CDocDevice::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	CheckDeviceTitle();

	return TRUE;
}

CDocDevice::~CDocDevice()
{
}


BEGIN_MESSAGE_MAP(CDocDevice, CDocument)
END_MESSAGE_MAP()


// CDocDevice diagnostics

#ifdef _DEBUG
void CDocDevice::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CDocDevice::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CDocDevice serialization

void CDocDevice::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
#endif


// CDocDevice commands
void CDocDevice::SetAddress(const DWORD dwAddress)
{
	m_dwAddress = dwAddress;

	CheckDeviceTitle();
}

const DWORD	CDocDevice::GetAddress()
{
	return m_dwAddress;
}

void CDocDevice::CheckDeviceTitle()
{
	if (0 == m_strDeviceTitle.GetLength())
	{
		this->SetTitle(m_strDeviceTitle);
		return;
	}

	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	CDeviceMotionGate* pDevice = NULL;
	if (pDeviceList->GetMotiongateByAddress(m_dwAddress, pDevice))
	{
		//	21. 4. 22일 요청사항에 따라 무조건 장치 이름만 표시
		CString strTitle;
		strTitle.Format(_T("%s - %s"), pDevice->GetSystemName(), m_strDeviceTitle );

		if (this->GetTitle() != strTitle)
		{
			this->SetTitle(strTitle);
		}
	}
}

void CDocDevice::SelectMotorIndex(const INT nMotorIndex)
{
	m_nSelectedMotorIndex = nMotorIndex;
}

const INT CDocDevice::GetSelectMotorIndex()
{
	return m_nSelectedMotorIndex;
}

void CDocDevice::SetDeviceTitle(LPCTSTR lpszTitle)
{
	m_strDeviceTitle = lpszTitle;

	if (m_dwAddress)
	{
		CheckDeviceTitle();
	}
}
