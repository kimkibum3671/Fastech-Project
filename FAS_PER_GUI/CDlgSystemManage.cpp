// CDlgSystemManage.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgSystemManage.h"
#include "afxdialogex.h"

#include "CDataBaseManager.h"

// CDlgSystemManage 대화 상자

IMPLEMENT_DYNAMIC(CDlgSystemManage, CDialogEx)

CDlgSystemManage::CDlgSystemManage(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SYSTEM_MAMAGE_BACKUP, pParent)
{
	m_strSelectedName = _T("");
	m_bCompare = FALSE;
}

CDlgSystemManage::~CDlgSystemManage()
{
	INT a = 0;
}

void CDlgSystemManage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MAIN, m_listCtrl);
}


BEGIN_MESSAGE_MAP(CDlgSystemManage, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_LIST_MAIN, &CDlgSystemManage::OnNMClickListMain)
	ON_BN_CLICKED(IDOK, &CDlgSystemManage::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSystemManage 메시지 처리기


BOOL CDlgSystemManage::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), IsCompareMode() ? MAKEINTRESOURCE(IDR_SYSTEM_COMPARE) : MAKEINTRESOURCE(IDR_SYSTEM_MANAGE));
	this->SetIcon(hIcon, FALSE);

	this->LoadProjectList();
	
	this->ShowListCtrl();
		
	this->LoadProjectImage();

	this->InitControlTextSize();
	
	this->SetWindowText(m_strTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


const BOOL CDlgSystemManage::LoadFromDatabase(LPVOID pDatabaseRecordData, CString& strName, INT& nBackupCount)
{
	CDataBaseRecordData* pRecord = static_cast<CDataBaseRecordData*>(pDatabaseRecordData);
	
	if (FALSE == pRecord->GetTextData(_T("Project_Name"), strName) ||
		FALSE == pRecord->GetIntergerData(_T("count([Backup_List].[Project_Name])"), nBackupCount))
	{
		return FALSE;
	}

	return TRUE;
}

const BOOL CDlgSystemManage::LoadFromDatabaseDetail(LPVOID pDatabaseRecordData, CString& strProjectName, CString& strNominalName, CString& strNominalTime)
{ // 사용 X
	CDataBaseRecordData* pRecord = static_cast<CDataBaseRecordData*>(pDatabaseRecordData);

	if (FALSE == pRecord->GetTextData(_T("Project_Name"), strProjectName) ||
		FALSE == pRecord->GetTextData(_T("Backup_Name"), strNominalName) ||
		FALSE == pRecord->GetTextData(_T("TimeStamp"), strNominalTime))
	{
		return FALSE;
	}

	return TRUE;
}


void CDlgSystemManage::LoadProjectList()
{
	if (1)
	{ // Device Name, Device Count 찾기..DB 작업
		CDataBaseProject* pProject = CDataBaseProject::GetInstance();
		if (pProject->OpenDataBase())
		{
			if (pProject->GetBackupList())
			{
				INT nCnt = 0;
				BACKUP_LIST list;
				while (CDataBaseRecordData* pRecord = pProject->SeekToNext())
				{
					if (LoadFromDatabase(pRecord, list.strName, list.nListCnt))
					{
						m_listMain.AddTail(list);
					}
				}
			}

			pProject->CloseDataBase();
		}
	}

	/*if (1)
	{
		CDataBaseProject* pProject = CDataBaseProject::GetInstance();
		if (pProject->OpenDataBase())
		{
			CString str1, str2;
			if (pProject->GetBackupListDetailData())
			{
				INT nCnt = 0;
				BACKUP_SUB_LIST list;
				while (CDataBaseRecordData* pRecord = pProject->SeekToNext())
				{
					if (LoadFromDatabaseDetail(pRecord, list.strName, list.strNominalName, list.strNominalTime))
					{
						m_listSub.AddTail(list);
					}
				}
			}

			pProject->CloseDataBase();
		}
	}*/
}


void CDlgSystemManage::ShowListCtrl()
{
	if (1)
	{
		// List Control 작업

		CRect rect;
		INT iItem = 0;

		m_listCtrl.GetWindowRect(&rect);
		m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

		/*
		SetExtendedStyle : List Control의 확장 스타일을 나타낸다.

		LVS_EX_FULLROWSELECT : 아이템 클릭시 포커스 표시.
		LVS_EX_GRIDLINES : 아이템 경계선 표시
		LVS_EX_DOUBLEBUFFER : 더블버퍼링(깜빡임 방지) 활성화
		*/

		m_listCtrl.InsertColumn(0, TEXT("Tooling Name"));
		m_listCtrl.InsertColumn(1, TEXT("Backup Count"));

		m_listCtrl.SetColumnWidth(0, 120);
		m_listCtrl.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

		POSITION pos = m_listMain.GetHeadPosition();


		while (NULL != pos)
		{
			CString strCnt;
			BACKUP_LIST list = m_listMain.GetNext(pos);
			strCnt.Format(_T("%d"), list.nListCnt);

			iItem = m_listCtrl.InsertItem(iItem, list.strName);
			m_listCtrl.SetItemText(iItem, 1, strCnt);

			iItem++;
		}

		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
}


CString CDlgSystemManage::GetSystemName()
{
	return m_strSelectedName;
}

void CDlgSystemManage::SetSelectedName(CString strName)
{
	m_strSelectedName = strName;
}

void CDlgSystemManage::OnNMClickListMain(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	int nSelectIndex = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);
	SetSelectedName(m_listCtrl.GetItemText(nSelectIndex, 0));
	//UpdateText(m_listCtrl.GetItemText(nSelectIndex, 0), m_listCtrl.GetItemText(nSelectIndex, 1));

	if (-1 != nSelectIndex)
	{ 
		m_wndViewer.EnableWindow(TRUE);
		CDeviceMotionGate* pDevice = (CDeviceMotionGate*)m_listCtrl.GetItemData(nSelectIndex);
		SetDeviceImage(pDevice);
	}
	else
	{ // 빈공간 Click
		m_wndViewer.EnableWindow(FALSE);
	}	

	GetDlgItem(IDOK)->EnableWindow(nSelectIndex != -1);
}



void CDlgSystemManage::UpdateText(CString strName, CString strCnt)
{ // 사용 X
	SetDlgItemText(IDC_STATIC_TOOL_NAME, strName);
	SetDlgItemText(IDC_STATIC_TOOL_COUNT, strCnt);

	if (1)
	{
		POSITION pos = m_listSub.GetHeadPosition();
		CString strName, strTime;

		while (NULL != pos)
		{
			BACKUP_SUB_LIST list = m_listSub.GetNext(pos);
			if (list.strName == GetSystemName())
			{
				strName = list.strNominalName;
				strTime = list.strNominalTime;
			}
			SetDlgItemText(IDC_STATIC_TOOL_NOMINAL_NAME, strName);
			SetDlgItemText(IDC_STATIC_TOOL_NOMINAL_TIME, strTime);
		}
	}
}


void CDlgSystemManage::LoadProjectImage()
{
	CDataBaseProject* pProject = CDataBaseProject::GetInstance();

	if (pProject->OpenDataBase())
	{
		if (pProject->GetDeviceList())
		{
			while (CDataBaseRecordData* pRecord = pProject->SeekToNext())
			{
				CDeviceMotionGate* pDevice = new CDeviceMotionGate;
				if (pDevice->LoadFromDatabase(pRecord))
				{
					for (INT i = 0; i < m_listCtrl.GetItemCount(); i++)
					{
						if (pDevice->GetDatabaseSystemName() == m_listCtrl.GetItemText(i,0))
						{
							m_listCtrl.SetItemData(i, reinterpret_cast<DWORD_PTR>(pDevice));
						}
     				}
				}
				
				else
				{
					delete pDevice;
					ASSERT(0);
				}
			}
		}

		pProject->CloseDataBase();
	}

	m_wndViewer.SubclassDlgItem(IDC_PICTURE_PROJECT, this);
	m_wndViewer.EnableWindow(FALSE);
}


const BOOL CDlgSystemManage::SetDeviceImage(CDeviceMotionGate* pDevice)
{
	m_wndViewer.LoadMainImage(pDevice->GetSystemImage());

	return TRUE;
}


HBITMAP CDlgSystemManage::GetEditSystemImage(CDeviceMotionGate* pDevice)
{
	ASSERT(pDevice);
	CString strSystemName;
	strSystemName = pDevice->GetDatabaseSystemName(); 
	CBitmap* pBitmap = pDevice->GetEditSystemImage();

	CDataBaseProject* pProject = CDataBaseProject::GetInstance();
	if (pProject->OpenDataBase())
	{
		if (pProject->GetDeviceList())
		{
			while (CDataBaseRecordData* pRecord = pProject->SeekToNext())
			{
				pBitmap = pDevice->LoadImageFromDatabase(pRecord, strSystemName);
			}
		}

		pProject->CloseDataBase();
	}
	return *pBitmap;
}


void CDlgSystemManage::SetTitle(CString strTitle)
{
	m_strTitle = strTitle;
}


void CDlgSystemManage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}

void CDlgSystemManage::SetCompareMode(const BOOL bCompare)
{
	m_bCompare = bCompare;
}


const BOOL CDlgSystemManage::IsCompareMode()
{
	return m_bCompare;
}


void CDlgSystemManage::InitControlTextSize()
{
	if (true)
	{
		LOGFONT lf = { 0, };
		afxGlobalData.fontRegular.GetLogFont(&lf);

		lf.lfHeight = -15;
		m_BigFont.CreateFontIndirect(&lf);

		GetDlgItem(IDC_STATIC_IMFORM)->SetFont(&m_BigFont);
		GetDlgItem(IDC_LIST_MAIN)->SetFont(&m_BigFont);
		GetDlgItem(IDOK)->SetFont(&m_BigFont);
		GetDlgItem(IDCANCEL)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}

	/*if (true)
	{
		m_BigFont.CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, TEXT_STYLE);

		GetDlgItem(IDC_STATIC_IMFORM)->SetFont(&m_BigFont);
		GetDlgItem(IDC_LIST_MAIN)->SetFont(&m_BigFont);
		GetDlgItem(IDOK)->SetFont(&m_BigFont);
		GetDlgItem(IDCANCEL)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}*/
	
}
