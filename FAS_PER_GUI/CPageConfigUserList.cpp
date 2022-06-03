// CPageConfigUserList.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageConfigUserList.h"
#include "afxdialogex.h"


// CPageConfigUserList dialog

IMPLEMENT_DYNAMIC(CPageConfigUserList, CMFCPropertyPage)

CPageConfigUserList::CPageConfigUserList(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDP_CONFIG_USER_LIST)
{

}

CPageConfigUserList::~CPageConfigUserList()
{
}

void CPageConfigUserList::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageConfigUserList, CMFCPropertyPage)
END_MESSAGE_MAP()


// CPageConfigUserList message handlers
