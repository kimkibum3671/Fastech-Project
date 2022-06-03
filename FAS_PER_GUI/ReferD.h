#pragma once

#define LOG_START

//#define LIFE_COUNT_TEST


// Autorize Control Index

typedef union _CONFIG_NODE
{
	WORD	wData;		// 64 bytes

	struct
	{
		WORD backup : 1;
		WORD backupManage : 1;
		WORD restore : 1;
		WORD compare : 1;
		WORD configuration : 1;
		WORD backupSchedule : 1;
		WORD reset : 1;
		WORD idAssign : 1;
		WORD idReset : 1;
	};
} CONFIG_NODE, * LPCONFIG_NODE;

//Change Password
typedef struct _PASSWORD_NODE
{
	CString strName;		// 64 bytes
	CString strPassword;
	BOOL bChange;
	
} PASSWORD_NODE, * LPPASSWORD_NODE;

typedef struct _AUTHORIZE_CONTROL_INFO
{
	BOOL bBackup;
	BOOL bBackupManage;
	BOOL bRestore;
	BOOL bCompare;
	BOOL bConfiguration;
	BOOL bBackupSchedule;
	BOOL bReset;
	BOOL bIdAssign;
	BOOL bIdReset;
}	AUTHORIZE_CONTROL_INFO;

//#define BTN_BACKUP			0b0000000000000001
//#define BTN_BACKUP_MANAGE	0b0000000000000010
//#define BTN_RESOTRE			0b0000000000000100
//#define BTN_COMPARE			0b0000000000001000
//#define BTN_COMFIGURATION	0b0000000000010000
//#define BTN_BACKUP_SCHEDULE	0b0000000000100000
//#define BTN_RESET			0b0000000001000000
//#define BTN_ID_ASSIGN		0b0000000010000000
//#define BTN_ID_RESET		0b0000000100000000

enum BTN_CONTROL
{
	BTN_BACKUP = 0,
	BTN_BACKUP_MANAGE,
	BTN_RESOTRE,
	BTN_COMPARE,
	BTN_COMFIGURATION,
	BTN_BACKUP_SCHEDULE,
	BTN_RESET,
	BTN_ID_ASSIGN,
	BTN_ID_RESET,
	BTN_RANGE_MAX
};

//LOG MESSAGE
typedef struct _OPERATIONLOG_INFORMATION
{	//	Log DB에 저장할 내용
	INT		nLevel;
	CTime	time;
	CString strTime;
	CString strDeviceName;
	CString strDeviceIP;
	CString strInform;
	DWORD dwAddress;
}	OPERATIONLOG_INFORMATION, * PST_OPERATIONLOG_INFORMATION;

typedef enum _STATUS_LOG_LEVEL
{
	STS_LOG_NORMAL = 0,
	STS_LOG_WARNING,
	STS_LOG_ERROR,
	STS_LOG_MAX
}	STATUS_LOG_LEVEL;

// Searching IP Num
#define MAX_IP 255

// Configuration Prev Require
typedef enum _PREV_REQUIRE
{
	require_system_modify = 0,
	require_system_insert,
	require_device_modify,
	require_notice_modify
} PREV_REQUIRE;

//
typedef enum _LOG_STATUS
{
	// Filtering Status
	LOG_ALL = 0,
	LOG_WARNING,
	LOG_ERROR,
	// Show Current Device 
	LOG_SELECT,
	// Show Device Name, IP, Name + IP
	LOG_NAME,
	LOG_IP,
	LOG_MIX,
	// Tree Select
	LOG_TREE_SELECT
}	LOG_STATUS;

#define FONT_SIZE 17
#define TEXT_STYLE _T("Arial")