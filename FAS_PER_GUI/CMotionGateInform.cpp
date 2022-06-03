// CMotionGateInform.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CMotionGateInform.h"
#include "afxdialogex.h"


// CMotionGateInform 대화 상자

IMPLEMENT_DYNAMIC(CMotionGateInform, CDialogEx)

CMotionGateInform::CMotionGateInform(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MOTION_INFORM, pParent)
{

}

CMotionGateInform::~CMotionGateInform()
{
}

void CMotionGateInform::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMotionGateInform, CDialogEx)
END_MESSAGE_MAP()


// CMotionGateInform 메시지 처리기


void CMotionGateInform::ShowMotionInform()
{
	CString str;
	str.Format(L"MotionGate #%d", m_nID);
	//((CStatic*)GetDlgItem(IDC_STATIC_GROUP))->SetWindowTextW(str);

	for (int i = 0; i < MGSTATE_MAX; i++)
	{
		switch (i)
		{
		case MGSTATE_IP:
			str = m_mgInform.strIP;
			break;

		case MGSTATE_NAME:
			str = m_mgInform.strSystemName;
			break;
		case MGSTATE_STATE:
			str = m_mgInform.strStatus;
			break;
		case MGSTATE_MOTOR_COUNT:
			str.Format(L"%d", m_mgInform.nMotorCnt);
			break;
		case MGSTATE_OPSTATUS:
			str = ShowOpCode(m_mgInform.nOPCode);
			//str.Format(L"%d", m_mgInform.nOPCode);
			//str = m_mgInform.strConnection;
			break;
		default:
			break;
		}

		((CStatic*)GetDlgItem(IDC_STATIC_DEV0 + i))->SetWindowTextW(str);
		//((CStatic*)GetDlgItem(IDC_STATIC_DEV0 + i))->ShowWindow(SW_SHOW);
	}

	// TODO: 여기에 구현 코드 추가.
}


CString CMotionGateInform::ShowOpCode(int nOpCode)
{
	CString str;

	switch (nOpCode)
	{
	case opCode_connect:
		str.Format(L"Device Connect");
		break;
	case opCode_disconenct:
		str.Format(L"Device DisConnect");
		break;
	case opCode_no_response:
		str.Format(L"Device No Response");
		break;
	case opCode_MotiongateBusy:
		str.Format(L"Device Busy");
		break;
	case opCode_scan_device:
		str.Format(L"Device Scan");
		break;
	case opCode_read_motiongate_status:
		str.Format(L"Read MotionGate");
		break;
	case opCode_standby_motiongate:
		str.Format(L"Stanby MotionGate");
		break;
	case opCode_backup_read_parameter:
		str.Format(L"Read Parameter [Backup]");
		break;
	case opCode_backup_read_position_table:
		str.Format(L"Read PositionTable [Backup]");
		break;
	case opCode_enter_monitor_mode:
		str.Format(L"Enter Monitor Mode");
		break;
	case opCode_exit_monitor_mode:
		str.Format(L"Exit Monitor Mode");
		break;
	case opCode_check_monitor_mode_on:
		str.Format(L"Monitor Mode On");
		break;
	case opCode_check_monitor_mode_off:
		str.Format(L"Monitor Mode Off");
		break;
	case opCode_check_device_name:
		str.Format(L"Check Device Name");
		break;
	case opCode_change_device_name:
		str.Format(L"Change Device Name");
		break;
	case opCode_change_register_name:
		str.Format(L"Change Register Name");
		break;
	case opCode_write_main_parameter:
		str.Format(L"Write Parameter");
		break;
	case opCode_write_position_table:
		str.Format(L"Write Position Table");
		break;

	default:
		str.Format(L"");
		break;
	}

	return str;
}
