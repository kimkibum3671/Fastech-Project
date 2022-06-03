#pragma once

#ifdef _WIN32
#include "Lib/Sqlite3/X86/sqlite3.h"
#pragma comment (lib, "Lib/Sqlite3/X86/sqlite3.lib")
#endif

#ifdef _WIN64
#include "Lib/Sqlite3/X64/sqlite3.h"
#pragma comment (lib, "Lib/Sqlite3/X64/sqlite3.lib")
#endif

#include "CDeviceMotionGate.h"
#include "MainFrm.h"
class CDataBaseMutex
{
public:
	CDataBaseMutex( CMutex* pMutex, const DWORD dwTimeout = INFINITE);
	~CDataBaseMutex();

protected:
	CMutex* m_pMutex;
	BOOL	m_bLock;

public:
	const BOOL IsLocked();
};

class CDataBaseData
{
public:
	CDataBaseData();
	CDataBaseData(LPCSTR lpszColumnName, const INT nDataType, const INT nValue);
	CDataBaseData(LPCSTR lpszColumnName, const INT nDataType, const DOUBLE dbValue);
	CDataBaseData(LPCSTR lpszColumnName, const INT nDataType, const BYTE* lpszText);
	CDataBaseData(LPCSTR lpszColumnName, const INT nDataType, LPCVOID lpData, const DWORD cbBuffer);

	~CDataBaseData();

protected:
	CString m_strName;
	INT		m_nDataType;
	LPVOID	m_ptData;
	DWORD	m_dwDataSize;

public:
	void			SetName(LPCTSTR lpszName);
	LPCTSTR			GetName();

	void			SetType(const INT nType);
	const INT		GetType();

	void			SetData(const BYTE* pData, const DWORD dwDataSize);

	const LPVOID	GetData();
	const DWORD		GetSize();
};

class CDataBaseRecordData
{
public:
	CDataBaseRecordData();
	~CDataBaseRecordData();

protected:
	CList<CDataBaseData*, CDataBaseData*> m_lstData;

public:
	void RemoveAll();

public:
	const CDataBaseData* AddDataInteger(LPCSTR lpszColumnName, const INT nValue);
	const CDataBaseData* AddDataFloat(LPCSTR lpszColumnName, const DOUBLE dbValue);
	const CDataBaseData* AddDataText(LPCSTR lpszColumnName, const BYTE* lpszText);
	const CDataBaseData* AddDataBlob(LPCSTR lpszColumnName, LPCVOID pbtData, const DWORD cbBuffer);

	CDataBaseData* FindData(LPCTSTR lpszColumnName);

	const BOOL	GetIntergerData(LPCTSTR lpszColumnName, INT& nValue);
	const BOOL	GetFloatData(LPCTSTR lpszColumnName, DOUBLE& dbValue);
	const BOOL	GetTextData(LPCTSTR lpszColumnName, CString& strValue, LPCTSTR lpszFormat = NULL );
	const BOOL	GetBlobData(LPCTSTR lpszColumnName, LPVOID pValue, const DWORD cbBuffer);
	const BOOL	GetImageData(LPCTSTR lpszColumnName, CBitmap* pBitmap);

	const INT	GetSize(LPCTSTR lpszColumnName);

	POSITION		GetStartPosition();
	CDataBaseData*	GetNext(POSITION& position);
};

class CDataBaseManager
{
public:
	CDataBaseManager(LPCTSTR lpszMutexName);
	~CDataBaseManager();

protected:
	CMutex m_mutex;
	CString m_strFileName, m_strCurTableName;
	
	CList<CDataBaseRecordData*, CDataBaseRecordData*> m_lstRecord;

	POSITION m_cursor;
	
	sqlite3* m_pDb;

	INT m_nLastDataBaseErrorCode;

protected:
	void		RemoveAll();

	const BOOL	Initialize();
	const BOOL	ExecuteQuery(LPCTSTR lpszQuery, const BOOL bRemovePrevRecord = TRUE);
	const BOOL	ExecuteQuery(LPCTSTR lpszQuery, sqlite3_stmt** pStmt);
	const BOOL	ExecuteQueryEx(LPCTSTR lpszQuery);
	
public:
	void		GetStringAddress(CString& strAddress, const DWORD dwAddress);
	const DWORD StringToAddress(LPCTSTR lpszAddress);	

public:
	void					Lock();
	void					UnLock();

	const INT				GetRecordCount();
	const BOOL				SeekToBegin();
	CDataBaseRecordData*	SeekToNext();
	CDataBaseRecordData*	GetRecord();
	CDataBaseRecordData*	SeekRecord(LPCTSTR lpszColumnName, LPCTSTR lpszValue);
	const BOOL				IsEndOfRecord();

