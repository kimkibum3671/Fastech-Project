#pragma once

#include "ColorStaticST.h"
#include "CDeviceMotionGate.h"
#include "CStaticImageViewer.h"
#include "CMFCButtonEx.h"
//#define SHOWSTATUS 4
// CDlgMotionGateInform 대화 상자

class CDlgMotionGateInform : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMotionGateInform)

public:
	CDlgMotionGateInform(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgMotionGateInform();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MOTION_INFORM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
private:
	CWnd* m_pParent;
	CToolTipCtrl	m_tooltip;
	int m_nID;

	CMFCButtonEx m_run, m_stop, m_backup, m_backupManage, m_restore, m_compare, m_backupSchedule, m_assign, m_reset, m_config;

protected:
	CDeviceMotionGate* m_pDevice;

public:
	struct MGINFORM {
		CString strIP;
		CString strStatus;
		CString strConnection;
		CString strBackupSchedule;
		CString strSystemName;
		COleDateTime BackupDateTime;
		DWORD dwMotorConnectStatus;
		DWORD dwMotorLifeCount;
		int nMotorCnt;
		int nOpCode;
		bool bFocus;
		bool bConnect;
		bool bMasterConnect;
	};

	MGINFORM m_mgInform, m_PrevInform;

	enum MGINFORMENUM
	{
		MGSTATE_IP,
		MGSTATE_NAME,
		MGSTATE_STATE,
		MGSTATE_MOTOR_COUNT,
		MGSTATE_CONNECT,
		//MGSTATE_BOXCONTROL,
		MGSTATE_MAX
	};

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
		opCode_read_life_counter,
		opCode_scan_next_device_ok,
		opCode_scan_next_device_ng,
		opCode_standby_motiongate,
		opCode_backup_read_parameter,
		opCode_backup_read_position_table,
		opCode_enter_monitor_mode,
		opCode_exit_monitor_mode,
		opCode_check_monitor_mode_on,
		opCode_check_monitor_mode_off,
		opCode_notify_backup_complete,
		opCode_check_device_name,
		opCode_change_device_info,
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

	enum MOTION_STATUS
	{
		STATUS_CONNECTED,
		STATUS_BUSY,
		STATUS_WARNNING,
		STATUS_ERROR,
		STATUS_MASTER,
		STATUS_MAX
	};

	enum MOTOR_NUM
	{
		MOTOR0 = 0,
		MOTOR1,
		MOTOR2,
		MOTOR3,
		MOTOR4,
		MOTOR5,
		MOTOR6,
		MOTOR7, 
		MOTOR8,
		MOTOR_MAX
	};

	void SetParent(CWnd* pParent) { m_pParent = pParent; }
	void SetMotionID(int nID) { m_nID = nID; }
	
	void ShowMotionInform();
	CString ShowOpCode(int nOpCode);
//	bool SetTextColor(CString str, int nCtrlID);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DWORD m_dwAddress;
	DWORD m_dwProgressEndTime;
	bool m_bConnected;
	BOOL m_bRun;
	CStatic m_MotionBox;
	CFont m_font;

	CColorStaticST m_ColorStatic[STATUS_MAX];
	CColorStaticST m_MotorConnectColorStatic[MOTOR_MAX];
	CColorStaticST m_MotorAlarmColorStatic[MOTOR_MAX];
	CColorStaticST m_MotorPTColorStatic[MOTOR_MAX];
	CColorStaticST m_MotorPTLifeColorStatic[MOTOR_MAX];
	CColorStaticST m_MotorReadyColorStatic[MOTOR_MAX];
	CColorStaticST m_ColorFocus;

	CRect m_rect[STATUS_MAX];
	CMFCButton m_btnBackup;
	CBitmapButton m_btnBackupSchedule;
	CProgressCtrl m_Progress;

	int m_ColorStCount;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	bool ShowMotionStLed(bool bConnected, bool bBusy, bool bWarning, bool bError, bool bMaster, bool bFocus = false);
	bool SetFocus(bool bChanged);
	//virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void InitButtonImage();
	void SetAddress(DWORD dwAddress);
	DWORD GetAddress();
	void SendCmdMsgToMainFrame(WPARAM wParam = NULL, LPARAM lParam = NULL);
	void SetProgressBar(INT nNewPos, INT nNewMin, INT nNewMax);
	
	void SetProgressStatus(CString strStatus);
	const BOOL SetDevice(CDeviceMotionGate* pDevice);
	void DoDeviceMonitoring(BOOL bRun);
	void ShowButton(CDeviceMotionGate* pDevice);
	const BOOL AskSystemRunState();
	const BOOL CheckControlGranted();
	void ShowProgressTime(DWORD dwTime);
	void SetProgressEndTime(DWORD dwTime);
	void SetMotorErrorState(CString str);
	void MotorConnectionStatus(DWORD dwMotorConnection);
	void MotorErrorStatus(CDeviceMotionGate* pDevice);

	public:
		typedef enum _STATUS_MOTOR_ERROR_CODE
		{
			sts_error_motor_none = 0,
			sts_error_motor_overcurrent = 1, // [과전류 이상]
			sts_error_motor_overspeed, // [과속도 이상], 속도가 3,000 rpm을 초과할경우
			sts_error_motor_position_tracking, // [위치 추종 이상], 모터회전중 위치 명령값과 실제 위치값의 차이가 180도 이상일 경우
			sts_error_motor_overload, // [과부하 이상], 모터의 최대 토크를 초과하는 부하가 5초이상 가해졌을경우
			sts_error_motor_overheat = 5, // [과열 이상], 드라이브의 내부온도가 85도 이상 초과하는 경우
			sts_error_motor_over_regeratived_voltage, // [회생 전압 이상], 모터의 역기전력 전압이 한계를 초과하였을경우
			sts_error_motor_connection, // [모터 접속 이상], 드라이브와 모터의 연결에 이상이 있을경우
			sts_error_motor_encoder_connection, // [엔코더 접속 이상], 드라이브와 모터의 연결에 이상이 있을경우
			sts_error_motor_motor_voltate_error, // 
			sts_error_motor_inposition_error = 10, // [인포지션 이상], 운전 완료후 1펄스 이상의 위치 오차가 3초이상 발생하였을경우
			sts_error_motor_drive_system_error, // [시스템 이상], 드라이브 시스템에 이상이 발생하였을경우
			sts_error_motor_drive_rom_error = 12, // [ROM 이상], 파라미터 저장 장치(ROM)에 이상이 발생하였을경우
			sts_error_motor_drive_voltage_error = 14,
			sts_error_motor_position_overflow, // [위치 오차 이상], 모터 정지 상태에서 180도 이상의 위치 오차가 발생한 경우
			sts_error_motor_position_change, // [new. 차종 위치 변동]
			sts_error_motor_break_short, // [new. 브레이크 단선]
			sts_error_motor_struct_postion_change, // [new, 차종 위치 변동]
			sts_error_motor_no_clam_load = 20, // 20
			sts_error_access_motor_not_exist,
			sts_error_access_motor_out_of_range, // [모터 접근 범위 초과 이상], 1 ~ 8 번모터 외 접근
			sts_error_access_pt_not_exist, // [포지션 테이블 0번 접근 이상]
			sts_error_access_pt_out_of_range, // [포지션 테이블 접근 범위 초과 이상], PT 번호가 30번 이상 접근할경우
		}	STATUS_MOTOR_ERROR_CODE;

		typedef struct _MOUSE_DETECTED_TOOLTIP
		{
			INT	 nMotor;
			BOOL bMotorError[MOTOR_MAX];
			BOOL bMotorStyle[MOTOR_MAX];
			BOOL bMotorLife[MOTOR_MAX];
		}	MOUSE_DETECTED_TOOLTIP;

		typedef struct _DEVICE_STATUS_TOOLTIP
		{
			CString strError[MOTOR_MAX];
			CString strStyleNum[MOTOR_MAX];
			CString strLifeCount[MOTOR_MAX];
			
			DWORD dwStyleNum;
			DWORD dwLifeCount;
		}	DEVICE_STATUS_TOOLTIP;

		MOUSE_DETECTED_TOOLTIP m_stTooltip;
		DEVICE_STATUS_TOOLTIP m_stDeviceTooltip;

		void MotorPositionTableStatus(CDeviceMotionGate* pDevice);
		void MotorShowDisable();
		DWORD GetConnectedMotorNum(CDeviceMotionGate* pDevice);
		DWORD GetMotorTotalAlarm(CDeviceMotionGate* pDevice);
		DWORD GetMotorPosionTableTotalNum(CDeviceMotionGate* pDevice);
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		void ShowToolTip(CDeviceMotionGate* pDevice);
		CStaticImageViewer m_wndViewer;
		CImage m_Image;
		CBitmap m_bmpImage;
		CBitmap m_bmpMain;
		BOOL m_bDisable;
		HBITMAP GetEditSystemImage(CDeviceMotionGate* pDevice);
//		const BOOL LoadMainImage(CBitmap* pBitmap);
		//CStatic m_Image;
		CStaticImageViewer m_wndPicture;
		const BOOL SetImage(CDeviceMotionGate* pDevice);
		void MotorReadyStatus(CDeviceMotionGate* pDevice);
		void SetZeroMotorStatus(CString strStatus);
		void ServoControl(BOOL bServoOn);
		void MotorAlarmStatus(CDeviceMotionGate* pDevice);
		void SetCommandResultText(CString str);
		void SetBackupScheduleDateTime(CString strTime);
		void SetBackupAutoSec(CString strSec);
		
		//CButton m_btnBackupToggle;
		void SetBackupToggle(BOOL bEnable);
		void SetBackupTimeControl(BOOL bBackupAutoSecControl, BOOL bBackupToggle, BOOL bBdakupTime);
		void OnBnClickedBtnToggleBakcup();
		//CStatic m_staticBackupCtrl;
		//CStatic m_ToggleBtn;
		afx_msg void OnStnClickedStaticToggleBtn();
		//CStatic m_ToggleButton;
		afx_msg void OnStnDblclickStaticToggleBtn();
		afx_msg void OnStnClickedStaticToggle();
		//CBitmapButton m_BtnBackupToggle;
		void MotorPositionTableLifeCount(CDeviceMotionGate* pDevice);
		DWORD GetMotorPTtotalLifeCount(CDeviceMotionGate* pDevice);
		void MotorLifeCountStatus(DWORD dwMotorConnection);
		const BOOL DisableControl();
		const BOOL IsControlEnable();
		void SetControlDisable(BOOL bDisable);
		void SetEnableSystemImage(BOOL bEnable);
		const INT GetProgressPosition();
		afx_msg void OnBnClickedMfcbuttonAlarmRst();
		void HomingTest();
		afx_msg void OnBnClickedMfcbuttonMotorReset();
		afx_msg void OnBnClickedMfcbuttonBackup();
		afx_msg void OnBnClickedMfcbuttonRestore();
		afx_msg void OnBnClickedMfcbuttonCompare();
		afx_msg void OnBnClickedMfcbuttonConfig();
		afx_msg void OnBnClickedMfcbuttonBackupSchedule();
		afx_msg void OnBnClickedMfcbuttonMgReset();
		afx_msg void OnBnClickedMfcbuttonReboot();
		afx_msg void OnBnClickedMfcbuttonAssign();
		afx_msg void OnBnClickedMfcbuttonBackupManage();
		afx_msg void OnBnClickedMfcbuttonRun();
		afx_msg void OnBnClickedMfcbuttonStop();
		afx_msg void OnBnClickedCheckServoControl();
		afx_msg void OnBnClickedButtonHoming();
		void InitButtonSetting();
		void InitControlTextSize();
		afx_msg void OnPaint();
		CFont m_BigFont;
		//BOOL CALLBACK EnumChildProcess(HWND hWnd, LPARAM lParam);
		LPVOID GetBigFont();
};
