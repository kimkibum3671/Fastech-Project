// CPageConfigDevicePLC.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageConfigDevicePLC.h"
#include "CSheetConfigDevice.h"
#include "CSheetConfig.h"
#include "CDataBaseManager.h"
#include <sstream>
// CPageConfigDevicePLC

IMPLEMENT_DYNAMIC(CPageConfigDevicePLC, CMFCPropertyPage)

CPageConfigDevicePLC::CPageConfigDevicePLC(CWnd* pParent)
	:CMFCPropertyPage(IDP_CONFIG_DEVICE_PLC)
{
	
}

CPageConfigDevicePLC::~CPageConfigDevicePLC()
{
}


BEGIN_MESSAGE_MAP(CPageConfigDevicePLC, CMFCPropertyPage)
	ON_EN_CHANGE(IDC_EDIT_INDEX, &CPageConfigDevicePLC::OnEnChangeEditIndex)
END_MESSAGE_MAP()



// CPageConfigDevicePLC 메시지 처리기




BOOL CPageConfigDevicePLC::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT id = (UINT)LOWORD(wParam);
	ASSERT(GetDlgItem(id));
	CString str;

	switch (id)
	{
	case IDC_BUTTON_SAVE:
	{
		GetDlgItem(id)->EnableWindow(false);
		GetDlgItem(id + 1)->EnableWindow(true);
		SaveIndexData();
	}	break;

	case IDC_BUTTON_DELETE:
	{
		GetDlgItem(id)->EnableWindow(false);
		GetDlgItem(id - 1)->EnableWindow(true);
		CDataBaseConfig* pProjectDB = CDataBaseConfig::GetInstance();
		if (pProjectDB->DeleteModBusWirteAddress(m_nModBusWriteAddress))
		{
			pProjectDB->SetModBusWriteAddressTypeWORD(m_wModBusWriteAddress);
		}
		else
		{
			AfxMessageBox(_T("DeleteModBusWirteAddress Fail"));
		}
		
		DeleteIndexData();
		
		str.Format(_T("If the address value has not been saved, pressing the 'OK' button will save the latest address value.('0x%02x%02x')."), HIBYTE(m_nModBusWriteAddress), LOBYTE(m_nModBusWriteAddress));
		((CStatic*)GetDlgItem(IDC_STATIC_INFORM))->SetWindowText(str);
	}	break;

	default:
		break;
	}
	

	return CMFCPropertyPage::OnCommand(wParam, lParam);
}


void CPageConfigDevicePLC::SaveIndexData()
{
	CString str;
	
	INT nStartIndex = 0;
	if (1) 
	{
		CEdit* pEdit = (CEdit*)GetDlgItemText(IDC_EDIT_INDEX, str);
		//m_nModBusWriteAddress = nStartIndex = _ttoi(str);
		str = str.Trim();
		if (str.GetLength() != 0)
		{
			nStartIndex = m_nModBusWriteAddress = m_wModBusWriteAddress = GetStringToWord(str);
		}
		
	}

	str.Format(L"Memory[%d ~ %d] \r\n", nStartIndex, nStartIndex + 12);
	SetTextData(nStartIndex,str);
	
	CDataBaseConfig* pProjectDB = CDataBaseConfig::GetInstance();
	if (pProjectDB->SetModBusWriteAddress(m_nModBusWriteAddress))
	{
		pProjectDB->SetModBusWriteAddressTypeWORD(m_wModBusWriteAddress);
		return;
	}

	else
	{
		AfxMessageBox(_T("SetModBusWriteAddress Funtion is Fail"));
	}
}

WORD CPageConfigDevicePLC::GetStringToWord(CString str)
{
	WORD wResult = 0;

	if (2 > str.GetLength())
	{
		wResult = _ttoi(str);
	}

	else
	{
		INT nLength = str.GetLength() / 2;
		for (INT i = 0; i < nLength; i++)
		{
			wResult |= GetState(str.Mid(2 * i, 2)) << (nLength == 1 ? 0 : (8 * (1 - i)));
		}
	}

	return wResult;
}

void CPageConfigDevicePLC::DeleteIndexData()
{
	m_ListCtrl.DeleteAllItems();
}


BOOL CPageConfigDevicePLC::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_INDEX);
	if (NULL != pEdit)
	{
		pEdit->SetWindowTextW(L"-");
	}
	CRect rt;
	m_ListCtrl.GetWindowRect(&rt);
	m_ListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT); // List 컨트롤에 선표시 및 Item 선택시 한행 전체 선택

	m_ListCtrl.InsertColumn(0, TEXT("INDEX              "), LVCFMT_LEFT, rt.Width() * 0.2);
	m_ListCtrl.InsertColumn(1, TEXT("INFORM"), LVCFMT_LEFT, rt.Width() * 0.7);
	
	m_ListCtrl.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_ListCtrl.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

	Initialize();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CPageConfigDevicePLC::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT_INDEX, m_edit);
	DDX_Control(pDX, IDC_LIST_INDEX, m_ListCtrl);
}


