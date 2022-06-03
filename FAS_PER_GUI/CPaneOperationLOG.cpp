// CPaneOperationLOG.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPaneOperationLOG.h"
#include "CDlgSaveLogData.h"
#include "MainFrm.h"

#define TIMER_PERIOD 5

// CLogData
CLogData::CLogData(LPVOID pParam)
{
	/*CMainFrame::PST_OPERAION_MESSAGE pMsg = reinterpret_cast<CMainFrame::PST_OPERAION_MESSAGE>(pParam);

	GetSystemTime(&m_timeStamp);

	m_nMsgType = pMsg->nLevel;
	pMsg->pDevice->GetAddress(m_strAddress);
	m_strName = pMsg->pDevice->GetSystemName();
	m_strMessage = pMsg->lpszMessage;*/
}

CLogData::~CLogData()
{

}

void CLogData::GetTimeStamp(CString& strTimeStamp)
{
	CTime time(m_timeStamp);
	// Format에 대한 고찰이 필요하다.
	switch (1)
	{
	case 1:
	{
		strTimeStamp = time.Format(_T("%Y-%m-%d, %H:%M:%S"));
	}	break;
	}
}

void CLogData::GetDeviceInformation(CString& strDeviceInfo)
{
	strDeviceInfo = m_strName + _T(" - ") + m_strAddress;
}

void CLogData::GetInformation(CString& strInformation)
{
	strInformation = m_strMessage;
}

// CPaneOperationLOG

IMPLEMENT_DYNAMIC(CPaneOperationLog, CDockablePane)

CPaneOperationLog::CPaneOperationLog()
{
	m_timer = { 0 };
}

CPaneOperationLog::~CPaneOperationLog()
{
	POSITION pos = m_lstLogData.GetHeadPosition();
	while (pos)
	{
		delete m_lstLogData.GetNext(pos);
	}
	m_lstLogData.RemoveAll();
	
}


BEGIN_MESSAGE_MAP(CPaneOperationLog, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CPaneOperationLOG 메시지 처리기
int CPaneOperationLog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (1)
	{
		//m_toolbar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PANE_LOG);
		m_toolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
		m_toolbar.LoadToolBar(IDR_PANE_LOG, 0, 0, TRUE /* 잠금 */);
		//m_toolbar.LoadToolBar(IDR_PANE_LOG,0,0);
		//INT nCount = m_toolbar.GetCount();
		m_toolbar.EnableTextLabels();
		//	m_toolbar.CleanUpLockedImages();
		//	m_toolbar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* 잠금 */);
		m_toolbar.SetPaneStyle(m_toolbar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
		m_toolbar.SetPaneStyle(m_toolbar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
		m_toolbar.SetOwner(this);

		// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
		m_toolbar.SetRouteCommandsViaFrame(FALSE);
	}

	if (1)
	{
		m_listCtrl.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL, CRect(10, 10, 150, 150), this, IDC_LIST);
		m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

		m_listCtrl.InsertColumn(0, _T("Time Stamp"), LVCFMT_LEFT, 200);
		m_listCtrl.InsertColumn(1, _T("Device"), LVCFMT_LEFT, 500);
		m_listCtrl.InsertColumn(2, _T("Information"), LVCFMT_LEFT, 500);

		m_listCtrl.LoadHeaderItemInfo();
	}

	if (1)
	{
		CFont font;
		font.CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, TEXT_STYLE);
		m_listCtrl.SetFont(&font);
		font.Detach();
	}

	m_pos = NULL;
	m_nLogStatus = LOG_ALL;
	m_nLogDeviceNaming = 0;
	m_nLogSelect = FALSE;
	return 0;
}


