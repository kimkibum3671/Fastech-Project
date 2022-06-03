// CSheetLogin.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CSheetLogin.h"


// CSheetLogin

IMPLEMENT_DYNAMIC(CSheetLogin, CMFCPropertySheetEx)

CSheetLogin::CSheetLogin()
{
	SetLook(CMFCPropertySheet::PropSheetLook_OneNoteTabs);

	BOOL b32BitIcons = afxGlobalData.m_nBitsPerPixel < 16 ? FALSE : TRUE;
	SetImageList(b32BitIcons ? IDB_SHEET32_LOGIN : IDB_SHEET32_LOGIN, 32, RGB(0xc0, 0xc0, 0xc0));
	
	m_nMode = mode_user;
	this->AddPage(&m_pageUser);
}

CSheetLogin::~CSheetLogin()
{

}


BEGIN_MESSAGE_MAP(CSheetLogin, CMFCPropertySheet)
END_MESSAGE_MAP()


// CSheetLogin message handlers
BOOL CSheetLogin::OnInitDialog()
{
	BOOL bResult = CMFCPropertySheetEx::OnInitDialog();

	return bResult;
}


void CSheetLogin::SetUserMode()
{
	switch (m_nMode)
	{
	case mode_user:
	{

	}	break;

	case mode_group:
	{
		this->RemovePage(&m_pageGroup);
		this->AddPage(&m_pageUser);
		
		m_nMode = mode_user;
	}
	}
}

const BOOL CSheetLogin::IsUserMode()
{
	return m_nMode == mode_user;
}

void CSheetLogin::SetGroupMode()
{
	switch (m_nMode)
	{
	case mode_user:
	{
		this->RemovePage(&m_pageUser);
		this->AddPage(&m_pageGroup);

		m_nMode = mode_group;
	}	break;

	case mode_group:
	{
		
	}
	}
}

const BOOL CSheetLogin::IsGroupMode()
{
	return m_nMode == mode_group;
}

void CSheetLogin::SetUserName(LPCTSTR lpszName)
{
	m_strUserName = lpszName;
}

LPCTSTR CSheetLogin::GetUserName()
{
	return m_strUserName;
}

void CSheetLogin::SetUserPassword(LPCTSTR lpszPassword)
{
	m_strUserPassword = lpszPassword;
}

LPCTSTR	CSheetLogin::GetUserPassword()
{
	return m_strUserPassword;
}

const BOOL CSheetLogin::SetActiveSafePage(const INT nPage)
{
	if (1 == this->GetPageCount() )
	{
		if (0 == this->GetActiveIndex())
		{
			AddPage(&m_pagePassword);
		}
	}
	
	return this->SetActivePage(nPage);
}

void CSheetLogin::SetUserImage(CBitmap* pBitmap)
{
	if (m_bmpUser.GetSafeHandle())
	{
		m_bmpUser.Detach();
	}

	m_bmpUser.Attach(::CopyImage(pBitmap->m_hObject, IMAGE_BITMAP, 0, 0, 0));
}

CBitmap* CSheetLogin::GetUserImage()
{
	return &m_bmpUser;
}
