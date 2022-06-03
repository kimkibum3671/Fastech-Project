// CPageRestorationCompare.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageRestorationCompare.h"
#include "afxdialogex.h"

#include "MainFrm.h"
#include "CDataBaseManager.h"
#include "CSheetRestoration.h"
#include "CDeviceMotionGate.h"

// CPageRestorationCompare dialog

IMPLEMENT_DYNAMIC(CPageRestorationCompare, CMFCPropertyPageEx)

CPageRestorationCompare::CPageRestorationCompare(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPageEx(IDP_RESTORATION_COMPARE_RESULT)
{
	m_nMotorIndex = 0;
}

CPageRestorationCompare::~CPageRestorationCompare()
{
}

void CPageRestorationCompare::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageRestorationCompare, CMFCPropertyPage)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_BACK, &CPageRestorationCompare::OnBnClickedBtnBack)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CPageRestorationCompare::OnBnClickedBtnNext)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MOTOR, &CPageRestorationCompare::OnTcnSelchangeTabMotor)
END_MESSAGE_MAP()


// CPageRestorationCompare message handlers


BOOL CPageRestorationCompare::OnInitDialog()
{
	CMFCPropertyPageEx::OnInitDialog();

	m_btnPrev.SubclassDlgItem(IDC_BTN_BACK, this);
	m_btnPrev.SetImage(IDB_BACK);

	m_btnNext.SubclassDlgItem(IDC_BTN_NEXT, this);
	m_btnNext.SetImage(IDB_CLOSE);
	
	

	if (1)
	{
		COLORREF colorArray[][2] = {
			//	Background, text 
			RGB(0x0a, 0x0a, 0x50),	RGB(0xc8, 0xc8, 0xc8),	//	Blue  // 
			RGB(0xf0, 0xf0, 0xf0),	RGB(0x00, 0x00, 0x00),	//	White
			RGB(0xff, 0x00, 0x00),  RGB(0x00, 0x00, 0x00),	//	Red
			RGB(0xc8, 0xc8, 0x00),  RGB(0x00, 0x00, 0x00),	//	Yellow
			RGB(0x1E, 0x96, 0xff),	RGB(0x00, 0x00, 0x00),  //  Blue2
			RGB(0x1E, 0x96, 0xff),  RGB(0x00, 0x00, 0x00),  // Sky
		};

		const INT nResInfo[wndMax][2] = {
			IDC_ST_TOTAL_ALL,				0,
			IDC_ST_COMMAND_ALL,				4, // 
			IDC_ST_POSITION_ALL,			2,
			IDC_ST_SPEED_ALL,				3,
			IDC_ST_TOTAL_ONE,				0,
			IDC_ST_COMMAND_ONE,				4, //
			IDC_ST_POSITION_ONE,			2,
			IDC_ST_SPEED_ONE,				3,
			IDC_ST_RESTORATION_ID,			1,
			IDC_ST_COMPARING_ID,			1,
			IDC_ST_CAPTION_COMMAND_ALL,		0,
			IDC_ST_CAPTION_POSITION_ALL,	0,
			IDC_ST_CAPTION_SPEED_ALL,		0,
			IDC_ST_CAPTION_COMMAND_ONE,		0,
			IDC_ST_CAPTION_POSITION_ONE,	0,
			IDC_ST_CAPTION_SPEED_ONE,		0,
			IDC_ST_CAPTION_RESTORATION_ID,	0,
			IDC_ST_CAPTION_COMPARING_ID,	0,
			IDC_ST_COMPARE_INFORM, 3,
		};

		for (INT nIndex = 0; nIndex < wndMax; nIndex++)
		{
			if (m_wndColor[nIndex].SubclassDlgItem(nResInfo[nIndex][0], this))
			{
				m_wndColor[nIndex].SetBkColor(colorArray[nResInfo[nIndex][1]][0]);
				m_wndColor[nIndex].SetTextColor(colorArray[nResInfo[nIndex][1]][1]);
			}
		}
		for (INT nIndex = 0; nIndex < 3; nIndex++)
		{
			
		}
		
		

		m_wndAlarmColor[0].SubclassDlgItem(IDC_STATIC_STYLE_BK, this);
		m_wndAlarmColor[0].SetBkColor(colorArray[5][0]);

		m_wndAlarmColor[1].SubclassDlgItem(IDC_STATIC_POSITION_BK, this);
		m_wndAlarmColor[1].SetBkColor(colorArray[2][0]);

		m_wndAlarmColor[2].SubclassDlgItem(IDC_STATIC_SPEED_BK, this);
		m_wndAlarmColor[2].SetBkColor(colorArray[3][0]);

	}
	
	CTabCtrl* pTabCtrl = static_cast<CTabCtrl*>(GetDlgItem(IDC_TAB_MOTOR));
	if (NULL == m_imgTab.GetSafeHandle())
	{
		m_imgTab.Create(IDB_FLAG_STATUS, 32, 1, RGB(0xc0, 0xc0, 0xc0));
	}

	pTabCtrl->SetImageList(&m_imgTab);
	CSheetRestoration* pParent = static_cast<CSheetRestoration*>(GetParent());
	INT nMotorCount = IsSystemControl() ? pParent->GetDataBaseMotorCount() : GetParent()->GetMotiongateCtrl()->GetMotorCount();  
	// Ribbon을 통해 들어온것이라면 백업파일에 존재하는 모터 개수를 가져오고, 운영화면을 통해 진행된 화면이라면 선택한 장치의 연결 모터 개수를 반환한다.
	for (INT nIndex = 0; nIndex < nMotorCount; nIndex++)
	{
		CString strTemp;
		strTemp.Format(_T("Motor #%d"), nIndex + 1);
		pTabCtrl->InsertItem(nIndex, strTemp, 0);
	}

	pTabCtrl->SetCurSel(m_nMotorIndex);

	InitializeListCtrl();

	InitControlTextSize();

	return TRUE;
}