void CPaneOperationLog::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	INT nTop = 0, nLeft = 0;

	int cyTlb = 0;
	if (m_toolbar.GetSafeHwnd())
	{
		cyTlb = m_toolbar.CalcFixedLayout(FALSE, TRUE).cy;
		m_toolbar.SetWindowPos(&wndTop, nLeft, nTop, cx - nLeft, cyTlb, SWP_NOZORDER);
		nTop += cyTlb;
	}

	if (m_listCtrl.GetSafeHwnd())
	{
		m_listCtrl.SetWindowPos(&wndTop, nLeft, nTop, cx - nLeft, cy - nTop, SWP_NOZORDER);
	}
}

void CPaneOperationLog::InsertMessage(LPVOID pParam)
{
	CMainFrame::PST_OPERAION_MESSAGE pMsg = reinterpret_cast<CMainFrame::PST_OPERAION_MESSAGE>(pParam);

	CLogData* pLogData = new CLogData(pMsg);
	m_lstLogData.AddTail(pLogData);
	while( 100 <= m_lstLogData.GetCount())
	{
		delete m_lstLogData.GetHead();
		m_lstLogData.RemoveHead();
	}

	InsertMessageToListCtrl(pLogData);
}

void CPaneOperationLog::InsertMessageToListCtrl(LPVOID pParam)
{
	CLogData* pLogData = reinterpret_cast<CLogData*>(pParam);

	CString strData;
	pLogData->GetTimeStamp(strData);
	while (100 <= m_listCtrl.GetItemCount())
	{
		m_listCtrl.DeleteItem(0);
	}

	const INT nIndex = m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), strData);
	pLogData->GetDeviceInformation(strData);
	m_listCtrl.SetItemText(nIndex, 1, strData);
	pLogData->GetInformation(strData);
	m_listCtrl.SetItemText(nIndex, 2, strData);

	if (NULL == m_listCtrl.GetFirstSelectedItemPosition())
	{
		m_listCtrl.EnsureVisible(nIndex, FALSE);
	}
}


void CPaneOperationLog::OnDestroy()
{
	m_listCtrl.SaveHeaderItemInfo();

	CDockablePane::OnDestroy();
}


const BOOL CPaneOperationLog::SaveLogList(INT nLevel, CString strTime, CString strDeviceName, CString strDeviceIP, CString strInform, CTime time)
{
	OPERATIONLOG_INFORMATION inform;
	
	inform.nLevel = nLevel;
	inform.time = time;
	inform.strTime = strTime;
	inform.strDeviceName = strDeviceName;
	inform.strDeviceIP = strDeviceIP;
	inform.strInform = strInform;

	m_lstSystemLog.AddTail(inform); // List에 저장시킨다.
	//m_pos = m_lstSystemLog.GetTailPosition();

	if (inform.strInform != _T("Program Terminated"))
	{ // Program 종료시 에러 예외 처리
		ShowLogList();
	}
	

	return TRUE;
}

//const BOOL CPaneOperationLog::SaveLogListTest(OPERATIONLOG_INFORMATION inform)
//{
//	m_lstSystemLog.AddTail(inform);
//	m_pos = m_lstSystemLog.GetTailPosition();
//	ShowLogList();
//	return TRUE;
//}




void CPaneOperationLog::SetCurrentLogStatus(INT nRequest)
{
	BOOL bActive = TRUE;
	switch (nRequest)
	{
	case LOG_ALL:
		m_nLogSelect = FALSE;
	case LOG_WARNING: 
	case LOG_ERROR: 
		m_nLogStatus = nRequest;
		break;
	case LOG_SELECT: 
	{
		m_nLogSelect = TRUE; 
	}
		break;
	case LOG_NAME: 
	case LOG_IP: 
	case LOG_MIX:
		m_nLogDeviceNaming = nRequest;
		break;
	case LOG_TREE_SELECT:
	{
		m_nLogSelect = TRUE;
		/*if (m_nLogStatus == LOG_ALL)
		{
			bActive = FALSE;
		}*/
	}	break;


	default:
		break;
	}
	if (bActive) m_nLogSelect ? SetReShowSelectedLogList() : SetReShowAllLogList();

	//m_nLogSelect ? SetReShowSelectedLogList() : 0;
}

