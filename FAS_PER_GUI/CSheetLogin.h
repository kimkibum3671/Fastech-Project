#pragma once

#include "CMFCPropertySheetEx.h"

#include "CPageLoginUserID.h"
#include "CPageLoginGroup.h"
#include "CPageLoginPassword.h"

// CSheetLogin

class CSheetLogin : public CMFCPropertySheetEx
{
	DECLARE_DYNAMIC(CSheetLogin)

public:
	CSheetLogin();
	virtual ~CSheetLogin();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CPageLoginUserID	m_pageUser;
	CPageLoginGroup		m_pageGroup;
	CPageLoginPassword	m_pagePassword;	

public:
	virtual BOOL OnInitDialog();

protected:
	typedef enum _OPERATION_MODE
	{
		mode_user = 0,
		mode_group,
	}	OPERATION_MODE;

	INT m_nMode;

public:
	void SetUserMode();
	const BOOL IsUserMode();

	void SetGroupMode();
	const BOOL IsGroupMode();

protected:
	CString m_strUserName, m_strUserPassword;

public:
	void	SetUserName(LPCTSTR lpszName);
	LPCTSTR GetUserName();
	
	void	SetUserPassword(LPCTSTR lpszPassword);
	LPCTSTR	GetUserPassword();

public:
	virtual const BOOL SetActiveSafePage(const INT nPage);

protected:
	CBitmap m_bmpUser;

public:
	void		SetUserImage( CBitmap* pBitmap );
	CBitmap*	GetUserImage();
};