void CPageRestorationCompare::InitializeListCtrl()
{
	CSheetRestoration* pParent = static_cast<CSheetRestoration*>(GetParent());
	CDeviceMotionGate* pDevice = pParent->GetMotiongateCtrl();


	INT nMotorCount = 0;
	CString strProjectName = _T("");

	if (1)
	{
		nMotorCount = IsSystemControl() ? pParent->GetDataBaseMotorCount() : pDevice->GetMotorCount();
		strProjectName = IsSystemControl() ? pParent->GetProjectName() : pDevice->GetSystemName();
	}

	if (m_wndList.SubclassDlgItem(IDC_LIST, this))
	{
		m_wndList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SINGLEROW | LVS_EX_DOUBLEBUFFER);
		SetWindowTheme(m_wndList.m_hWnd, _T("Explorer"), NULL);

		/*
		if (1)
		{// 기존
			INT nIndex = 0;
			m_wndList.InsertColumn(nIndex++, _T(""), 0, 30);	// Image Index
			m_wndList.InsertColumn(nIndex++, _T("PT No."), 0, 100);
			m_wndList.InsertColumn(nIndex++, _T("Restoration Position"), 0, 100);

			if (CSheetRestoration::method_compare_motor == pParent->GetMethod())
			{
				m_wndList.InsertColumn(nIndex++, _T("Current Position"), 0, 100);
			}
			else
			{
				m_wndList.InsertColumn(nIndex++, _T("Nomianl Position"), 0, 100);
			}

			 m_wndList.InsertColumn(nIndex++, _T("Restoration Speed"), 0, 100);

			if (CSheetRestoration::method_compare_motor == pParent->GetMethod())
			{
				m_wndList.InsertColumn(nIndex++, _T("Current Speed"), 0, 100);
			}
			else
			{
				m_wndList.InsertColumn(nIndex++, _T("Nominal Speed"), 0, 100);
			}
		}
		*/

		if (true)
		{// 수정본
			INT nIndex = 0;
			INT nWidth = 230;
			m_wndList.InsertColumn(nIndex++, _T(""), 0, -1);	// Image Index
			m_wndList.InsertColumn(nIndex++, _T("Style No."), 0, 50);

			// m_wndList.InsertColumn(nIndex++, _T("Nominal Position"), 0, nWidth);

			if (CSheetRestoration::method_compare_motor == pParent->GetMethod())
			{
				m_wndList.InsertColumn(nIndex++, _T("Selected Position"), 0, nWidth);
			}
			else
			{
				m_wndList.InsertColumn(nIndex++, _T("Nominal Position"), 0, nWidth);
			}

			if (CSheetRestoration::method_compare_motor == pParent->GetMethod())
			{
				m_wndList.InsertColumn(nIndex++, _T("Current Tool Position"), 0, nWidth);
			}
			else
			{
				m_wndList.InsertColumn(nIndex++, _T("Selected Position"), 0, nWidth);
			}

			//m_wndList.InsertColumn(nIndex++, _T("Nominal Speed"), 0, nWidth);

			if (CSheetRestoration::method_compare_motor == pParent->GetMethod())
			{
				m_wndList.InsertColumn(nIndex++, _T("Selected Speed"), 0, nWidth);
			}
			else
			{
				m_wndList.InsertColumn(nIndex++, _T("Nominal Speed"), 0, nWidth);
			}


			if (CSheetRestoration::method_compare_motor == pParent->GetMethod())
			{
				m_wndList.InsertColumn(nIndex++, _T("Current Tool Speed"), 0, nWidth);
			}
			else
			{
				m_wndList.InsertColumn(nIndex++, _T("Selected Speed"), 0, nWidth);
			}
		}

		if (NULL == m_imgList.GetSafeHandle())
		{
			m_imgList.Create(IDB_DEVICE_STATUS, 16, 1, RGB(0xff, 0xff, 0xff));
		}

		m_wndList.SetImageList(&m_imgList, LVSIL_NORMAL | LVSIL_SMALL);

		m_wndList.LoadHeaderItemInfo();

		m_nRestorationID = pParent->GetProjectBackupID();
		m_nCompareID = pParent->GetCompareBackupID();
	}

	CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();
	// CString strSystemName(pDevice->GetSystemName()); 기존..
	CString strSystemName = strProjectName;
	if (1)
	{
		::ZeroMemory(&m_stParamBackup, sizeof(m_stParamBackup));

		/*for (INT i = 0; i < nMotorCount; i++)
		{
			LONG lPos = m_stParamBackup[1].nodeItem[i].lPosition;
			if (lPos == -1)
			{
				BOOL bCheck = FALSE;
			}
		}*/

		if (pProjectDB->LoadMotorParameter(strSystemName, m_nRestorationID) && pProjectDB->SeekToBegin())
		{
			while (CDataBaseRecordData* pDataRecord = pProjectDB->SeekToNext())
			{
				INT nMotorID, nParamIndex, nParamValue;
				if (pDataRecord->GetIntergerData(_T("Motor_ID"), nMotorID) &&
					pDataRecord->GetIntergerData(_T("Parameter_Index"), nParamIndex) &&
					pDataRecord->GetIntergerData(_T("Parameter_Value"), nParamValue))
				{
					const INT nCurMotorID = nMotorID - 1;

					const INT nTableIndex = nParamIndex / 1000;
					const INT nIndex = nParamIndex % 1000;

					switch (nTableIndex)
					{
					case 0:
					{ // 파라미터
						m_stParamBackup[nCurMotorID].lParameter[nIndex] = static_cast<LONG>(nParamValue);
					}	break;

					default:
					{ // 포지션 테이블
						m_stParamBackup[nCurMotorID].nodeItem[nTableIndex - 1].wBuffer[nIndex] = static_cast<WORD>(nParamValue);
					}	break;
					}
				}
			}
		}

		/*for (INT i = 0; i < nMotorCount; i++)
		{
			LONG lPos = m_stParamBackup[1].nodeItem[i].lPosition;
			if (lPos == -1)
			{
				BOOL bCheck = FALSE;
			}
		}*/


	}

	switch (pParent->GetMethod())
	{
	case CSheetRestoration::method_compare_motor:
	{
		//	Fill Comparing ID
		::ZeroMemory(&m_stParamCompare, sizeof(m_stParamCompare)); // 현재 연결된 모터값
		
		const INT nCount = nMotorCount;

		for (INT nIndex = 0; nIndex < nCount; nIndex++)
		{
			pDevice->GetParameter(nIndex + 1, &m_stParamCompare[nIndex]);
		}

	}	break;

	case CSheetRestoration::method_compare_backup:
	{
		//	Fill Comparing ID
		::ZeroMemory(&m_stParamCompare, sizeof(m_stParamCompare));
		if (pProjectDB->LoadMotorParameter(strSystemName, m_nCompareID) && pProjectDB->SeekToBegin())
		{
			while (CDataBaseRecordData* pDataRecord = pProjectDB->SeekToNext())
			{
				INT nMotorID, nParamIndex, nParamValue;
				if (pDataRecord->GetIntergerData(_T("Motor_ID"), nMotorID) &&
					pDataRecord->GetIntergerData(_T("Parameter_Index"), nParamIndex) &&
					pDataRecord->GetIntergerData(_T("Parameter_Value"), nParamValue))
				{
					const INT nCurMotorID = nMotorID - 1;
					const INT nTableIndex = nParamIndex / 1000;
					const INT nIndex = nParamIndex % 1000;

					switch (nTableIndex)
					{
					case 0:
					{
						m_stParamCompare[nCurMotorID].lParameter[nIndex] = static_cast<LONG>(nParamValue);
					}	break;

					default:
					{
						m_stParamCompare[nCurMotorID].nodeItem[nTableIndex - 1].wBuffer[nIndex] = static_cast<WORD>(nParamValue);
					}	break;
					}
				}
			}
		}
	}	break;
	}

	if (1)
	{

		CTabCtrl* pTabCtrl = static_cast<CTabCtrl*>(GetDlgItem(IDC_TAB_MOTOR));
		INT nTotalCommandCount = 0, nTotalPositionCount = 0, nTotalSpeedCount = 0;
		// const INT nCount = pDevice->GetMotorCount(); 기존..
		const INT nCount = nMotorCount;
		for (INT nMotorIndex = 0; nMotorIndex < nCount; nMotorIndex++)
		{
			INT nCommandCount = 0, nPositionCount = 0, nSpeedCount = 0, nCommandErrorCount = 0;
			for (INT nIndex = 0; nIndex < 31; nIndex++)
			{
				/* 기존... position 과 speed가 겹칠경우 CompareBackupData는 diff_positoin만 Count한다.
				switch (CompareBackupData(nMotorIndex, nIndex))
				{
				case diff_command:		nCommandCount++;	break;
				case diff_positoin:		nPositionCount++;	break;
				case diff_speed:		nSpeedCount++;		break;
				}
				*/

				if (sts_abs_normotion != GetParamBackupCommandStatus(nMotorIndex, nIndex))
				{ // abs normal motion 이 아닌경우 해당 Index의 모든값 -1처리..
					for (INT i = 0; i < 32; i++)
					{
						m_stParamBackup[nMotorIndex].nodeItem[nIndex].wBuffer[i] = -1;
					}
				}

				if (sts_abs_normotion != GetParamCompareCommandStatus(nMotorIndex, nIndex))
				{ // abs normal motion 이 아닌경우 해당 Index의 모든값 -1처리..
					for (INT i = 0; i < 32; i++)
					{
						m_stParamCompare[nMotorIndex].nodeItem[nIndex].wBuffer[i] = -1;
					}
				}

				
				if(true)
				{
					BOOL bCommandCountUP = FALSE;
					// 둘중 하나가 abs normal motion이 아닐경우 command 값이 서로 다르기에 Count 진행됨.
					
					if (diff_command == CompareBackupCommand(nMotorIndex, nIndex))
					{
						bCommandCountUP = TRUE;
					}

					if (diff_positoin == CompareBackupPostion(nMotorIndex, nIndex))
					{
						nPositionCount++;
						bCommandCountUP = TRUE;
					}

					if (diff_speed == CompareBackupSpeed(nMotorIndex, nIndex))
					{
						nSpeedCount++;
						bCommandCountUP = TRUE;
					}
					
					if (bCommandCountUP)
					{
						nCommandCount++;
					}
					
					
				}
				
			}

			TCITEM item = { 0, };
			item.mask = TCIF_IMAGE;
			if (pTabCtrl->GetItem(nMotorIndex, &item))
			{
				if (nCommandCount || nPositionCount)
				{
					item.iImage = 2;
				}
				else if (nSpeedCount)
				{
					item.iImage = 1;
				}
				else
				{
					item.iImage = 0;
				}
				pTabCtrl->SetItem(nMotorIndex, &item);
			}

			nTotalCommandCount += nCommandCount;
			nTotalPositionCount += nPositionCount;
			nTotalSpeedCount += nSpeedCount;
		}

		SetErrorDataBkColor(nTotalCommandCount, nTotalPositionCount, nTotalSpeedCount, TRUE);

		CString strText;
		strText.Format(_T("Unmatched data from all motors : %d"), nTotalCommandCount + nTotalPositionCount + nTotalSpeedCount);
		SetDlgItemText(IDC_ST_TOTAL_ALL, strText);

		strText.Format(_T("%d "), nTotalCommandCount);
		SetDlgItemText(IDC_ST_COMMAND_ALL, strText);

		strText.Format(_T("%d "), nTotalPositionCount);
		SetDlgItemText(IDC_ST_POSITION_ALL, strText);

		strText.Format(_T("%d "), nTotalSpeedCount);
		SetDlgItemText(IDC_ST_SPEED_ALL, strText);
	}
}

