// CPageConfig.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageConfig.h"
#include "afxdialogex.h"


// CPageConfig dialog

IMPLEMENT_DYNAMIC(CPageConfig, CMFCPropertyPageEx)

CPageConfig::CPageConfig() :
	CMFCPropertyPageEx()
{
	Initialize();
}

CPageConfig::CPageConfig(UINT nIDTemplate, UINT nIDCaption) :
	CMFCPropertyPageEx(nIDTemplate, nIDCaption)
{
	Initialize();
}


CPageConfig::CPageConfig(LPCTSTR lpszTemplateName, UINT nIDCaption) :
	CMFCPropertyPageEx(lpszTemplateName, nIDCaption)
{
	Initialize();
}


CPageConfig::~CPageConfig()
{

}


void CPageConfig::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageConfig, CMFCPropertyPage)
END_MESSAGE_MAP()


// CPageConfig message handlers
void CPageConfig::Initialize()
{
	m_nOpMode = op_socket;
}


void CPageConfig::SetOpCode(const INT nOpMode)
{
	if (nOpMode != m_nOpMode)
	{
		m_nOpMode = nOpMode;
	}
}


const INT CPageConfig::GetOpMode()
{
	return m_nOpMode;
}
