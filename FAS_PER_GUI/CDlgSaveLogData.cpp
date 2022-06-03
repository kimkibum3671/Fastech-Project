// CDlgSaveLogData.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgSaveLogData.h"
#include "afxdialogex.h"
#include "CDataBaseManager.h"

// CDlgSaveLogData 대화 상자

IMPLEMENT_DYNAMIC(CDlgSaveLogData, CDialogEx)

#define TIMER_SAVE 0

CDlgSaveLogData::CDlgSaveLogData(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SAVE_LOG, pParent)
{

}

CDlgSaveLogData::~CDlgSaveLogData()
{
}

void CDlgSaveLogData::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
}


BEGIN_MESSAGE_MAP(CDlgSaveLogData, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgSaveLogData 메시지 처리기


void CDlgSaveLogData::SetList(LPVOID list)
{
	OPERATIONLOG_INFORMATION* pData = reinterpret_cast<OPERATIONLOG_INFORMATION*>(list);
	m_lstSystemLog.AddTail(*pData);
}


BOOL CDlgSaveLogData::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetTimer(TIMER_SAVE, 50, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDlgSaveLogData::SaveListData()
{
	
	if (1)
	{
		INT nPos = 0;
		INT nRange = m_lstSystemLog.GetCount();
		CString str = 0;

		CDataBaseLog* pBase = CDataBaseLog::GetInstance();

		m_Progress.SetRange(0, nRange);

		POSITION pos = m_lstSystemLog.GetHeadPosition();

		while (pos)
		{
			OPERATIONLOG_INFORMATION log = m_lstSystemLog.GetNext(pos);
			pBase->InsertLogData(log.nLevel, log.strInform, log.strDeviceName, log.strDeviceIP);
			m_Progress.SetPos(++nPos);
			str.Format(_T("Saving Log Data... ( %d / %d )"), nPos, m_lstSystemLog.GetCount());
			SetDlgItemText(IDC_STATIC_PROGRESS_DATA, str);
		}

		str.Format(_T("Saving Complete..."));
		SetDlgItemText(IDC_STATIC_PROGRESS_DATA, str);

		if (nRange == m_Progress.GetPos())
		{
			OnOK();
		}
	}
	
	

	/*
	if (true)
	{
		INT nPos = 0;
		INT nRange = m_lstSystemLog.GetCount();
		CString str = 0;

		CDataBaseLog* pBase = CDataBaseLog::GetInstance();

		POSITION pos = m_lstSystemLog.GetHeadPosition();

		while (pos)
		{
			OPERATIONLOG_INFORMATION log = m_lstSystemLog.GetNext(pos);
			pBase->InsertLogData(log.nLevel, log.strInform, log.strDeviceName, log.strDeviceIP);
			str.Format(_T("Saving Log Data... ( %d / %d )"), nPos, m_lstSystemLog.GetCount());
		}

		str.Format(_T("Saving Complete..."));
		
	}
	*/
	

	return 0;
}


void CDlgSaveLogData::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_SAVE:
	{
		KillTimer(TIMER_SAVE);
		SaveListData();
	}	break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}