void CPageRestorationCompare::LoadMotorCompareData(const INT nMotorIndex)
{
	m_wndList.RemoveAll();


	//INT nSelectedPos = 2, nNominalPos = 3, nSelectedSpd = 4, nNominalSpd = 5; //기존..
	//INT nSelectedPos, nNominalPos, nSelectedSpd, nNominalSpd;

	CSheetRestoration* pParent = static_cast<CSheetRestoration*>(GetParent());
	INT nCommandCount = 0, nPositoinCount = 0, nSpeedCount = 0;
	INT nSelectedPos, nNominalPos, nSelectedSpd, nNominalSpd;

	m_wndList.RemoveAll();

	//INT nSelectedPos = 2, nNominalPos = 3, nSelectedSpd = 4, nNominalSpd = 5; //기존..
	if (CSheetRestoration::method_compare_motor == pParent->GetMethod())
	{
		nSelectedPos = 2, nNominalPos = 3, nSelectedSpd = 4, nNominalSpd = 5; // Compare Current System
	}
	else
	{
		nSelectedPos = 3, nNominalPos = 2, nSelectedSpd = 5, nNominalSpd = 4; // Compare File
	}

	const COLORREF colorRed = RGB(0xff, 0x00, 0x00);
	const COLORREF colorYellow = RGB(0xc8, 0xc8, 0x00);
	const COLORREF colorBlack = RGB(0x00, 0x00, 0x00);
	const COLORREF colorWhite = RGB(0xff, 0xff, 0xff);
	const COLORREF colorBlue = RGB(0x00, 0x00, 0xff);
	//const COLORREF colorSky = RGB(0xbf, 0xe4, 0xff);
	const COLORREF colorSky = RGB(0x1E, 0x96, 0xff);

	CString strData;
	
	for (INT nIndex = 0; nIndex < 31; nIndex++)
	{
		m_wndList.InsertItem(nIndex, _T(""));
		strData.Format(_T("%d"), nIndex + 1);
		m_wndList.SetItemText(nIndex, 1, strData);

		/* 기존 ..
		if (static_cast<WORD>(0) != m_stParamBackup[nMotorIndex].nodeItem[nIndex].wCommand &&
			static_cast<WORD>(-1) != m_stParamBackup[nMotorIndex].nodeItem[nIndex].wCommand &&
			static_cast<WORD>(0x0a) != m_stParamBackup[nMotorIndex].nodeItem[nIndex].wCommand)
		{
			strData.Format(_T("%.2f"), ConvertPulseToMillimeter(&m_stParamBackup[nMotorIndex], m_stParamBackup[nMotorIndex].nodeItem[nIndex].lPosition));
			m_wndList.SetItemText(nIndex, nSelectedPos, strData);

			strData.Format(_T("%.2f"), ConvertPulseToMillimeter(&m_stParamBackup[nMotorIndex], m_stParamBackup[nMotorIndex].nodeItem[nIndex].dwMoveSpd));
			m_wndList.SetItemText(nIndex, nSelectedSpd, strData);
		}

		if (static_cast<WORD>(0) != m_stParamCompare[nMotorIndex].nodeItem[nIndex].wCommand &&
			static_cast<WORD>(-1) != m_stParamCompare[nMotorIndex].nodeItem[nIndex].wCommand &&
			static_cast<WORD>(0x0a) != m_stParamCompare[nMotorIndex].nodeItem[nIndex].wCommand)
		{
			strData.Format(_T("%.2f"), ConvertPulseToMillimeter(&m_stParamCompare[nMotorIndex], m_stParamCompare[nMotorIndex].nodeItem[nIndex].lPosition));
			m_wndList.SetItemText(nIndex, nNominalPos, strData);

			strData.Format(_T("%.2f"), ConvertPulseToMillimeter(&m_stParamCompare[nMotorIndex], m_stParamCompare[nMotorIndex].nodeItem[nIndex].dwMoveSpd));
			m_wndList.SetItemText(nIndex, nNominalSpd, strData);
		}
		*/

		/*
		if (sts_abs_normotion == GetParamBackupCommandStatus(nMotorIndex, nIndex))
		{
			strData.Format(_T("%.2f"), ConvertPulseToMillimeter(&m_stParamBackup[nMotorIndex], m_stParamBackup[nMotorIndex].nodeItem[nIndex].lPosition));
			m_wndList.SetItemText(nIndex, nSelectedPos, strData);

			strData.Format(_T("%.2f"), ConvertPulseToMillimeter(&m_stParamBackup[nMotorIndex], m_stParamBackup[nMotorIndex].nodeItem[nIndex].dwMoveSpd));
			m_wndList.SetItemText(nIndex, nSelectedSpd, strData);
		}
		else
		{
			strData.Format(_T("Command Error"));
			m_wndList.SetItemText(nIndex, nSelectedPos, strData);

			strData.Format(_T("Command Error"));
			m_wndList.SetItemText(nIndex, nSelectedSpd, strData);
		}

		if (sts_abs_normotion == GetParamCompareCommandStatus(nMotorIndex, nIndex))
		{
			strData.Format(_T("%.2f"), ConvertPulseToMillimeter(&m_stParamCompare[nMotorIndex], m_stParamCompare[nMotorIndex].nodeItem[nIndex].lPosition));
			m_wndList.SetItemText(nIndex, nNominalPos, strData);

			strData.Format(_T("%.2f"), ConvertPulseToMillimeter(&m_stParamCompare[nMotorIndex], m_stParamCompare[nMotorIndex].nodeItem[nIndex].dwMoveSpd));
			m_wndList.SetItemText(nIndex, nNominalSpd, strData);
		}
		else
		{
			strData.Format(_T("Command Error"));
			m_wndList.SetItemText(nIndex, nNominalPos, strData);

			strData.Format(_T("Command Error"));
			m_wndList.SetItemText(nIndex, nNominalSpd, strData);
		}

		*/
		if (true)
		{ 
			if (sts_abs_normotion == GetParamBackupCommandStatus(nMotorIndex, nIndex))
			{ // 기준..
			  // sts_abs_normotion 데이터일 경우만 값 기재
				strData.Format(_T("%.2f"), ConvertPulseToMillimeter(&m_stParamBackup[nMotorIndex], m_stParamBackup[nMotorIndex].nodeItem[nIndex].lPosition));
				m_wndList.SetItemText(nIndex, nSelectedPos, strData);

				strData.Format(_T("%.2f"), ConvertPulseToMillimeter(&m_stParamBackup[nMotorIndex], m_stParamBackup[nMotorIndex].nodeItem[nIndex].dwMoveSpd));
				m_wndList.SetItemText(nIndex, nSelectedSpd, strData);
			}
			else
			{ // reset 데이터일 경우
				strData.Format(_T("-"));
				m_wndList.SetItemText(nIndex, nSelectedPos, strData);
				m_wndList.SetItemText(nIndex, nSelectedSpd, strData);
			}

			if (sts_abs_normotion == GetParamCompareCommandStatus(nMotorIndex, nIndex))
			{ // 대상..
			  // sts_abs_normotion 데이터일 경우만 값 기재
				strData.Format(_T("%.2f"), ConvertPulseToMillimeter(&m_stParamCompare[nMotorIndex], m_stParamCompare[nMotorIndex].nodeItem[nIndex].lPosition));
				m_wndList.SetItemText(nIndex, nNominalPos, strData);

				strData.Format(_T("%.2f"), ConvertPulseToMillimeter(&m_stParamCompare[nMotorIndex], m_stParamCompare[nMotorIndex].nodeItem[nIndex].dwMoveSpd));
				m_wndList.SetItemText(nIndex, nNominalSpd, strData);
			}
			else
			{ // reset 데이터일 경우
				strData.Format(_T("-"));
				m_wndList.SetItemText(nIndex, nNominalPos, strData);
				m_wndList.SetItemText(nIndex, nNominalSpd, strData);
			}
		}
		
		if (true)
		{ // 값 비교 및 인덱스 칸 채우기
			BOOL bCommandCountUp = FALSE;

			if (diff_command == CompareBackupCommand(nMotorIndex, nIndex))
			{ // Command값이 다를때..
				bCommandCountUp = TRUE;
			}

			if (m_wndList.GetItemText(nIndex, nSelectedPos) != m_wndList.GetItemText(nIndex, nNominalPos))
			{ // Position 비교
				bCommandCountUp = TRUE; // Command값이 같을수 있으나, Position이 다를경우 Style Error Count 값을 Up 시켜줘야 한다.
				
				m_wndList.SetCellBkColor(nIndex, 2, colorRed);
				m_wndList.SetCellBkColor(nIndex, 3, colorRed);

				nPositoinCount++;
			}

			if (m_wndList.GetItemText(nIndex, nSelectedSpd) != m_wndList.GetItemText(nIndex, nNominalSpd))
			{ // Speed 비교
				
				bCommandCountUp = TRUE;// Command값이 같을수 있으나, Speed가 다를경우 Style Error Count 값을 Up 시켜줘야 한다.

				m_wndList.SetCellBkColor(nIndex, 4, colorYellow);
				m_wndList.SetCellBkColor(nIndex, 5, colorYellow);

				nSpeedCount++;
			}

			if (bCommandCountUp)
			{
				nCommandCount++;
				m_wndList.SetCellBkColor(nIndex, 1, colorSky);
			}
		}

		

		INT nImageIndex = diff_none;

		if (diff_positoin == CompareBackupCommand(nMotorIndex, nIndex))
		{
			nImageIndex = diff_command;
		}
		else if (diff_positoin == CompareBackupPostion(nMotorIndex, nIndex))
		{
			nImageIndex = diff_positoin;
		}
		else if (diff_speed == CompareBackupSpeed(nMotorIndex, nIndex))
		{
			nImageIndex = diff_speed;
		}

		nImageIndex = diff_none;

		m_wndList.SetItem(nIndex, 0, LVIF_IMAGE | LVIF_TEXT, NULL, nImageIndex, 0, 0, NULL);
		/* 기존..
		
		INT nImageIndex = CompareBackupData(nMotorIndex, nIndex);

		switch (nImageIndex)
		{
		case diff_command:
		{
			nCommandCount++;

			m_wndList.SetCellBkColor(nIndex, 1, colorRed);
		}	break;

		case diff_positoin:
		{
			
			해당 인덱스의 비교위치가 다를경우 해당 Column에는 색칠하지만.. 만약 speed 도 다를경우 거치지 않음..
			//nPositoinCount++;
			//m_wndList.SetCellBkColor(nIndex, 2, colorRed);
			//m_wndList.SetCellBkColor(nIndex, 3, colorRed);
		}	break;

		case diff_speed:
		{
			
			 nSpeedCount++; //diff_position에 영향받아 아예 검사를 안함..
			//m_wndList.SetCellBkColor(nIndex, 4, colorYellow);
			//m_wndList.SetCellBkColor(nIndex, 5, colorYellow);
		}	break;
		}
		*/
		
	}

	SetErrorDataBkColor(nCommandCount, nPositoinCount, nSpeedCount, FALSE);

	CString strText;
	strText.Format(_T("Unmatched data from motor #%d : %d"), m_nMotorIndex + 1, nCommandCount + nPositoinCount + nSpeedCount);
	SetDlgItemText(IDC_ST_TOTAL_ONE, strText);

	strText.Format(_T("%d "), nCommandCount);
	SetDlgItemText(IDC_ST_COMMAND_ONE, strText);

	strText.Format(_T("%d "), nPositoinCount);
	SetDlgItemText(IDC_ST_POSITION_ONE, strText);

	strText.Format(_T("%d "), nSpeedCount);
	SetDlgItemText(IDC_ST_SPEED_ONE, strText);

	m_wndList.Invalidate(FALSE);
}

