// CMotionGateManager.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CMotionGateManager.h"
#include "afxdialogex.h"

#include "COutlookDevice.h"
#include "MainFrm.h"

// CMotionGateManager 대화 상자

IMPLEMENT_DYNAMIC(CMotionGateManager, CDialogEx)

#define MONITORTIMMER 0

const int m_nBasic = 100;

CMotionGateManager::CMotionGateManager(CWnd* pParent /*=nullptr*/, int nMotionCnt)
	: CDialogEx(IDD_MOTION_MANAGER, pParent)
{
	m_pParent = pParent;
	m_nCurrentCount = nMotionCnt;
	m_bProgramStart = true;
	m_nVScrolPos = 0;
}

CMotionGateManager::~CMotionGateManager()
{
	KillTimer(MONITORTIMMER);
	delete m_pMotionGateInform;
}

void CMotionGateManager::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCROLLBAR_DEVICE1, m_VScroll);
}


BEGIN_MESSAGE_MAP(CMotionGateManager, CDialogEx)
	ON_WM_TIMER()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


BOOL CMotionGateManager::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CMainFrame* m_pFrame = (CMainFrame*)m_pParent;
	m_nCurrentCount = GetMotionGateCount();

	CRect rect;
	GetClientRect(rect);

	InitScrollObj();

	return TRUE;  // return TRUE unless you set the focus to a control
}

int CMotionGateManager::GetMotionGateCount()
{
	return m_nMotionGateCount;
}

bool CMotionGateManager::ReMakeInstance()
{
	m_nCurrentCount = GetMotionGateCount();
	return true;
}


bool CMotionGateManager::ShowMotionInform()
{
	static int nCnt = 0;
	TRACE("CMotionGateManager::OnPaint() : %d \n", nCnt++);

	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	DoMakeMotionInform(pDeviceList, m_nCurrentCount);

	CRect rect;
	GetClientRect(rect);

	CRect rectInform;
	m_pMotionGateInform->GetWindowRect(rectInform);

	int nMatrixRowCnt = 0;
	int nMatrixColumnCnt = 0;

	nMatrixRowCnt = rect.Width() / rectInform.Width(); // 행의 개수
	if (nMatrixRowCnt == 0)
	{
		AfxMessageBox(L"OutlookBar screen exceeds MotionGateInformation screen width.");
	}
	m_nColumnCnt = nMatrixColumnCnt = (m_nCurrentCount / nMatrixRowCnt) + ((m_nCurrentCount % nMatrixRowCnt > 0) ? 1 : 0);// 필요 열의 개수

	int arr = 0;

	for (int y = 0; y < nMatrixColumnCnt; y++)
	{
		for (int x = 0; x < nMatrixRowCnt; x++)
		{
			if (arr > (m_nCurrentCount - 1)) break;
			m_pMotionGateInform[arr++].SetWindowPos(NULL, rectInform.Width() * x, (rectInform.Height()) * y, rectInform.Width(), rectInform.Height(), SWP_SHOWWINDOW | SWP_NOZORDER);
		}
	}

	ShowScroll(rectInform.Height() * m_nColumnCnt - rect.Height() > 0 ? SW_SHOW : SW_HIDE);

	// 스크롤바의 모든 정보를 한번에 설정.
	m_nVPageSize = rect.Height();
	m_nScrollPos = 0;
	m_nPossiblePos = rectInform.Height() * m_nColumnCnt - m_nVPageSize;
	m_VScroll.SetScrollRange(0, m_nPossiblePos);
	m_VScroll.SetScrollPos(m_nScrollPos);

	return false;
}

void CMotionGateManager::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case MONITORTIMMER:
	{
		SetInformText();
	}
	break;

	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}
void CMotionGateManager::DoMakeMotionInform(CDeviceList* pDeviceList, int nCount)
{
	m_nCurrentCount = pDeviceList->GetAddressCount();

	if (m_bProgramStart)
	{
		m_pMotionGateInform = new CMotionGateInform[m_nCurrentCount];

		for (int i = 0; i < m_nCurrentCount; i++)
		{
			m_pMotionGateInform[i].Create(IDD_MOTION_INFORM, this);
			m_pMotionGateInform[i].SetParent(this);
			m_pMotionGateInform[i].SetMotionID(i);
		}
		SetInformText();
		m_bProgramStart = false;

		SetTimer(MONITORTIMMER, 100, NULL);
	}
	else
	{
		for (int i = 0; i < m_nPrevCount; i++)
		{
			m_pMotionGateInform[i].DestroyWindow();

			delete m_pMotionGateInform[i];
		}

		m_pMotionGateInform = new CMotionGateInform[m_nCurrentCount];

		for (int i = 0; i < m_nCurrentCount; i++)
		{
			m_pMotionGateInform[i].Create(IDD_MOTION_INFORM, this);
			m_pMotionGateInform[i].SetParent(this);
			m_pMotionGateInform[i].SetMotionID(i);
		}
		SetInformText();
	}

	m_nPrevCount = m_nCurrentCount;
}