	const BOOL				OpenDataBase();
	const BOOL				IsDataBaseOpened();
	const BOOL				Begin();
	const BOOL				Commit();
	const BOOL				CancelCommit();
	const BOOL				CloseDataBase();

	const BOOL				BindText(sqlite3_stmt* pStmt, const INT nIndex, LPCSTR lpszValue);
	const BOOL				BindInteger(sqlite3_stmt* pStmt, const INT nIndex, const INT nValue);
	const BOOL				BindBlob(sqlite3_stmt* pStmt, const INT nIndex, LPBYTE pBuffer, const INT cbBuffer );

	const BOOL				StepQuery(sqlite3_stmt* pStmt);
	const BOOL				FinalizeQuery(sqlite3_stmt* pStmt);

	const BOOL				DoVacuum();

	const INT				GetLastErrorCode();
};

class CDataBaseLog : public CDataBaseManager
{
protected:
	CDataBaseLog();

public:
	~CDataBaseLog();

public:
	static CDataBaseLog* GetInstance();

public:
	const BOOL InsertLog(const INT nLevel, LPCTSTR lpszInformation, LPCTSTR lpszProjectName = NULL);
	const BOOL InsertLogData(const INT nLevel, LPCTSTR lpszInformation, CString strDeviceName, CString strDeviceIP, LPCTSTR lpszProjectName = NULL);
	const BOOL GetLogList(CString str);
	const BOOL GetLogInformationAll(LPVOID pDatabaseRecordData, INT& nRowid,CString& strInform, CString& strIP, CString& strID, CString& strTime);
};


class CDataBaseProject : public CDataBaseManager
{
protected:
	CDataBaseProject();

public:
	~CDataBaseProject();

protected:
	CDeviceMotionGate::MOTOR_PARAMETER_DATA m_stDefaultMotorParameter;

protected:
	const BOOL		LoadMotorDefaultParameter();

public:
	static CDataBaseProject* GetInstance();

	const BOOL		LoadAddressList();

	const BOOL		GetCursorAddress(DWORD& dwAddrss);
	const BOOL		GetCursorProjectName(CString& strName);
	const BOOL		GetCursorMotorCount(INT& nCount);

	const BOOL		DeleteProjectTable();

	const BOOL		ResetPriority();

	const BOOL		UpdateProject(LPCTSTR lpszProjectName, const INT nPriority, const DWORD dwAddress, const INT nMotorCount, const BOOL bRemovePrevRecord = TRUE);
	const BOOL		SetPriorityProject(LPCTSTR lpszProjectName, const INT nPriority, const BOOL bRemovePrevRecord = TRUE);
	
//	const BOOL		InsertProject(LPCTSTR lpszProjectName, const INT nPriority, const DWORD dwAddress, const INT nMotorCount, const BOOL bRemovePrevRecord = TRUE);
	const BOOL		InsertProject(LPCTSTR lpszProjectName, const INT nDeviceID,const INT nPriority, const INT nMotorCount, const INT nHomingTimeout, const INT nNominal, const INT nRestored,CBitmap* pImage);
	const BOOL		UpdateProject(LPCTSTR lpszOldProjectName, LPCTSTR lpszNewProjectName, const INT nMotorCount, const INT nHomingTimeout, CBitmap* pImage, INT nDeviceID);

	const BOOL		DeleteDisabledProject();
	
	const BOOL		UpdateNominalBackupID(LPCTSTR lpszProjectName, const INT nBackupID);
	const INT		GetNominalBackupID(LPCTSTR lpszProjectName);
	const BOOL		GetNominalBackupInfo(LPCTSTR lpszProjectName, INT* pnBackupID = NULL, CString* pstrTimeStamp = NULL);
	
	const BOOL		UpdateLatestRestoredID(LPCTSTR lpszProjectName, const INT nBackupID);
	const BOOL		GetLatestRestoredInfo(LPCTSTR lpszProjectName, INT* pnBackupID = NULL, CString* pstrTimeStamp = NULL);
	const INT		GetLatestRestoredID(LPCTSTR lpszProjectName);

	const INT		CreateBackupID(LPCTSTR lpszProjectName, LPCTSTR lpszBackupName);
	const BOOL		WriteBackupData(LPCTSTR lpszProjectName, const INT nBackupID, const INT nMotorID, const INT nIndex, const INT nValue);

	
	const BOOL		GetProjectBackupList(LPCTSTR lpszProjectName);
	const INT		GetProjectBackupListCount(LPCTSTR lpszProjectName);

	const BOOL		GetMotorCount(LPCTSTR lpszProjectName, INT& nCount);