void CPageRestorationCompare::OnDestroy()
{
	m_wndList.SaveHeaderItemInfo();

	CMFCPropertyPageEx::OnDestroy();
}


BOOL CPageRestorationCompare::OnSetActive()
{
	CSheetRestoration* pParent = static_cast<CSheetRestoration*>(GetParent());

	CString str = pParent->GetProjectTimeStamp();
	SetDlgItemText(IDC_ST_RESTORATION_ID, pParent->GetProjectTimeStamp());

	m_nRestorationID = pParent->GetProjectBackupID();

	if (CSheetRestoration::method_compare_motor == pParent->GetMethod())
	{
		GetDlgItem(IDC_ST_COMPARING_ID)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ST_CAPTION_COMPARING_ID)->ShowWindow(SW_HIDE);
		SetDlgItemText(IDC_ST_COMPARE_INFORM, _T("'Selection TimeStamp' is the file you selected in the previous step."));
	}
	else
	{
		GetDlgItem(IDC_ST_COMPARING_ID)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ST_CAPTION_COMPARING_ID)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ST_COMPARE_INFORM)->ShowWindow(SW_HIDE);

		// 기존.. SetDlgItemText(IDC_ST_COMPARING_ID, pParent->GetCompareTimeStamp());
		SetDlgItemText(IDC_ST_CAPTION_COMPARING_ID, _T("Nominal TimeStamp"));
		SetDlgItemText(IDC_ST_COMPARING_ID, pParent->GetNominalTimeStamp());

		m_nCompareID = pParent->GetCompareBackupID();
	}

	this->LoadMotorCompareData(m_nMotorIndex);

	return CMFCPropertyPageEx::OnSetActive();
}