bool CMotionGateManager::SetInformText()
{
	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	for (int i = 0; i < m_nCurrentCount; i++)
	{
		const DWORD dwAddress = pDeviceList->GetAddressByIndex(i);
		CDeviceMotionGate* pDevice = NULL;
		pDeviceList->GetMotiongateByAddress(dwAddress, pDevice);

		pDevice->GetAddress(m_pMotionGateInform[i].m_mgInform.strIP);
		pDevice->GetAutoBackupDatetime(m_pMotionGateInform[i].m_mgInform.BackupDateTime);
		pDevice->GetSimpleStatus((m_pMotionGateInform[i].m_mgInform.strStatus = ""));
		m_pMotionGateInform[i].m_mgInform.nMotorCnt = pDevice->GetMotorCount();
		m_pMotionGateInform[i].m_mgInform.dwMottorConnectStatus = pDevice->GetConnectedMotorStatus();
		m_pMotionGateInform[i].m_mgInform.strSystemName = pDevice->GetSystemName();
		m_pMotionGateInform[i].m_mgInform.nOPCode = pDevice->GetOpCode();
		m_pMotionGateInform[i].ShowMotionInform();
	}

	return false;
}

void CMotionGateManager::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int delta;
	CRect rect;
	GetWindowRect(rect);

	switch (nSBCode)
	{
	case SB_LINEUP:
	{
		delta = -m_nBasic;
	}
	break;
	case SB_PAGEUP:
	{
		delta = -m_nVPageSize;
	}
	break;
	case SB_THUMBTRACK:
	{
		delta = static_cast<int>(nPos) - m_nScrollPos;
	}
	break;
	case SB_PAGEDOWN:
	{
		delta = m_nVPageSize;
	}

	break;
	case SB_LINEDOWN:
	{
		delta = m_nBasic;
	}
	break;

	default:
	{
	}
	return;
	}

	int scrollpos = m_nScrollPos + delta;
	int maxpos = m_nPossiblePos;

	if (scrollpos < 0)
		delta = -m_nScrollPos;
	else
		if (scrollpos > maxpos)
			delta = maxpos - m_nScrollPos;


	if (delta != 0)
	{
		m_nScrollPos += delta;
		pScrollBar->SetScrollPos(m_nScrollPos);
		ScrollWindow(0, -delta);

		/*
		int nMatrixRowCnt = 0;
		int nMatrixColumnCnt = 0;
		int arr = 0;

		CRect rectInform;
		m_pMotionGateInform->GetWindowRect(rectInform);

		nMatrixRowCnt = rect.Width() / rectInform.Width(); // 행의 개수
		m_nColumnCnt = nMatrixColumnCnt = (m_nCurrentCount / nMatrixRowCnt) + ((m_nCurrentCount % nMatrixRowCnt > 0) ? 1 : 0);// 필요 열의 개수

		for (int y = 0; y < nMatrixColumnCnt; y++)
		{
			for (int x = 0; x < nMatrixRowCnt; x++)
			{
				m_pMotionGateInform[arr++].SetWindowPos(NULL, rectInform.Width() * x, (rectInform.Height()) * y - m_nScrollPos, 0, 0, SWP_NOSIZE );
			}
		}*/
	}
	pScrollBar->SetWindowPos(NULL, rect.Width() - 20, 0, VScroll.Width(), rect.Height(), SWP_SHOWWINDOW | SWP_NOZORDER);

	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CMotionGateManager::InitScrollObj()
{
	m_nInitScrollPos = 0;
	m_nVScrolPos = 0;

	CRect rect;
	GetClientRect(&rect);

	CMotionGateInform* pMotionGate = new CMotionGateInform();
	pMotionGate->Create(IDD_MOTION_INFORM, this);
	pMotionGate->GetClientRect(m_InformDeviceRect);

	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	m_nCurrentCount = pDeviceList->GetAddressCount();

	m_nScrollPos = 0;
	m_nFrameSizePos = rect.Height();
	m_nTotalDevicsPos = m_InformDeviceRect.Height() * m_nColumnCnt;
	m_nPossiblePos = m_nTotalDevicsPos - m_nFrameSizePos;
}

bool CMotionGateManager::ShowScroll(int nShow, int nPos)
{
	CRect rect;
	GetClientRect(rect);
	m_VScroll.GetWindowRect(&VScroll);
	m_VScroll.SetWindowPos(NULL, rect.Width() - 20, 0, VScroll.Width(), rect.Height(), SWP_SHOWWINDOW | SWP_NOZORDER);
	m_VScroll.ShowWindow(nShow);

	return false;
}