const BOOL CPaneOperationLog::ShowLogList()
{
	//INT nItemCount = m_listCtrl.GetItemCount();
	CString strData;
	INT nItemCount = 0;
	OPERATIONLOG_INFORMATION opInform = m_lstSystemLog.GetTail();
	
	CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	CDeviceMotionGate* pDevice = pFrame->GetOutlookVisible() ? pFrame->GetSelectedMotionGateControl() : pFrame->GetSelectedMotiongateCtrl();
	CString strSelectAddress; if(pDevice != NULL) pDevice->GetAddress(strSelectAddress);
	
	strData = opInform.strInform;
	
	if (LOG_ALL == m_nLogStatus && (m_nLogSelect ? opInform.strDeviceIP == strSelectAddress : TRUE))
	{ 
		m_listCtrl.InsertItem(nItemCount, opInform.strTime);

		if (m_nLogDeviceNaming < LOG_NAME || m_nLogDeviceNaming == LOG_MIX)
		{
			m_listCtrl.SetItemText(nItemCount, 1, opInform.strDeviceName == _T("Unknown.") ? opInform.strDeviceIP : opInform.strDeviceName);
		}
		else
		{
			m_listCtrl.SetItemText(nItemCount, 1, m_nLogDeviceNaming == LOG_NAME ? opInform.strDeviceName : opInform.strDeviceIP);
		}
		
		strData.Replace(_T("..."), _T("."));
		strData.Replace(_T("."), _T(""));
		m_listCtrl.SetItemText(nItemCount, 2, strData);
	}

	else if (opInform.nLevel == m_nLogStatus && (m_nLogSelect ? opInform.strDeviceIP == strSelectAddress : TRUE))
	{ // Warning or Error
		m_listCtrl.InsertItem(nItemCount, opInform.strTime);

		if (m_nLogDeviceNaming < LOG_NAME || m_nLogDeviceNaming == LOG_MIX)
		{
			m_listCtrl.SetItemText(nItemCount, 1, opInform.strDeviceName == _T("Unknown.") ? opInform.strDeviceIP : opInform.strDeviceName);
		}
		else
		{
			m_listCtrl.SetItemText(nItemCount, 1, m_nLogDeviceNaming == LOG_NAME ? opInform.strDeviceName : opInform.strDeviceIP);
		}
		strData.Replace(_T("..."), _T("."));
		strData.Replace(_T("."), _T(""));
		m_listCtrl.SetItemText(nItemCount, 2, strData);
	}

	return 0;
}