void CPageRestorationCompare::OnBnClickedBtnBack()
{
	ActivatePrevPage();
}


void CPageRestorationCompare::OnBnClickedBtnNext()
{
	//	ActivateNextPage();
	OnCancel();
}


void CPageRestorationCompare::OnTcnSelchangeTabMotor(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	m_nMotorIndex = static_cast<CTabCtrl*>(GetDlgItem(IDC_TAB_MOTOR))->GetCurSel();

	LoadMotorCompareData(m_nMotorIndex);
}

const INT CPageRestorationCompare::CompareBackupData(const INT nMotorIndex, const INT nIndex)
{ // 사용X
	const BOOL bBackupCommandEnabled = (
		static_cast<WORD>(0) != m_stParamBackup[nMotorIndex].nodeItem[nIndex].wCommand &&
		static_cast<WORD>(-1) != m_stParamBackup[nMotorIndex].nodeItem[nIndex].wCommand &&
		static_cast<WORD>(0x0a) != m_stParamBackup[nMotorIndex].nodeItem[nIndex].wCommand);

	const BOOL bCompareCommandEnabled = (static_cast<WORD>(0) != m_stParamCompare[nMotorIndex].nodeItem[nIndex].wCommand &&
		static_cast<WORD>(-1) != m_stParamCompare[nMotorIndex].nodeItem[nIndex].wCommand &&
		static_cast<WORD>(0x0a) != m_stParamCompare[nMotorIndex].nodeItem[nIndex].wCommand);

	//if (static_cast<WORD>(3) != m_stParamBackup[nMotorIndex].nodeItem[nIndex].wCommand ||
	//	static_cast<WORD>(3) != m_stParamCompare[nMotorIndex].nodeItem[nIndex].wCommand )
	//{// stCommand값이 2가 아닐경우는 무조건 값을 현시해주면 안됨(이재우 차장 요청 사항 )
	//	return error_command;
	//}
	/*if (bBackupCommandEnabled != bCompareCommandEnabled)
	{
		return diff_command;
	}*/
	if (m_stParamBackup[nMotorIndex].nodeItem[nIndex].wCommand != m_stParamCompare[nMotorIndex].nodeItem[nIndex].wCommand)
	{
		return diff_command;
	}
	else if (m_stParamBackup[nMotorIndex].nodeItem[nIndex].lPosition != m_stParamCompare[nMotorIndex].nodeItem[nIndex].lPosition)
	{
		return diff_positoin;
	}
	else if (m_stParamBackup[nMotorIndex].nodeItem[nIndex].dwMoveSpd != m_stParamCompare[nMotorIndex].nodeItem[nIndex].dwMoveSpd)
	{
		return diff_speed;
	}


	return diff_none;
}

