// CPageConfigDeviceSerial.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageConfigDeviceSerial.h"
#include "afxdialogex.h"


// CPageConfigDeviceSerial dialog

IMPLEMENT_DYNAMIC(CPageConfigDeviceSerial, CMFCPropertyPage)

CPageConfigDeviceSerial::CPageConfigDeviceSerial(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDP_CONFIG_DEVICE_SERIAL)
{

}

CPageConfigDeviceSerial::~CPageConfigDeviceSerial()
{
}

void CPageConfigDeviceSerial::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageConfigDeviceSerial, CMFCPropertyPage)
END_MESSAGE_MAP()


// CPageConfigDeviceSerial message handlers


BOOL CPageConfigDeviceSerial::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();


	
	return TRUE;
}