void CPaneOperationLog::SetReShowAllLogList()
{
	POSITION pos= m_lstSystemLog.GetHeadPosition();
	
	m_nLogSelect = FALSE;
	m_listCtrl.DeleteAllItems();

	// 추가.//
	m_pos = m_lstSystemLog.GetHeadPosition(); 
	if (!m_timer.bReAllList)
	{
		if (m_timer.bSelectList)
		{
			KillTimer(TIMER_SELECT);
			m_timer.bSelectList = FALSE;
		}
		SetTimer(TIMER_REALL, TIMER_PERIOD, NULL);
		m_timer.bReAllList = TRUE;
	}

	/*
	while (pos)
	{
		Inform = m_lstSystemLog.GetNext(pos);
		INT nItemCount = m_listCtrl.GetItemCount();
		nItemCount = 0;

		if (LOG_ALL ==  m_nLogStatus)
		{
			m_listCtrl.InsertItem(nItemCount, Inform.strTime);

			if (m_nLogDeviceNaming < LOG_NAME || m_nLogDeviceNaming == LOG_MIX)
			{
				//m_listCtrl.SetItemText(nItemCount, 1, Inform.strDeviceName == L"" ? Inform.strDeviceIP : Inform.strDeviceName);
				m_listCtrl.SetItemText(nItemCount, 1, (Inform.strDeviceName == L"" || Inform.strDeviceName == _T("Unknown.")) ? Inform.strDeviceIP : Inform.strDeviceName);
			}

			else
			{
				m_listCtrl.SetItemText(nItemCount, 1, m_nLogDeviceNaming == LOG_NAME ? Inform.strDeviceName : Inform.strDeviceIP);
			}

			if (nPos == 0)
			{
				m_listCtrl.SetItemText(nItemCount, 1, _T("SYSTEM"));
			}

			m_listCtrl.SetItemText(nItemCount, 2, Inform.strInform);
			nPos++;
		}

		else if (Inform.nLevel == m_nLogStatus)
		{ // Warining or Error 일 경우
			m_listCtrl.InsertItem(nItemCount, Inform.strTime);
			
			if (m_nLogDeviceNaming < LOG_NAME || m_nLogDeviceNaming == LOG_MIX)
			{
				//m_listCtrl.SetItemText(nItemCount, 1, Inform.strDeviceName == L"" ? Inform.strDeviceIP : Inform.strDeviceName);
				m_listCtrl.SetItemText(nItemCount, 1, (Inform.strDeviceName == L"" || Inform.strDeviceName == _T("Unknown.")) ? Inform.strDeviceIP : Inform.strDeviceName);
			}

			else
			{
				m_listCtrl.SetItemText(nItemCount, 1, m_nLogDeviceNaming == LOG_NAME ? Inform.strDeviceName : Inform.strDeviceIP);
			}

			m_listCtrl.SetItemText(nItemCount, 2, Inform.strInform);
			nPos++;
		}
	}
	*/
	
}

void CPaneOperationLog::ReShowAllLogList()
{
	OPERATIONLOG_INFORMATION Inform;
	
	INT nItemCount = m_listCtrl.GetItemCount();
	nItemCount = 0;
	INT nPos = 0;

	if (m_pos == NULL)
	{
		KillTimer(TIMER_REALL);
		m_timer.bReAllList = FALSE;
		return;
	}

	Inform = m_lstSystemLog.GetNext(m_pos);
	
	if (!(m_time > Inform.time))
	{
		if (LOG_ALL == m_nLogStatus)
		{
			m_listCtrl.InsertItem(nItemCount, Inform.strTime);

			if (m_nLogDeviceNaming < LOG_NAME || m_nLogDeviceNaming == LOG_MIX)
			{
				//m_listCtrl.SetItemText(nItemCount, 1, Inform.strDeviceName == L"" ? Inform.strDeviceIP : Inform.strDeviceName);
				m_listCtrl.SetItemText(nItemCount, 1, (Inform.strDeviceName == L"" || Inform.strDeviceName == _T("Unknown.")) ? Inform.strDeviceIP : Inform.strDeviceName);
			}

			else
			{
				m_listCtrl.SetItemText(nItemCount, 1, m_nLogDeviceNaming == LOG_NAME ? Inform.strDeviceName : Inform.strDeviceIP);
			}

			/*if (nPos == 0)
			{
				m_listCtrl.SetItemText(nItemCount, 1, _T("SYSTEM"));
			}*/

			m_listCtrl.SetItemText(nItemCount, 2, Inform.strInform);
			nPos++;
		}

		else if (Inform.nLevel == m_nLogStatus)
		{ // Warining or Error 일 경우
			m_listCtrl.InsertItem(nItemCount, Inform.strTime);

			if (m_nLogDeviceNaming < LOG_NAME || m_nLogDeviceNaming == LOG_MIX)
			{
				//m_listCtrl.SetItemText(nItemCount, 1, Inform.strDeviceName == L"" ? Inform.strDeviceIP : Inform.strDeviceName);
				m_listCtrl.SetItemText(nItemCount, 1, (Inform.strDeviceName == L"" || Inform.strDeviceName == _T("Unknown.")) ? Inform.strDeviceIP : Inform.strDeviceName);
			}

			else
			{
				m_listCtrl.SetItemText(nItemCount, 1, m_nLogDeviceNaming == LOG_NAME ? Inform.strDeviceName : Inform.strDeviceIP);
			}

			m_listCtrl.SetItemText(nItemCount, 2, Inform.strInform);
			nPos++;
		}

	}
	
	/*if (m_pos == NULL)
	{
		KillTimer(TIMER_REALL);
		m_timer.bReAllList = FALSE;
		return;
	}*/

	
}