const INT CPageRestorationCompare::CompareBackupCommand(const INT nMotorIndex, const INT nIndex)
{
	if (m_stParamBackup[nMotorIndex].nodeItem[nIndex].wCommand != m_stParamCompare[nMotorIndex].nodeItem[nIndex].wCommand)
	{
		return diff_positoin;
	}

	return diff_none;
}

const INT CPageRestorationCompare::CompareBackupPostion(const INT nMotorIndex, const INT nIndex)
{
	if (m_stParamBackup[nMotorIndex].nodeItem[nIndex].lPosition != m_stParamCompare[nMotorIndex].nodeItem[nIndex].lPosition)
	{
		return diff_positoin;
	}

	return diff_none;
}

const INT CPageRestorationCompare::CompareBackupSpeed(const INT nMotorIndex, const INT nIndex)
{
	if (m_stParamBackup[nMotorIndex].nodeItem[nIndex].dwMoveSpd != m_stParamCompare[nMotorIndex].nodeItem[nIndex].dwMoveSpd)
	{
		return diff_speed;
	}

	return diff_none;
}


const DOUBLE CPageRestorationCompare::ConvertPulseToMillimeter(CDeviceMotionGate::MOTOR_PARAMETER_DATA* pstParameter, const LONG lPosition)
{
	const DOUBLE dbResolution[] = {
			500,	//	index = 0
			500,	//	index = 1
			1000,	//	index = 2
			1600,	//	index = 3
			2000,	//	index = 4
			3600,	//	index = 5
			5000,	//	index = 6
			6400,	//	index = 7
			7200,	//	index = 8
			10000	//	index = 9
	};

	const DOUBLE dbDistance = static_cast<DOUBLE>(lPosition) * pstParameter->lParameter[31] / (dbResolution[pstParameter->lParameter[0]] * pstParameter->lParameter[32]);

	return dbDistance;
}

