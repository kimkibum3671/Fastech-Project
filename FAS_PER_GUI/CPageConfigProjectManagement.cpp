// CPageConfigProjectManagement.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageConfigProjectManagement.h"

#include "CDataBaseManager.h"
#include "CDeviceMotionGate.h"
// CPageConfigProjectManagement

IMPLEMENT_DYNAMIC(CPageConfigProjectManagement, CMFCPropertyPage)

CPageConfigProjectManagement::CPageConfigProjectManagement()
	:CMFCPropertyPage(IDP_CONFIG_DEVICE_PROJECT_MANAGE)
{

}

CPageConfigProjectManagement::~CPageConfigProjectManagement()
{
}


BEGIN_MESSAGE_MAP(CPageConfigProjectManagement, CMFCPropertyPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DEVICE, &CPageConfigProjectManagement::OnLvnItemchangedListDevice)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CPageConfigProjectManagement::OnBnClickedBtnDelete)
END_MESSAGE_MAP()



// CPageConfigProjectManagement 메시지 처리기




BOOL CPageConfigProjectManagement::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_RECOVER)->EnableWindow(false);

	if (1)	// Initialize ListCtrl
	{
		m_listCtrl.SubclassDlgItem(IDC_LIST_DEVICE, this);
		m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
		HRESULT result = ::SetWindowTheme(m_listCtrl.m_hWnd, _T("Explorer"), NULL);

		m_listCtrl.InsertColumn(0, _T("Device Name"));
		m_listCtrl.InsertColumn(1, _T("Device ID"));
		m_listCtrl.InsertColumn(2, _T("Motor Count"));
		m_listCtrl.InsertColumn(3, _T("Homing Timeout"));
		m_listCtrl.InsertColumn(4, _T("Nominal Info."));
		m_listCtrl.InsertColumn(5, _T("Latest Info."));

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
						if (pDevice->GetDeletedStatus())
						{
							SetListItem(pDevice);
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

			const INT nCount = m_listCtrl.GetItemCount();
			for (INT nIndex = 0; nIndex < nCount; nIndex++)
			{
				CString strDevice(m_listCtrl.GetItemText(nIndex, 0));
				INT nID = 0;

				CString strTimeStamp;

				if (pProject->GetNominalBackupInfo(strDevice, &nID, &strTimeStamp) &&
					-1 != nID)
				{
					m_listCtrl.SetItemText(nIndex, 4, strTimeStamp);
				}

				if (pProject->GetLatestRestoredInfo(strDevice, &nID, &strTimeStamp) &&
					-1 != nID)
				{
					m_listCtrl.SetItemText(nIndex, 5, strTimeStamp);
				}
			}
		}

		INT nWidth1, nWidth2;
		m_listCtrl.SetColumnWidth(0, LVSCW_AUTOSIZE);
		nWidth1 = m_listCtrl.GetColumnWidth(0);
		m_listCtrl.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
		nWidth2 = m_listCtrl.GetColumnWidth(0);
		m_listCtrl.SetColumnWidth(0, max(nWidth1, nWidth2) + 30);

		m_listCtrl.SetColumnWidth(1, LVSCW_AUTOSIZE);
		nWidth1 = m_listCtrl.GetColumnWidth(1);
		m_listCtrl.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
		nWidth2 = m_listCtrl.GetColumnWidth(1);
		m_listCtrl.SetColumnWidth(1, max(nWidth1, nWidth2) + 30);

		m_listCtrl.SetColumnWidth(2, LVSCW_AUTOSIZE);
		nWidth1 = m_listCtrl.GetColumnWidth(1);
		m_listCtrl.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);
		nWidth2 = m_listCtrl.GetColumnWidth(2);
		m_listCtrl.SetColumnWidth(2, max(nWidth1, nWidth2) + 30);

		m_listCtrl.SetColumnWidth(3, LVSCW_AUTOSIZE);
		nWidth1 = m_listCtrl.GetColumnWidth(1);
		m_listCtrl.SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER);
		nWidth2 = m_listCtrl.GetColumnWidth(2);
		m_listCtrl.SetColumnWidth(3, max(nWidth1, nWidth2) + 30);

		m_listCtrl.SetColumnWidth(4, LVSCW_AUTOSIZE);
		nWidth1 = m_listCtrl.GetColumnWidth(1);
		m_listCtrl.SetColumnWidth(4, LVSCW_AUTOSIZE_USEHEADER);
		nWidth2 = m_listCtrl.GetColumnWidth(2);
		m_listCtrl.SetColumnWidth(4, max(nWidth1, nWidth2) - 30);

		m_listCtrl.SetColumnWidth(5, LVSCW_AUTOSIZE);
		nWidth1 = m_listCtrl.GetColumnWidth(1);
		m_listCtrl.SetColumnWidth(5, LVSCW_AUTOSIZE_USEHEADER);
		nWidth2 = m_listCtrl.GetColumnWidth(2);
		m_listCtrl.SetColumnWidth(5, max(nWidth1, nWidth2) - 30);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CPageConfigProjectManagement::SetListItem(LPVOID pDeviceMotiongate, const INT nIndex)
{
	CDeviceMotionGate* pDevice = static_cast<CDeviceMotionGate*>(pDeviceMotiongate);

	CString strTemp;

	INT nItem = 0;

	if (-1 == nIndex)
	{
		const INT nCount = m_listCtrl.GetItemCount();
		nItem = m_listCtrl.InsertItem(nCount, pDevice->GetDatabaseSystemName());
	}
	else
	{
		nItem = nIndex;
		m_listCtrl.InsertItem(nItem, pDevice->GetDatabaseSystemName());
	}

	strTemp.Format(_T("%d"), pDevice->GetDatabaseDeviceID());
	m_listCtrl.SetItemText(nItem, 1, strTemp);

	strTemp.Format(_T("%d"), pDevice->GetDatabaseMotorCount());
	m_listCtrl.SetItemText(nItem, 2, strTemp);

	strTemp.Format(_T("%d"), pDevice->GetDatabaseHomingTimeout());
	m_listCtrl.SetItemText(nItem, 3, strTemp);

	m_listCtrl.SetItemData(nItem, reinterpret_cast<DWORD_PTR>(pDevice));
}

void CPageConfigProjectManagement::OnLvnItemchangedListDevice(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	INT nSelectionCount = 0;
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();

	while (pos)
	{
		nSelectionCount++;
		m_listCtrl.GetNextSelectedItem(pos);
	}

	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(1 == nSelectionCount);
	GetDlgItem(IDC_BUTTON_RECOVER)->EnableWindow(1 == nSelectionCount);
}


void CPageConfigProjectManagement::OnBnClickedBtnDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