	const BOOL		LoadMotorParameter(LPCTSTR lpszProjectName, const INT nBackupID, const INT nMotorIndex = -1);
	const BOOL		GetBackupName(LPCTSTR lpszProjectName, const INT nBackupID);

	const BOOL		GetDefaultLoadParameterParameter(CDeviceMotionGate::MOTOR_PARAMETER_DATA* pParameter);

	const BOOL		SetHomingTimeout(LPCTSTR lpszProjectName, const INT nTimeout);
	const BOOL		GetHomingTimeout(LPCTSTR lpszProjectName, INT& nTimeout );

	const BOOL		ChangeRegisterName(LPCTSTR lpszProjectName, LPCTSTR lpszNewProjectName);

	const BOOL		ChangeBackupKey(LPCTSTR lpszCurKeyName, LPCTSTR lpszNewKeyName);

	const BOOL		DeleteBackupData(const INT nBackupID, LPCTSTR lpszProjectName);
	const BOOL		DeleteBackupReservationDataAll(LPCTSTR lpszProjectName);
	const BOOL		DeletePassedBackupReservationData(LPCTSTR lpszProjectName);
	const BOOL		InsertBackupReservationData(LPCTSTR lpszProjectName, COleDateTime dateTime, LPCTSTR lpszWeekCode);

	const BOOL		LoadBackupReservation(LPCTSTR lpszProjectName);
	
	const BOOL		GetSingleReservationTime(LPCTSTR lpszProjectName, COleDateTime& dateTime);
	const BOOL		GetRepeatReservationTime(LPCTSTR lpszProjectName, COleDateTime& dateTime);

	const BOOL		GetSystemImage(LPCTSTR lpszProjectName, CBitmap* pBitmap);

	const BOOL		ChangeNominalBackupID( const INT nNominalBackupID, LPCTSTR lpszProjectName);
	const BOOL		RemoveLastestBackupID(LPCTSTR lpszProjectName);

	const BOOL		ChangeIpAddress(const DWORD dwCurrentIpAddress, const DWORD dwNextIpAddress);

	const BOOL		GetDeviceList( LPCTSTR lpszProjectName = NULL);

	const BOOL		DeleteAddressIndexAll();
	const BOOL		InsertAddressIndex(const DWORD dwAddress, const INT nPriority);

	const INT		GetMaxPriority();
	const BOOL		CheckOverlapDeviceID(INT nID, CString& strGetName, CString strCurrenteName);
	const BOOL SetBooleanProject(LPCTSTR lpszProjectName, CString str, BOOL bDelete);
	const BOOL SetDeleteValueProjcetTable(CString strProjectName, const BOOL bDelete);
	const BOOL SetDeleteProjectAll(const BOOL bDelete);
	const BOOL UpdateRestoreBackupListTable(CString strProjectName = L"",  BOOL bCopy = false, BOOL bDelete = false);
	const BOOL CopyList(CString strOrigin, CString strTarget, CString strProjectName);
	const BOOL DeleteBackupListTable();
	const BOOL DeleteBackupListItem(CString strName);
//	const INT GetLogInformationAll();
	const BOOL GetProjectName(INT nRowid, CString& strName);
	const BOOL GetBackupList();
	const BOOL GetBackupListDetailData();
	
	const BOOL DeleteBackupList(const INT nBackupID, LPCTSTR lpszProjectName);
	const BOOL GetBackupFileMotorIndex(LPCTSTR lpszProjectName, INT nBackupID);
	const BOOL GetBackupMotorCount(LPCTSTR strName, INT nBackupID, INT& nCnt);
	const BOOL GetBackupMotorEndNumber(LPCTSTR strName, INT nBackupID, INT& nIndex);
	const BOOL DeleteBackupDataItem(CString strName);
	const BOOL UpdateChangeProject(LPCTSTR lpszOldProjectName, LPCTSTR lpszNewProjectName, const INT nMotorCount, const INT nHomingTimeout, CBitmap* pImage, INT nDeviceID);
};


class CDataBaseConfig : public CDataBaseManager
{
protected:
	CDataBaseConfig();

public:
	~CDataBaseConfig();

public:
	static CDataBaseConfig* GetInstance();

protected:
	//	User Information
	CString m_strName;
	CBitmap m_bmpNoImage;
	CImage m_picture;
	BOOL m_bDefault, m_bAdmin, m_bOperator, m_bEngineer, m_bControl;
	BOOL m_bBackup, m_bBackupManage, m_bRestore, m_bCompare, m_bConfiguration, m_bBackupSchedule, m_bReset, m_bIdAssignment, m_bIDReset;

protected:
	void LoadNoImage();

public:
	const BOOL ChangeUser( LPCTSTR lpszName, LPCTSTR lpszPassword = NULL);
	const BOOL ChangeDefaultUser();
	