const LONG CPageRestorationCompare::ConvertMillimeterToPulse(CDeviceMotionGate::MOTOR_PARAMETER_DATA* pstParameter, const DOUBLE dbDistance)
{
	const DOUBLE dbResolution[] = {
			500,	//	index = 0
			500,	//	index = 1
			1000,	//	index = 2
			1600,	//	index = 3
			2000,	//	index = 4
			3600,	//	index = 5
			5000,	//	index = 6
			6400,	//	index = 7
			7200,	//	index = 8
			10000	//	index = 9
	};

	const LONG lPosition = static_cast<LONG>(dbDistance * dbResolution[pstParameter->lParameter[0]] * pstParameter->lParameter[32] / pstParameter->lParameter[31]);

	return lPosition;
}

const BOOL CPageRestorationCompare::IsSystemControl()
{
	return ((CMainFrame*)AfxGetMainWnd())->IsSystemCompare();
}

const INT CPageRestorationCompare::GetParamBackupCommandStatus(INT nMotorIndex, INT nIndex)
{
	INT nResult = sts_reset;
	
	for (INT status = sts_abs_lowspeed; status < sts_clear_position; status++)
	{
		if (static_cast<WORD>(status) == m_stParamBackup[nMotorIndex].nodeItem[nIndex].wCommand)
		{
			nResult = status;
		}
	}
	
	return nResult;
}

