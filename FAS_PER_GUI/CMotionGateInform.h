#pragma once


// CMotionGateInform 대화 상자

class CMotionGateInform : public CDialogEx
{
	DECLARE_DYNAMIC(CMotionGateInform)

public:
	CMotionGateInform(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CMotionGateInform();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MOTION_INFORM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
private:
	CWnd* m_pParent;
	int m_nID;

public:
	struct MGINFORM {
		CString strIP;
		CString strStatus;
		CString strConnection;
		CString strBackupSchedule;
		CString strSystemName;
		COleDateTime BackupDateTime;
		DWORD dwMottorConnectStatus;
		int nMotorCnt;
		int nOPCode;
	};

	enum MGINFORMENUM
	{
		MGSTATE_IP,
		MGSTATE_NAME,
		MGSTATE_STATE,
		MGSTATE_MOTOR_COUNT,
		MGSTATE_OPSTATUS,
		MGSTATE_MAX
	};
	MGINFORM m_mgInform;

	typedef enum _OPCODE_STATUS_MACHINE
	{
		opCode_stop = 0,
		opCode_connect,
		opCode_disconenct,				// Cannot connect to conveter
		opCode_no_response,
		opCode_MotiongateBusy,			//	Teaching Pendant가 연결되었을 때
		opCode_read_motiongate_info,
		opCode_scan_device,
		opCode_read_motiongate_status,
		opCode_standby_motiongate,
		opCode_backup_read_parameter,
		opCode_backup_read_position_table,
		opCode_enter_monitor_mode,
		opCode_exit_monitor_mode,
		opCode_check_monitor_mode_on,
		opCode_check_monitor_mode_off,
		opCode_check_device_name,
		opCode_change_device_name,
		opCode_change_register_name,
		opCode_write_main_parameter,
		opCode_write_position_table,
		opCode_save_main_parameter,
		opCode_save_position_table,
		opCode_confirm_main_parameter,
		opCode_load_position_table,
		opCode_confirm_position_table,
		opCode_change_motiongate_id,
		opCode_reboot_motiongate,
		opCode_homing_servo_on,
		opCode_homing_start,
		opCode_homing_wait,
		opCode_homing_stop,
	}	OPCODE_STATUS_MACHINE;

	void SetParent(CWnd* pParent) { m_pParent = pParent; }
	void SetMotionID(int nID) { m_nID = nID; }
	
	void ShowMotionInform();
	CString ShowOpCode(int nOpCode);
};
