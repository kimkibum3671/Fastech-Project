// CDlgSystemOption.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgSystemOption.h"
#include "afxdialogex.h"


// CDlgSystemOption dialog

IMPLEMENT_DYNAMIC(CDlgSystemOption, CDialogEx)

CDlgSystemOption::CDlgSystemOption(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SYSTEM_OPTION, pParent)
{

}

CDlgSystemOption::~CDlgSystemOption()
{
}

void CDlgSystemOption::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSystemOption, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgSystemOption::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSystemOption message handlers


BOOL CDlgSystemOption::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CMFCButton* pBtn = static_cast<CMFCButton*>(GetDlgItem(IDC_BTN_RESET));
	pBtn->ModifyStyle(0, BS_MULTILINE);
	pBtn->SetImage(IDB_BACK);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgSystemOption::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}