const INT CPageRestorationCompare::GetParamCompareCommandStatus(INT nMotorIndex, INT nIndex)
{
	INT nResult = sts_reset;

	for (INT status = sts_abs_lowspeed; status < sts_clear_position; status++)
	{
		if (static_cast<WORD>(status) == m_stParamCompare[nMotorIndex].nodeItem[nIndex].wCommand)
		{
			nResult = status;
		}
	}

	return nResult;
}

void CPageRestorationCompare::SetErrorDataBkColor(INT nStyleErrorCnt, INT nPositionErrorCnt, INT nSpeedErrorCnt, BOOL bTotal)
{
	COLORREF red = RGB(0xff, 0x00, 0x00);
	COLORREF white = RGB(0xf0, 0xf0, 0xf0);
	COLORREF black = RGB(0x00, 0x00, 0x00);

	enum _STATIC_INDEX
	{
		all_command = 1,
		all_position,
		all_speed,
		all_max,
		one_command,
		one_position,
		one_speed,
		one_max
	};
	
	if (TRUE == bTotal)
	{ // 1, 2, 3
		for (INT nIndex = all_command; nIndex < all_max; nIndex++)
		{
			COLORREF bkColor = white;
			
			if (0 != nStyleErrorCnt && all_command == nIndex) bkColor = red;
			if (0 != nPositionErrorCnt && all_position == nIndex) bkColor = red;
			if (0 != nSpeedErrorCnt && all_speed == nIndex) bkColor = red;
			
			m_wndColor[nIndex].SetBkColor(bkColor);
			m_wndColor[nIndex].SetTextColor(black);
		}
	}
	else
	{ // 5, 6, 7
		for (INT nIndex = one_command; nIndex < one_max; nIndex++)
		{
			COLORREF bkColor = white;

			if (0 != nStyleErrorCnt && one_command == nIndex) bkColor = red;
			if (0 != nPositionErrorCnt && one_position == nIndex) bkColor = red;
			if (0 != nSpeedErrorCnt && one_speed == nIndex) bkColor = red;

			m_wndColor[nIndex].SetBkColor(bkColor);
			m_wndColor[nIndex].SetTextColor(black);
		}
	}
}


void CPageRestorationCompare::InitControlTextSize()
{
	if (true)
	{
		LOGFONT lf = { 0, };
		afxGlobalData.fontRegular.GetLogFont(&lf);

		lf.lfHeight = -17;
		m_BigFont.CreateFontIndirect(&lf);

		GetDlgItem(IDC_ST_TOTAL_ALL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_CAPTION_COMMAND_ALL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_CAPTION_POSITION_ALL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_CAPTION_SPEED_ALL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_COMMAND_ALL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_POSITION_ALL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_SPEED_ALL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_TOTAL_ONE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_CAPTION_COMMAND_ONE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_CAPTION_POSITION_ONE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_CAPTION_SPEED_ONE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_COMMAND_ONE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_POSITION_ONE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_SPEED_ONE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_CAPTION_RESTORATION_ID)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_RESTORATION_ID)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_CAPTION_COMPARING_ID)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_COMPARING_ID)->SetFont(&m_BigFont);
		GetDlgItem(IDC_TAB_MOTOR)->SetFont(&m_BigFont);
		GetDlgItem(IDC_LIST)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_BACK)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_NEXT)->SetFont(&m_BigFont);

		GetDlgItem(IDC_STATIC_STYLE_ERR)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_POSITION_ERR)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_SPEED_ERR)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}

	/*if (true)
	{
		m_BigFont.CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, TEXT_STYLE);

	GetDlgItem(IDC_ST_TOTAL_ALL)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_CAPTION_COMMAND_ALL)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_CAPTION_POSITION_ALL)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_CAPTION_SPEED_ALL)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_COMMAND_ALL)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_POSITION_ALL)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_SPEED_ALL)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_TOTAL_ONE)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_CAPTION_COMMAND_ONE)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_CAPTION_POSITION_ONE)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_CAPTION_SPEED_ONE)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_COMMAND_ONE)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_POSITION_ONE)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_SPEED_ONE)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_CAPTION_RESTORATION_ID)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_RESTORATION_ID)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_CAPTION_COMPARING_ID)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_COMPARING_ID)->SetFont(&m_BigFont);
	GetDlgItem(IDC_TAB_MOTOR)->SetFont(&m_BigFont);
	GetDlgItem(IDC_LIST)->SetFont(&m_BigFont);
	GetDlgItem(IDC_BTN_BACK)->SetFont(&m_BigFont);
	GetDlgItem(IDC_BTN_NEXT)->SetFont(&m_BigFont);

	GetDlgItem(IDC_STATIC_STYLE_ERR)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_POSITION_ERR)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_SPEED_ERR)->SetFont(&m_BigFont);

	m_BigFont.Detach();
	}*/

}
