#pragma once
#include "CMotionGateInform.h"
#include "CDeviceList.h"

// CMotionGateManager 대화 상자

class CMotionGateManager : public CDialogEx
{
	DECLARE_DYNAMIC(CMotionGateManager)

public:
	CMotionGateManager(CWnd* pParent = nullptr, int nMotionCnt = 0);   // 표준 생성자입니다.
	virtual ~CMotionGateManager();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MOTION_MANAGER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	CWnd* m_pParent;

public:

	void SetParent(CWnd* pParent) { m_pParent = pParent; }
	void SetMotionCount(int nCnt) { m_nMotionGateCount = nCnt; }

private:
	unsigned int m_nMotionGateCount;

public:

	struct MGINFORM {
		CString strIP;
		CString strStatus;
		CString Connection;
		CString BackupSchedule;
		CString SystemName;
		COleDateTime BackupDateTime;
		DWORD dwMottorConnectStatus;
		int nMotorCnt;
		int nOPCode;
	};
	// 객체
	CMotionGateInform* m_pMotionGateInform;

	// 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	int GetMotionGateCount();
	bool ReMakeInstance();
	bool ShowMotionInform();
	bool SetInformText();
	bool ShowScroll(int nShow, int nPos = 0);
	void DoMakeMotionInform(CDeviceList* pDeviceList, int nCount);
	void InitScrollObj();

	// 변수
	CScrollBar m_VScroll;
	CRect VScroll;
	CRect m_InformDeviceRect;

	unsigned int m_nCurrentCount;
	unsigned int m_nPrevCount;

	int m_nVScrolPos;
	int m_nVPageSize;
	int m_nInitScrollPos;
	int m_nColumnCnt;
	int m_nPossiblePos;
	int m_nTotalDevicsPos;
	int m_nScrollPos;
	int m_nFrameSizePos;

	bool m_bProgramStart;
};