void CPaneOperationLog::SetReShowSelectedLogList()
{
	POSITION pos = m_lstSystemLog.GetHeadPosition();
	m_pos = m_lstSystemLog.GetHeadPosition();
	m_listCtrl.DeleteAllItems();

	if (/*!m_timer.bSelectList*/ TRUE) // Tree에서 다른 디바이스 선택할경우 재 실행 해야함..
	{
		KillTimer(TIMER_SELECT);
		if (m_timer.bReAllList)
		{
			KillTimer(TIMER_REALL);
			m_timer.bReAllList = FALSE;
		}
			
		SetTimer(TIMER_SELECT, TIMER_PERIOD, NULL);
		m_timer.bSelectList = TRUE;
	}

	
	/*
	while (pos)
	{
		Inform = m_lstSystemLog.GetNext(pos);
		if (Inform.nLevel == LOG_ERROR)
		{
			CString str = Inform.strInform;
		}
		// 기존.. INT nItemCount = m_listCtrl.GetItemCount();
		INT nItemCount = 0;
		if (LOG_ALL == m_nLogStatus && strSelectAddress == Inform.strDeviceIP)
		{ // ALL 일 경우..
			m_listCtrl.InsertItem(nItemCount, Inform.strTime);

			if (m_nLogDeviceNaming < LOG_NAME || m_nLogDeviceNaming == LOG_MIX)
			{
				//m_listCtrl.SetItemText(nItemCount, 1, Inform.strDeviceName == L"" ? Inform.strDeviceIP : Inform.strDeviceName);
				m_listCtrl.SetItemText(nItemCount, 1, (Inform.strDeviceName == L"" || Inform.strDeviceName == _T("Unknown.")) ? Inform.strDeviceIP : Inform.strDeviceName);
			}

			else
			{
				m_listCtrl.SetItemText(nItemCount, 1, m_nLogDeviceNaming == LOG_NAME ? Inform.strDeviceName : Inform.strDeviceIP);
			}

			m_listCtrl.SetItemText(nItemCount, 2, Inform.strInform);
			nPos++;
		}

		else if (Inform.nLevel == m_nLogStatus && strSelectAddress == Inform.strDeviceIP)
		{ // Warining or Error 일 경우
			m_listCtrl.InsertItem(nItemCount, Inform.strTime);

			if (m_nLogDeviceNaming < LOG_NAME || m_nLogDeviceNaming == LOG_MIX)
			{
				//m_listCtrl.SetItemText(nItemCount, 1, Inform.strDeviceName == L"" ? Inform.strDeviceIP : Inform.strDeviceName);
				m_listCtrl.SetItemText(nItemCount, 1, (Inform.strDeviceName == L"" || Inform.strDeviceName == _T("Unknown.")) ? Inform.strDeviceIP : Inform.strDeviceName);
			}

			else
			{
				m_listCtrl.SetItemText(nItemCount, 1, m_nLogDeviceNaming == LOG_NAME ? Inform.strDeviceName : Inform.strDeviceIP);
			}

			m_listCtrl.SetItemText(nItemCount, 2, Inform.strInform);
			nPos++;
		}
	}
	*/
	

}