	LPCTSTR GetOperatorName();
	const BOOL IsDefaultOperator();
	CImage* GetUserPicture();

	const BOOL	IsAdministrator();
	const BOOL	IsDeviceBackupGranted();
	const BOOL	IsDeviceRestorationGranted();
	const BOOL	IsDeviceFactoryResetGranted();
	const BOOL	IsDeviceIdAssignmentGranted();
	const BOOL	IsControlGranted();

	HBITMAP		SetNoImage(HBITMAP hBitmap);
	HBITMAP		GetNoImage();
	const BOOL	LoadNoImage(CBitmap* pBitmap);
	const BOOL	IsNoImage(CBitmap* pBitmap);
	const BOOL	LoadUserImage(LPCTSTR lpszName, CBitmap* pBitmap);

public:
	typedef enum _INTERFACE_TYPE
	{
		interface_Unknown = 0,
		interface_TCP,
		interface_UDP,
		interface_Serial,
	}	_INTERFACE_TYPE;

protected:
	typedef struct _INTERACE_INFO
	{
		INT		nInterface;
		INT		nSerialPort;
		BOOL	bEnableMotionGate;
		BOOL	bConfirmToContinue;
	}	INTERACE_INFO;

protected:

	INTERACE_INFO m_stInferface;

protected:
	void		LoadInterface();

public:
	void		SetInterface( const INT nType);
	const INT	GetInterface();

	void		SetSerialPort(const INT nPortNo);
	const INT	GetSerialPort();

	void		EnableMotionGate(const BOOL bEnable);
	const BOOL	IsMotionGateEnabled();

	void		ConfirmToContinue(const BOOL bEnable);
	const BOOL	IsConfirmToContinueEnabled();

protected:
	typedef struct _SERIAL_DEVICE_INFO
	{
		DWORD	dwAddress;
		CString	strSystemName;
		INT		nMotorCount;
		INT		nHomingTimeout;
		INT		nModbusWriteAddress;
		WORD	wModbusWirteAddress;
		CBitmap	bmpSystemImage;
	}	SERIAL_DEVICE_INFO;

	SERIAL_DEVICE_INFO m_stSerialDeviceInfo;

	typedef struct _TOTAL_SYSTEM_CONFIG
	{
		INT nLifeCountStandard;
		INT nLifeCountCycle;
	}	TOTAL_SYSTEM_CONFIG;

	TOTAL_SYSTEM_CONFIG m_stTotalDeviceConfig;

protected:

	AUTHORIZE_CONTROL_INFO authorize;

protected:
	void	LoadSerialSystemInfo();

public:
	void		SetSerialAddress(const DWORD dwAddress);
	const DWORD	GetSerialAddress();

	void		SetSerialName(LPCTSTR lpszName);
	LPCTSTR		GetSerialName();

	void		SetSerialMotorCount(const INT nCount);
	const INT	GetSerialMotorCount();

	void		SetSerialHomingTimeout(const INT nTimeout, const BOOL bSecond = TRUE);
	const INT	GetSerialHomingTimeout(const BOOL bSecond = TRUE);

	HBITMAP		SetSerialSystemImage(HBITMAP hBitmap);
	HBITMAP		GetSerialSystemImage();
	const INT GetModBusWriteAddress();
	const BOOL SetModBusWriteAddress(const INT nWriteAddress);
	const BOOL DeleteModBusWirteAddress(const INT nLatestAddress);
	WORD GetStringToWord(CString str);
	unsigned char GetState(CString str);
	void SetModBusWriteAddressTypeWORD(WORD wValue);
	WORD GetModBusWriteAddressTypeWORD();
	const BOOL IsEngineer();
	const BOOL IsOperator();
	BOOL SetLifeCountStandardAlarmValue(INT nStandard);
	BOOL SetLifeCountCycleAlarmValue(INT nCycle);
	void LoadTotalSystemInform();
	INT GetLifeCountStandardValue();
	INT GetLifeCountCycleValue();
	WORD GetAuthorizeControl(LPCTSTR lpszName);
	const BOOL SetAuthorizeControl(LPCTSTR lpszName, CONFIG_NODE wData);
	const BOOL IsDeviceBackupManageGranted();
	const BOOL IsDeviceCompareGranted();
	const BOOL IsDeviceConfigGranted();
	const BOOL IsDeviceBackupScheduleGranted();
	const BOOL IsDeviceIdResetGranted();
	const BOOL LoadUserPassword(LPCTSTR lpszName, CString& strPassword);
	const BOOL ChangePassword(PASSWORD_NODE node);
	AUTHORIZE_CONTROL_INFO GetAuthorizeState();
	const BOOL ReLoadAuthorizeControl(LPCTSTR lpszName);
};