const BOOL CPageConfigDevicePLC::SetTextData(INT nStartIndex, CString str, COLORREF crText)
{
	DeleteIndexData();

	if (str == L"") return FALSE;

	COleDateTime dt = COleDateTime::GetCurrentTime();
	CString strText = dt.Format(L"[%H : %M : %S]");
	strText += str;
	
	if (m_edit.GetSafeHwnd())
	{
		CHARFORMAT cf;
		m_edit.GetDefaultCharFormat(cf);
		cf.dwMask = CFM_COLOR;
		cf.dwEffects = NULL;
		cf.crTextColor = crText;
		m_edit.SetWordCharFormat(cf);
		m_edit.SetSel(-1, -1);
		m_edit.ReplaceSel(str);
	}

	CString strIndex;
	int num = m_ListCtrl.GetItemCount();
	

	CString strInform;

	for (INT nIndex = num; nIndex < num + cmd_max; nIndex++)
	{
		strIndex.Format(L"[INDEX + %d]", nIndex);

		switch (nIndex)
		{
		case cmd_req_eeno:
		{
			strInform.Format(L"[REQ] MotionGate Part No");
		}	break;

		case cmd_req_pt_mt_no:
		{
			strInform.Format(L"[REQ] Position No / Motor No");
		}	break;

		case cmd_res_eeno:
		{
			strInform.Format(L"[RES] MotionGate Part No");
		}	break;

		case cmd_res_pt_mt_no:
		{
			strInform.Format(L"[RES] Position No / Motor No");
		}	break;

		case cmd_errcode:
		{
			strInform.Format(L"(MotionGate & Motor) Error Code");
		}	break;

		case cmd_pos:
		{
			strIndex.Format(L"[INDEX + %d ~ %d]", nIndex, nIndex + 1);
			strInform.Format(L"POSITION");
		}	break;

		case cmd_speed:
		{
			strIndex.Format(L"[INDEX + %d]", nIndex + 1);
			strInform.Format(L"Speed");
		}	break;

		default:
			break;
		}
		
		m_ListCtrl.InsertItem(nIndex, strIndex);
		m_ListCtrl.SetItem(nIndex, 1, LVIF_TEXT, strInform, NULL, NULL, NULL, NULL);
	}


	return TRUE;
}


CPageConfigDevicePLC* CPageConfigDevicePLC::GetInstance()
{
	static CPageConfigDevicePLC _instance;
	return &_instance;
}


void CPageConfigDevicePLC::Initialize()
{
	CDataBaseConfig* pProjectDB = CDataBaseConfig::GetInstance();
	
	m_nModBusWriteAddress = pProjectDB->GetModBusWriteAddress();
	m_wModBusWriteAddress = pProjectDB->GetModBusWriteAddressTypeWORD();
	CString str;
	str.Format(_T("%02x%02x"), HIBYTE(m_wModBusWriteAddress), LOBYTE(m_wModBusWriteAddress));
	
	((CEdit*)GetDlgItem(IDC_EDIT_INDEX))->SetWindowText(str);
	
	SaveIndexData();
}


void CPageConfigDevicePLC::OnEnChangeEditIndex()
{
	CString str;
	CString strAddress;
	INT nAddress;
	BOOL bContinue = true;

	CSheetConfig* pParent = dynamic_cast<CSheetConfig*>(GetParent());
	ASSERT(pParent);
	
	((CEdit*)GetDlgItem(IDC_EDIT_INDEX))->GetWindowText(strAddress);
	
	strAddress.MakeUpper();
	nAddress = _ttoi(strAddress);
	
	for (int i = 0; i < strAddress.GetLength(); i++) // 숫자로만 구성되어있는지 확인!!
	{
		bContinue &= (isdigit(strAddress.GetAt(i)) || ('A' <= strAddress.GetAt(i) && 'F' >= strAddress.GetAt(i))) ? true : false;
	}

	if (strAddress.GetLength() == 0)
	{
		str.Format(_T("If the EditControl value is blank and the 'OK' button is pressed, it will be saved as the latest Start Index ('%02x%02x')."), HIBYTE(m_wModBusWriteAddress), LOBYTE(m_wModBusWriteAddress));
	}
	else if (strAddress.GetLength() < 4)
	{
		str.Format(_T("Please enter a 4-digt number"));
	}
	else
	{
		str.Format(_T(""));
	}
	
	pParent->ShowWizardButton(IDOK, bContinue && (4 == strAddress.GetLength()) /*&& (m_wModBusWriteAddress != GetStringToWord(strAddress))*/ ? SW_SHOWNORMAL : SW_HIDE);

	((CStatic*)GetDlgItem(IDC_STATIC_INFORM))->SetWindowText(str);
}

unsigned char CPageConfigDevicePLC::GetState(CString str)
{
	//CString str;
	CString strReturnValue;
	CString strValue;
	unsigned char cVal = 0;
	//CEdit* pEdit = (CEdit*)GetDlgItemText(id, str);
	str.MakeUpper();
	for (int i = 0; i < str.GetLength(); i++)
	{
		strReturnValue = str[i];
		if (strReturnValue >= L"A" && strReturnValue <= L"Z")
		{
			cVal += (i == 0 ? (str[i] - 55) << 4 : (str[i] - 55));
		}
		else if (strReturnValue >= L"0" && strReturnValue <= L"9")
		{
			cVal += (i == 0 ? (str[i] - 48) << 4 : (str[i] - 48));
		}
	}
	return cVal;
}


void CPageConfigDevicePLC::OnOK()
{
	SaveIndexData();
	CMFCPropertyPage::OnOK();
}