void CPaneOperationLog::ReShowSelectedLogList()
{
	OPERATIONLOG_INFORMATION Inform;
	CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	CDeviceMotionGate* pDevice = pFrame->GetSelectedMotiongateCtrl();
	CString strSelectAddress; pDevice->GetAddress(strSelectAddress);
	static INT nLog = 0;
	
	if (m_pos == NULL)
	{
		KillTimer(TIMER_SELECT);
		m_timer.bSelectList = FALSE;
		return;
	}

	Inform = m_lstSystemLog.GetNext(m_pos);

	// 기존.. INT nItemCount = m_listCtrl.GetItemCount();
	INT nItemCount = 0;
	if (!(m_time > Inform.time))
	{
		if (LOG_ALL == m_nLogStatus && strSelectAddress == Inform.strDeviceIP)
		{ // ALL 일 경우..
			m_listCtrl.InsertItem(nItemCount, Inform.strTime);

			if (m_nLogDeviceNaming < LOG_NAME || m_nLogDeviceNaming == LOG_MIX)
			{
				//m_listCtrl.SetItemText(nItemCount, 1, Inform.strDeviceName == L"" ? Inform.strDeviceIP : Inform.strDeviceName);
				m_listCtrl.SetItemText(nItemCount, 1, (Inform.strDeviceName == L"" || Inform.strDeviceName == _T("Unknown.")) ? Inform.strDeviceIP : Inform.strDeviceName);
			}

			else
			{
				m_listCtrl.SetItemText(nItemCount, 1, m_nLogDeviceNaming == LOG_NAME ? Inform.strDeviceName : Inform.strDeviceIP);
			}

			m_listCtrl.SetItemText(nItemCount, 2, Inform.strInform);
		}

		else if (Inform.nLevel == m_nLogStatus && strSelectAddress == Inform.strDeviceIP)
		{ // Warining or Error 일 경우
			m_listCtrl.InsertItem(nItemCount, Inform.strTime);

			if (m_nLogDeviceNaming < LOG_NAME || m_nLogDeviceNaming == LOG_MIX)
			{
				//m_listCtrl.SetItemText(nItemCount, 1, Inform.strDeviceName == L"" ? Inform.strDeviceIP : Inform.strDeviceName);
				m_listCtrl.SetItemText(nItemCount, 1, (Inform.strDeviceName == L"" || Inform.strDeviceName == _T("Unknown.")) ? Inform.strDeviceIP : Inform.strDeviceName);
			}

			else
			{
				m_listCtrl.SetItemText(nItemCount, 1, m_nLogDeviceNaming == LOG_NAME ? Inform.strDeviceName : Inform.strDeviceIP);
			}

			m_listCtrl.SetItemText(nItemCount, 2, Inform.strInform);
		}
	}
	
	
	/*if (m_pos == NULL)
	{
		KillTimer(TIMER_SELECT);
		m_timer.bSelectList = FALSE;
		return;
	}*/
}

BOOL CPaneOperationLog::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDockablePane::OnCommand(wParam, lParam);
}


void CPaneOperationLog::SaveLogData()
{
	CDlgSaveLogData dlg;
	
	POSITION pos = m_lstSystemLog.GetHeadPosition();
	while (pos)
	{
		OPERATIONLOG_INFORMATION* log = &m_lstSystemLog.GetNext(pos);
		dlg.SetList(log);
	}
	//dlg.SaveListData();
	//dlg.SetList(m_lstSystemLog);
	//OPERATIONLOG_INFORMATION

	if (IDOK == dlg.DoModal())
	{
		m_lstSystemLog.RemoveAll();
	}

	m_lstSystemLog.RemoveAll();
}





void CPaneOperationLog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nIDEvent)
	{
	case TIMER_REALL:
	{
		ReShowAllLogList();
	}	break;

	case TIMER_SELECT:
	{
		ReShowSelectedLogList();
	}	break;

	default:
		break;
	}


	CDockablePane::OnTimer(nIDEvent);
}



void CPaneOperationLog::RemoveList()
{
	m_lstSystemLog.RemoveAll();
	m_listCtrl.RemoveAll();
}


void CPaneOperationLog::SetLogTime()
{
	m_time = CTime::GetCurrentTime();
}
