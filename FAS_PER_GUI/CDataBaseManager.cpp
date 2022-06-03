#include "pch.h"
#include "CDataBaseManager.h"

#include <iostream>


#define STRING_INTERFACE_TYPE			_T("Interface_Type")
#define STRING_INTERFACE_SERIAL_PORT	_T("Interface_Serial_Port")
#define STRING_INTERFACE_MOTIONGATE		_T("Interface_Motiongate")
#define STRING_INTERFACE_CONFIRM		_T("Interface_Confirm")

#define STRING_TOTAL_LIFECOUNT_STAND	_T("Life_Count_Alarm_Standard")
#define STRING_TOTAL_LIFECOUNT_CYCLE	_T("Life_Count_Alarm_Cycle")

#define STRING_SERIAL_ADDRESS			_T("Serial_Address")
#define STRING_SERIAL_NAME				_T("Serial_Name")
#define STRING_SERIAL_MOTOR_COUNT		_T("Serial_Motor_Count")
#define STRING_SERIAL_HOMING_TIMEOUT	_T("Serial_Homing_Timeout")

#define STRING_MODBUS_WRITEADDRESS		_T("Modbus_WriteAddress")

#define IMAGE_SERIAL_SYSTEM				_T("Serial_System")

////////////////////////////////////////
//	CDataBaseMutex
CDataBaseMutex::CDataBaseMutex(CMutex* pMutex, const DWORD dwTimeout)
{
	if (m_pMutex != NULL)
	{
		m_pMutex = pMutex;
		m_bLock = m_pMutex->Lock(dwTimeout);
	}

	/*m_pMutex = pMutex;
	m_bLock = m_pMutex->Lock(dwTimeout);*/
}

CDataBaseMutex::~CDataBaseMutex()
{
	if (m_bLock)
	{
		m_pMutex->Unlock();
	}
}

const BOOL CDataBaseMutex::IsLocked()
{
	return m_bLock;
}

////////////////////////////////////////
//	CDataBaseRecordData
CDataBaseData::CDataBaseData()
{
	m_ptData = NULL;
	m_nDataType = SQLITE_NULL;
	m_dwDataSize = 0;
}

CDataBaseData::CDataBaseData(LPCSTR lpszColumnName, const INT nDataType, const INT nValue) : m_strName(lpszColumnName)
{
	m_ptData = new INT;
	*reinterpret_cast<INT*>(m_ptData) = nValue;
	m_nDataType = SQLITE_INTEGER;
	m_dwDataSize = sizeof(INT);
}

CDataBaseData::CDataBaseData(LPCSTR lpszColumnName, const INT nDataType, const DOUBLE dbValue) : m_strName(lpszColumnName)
{
	m_ptData = new DOUBLE;
	*reinterpret_cast<DOUBLE*>(m_ptData) = dbValue;
	m_nDataType = SQLITE_FLOAT;
	m_dwDataSize = sizeof(DOUBLE);
}

CDataBaseData::CDataBaseData(LPCSTR lpszColumnName, const INT nDataType, const BYTE* lpszText) : m_strName(lpszColumnName)
{
	const INT nStrLen = strlen(reinterpret_cast<CONST CHAR*>(lpszText)) + 1;
	m_ptData = new CHAR[nStrLen];
	strcpy_s(reinterpret_cast<CHAR*>(m_ptData), nStrLen, reinterpret_cast<const CHAR*>(lpszText));
	m_nDataType = SQLITE_TEXT;
	m_dwDataSize = nStrLen - 1;
}

CDataBaseData::CDataBaseData(LPCSTR lpszColumnName, const INT nDataType, LPCVOID lpData, const DWORD cbBuffer) : m_strName(lpszColumnName)
{
	m_ptData = new BYTE[cbBuffer];
	memcpy(m_ptData, lpData, cbBuffer);
	m_nDataType = SQLITE_BLOB;
	m_dwDataSize = cbBuffer;
}

CDataBaseData::~CDataBaseData()
{
	if (m_ptData)
	{
		switch (m_nDataType)
		{
		case SQLITE_INTEGER:
		{
			delete reinterpret_cast<INT*>(m_ptData);
		}	break;

		case SQLITE_FLOAT:
		{
			delete reinterpret_cast<DOUBLE*>(m_ptData);
		}	break;

		case SQLITE_BLOB:
		{
			delete reinterpret_cast<BYTE*>(m_ptData);
		}	break;

		case SQLITE_TEXT:
		{
			delete reinterpret_cast<CHAR*>(m_ptData);
		}	break;
		}
	}
}

void CDataBaseData::SetName(LPCTSTR lpszName)
{
	m_strName = lpszName;
}

LPCTSTR CDataBaseData::GetName()
{
	return m_strName;
}

void CDataBaseData::SetType(const INT nType)
{
	m_nDataType = nType;
}

const INT CDataBaseData::GetType()
{
	return m_nDataType;
}

void CDataBaseData::SetData(const BYTE* pData, const DWORD dwDataSize)
{
	if (m_ptData)
	{
		delete static_cast<BYTE*>(m_ptData);
	}

	m_ptData = new BYTE[dwDataSize];
	m_dwDataSize = dwDataSize;

	memcpy(m_ptData, pData, dwDataSize);
}

const LPVOID CDataBaseData::GetData()
{
	return m_ptData;
}

const DWORD CDataBaseData::GetSize()
{
	return m_dwDataSize;
}

////////////////////////////////////////
//	CDataBaseRecordData
CDataBaseRecordData::CDataBaseRecordData()
{

}

CDataBaseRecordData::~CDataBaseRecordData()
{
	this->RemoveAll();
}

void CDataBaseRecordData::RemoveAll()
{
	POSITION pos = m_lstData.GetHeadPosition();
	while (pos)
	{
		delete m_lstData.GetNext(pos);
	}
	
	m_lstData.RemoveAll();
}

const CDataBaseData* CDataBaseRecordData::AddDataInteger(LPCSTR lpszColumnName, const INT nValue)
{
	CDataBaseData* pData = new CDataBaseData(lpszColumnName, SQLITE_INTEGER, nValue);
	m_lstData.AddTail(pData);
	return pData;
}

const CDataBaseData* CDataBaseRecordData::AddDataFloat(LPCSTR lpszColumnName, const DOUBLE dbValue)
{
	CDataBaseData* pData = new CDataBaseData(lpszColumnName, SQLITE_FLOAT, dbValue);
	m_lstData.AddTail(pData);
	return pData;
}

const CDataBaseData* CDataBaseRecordData::AddDataText(LPCSTR lpszColumnName, const BYTE* lpszText)
{
	CDataBaseData* pData = new CDataBaseData(lpszColumnName, SQLITE_TEXT, lpszText);
	m_lstData.AddTail(pData);
	return pData;
}

const CDataBaseData* CDataBaseRecordData::AddDataBlob(LPCSTR lpszColumnName, LPCVOID lpData, const DWORD cbBuffer)
{
	CDataBaseData* pData = new CDataBaseData(lpszColumnName, SQLITE_BLOB, lpData, cbBuffer);
	m_lstData.AddTail(pData);
	return pData;
}

CDataBaseData* CDataBaseRecordData::FindData(LPCTSTR lpszColumnName)
{
	CDataBaseData* pResult = NULL;
	INT nCnt = m_lstData.GetCount();
	POSITION pos = m_lstData.GetHeadPosition();
 	while (pos)
	{
		
		pResult = m_lstData.GetNext(pos);

		if (CString(pResult->GetName()).CompareNoCase(lpszColumnName) == 0)
		{
			return pResult;
		}
	}

	return NULL;
}

const BOOL CDataBaseRecordData::GetIntergerData(LPCTSTR lpszColumnName, INT& nValue)
{
	CDataBaseData* pData = FindData(lpszColumnName);

	if (NULL == pData)
	{
		return FALSE;
	}
	
	switch (pData->GetType())
	{
	case SQLITE_INTEGER:
	{
		nValue = *static_cast<INT*>(pData->GetData());
	}	break;

	case SQLITE_FLOAT:
	{
		nValue = static_cast<INT>(*static_cast<DOUBLE*>(pData->GetData()));
	}	break;

	case SQLITE_TEXT:
	{
		nValue = atoi(static_cast<LPCSTR>(pData->GetData()));
	}	break;

	case SQLITE_BLOB:
	case SQLITE_NULL:
	{
		return FALSE;
	}	break;
	}

	return TRUE;
}

const BOOL CDataBaseRecordData::GetFloatData(LPCTSTR lpszColumnName, DOUBLE& dbValue)
{
	CDataBaseData* pData = FindData(lpszColumnName);

	if (NULL == pData)
	{
		return FALSE;
	}

	switch (pData->GetType())
	{
	case SQLITE_INTEGER:
	{
		dbValue = static_cast<DOUBLE>(*static_cast<INT*>(pData->GetData()));
	}	break;

	case SQLITE_FLOAT:
	{
		dbValue = *static_cast<DOUBLE*>(pData->GetData());
	}	break;

	case SQLITE_TEXT:
	{
		dbValue = atof(static_cast<LPCSTR>(pData->GetData()));
	}	break;

	case SQLITE_BLOB:
	case SQLITE_NULL:
	{
		return FALSE;
	}	break;
	}

	return TRUE;
}

const BOOL CDataBaseRecordData::GetTextData(LPCTSTR lpszColumnName, CString& strValue, LPCTSTR lpszFormat)
{
	CDataBaseData* pData = FindData(lpszColumnName);

	if (NULL == pData)
	{
		return FALSE;
	}

	switch (pData->GetType())
	{
	case SQLITE_INTEGER:
	{
		strValue.Format(NULL != lpszFormat ? lpszFormat : _T("%d"), *static_cast<INT*>(pData->GetData()));
	}	break;

	case SQLITE_FLOAT:
	{
		strValue.Format(NULL != lpszFormat ? lpszFormat : _T("%f"), *static_cast<INT*>(pData->GetData()));
	}	break;

	case SQLITE_TEXT:
	{
		strValue = static_cast<LPCSTR>(pData->GetData());
	}	break;

	case SQLITE_BLOB:
	case SQLITE_NULL:
	{
		return FALSE;
	}	break;
	}

	return TRUE;
}

const BOOL CDataBaseRecordData::GetBlobData(LPCTSTR lpszColumnName, LPVOID pValue, const DWORD cbBuffer)
{
	CDataBaseData* pData = FindData(lpszColumnName);

	if (NULL == pData)
	{
		return FALSE;
	}

	switch (pData->GetType())
	{
	case SQLITE_BLOB:
	{
		const size_t cbSize = pData->GetSize();
		if (cbSize > cbBuffer)
		{
			return FALSE;	//	용량 부족
		}

		memcpy(pValue, pData->GetData(), cbSize);
	}	break;

	case SQLITE_INTEGER:
	case SQLITE_FLOAT:
	case SQLITE_TEXT:
	case SQLITE_NULL:
	{
		return FALSE;
	}	break;
	}

	return TRUE;
}

const BOOL CDataBaseRecordData::GetImageData(LPCTSTR lpszColumnName, CBitmap* pBitmap)
{
	const INT cbSize = this->GetSize(lpszColumnName);
	if (-1 == cbSize)
	{
		return FALSE;
	}

	BOOL bResult = FALSE;
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, cbSize);
	void* pData = GlobalLock(hGlobal);
	if (this->GetBlobData(lpszColumnName, pData, cbSize))
	{
		IStream* pStream = NULL;
		if (S_OK == ::CreateStreamOnHGlobal(hGlobal, TRUE, &pStream))
		{
			if (pBitmap->GetSafeHandle())
			{
				pBitmap->Detach();
			}

			CImage image;
			if (NOERROR == image.Load(pStream))
			{
				pBitmap->Attach(image.Detach());

				bResult = TRUE;
			}
			pStream->Release();
		}
	}
	else
	{

	}

	GlobalUnlock(pData);
	GlobalFree(hGlobal);

	return bResult;
}

const INT CDataBaseRecordData::GetSize(LPCTSTR lpszColumnName)
{
	CDataBaseData* pData = FindData(lpszColumnName);
	if (NULL == pData)
	{
		return -1;
	}

	return pData->GetSize();
}

POSITION CDataBaseRecordData::GetStartPosition()
{
	return m_lstData.GetHeadPosition();
}

CDataBaseData* CDataBaseRecordData::GetNext(POSITION& position)
{
	return m_lstData.GetNext(position);
}

////////////////////////////////////////
//	CDataBaseManager
CDataBaseManager::CDataBaseManager(LPCTSTR lpszMutexName) : m_mutex(FALSE, lpszMutexName)
{
	m_strFileName.ReleaseBufferSetLength(::GetModuleFileName(NULL, m_strFileName.GetBuffer(MAX_PATH), MAX_PATH));
	const INT nIndex = m_strFileName.ReverseFind(_T('\\')) + 1;
	m_strFileName.Delete(nIndex, m_strFileName.GetLength());

	m_cursor = NULL;

	m_pDb = NULL;

	m_nLastDataBaseErrorCode = 0;
}

CDataBaseManager::~CDataBaseManager()
{
	this->RemoveAll();
}

void CDataBaseManager::RemoveAll()
{
	POSITION pos = m_lstRecord.GetHeadPosition();
	while (pos)
	{
		//TRACE(_T("delete m_lstRecord \r\n"));
		delete m_lstRecord.GetNext(pos);
	}

	m_lstRecord.RemoveAll();
}

void CDataBaseManager::Lock()
{
	m_mutex.Lock();
}

void CDataBaseManager::UnLock()
{
	m_mutex.Unlock();
}

const BOOL CDataBaseManager::Initialize()
{
	BOOL bResult = FALSE;

	return bResult;
}

void CDataBaseManager::GetStringAddress(CString& strAddress, const DWORD dwAddress)
{
	strAddress.Format(_T("%03d.%03d.%03d.%03d"), HIBYTE(HIWORD(dwAddress)), LOBYTE(HIWORD(dwAddress)), HIBYTE(LOWORD(dwAddress)), LOBYTE(LOWORD(dwAddress)));
}

const DWORD CDataBaseManager::StringToAddress(LPCTSTR lpszAddress)
{
	CString strValue(lpszAddress);

	const BYTE octet1 = _ttoi(strValue);
	strValue.Delete(0, strValue.Find(_T('.')) + 1);
	const BYTE octet2 = _ttoi(strValue);
	strValue.Delete(0, strValue.Find(_T('.')) + 1);
	const BYTE octet3 = _ttoi(strValue);
	strValue.Delete(0, strValue.Find(_T('.')) + 1);
	const BYTE octet4 = _ttoi(strValue);

	return static_cast<DWORD>(MAKELONG(MAKEWORD(octet4, octet3), MAKEWORD(octet2, octet1)));
}

const BOOL	CDataBaseManager::ExecuteQuery(LPCTSTR lpszQuery, const BOOL bRemovePrevRecord)
{
	if (NULL == m_pDb)
	{
		Lock();
	}

	sqlite3* pDB = m_pDb;

	BOOL bResult = FALSE;

	if (bRemovePrevRecord)
	{
		RemoveAll();
	}

	if (NULL == m_pDb)
	{
		const INT nStrLen = _tcslen(m_strFileName);
		const INT cbBuffer = ::WideCharToMultiByte(CP_UTF8, 0, m_strFileName, nStrLen, NULL, NULL, NULL, NULL) + 1;
		CHAR* pchFilename = new CHAR[cbBuffer];
		ZeroMemory(pchFilename, cbBuffer);
		::WideCharToMultiByte(CP_UTF8, 0, m_strFileName, nStrLen, pchFilename, cbBuffer, NULL, NULL);

		INT nResult = ::sqlite3_open(CStringA(pchFilename), &pDB);
		delete pchFilename;

		if (SQLITE_OK != nResult)
		{
			UnLock();
			return nResult;
		}
	}
	
	CStringA strQuery(lpszQuery);
	sqlite3_stmt* stmt = NULL;
	if (SQLITE_OK == ::sqlite3_prepare(pDB, strQuery, strQuery.GetLength(), &stmt, NULL))
	{
		const INT nCount = ::sqlite3_column_count(stmt);

		while (SQLITE_ROW == ::sqlite3_step(stmt))
		{
			const INT nCount = ::sqlite3_column_count(stmt);
			if (0 < nCount)
			{
				CDataBaseRecordData* pRerodData = new CDataBaseRecordData;
				m_lstRecord.AddTail(pRerodData);

				for (INT nCol = 0; nCol < nCount; nCol++)
				{
					const INT nType = ::sqlite3_column_type(stmt, nCol);

					switch (nType)
					{
					case SQLITE_INTEGER:
					{
						pRerodData->AddDataInteger(::sqlite3_column_name(stmt, nCol), ::sqlite3_column_int(stmt, nCol));
					}	break;

					case SQLITE_FLOAT:
					{
						pRerodData->AddDataFloat(::sqlite3_column_name(stmt, nCol), ::sqlite3_column_double(stmt, nCol));
					}	break;

					case SQLITE3_TEXT:
					{
						pRerodData->AddDataText(::sqlite3_column_name(stmt, nCol), ::sqlite3_column_text(stmt, nCol));
					}	break;

					case SQLITE_BLOB:
					{
						pRerodData->AddDataBlob(::sqlite3_column_name(stmt, nCol), ::sqlite3_column_blob(stmt, nCol), ::sqlite3_column_bytes(stmt, nCol));
					}	break;

					case SQLITE_NULL:
					{
						pRerodData->AddDataInteger(::sqlite3_column_name(stmt, nCol), 0);
					}	break;
					}
				}
			}
		}

		::sqlite3_finalize(stmt);

		bResult = TRUE;
	}

	if (NULL == m_pDb)
	{
		::sqlite3_close(pDB);
		UnLock();	
	}

	return bResult;
}

const BOOL CDataBaseManager::ExecuteQueryEx(LPCTSTR lpszQuery)
{
	if (m_pDb)
	{
		return SQLITE_OK == ::sqlite3_exec(m_pDb, CStringA(lpszQuery), NULL, NULL, NULL);
	}

	CDataBaseMutex _instance(&m_mutex);

	sqlite3* pDB = NULL;

	BOOL bResult = FALSE;

	const INT nStrLen = _tcslen(m_strFileName);
	const INT cbBuffer = ::WideCharToMultiByte(CP_UTF8, 0, m_strFileName, nStrLen, NULL, NULL, NULL, NULL) + 1;
	CHAR* pchFilename = new CHAR[cbBuffer];
	ZeroMemory(pchFilename, cbBuffer);
	::WideCharToMultiByte(CP_UTF8, 0, m_strFileName, nStrLen, pchFilename, cbBuffer, NULL, NULL);

	INT nResult = ::sqlite3_open(CStringA(pchFilename), &pDB);
	delete pchFilename;

	if (SQLITE_OK == nResult)
	{
		CStringA strQuery(lpszQuery);

		nResult = ::sqlite3_exec(pDB, strQuery, NULL, NULL, NULL);
		if (SQLITE_OK != nResult)
		{
			m_nLastDataBaseErrorCode = ::sqlite3_errcode(pDB);
			const char* chCheck = sqlite3_errmsg(pDB);
			return FALSE;
		}

		::sqlite3_close(pDB);
	}
	else
	{
		if (SQLITE_OK != nResult)
		{
			m_nLastDataBaseErrorCode = ::sqlite3_errcode(pDB);
			return FALSE;
		}
	}

	return TRUE;
}

const BOOL CDataBaseManager::ExecuteQuery(LPCTSTR lpszQuery, sqlite3_stmt** pStmt)
{
	if (m_pDb)
	{
		return SQLITE_OK == ::sqlite3_prepare(m_pDb, CStringA(lpszQuery), -1, pStmt, NULL);
	}

	return FALSE;
}

const INT CDataBaseManager::GetRecordCount()
{
	return m_lstRecord.GetCount();
}

const BOOL CDataBaseManager::SeekToBegin()
{
	if (0 == m_lstRecord.GetCount())
	{
		return FALSE;
	}
	
	m_cursor = m_lstRecord.GetHeadPosition();

	return TRUE;
}

const BOOL CDataBaseManager::IsEndOfRecord()
{
	return m_cursor ? FALSE : TRUE;
}

CDataBaseRecordData* CDataBaseManager::SeekToNext()
{
	CDataBaseRecordData* pResult = NULL;

	if (m_cursor)
	{
		pResult = m_lstRecord.GetNext(m_cursor);
	}

	return pResult;
}

CDataBaseRecordData* CDataBaseManager::GetRecord()
{
	return m_lstRecord.GetAt(m_cursor);
}

CDataBaseRecordData* CDataBaseManager::SeekRecord(LPCTSTR lpszColumnName, LPCTSTR lpszValue)
{
	CDataBaseRecordData* pResult = NULL;

	POSITION pos = m_lstRecord.GetHeadPosition();
	while (pos)
	{
		pResult = m_lstRecord.GetNext(pos);
		CString strValue;
		if (pResult->GetTextData(lpszColumnName, strValue) && (NULL == lpszValue || strValue == lpszValue))
		{
			return pResult;
		}
	}

	return NULL;
}

/// <summary>
/// Query의 동작 속도를 향상시키기 위한 방법으로 트랜잭션을 이용할 때 사용한다.
/// </summary>
/// 
/// <caution>
/// 본 함수는 DB를 미리 열어서 잠궈버리므로 OpenDataBase() 이후에는 반드시 CloseDataBase()를 정상적으로 호출해줘야 한다.
/// </caution>
const BOOL CDataBaseManager::OpenDataBase()
{
	/*if (m_pDb)
	{
		return TRUE;
	}*/

	Lock();

	const INT nStrLen = _tcslen(m_strFileName);
	const INT cbBuffer = ::WideCharToMultiByte(CP_UTF8, 0, m_strFileName, nStrLen, NULL, NULL, NULL, NULL) + 1;
	CHAR* pchFilename = new CHAR[cbBuffer];
	ZeroMemory(pchFilename, cbBuffer);
	::WideCharToMultiByte(CP_UTF8, 0, m_strFileName, nStrLen, pchFilename, cbBuffer, NULL, NULL);

	const BOOL bResult = (SQLITE_OK == ::sqlite3_open(CStringA(pchFilename), &m_pDb));

	delete pchFilename;

	if (FALSE == bResult)
	{
		UnLock();
	}

	return bResult;
}

const BOOL CDataBaseManager::IsDataBaseOpened()
{
	return NULL != m_pDb ? TRUE : FALSE;
}


const BOOL CDataBaseManager::Begin()
{
	const INT nTimeout = 1000;
	for (INT nTimeout = 0; nTimeout < 1000; nTimeout++)
	{
		const INT nResult = sqlite3_exec(m_pDb, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
		switch (nResult)
		{
		case SQLITE_OK:		return TRUE;
		case SQLITE_BUSY:	Sleep(1);		break;
		default:			ASSERT(0);		break;
		}
	}

	return FALSE;
}

const BOOL CDataBaseManager::Commit()
{
	const INT nTimeout = 1000;
	for (INT nTimeout = 0; nTimeout < 1000; nTimeout++)
	{
		const INT nResult = sqlite3_exec(m_pDb, "END TRANSACTION;", NULL, NULL, NULL);
		switch (nResult)
		{
		case SQLITE_OK:		return TRUE;
		case SQLITE_BUSY:	Sleep(1);		break;
		default:			ASSERT(0);		break;
		}
	}

	return FALSE;
}

const BOOL CDataBaseManager::CancelCommit()
{
	const INT nTimeout = 1000;
	for (INT nTimeout = 0; nTimeout < 1000; nTimeout++)
	{
		const INT nResult = sqlite3_exec(m_pDb, "ROLLBACK TRANSACTION;", NULL, NULL, NULL);
		switch (nResult)
		{
		case SQLITE_OK:		return TRUE;
		case SQLITE_BUSY:	Sleep(1);		break;
		default:			ASSERT(0);		break;
		}
	}

	return FALSE;
}

const BOOL CDataBaseManager::CloseDataBase()
{
	if (NULL == m_pDb)
	{
		return TRUE;
	}

	::sqlite3_close(m_pDb);

	m_pDb = NULL;

	UnLock();

	return TRUE;
}

const BOOL CDataBaseManager::BindText(sqlite3_stmt* pStmt, const INT nIndex, LPCSTR lpszValue)
{
	if (NULL == m_pDb)
	{
		return FALSE;
	}

	const INT nValue = ::sqlite3_bind_text(pStmt, nIndex, lpszValue, -1, NULL);
	return SQLITE_OK == nValue;
}

const BOOL CDataBaseManager::BindInteger(sqlite3_stmt* pStmt, const INT nIndex, const INT nValue)
{
	if (NULL == m_pDb)
	{
		return FALSE;
	}

	return SQLITE_OK == ::sqlite3_bind_int(pStmt, nIndex, nValue);
}

const BOOL CDataBaseManager::BindBlob(sqlite3_stmt* pStmt, const INT nIndex, LPBYTE pBuffer, const INT cbBuffer)
{
	if (NULL == m_pDb)
	{
		return FALSE;
	}

	return SQLITE_OK == ::sqlite3_bind_blob(pStmt, nIndex, pBuffer, cbBuffer, NULL );
}

const BOOL CDataBaseManager::StepQuery(sqlite3_stmt* pStmt)
{
	if (NULL == m_pDb)
	{
		return FALSE;
	}

	return SQLITE_DONE == ::sqlite3_step(pStmt);
}

const BOOL CDataBaseManager::FinalizeQuery(sqlite3_stmt* pStmt)
{
	if (NULL == m_pDb)
	{
		return FALSE;
	}

	return SQLITE_OK == ::sqlite3_finalize(pStmt);
}

const BOOL CDataBaseManager::DoVacuum()
{
	return ExecuteQuery(_T("vacuum;"), FALSE);
}


const INT CDataBaseManager::GetLastErrorCode()
{
	return m_nLastDataBaseErrorCode;
}

////////////////////////////////////////
//	CDataBaseLog
CDataBaseLog::CDataBaseLog() : CDataBaseManager(_T("Log"))
{
	m_strFileName += _T("Log.db");

	Initialize();
}


CDataBaseLog::~CDataBaseLog()
{

}

CDataBaseLog* CDataBaseLog::GetInstance()
{
	static CDataBaseLog _instance;
	return &_instance;
}

const BOOL CDataBaseLog::InsertLog(const INT nLevel, LPCTSTR lpszInformation, LPCTSTR lpszProjectName /*= NULL*/)
{
	CString strQuery;

	CDataBaseConfig* pConfig = CDataBaseConfig::GetInstance();

	if (lpszProjectName)
	{
		strQuery.Format(_T("insert into [Log_Data] values(DATETIME('now', 'localtime'), %d, '%s', '%s', '%s')"), nLevel, pConfig->GetOperatorName(), lpszProjectName, lpszInformation);
	}
	/*else
	{
		strQuery.Format(_T("insert into [Log_Data](Record_Info, Level, User, Information) values(DATETIME('now', 'localtime'), %d, '%s', '%s')"), nLevel, pConfig->GetOperatorName(), lpszInformation);
	}*/
	else
	{
	strQuery.Format(_T("insert into [Log_Data](Record_Info, Level, User, Device_Name, Device_IP, Information) values(DATETIME('now', 'localtime'), %d, '%s', '%s', '%s', '%s')"), nLevel, pConfig->GetOperatorName(), NULL, NULL,lpszInformation);
	}

	return ExecuteQueryEx(strQuery);
}


const BOOL CDataBaseLog::InsertLogData(const INT nLevel, LPCTSTR lpszInformation, CString strDeviceName, CString strDeviceIP, LPCTSTR lpszProjectName)
{
	CString strQuery;
	CDataBaseConfig* pConfig = CDataBaseConfig::GetInstance();

	strQuery.Format(_T("insert into [Log_Data](Record_Info, Level, User, Device_Name, Device_IP, Information) values(DATETIME('now', 'localtime'), %d, '%s', '%s', '%s', '%s')"), nLevel, pConfig->GetOperatorName(), strDeviceName, (LPCTSTR)strDeviceIP, lpszInformation);

	return ExecuteQueryEx(strQuery);
}


const BOOL CDataBaseLog::GetLogList(CString str)
{
	BOOL bResult = FALSE;
	CString strQuery;
	strQuery.Format(_T("select Record_Info, Information  from [Log_data] where (Information, Record_Info) in (select Information, max(Record_Info) as Record_Info from [Log_data] where Information = '%s') order by Record_Info desc"), str);
	
	bResult = ExecuteQuery(strQuery);

	return bResult;
}

const BOOL CDataBaseLog::GetLogInformationAll(LPVOID pDatabaseRecordData, INT& nRowid, CString& strInform, CString& strIP, CString& strID, CString& strTime)
{
	INT nResult = -1;
	CDataBaseRecordData* pRecord = static_cast<CDataBaseRecordData*>(pDatabaseRecordData);
	/*CString strInform;
	CString strIP;
	CString strID;*/
	CString strQuery;

	//if (ExecuteQuery(strQuery) && SeekToBegin())
	if(1)
	{
		Lock();
		//CDataBaseRecordData* pRecord = SeekToNext();
		CDataBaseRecordData* pRecord = m_lstRecord.GetAt(m_cursor);
		
		if (FALSE == pRecord->GetIntergerData(_T("rowid"), nRowid))
		{
			return -1;
		}
		
		/*if (FALSE == pRecord->GetTextData(_T("TimeStamp"), strTime) ||
			FALSE == pRecord->GetTextData(_T("Information"), strInform) ||
			FALSE == pRecord->GetTextData(_T("Device_Name"), strID) ||
			FALSE == pRecord->GetTextData(_T("Device_IP"), strIP))
		{
			return -1;
		}*/

		//CDataBaseLog* pLog = CDataBaseLog::GetInstance();

		UnLock();
		//CDataBaseLog* pLog = m_lstRecord.GetAt(m_cursor);
	}
	return TRUE;
}




////////////////////////////////////////
//	CDataBaseProject
CDataBaseProject::CDataBaseProject() : CDataBaseManager(_T("Project"))
{
	m_strFileName += _T("Project.db");

	LoadMotorDefaultParameter();
}


CDataBaseProject::~CDataBaseProject()
{

}

CDataBaseProject* CDataBaseProject::GetInstance()
{
	static CDataBaseProject _instance;
	return &_instance;
};

const BOOL CDataBaseProject::LoadAddressList()
{
	return ExecuteQuery(_T("select * from [IP_Address] order by [Priority] asc"));
}

//const BOOL CDataBaseProject::LoadDeviceList()
//{
//	return FALSE;
//}

const BOOL CDataBaseProject::GetCursorAddress(DWORD& dwAddrss)
{
	if (NULL == m_cursor)
	{
		return FALSE;
	}

	CDataBaseRecordData* pData = m_lstRecord.GetAt(m_cursor);
	CString strValue;
	if (FALSE == pData->GetTextData(_T("Address"), strValue))
	{
		return FALSE;
	}

	const BYTE octet1 = _ttoi(strValue);
	strValue.Delete(0, strValue.Find(_T('.')) + 1);
	const BYTE octet2 = _ttoi(strValue);
	strValue.Delete(0, strValue.Find(_T('.')) + 1);
	const BYTE octet3 = _ttoi(strValue);
	strValue.Delete(0, strValue.Find(_T('.')) + 1);
	const BYTE octet4 = _ttoi(strValue);

	dwAddrss = static_cast<DWORD>(MAKELONG(MAKEWORD(octet4, octet3), MAKEWORD(octet2, octet1)));

	return TRUE;
}

const BOOL CDataBaseProject::GetCursorProjectName(CString& strName)
{
	if (NULL == m_cursor)
	{
		return FALSE;
	}

	CDataBaseRecordData* pData = m_lstRecord.GetAt(m_cursor);
	return pData->GetTextData(_T("Project_Name"), strName);
}

const BOOL CDataBaseProject::GetCursorMotorCount(INT& nCount)
{
	if (NULL == m_cursor)
	{
		return FALSE;
	}

	CDataBaseRecordData* pData = m_lstRecord.GetAt(m_cursor);
	return pData->GetIntergerData(_T("Motor_Count"), nCount);
}

const BOOL CDataBaseProject::DeleteProjectTable()
{
	return ExecuteQuery(_T("Delete from [Project]"));
}

const BOOL CDataBaseProject::DeleteBackupListTable()
{
	return ExecuteQuery(_T("Delete from [Backup_List]"));
}

const BOOL CDataBaseProject::DeleteBackupListItem(CString strName)
{
	CString strQuery;
	strQuery.Format(_T("Delete from [Backup_List] where [Project_Name] = '%s';"),strName);
					
	return ExecuteQuery(strQuery);
}

const BOOL CDataBaseProject::DeleteBackupDataItem(CString strName)
{
	CString strQuery;
	strQuery.Format(_T("Delete from [Backup_Data] where [Project_Name] = '%s';"), strName);

	return ExecuteQuery(strQuery);
}

const BOOL CDataBaseProject::SetDeleteProjectAll(const BOOL bDelete)
{
	CString strQuery;
	strQuery.Format(_T("update [Project] set [Delete] = %d"), bDelete);
	return ExecuteQuery(strQuery);
}


const BOOL CDataBaseProject::SetDeleteValueProjcetTable(CString strProjectName, const BOOL bDelete)
{
	CString strQuery;
	strQuery.Format(_T("update [Project] set [Delete] = %d where [Project_Name] = '%s'"), bDelete, strProjectName);
	return ExecuteQuery(strQuery);
}



const BOOL CDataBaseProject::CopyList(CString strOrigin, CString strTarget, CString strProjectName)
{
	CString strQuery;
	//strQuery.Format(_T("Delete from [Restore_Backup_List] where [Project_Name] = '%s'"), strProjectName);
	strQuery.Format(_T("insert into [%s] select * from [%s] where [Project_Name] = '%s'"), strTarget, strOrigin, strProjectName);
	return ExecuteQuery(strQuery);
}



const BOOL CDataBaseProject::UpdateRestoreBackupListTable(CString strProjectName, BOOL bCopy, BOOL bDelete)
{
	CString strQuery = L"";
	if (bCopy)
	{
		strQuery.Format(_T("insert into [Restore_Backup_List] select * from [Backup_List]"));
		//strQuery.Format(_T("insert into [Restore_Backup_List] select * from [Backup_List] where [Project_Name] = '%s'"), strProjectName);
	}
	
	if (bDelete)
	{
		strQuery.Format(_T("Delete from [Restore_Backup_List] where [Project_Name] = '%s'"), strProjectName);
	}
	
	return ExecuteQuery(strQuery);
}


const BOOL CDataBaseProject::ResetPriority()
{
	return ExecuteQueryEx(_T("update [Project] set Priority = -1"));
}

const BOOL CDataBaseProject::UpdateProject(LPCTSTR lpszProjectName, const INT nPriority, const DWORD dwAddress, const INT nMotorCount, const BOOL bRemovePrevRecord)
{
	CString strAddress;
	this->GetStringAddress(strAddress, dwAddress);

	CString strQuery;
	strQuery.Format(_T("update [Project] set [Priority] = %d, [Address] = '%s', [Motor_Count] = %d, [Time_Modify] = DATETIME('now', 'localtime') where [Project_Name] = '%s'"),
		nPriority,
		strAddress,
		nMotorCount,
		lpszProjectName);

	return ExecuteQuery(strQuery, bRemovePrevRecord);
}


const BOOL CDataBaseProject::SetPriorityProject(LPCTSTR lpszProjectName, const INT nPriority, const BOOL bRemovePrevRecord)
{
	CString strQuery;
	strQuery.Format(_T("update [Project] set [Priority] = %d where [Project_Name] = '%s'"),
		nPriority,
		lpszProjectName);

	return ExecuteQuery(strQuery, bRemovePrevRecord);
}


//const BOOL CDataBaseProject::InsertProject(LPCTSTR lpszProjectName, const INT nPriority, const DWORD dwAddress, const INT nMotorCount, const BOOL bRemovePrevRecord)
//{
//	CString strAddress;
//	this->GetStringAddress(strAddress, dwAddress);
//
//	CString strQuery;
//	strQuery.Format(_T("insert into [Project](Project_Name, Priority, Address, Motor_Count) values('%s', %d, '%s', %d)"),
//		lpszProjectName,
//		nPriority,
//		strAddress,
//		nMotorCount);
//
//	return ExecuteQuery(strQuery, bRemovePrevRecord);
//}

const BOOL CDataBaseProject::SetBooleanProject(LPCTSTR lpszProjectName, CString str, BOOL bDelete)
{
	CString strQuery;
	BOOL bAlreadyOpen = IsDataBaseOpened();

	if (FALSE == bAlreadyOpen)
	{
		if (FALSE == OpenDataBase())
		{
			return FALSE;
		}
	}

	strQuery.Format(_T("update [Project] set [Delete] = %d where [Project_Name] = '%s'"), bDelete, str);
	BOOL bResult = ExecuteQuery(strQuery);

	if (FALSE == bAlreadyOpen)
	{
		CloseDataBase();
	}

	return bResult;
}

const BOOL CDataBaseProject::UpdateChangeProject(LPCTSTR lpszOldProjectName, LPCTSTR lpszNewProjectName, const INT nMotorCount, const INT nHomingTimeout, CBitmap* pImage, INT nDeviceID)
{
	CString strQuery;

	BOOL bOpened = IsDataBaseOpened();

	if (FALSE == bOpened)
	{
		if (FALSE == OpenDataBase())
		{
			return FALSE;
		}
	}

	BOOL bResult = FALSE;

	if (pImage && pImage->GetSafeHandle() && FALSE == CDataBaseConfig::GetInstance()->IsNoImage(pImage))
	{ // 이미지가 있을경우
		strQuery.Format(_T("update [Project] set Device_ID = %d ,Motor_Count = %d, Homing_Timeout = %d, Image = ? where [Project_Name] = '%s';"),
			nDeviceID,
			nMotorCount,
			nHomingTimeout,
			lpszNewProjectName);

		sqlite3_stmt* pStmt = NULL;
		if (ExecuteQuery(strQuery, &pStmt))
		{
			CImage image;
			image.Attach(static_cast<HBITMAP>(::CopyImage(*pImage, IMAGE_BITMAP, 0, 0, 0)));
			IStream* pStream = NULL;
			HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, &pStream);
			if (S_OK != image.Save(pStream, Gdiplus::ImageFormatBMP))
			{
				int i = 0;
			}

			ULARGE_INTEGER cbStream;
			IStream_Size(pStream, &cbStream);
			BYTE* pbtBuffer = new BYTE[cbStream.LowPart];
			IStream_Reset(pStream);
			IStream_Read(pStream, pbtBuffer, cbStream.LowPart);
			pStream->Release();

			INT nIndex = 1;
			if (BindBlob(pStmt, nIndex++, pbtBuffer, cbStream.LowPart) &&
				StepQuery(pStmt) &&
				FinalizeQuery(pStmt))
			{
				bResult = TRUE;
			}

			delete[] pbtBuffer;
		}
	}
	else
	{
		strQuery.Format(_T("update [Project] set Device_ID = %d ,Motor_Count = %d, Homing_Timeout = %d, Image = ? where [Project_Name] = '%s';"),
			nDeviceID,
			nMotorCount,
			nHomingTimeout,
			lpszNewProjectName);

		bResult = ExecuteQuery(strQuery);
	}

	if (FALSE == bOpened)
	{
		CloseDataBase();
	}

	return bResult;
}


const BOOL CDataBaseProject::InsertProject(LPCTSTR lpszProjectName, const INT nDeviceID, const INT nPriority, const INT nMotorCount, const INT nHomingTimeout, const INT nNominal, const INT nRestored, CBitmap* pImage)
{
	CString strQuery;

	BOOL bAlreayOpen = IsDataBaseOpened();

	if (FALSE == bAlreayOpen)
	{
		if (FALSE == OpenDataBase())
		{
			return FALSE;
		}
	}
	
	BOOL bResult = FALSE;

	if (pImage && pImage->GetSafeHandle() && FALSE == CDataBaseConfig::GetInstance()->IsNoImage(pImage) )
	{
		strQuery.Format(_T("insert or replace into [Project](Project_Name, Device_ID, Priority, Motor_Count, Homing_Timeout, Nominal_ID, Restored_ID, Image ) values( '%s', %d, %d, %d, %d, %d, %d, ?); "),
			lpszProjectName,
			nDeviceID,
			nPriority,
			nMotorCount,
			nHomingTimeout,
			nNominal,
			nRestored);
		
		sqlite3_stmt* pStmt = NULL;
		if (ExecuteQuery(strQuery, &pStmt))
		{
			CImage image;
			image.Attach(static_cast<HBITMAP>(::CopyImage(*pImage, IMAGE_BITMAP, 0, 0, 0)));
			IStream* pStream = NULL;
			HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, &pStream);
			if (S_OK != image.Save(pStream, Gdiplus::ImageFormatBMP))
			{
				int i = 0;
			}

			ULARGE_INTEGER cbStream;
			IStream_Size(pStream, &cbStream);
			BYTE* pbtBuffer = new BYTE[cbStream.LowPart];
			IStream_Reset(pStream);
			IStream_Read(pStream, pbtBuffer, cbStream.LowPart);
			pStream->Release();

			INT nIndex = 1;
			if (BindBlob(pStmt, nIndex++, pbtBuffer, cbStream.LowPart) &&
				StepQuery(pStmt) &&
				FinalizeQuery(pStmt) )
			{
				bResult = TRUE;
			}

			delete[] pbtBuffer;
		}
	}
	else
	{
		strQuery.Format(_T("insert or replace into [Project](Project_Name, Device_ID, Priority, Motor_Count, Homing_Timeout, Nominal_ID, Restored_ID, Image ) values( '%s', %d, %d, %d, %d, %d, %d, ?); "),
			lpszProjectName,
			nDeviceID,
			nPriority,
			nMotorCount,
			nHomingTimeout,
			nNominal,
			nRestored);

		bResult = ExecuteQuery(strQuery);
	}

	if (FALSE == bAlreayOpen)
	{
		CloseDataBase();
	}

	return bResult;
}



const BOOL CDataBaseProject::UpdateProject(LPCTSTR lpszOldProjectName, LPCTSTR lpszNewProjectName, const INT nMotorCount, const INT nHomingTimeout, CBitmap* pImage, INT nDeviceID)
{
	CString strQuery;

	BOOL bAlreayOpen = IsDataBaseOpened();

	if (FALSE == bAlreayOpen)
	{
		if (FALSE == OpenDataBase())
		{
			return FALSE;
		}
	}

	BOOL bResult = FALSE;

	if (pImage && pImage->GetSafeHandle() && FALSE == CDataBaseConfig::GetInstance()->IsNoImage(pImage))
	{ // Default Image가 아닐경우
		strQuery.Format(_T("update [Project] set Project_Name = '%s',Device_ID = %d ,Motor_Count = %d, Homing_Timeout = %d, Image = ? where [Project_Name] = '%s';"),
			lpszNewProjectName,
			nDeviceID,
			nMotorCount,
			nHomingTimeout,
			lpszOldProjectName);

		sqlite3_stmt* pStmt = NULL;
		if (ExecuteQuery(strQuery, &pStmt))
		{
			CImage image;
			image.Attach(static_cast<HBITMAP>(::CopyImage(*pImage, IMAGE_BITMAP, 0, 0, 0)));
			IStream* pStream = NULL;
			HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, &pStream);
			if (S_OK != image.Save(pStream, Gdiplus::ImageFormatBMP))
			{
				int i = 0;
			}

			ULARGE_INTEGER cbStream;
			IStream_Size(pStream, &cbStream);
			BYTE* pbtBuffer = new BYTE[cbStream.LowPart];
			IStream_Reset(pStream);
			IStream_Read(pStream, pbtBuffer, cbStream.LowPart);
			pStream->Release();

			INT nIndex = 1;
			if (BindBlob(pStmt, nIndex++, pbtBuffer, cbStream.LowPart) &&
				StepQuery(pStmt) &&
				FinalizeQuery(pStmt))
			{
				bResult = TRUE;
			}

			delete[] pbtBuffer;
		}
	}
	else
	{
		/* 기존
			strQuery.Format(_T("update [Project] set Project_Name = '%s', Motor_Count = %d, Homing_Timeout = %d, Image = null where [Project_Name] = '%s';"),
			lpszNewProjectName,
			nMotorCount,
			nHomingTimeout,
			lpszOldProjectName);*/

		strQuery.Format(_T("update [Project] set Project_Name = '%s',Device_ID = %d ,Motor_Count = %d, Homing_Timeout = %d, Image = ? where [Project_Name] = '%s';"),
			lpszNewProjectName,
			nDeviceID,
			nMotorCount,
			nHomingTimeout,
			lpszOldProjectName);

		bResult = ExecuteQuery(strQuery);
	}

	if (FALSE == bAlreayOpen)
	{
		CloseDataBase();
	}

	return bResult;
}

const BOOL CDataBaseProject::DeleteDisabledProject()
{
	return ExecuteQuery(_T("delete from [Project] where [Priority] = -1"));
}

const BOOL	CDataBaseProject::UpdateNominalBackupID(LPCTSTR lpszSystemName, const INT nBackupID)
{
	CString strQuery;
	strQuery.Format(_T("update [Project] set [Nominal_ID] = %d where [Project_Name] = '%s'"), nBackupID, lpszSystemName);
	return ExecuteQuery(strQuery);
}

const INT CDataBaseProject::GetNominalBackupID(LPCTSTR lpszSystemName)
{
	INT nResult = -1;

	CString strQuery;
	strQuery.Format(_T("Select [Nominal_ID] from [Project] where [Project_Name] = '%s'"), lpszSystemName);
	if (ExecuteQuery(strQuery) && SeekToBegin())
	{
		Lock();

		CDataBaseRecordData* pData = m_lstRecord.GetAt(m_cursor);
		pData->GetIntergerData(_T("Nominal_ID"), nResult);

		UnLock();
	}

	return nResult;
}



const BOOL CDataBaseProject::GetNominalBackupInfo(LPCTSTR lpszSystemName, INT* pnBackupID, CString* pstrTimeStamp)
{
	CString strQuery;
	strQuery.Format(_T("Select [Backup_ID], [TimeStamp] from [Backup_List] where [Project_Name] = '%s' and [Backup_ID] = ")
		_T("(Select [Nominal_ID] from [Project] where [Project_Name] = '%s');"), lpszSystemName, lpszSystemName);

	if (ExecuteQuery(strQuery) && SeekToBegin())
	{
		INT nBackupID;
		CString strTimeStamp;

		CDataBaseRecordData* pRecord = SeekToNext();
		if (pRecord->GetIntergerData(_T("Backup_ID"), nBackupID) &&
			-1 != nBackupID &&
			pRecord->GetTextData(_T("TimeStamp"), strTimeStamp))
		{
			if (pnBackupID)
			{
				*pnBackupID = nBackupID;
			}

			if (pstrTimeStamp)
			{
				*pstrTimeStamp = strTimeStamp;
			}

			UnLock();

			return TRUE;
		}

		UnLock();
	}

	return FALSE;
}

const BOOL CDataBaseProject::UpdateLatestRestoredID(LPCTSTR lpszSystemName, const INT nBackupID)
{
	CString strQuery;
	strQuery.Format(_T("update [Project] set [Restored_ID] = %d where [Project_Name] = '%s'"), nBackupID, lpszSystemName);
	return ExecuteQuery(strQuery);
}

const INT CDataBaseProject::GetLatestRestoredID(LPCTSTR lpszSystemName)
{
	INT nResult = -1;

	CString strQuery;
	strQuery.Format(_T("Select [Restored_ID] from [Project] where [Project_Name] = '%s'"), lpszSystemName);
	if (ExecuteQuery(strQuery) && SeekToBegin())
	{
		Lock();

		CDataBaseRecordData* pData = m_lstRecord.GetAt(m_cursor);
		pData->GetIntergerData(_T("Restored_ID"), nResult);

		UnLock();
	}

	return nResult;
}

const BOOL CDataBaseProject::GetLatestRestoredInfo(LPCTSTR lpszSystemName, INT* pnBackupID, CString* pstrTimeStamp)
{
	CString strQuery;
	strQuery.Format(_T("Select [Backup_ID], [TimeStamp] from [Backup_List] where [Project_Name] = '%s' and [Backup_ID] = ")
		_T("(Select [Restored_ID] from [Project] where [Project_Name] = '%s')"), lpszSystemName, lpszSystemName);

	if (ExecuteQuery(strQuery) && SeekToBegin())
	{
		Lock();

		INT nBackupID;
		CString strTimeStamp;

		CDataBaseRecordData* pRecord = SeekToNext();
		if (pRecord->GetIntergerData(_T("Backup_ID"), nBackupID) &&
			-1 != nBackupID &&
			pRecord->GetTextData(_T("TimeStamp"), strTimeStamp))
		{
			if (pnBackupID)
			{
				*pnBackupID = nBackupID;
			}

			if (pstrTimeStamp)
			{
				*pstrTimeStamp = strTimeStamp;
			}

			UnLock();

			return TRUE;
		}

		UnLock();
	}

	return FALSE;
}

const INT CDataBaseProject::CreateBackupID(LPCTSTR lpszSystemName, LPCTSTR lpszBackupName)
{
	INT nBackupID = 0;

	if (OpenDataBase())
	{
		CString strQuery;
		strQuery.Format(_T("select [Backup_ID] from [backup_list] where [Project_Name] = '%s' order by [Backup_ID] DESC limit 1"), lpszSystemName);

		CString strDebug;
		//TRACE(strQuery + _T("\r\n"));

		if (FALSE == ExecuteQuery(strQuery))
		{
			return -1;
		}

		
		if (SeekToBegin())
		{
			Lock();
			strDebug.Format(_T("Project : %s / pRecord->GetIntergerData(_T('Backup_ID'), nBackupID) \r\n"), lpszSystemName);
			//TRACE(strDebug);

			CDataBaseRecordData* pRecord = SeekToNext();
			pRecord->GetIntergerData(_T("Backup_ID"), nBackupID);
			nBackupID++;

			UnLock();
		}

		strQuery.Format(_T("insert into backup_list(Project_Name, backup_name, backup_id) values('%s', '%s', %d)"), lpszSystemName, lpszBackupName, nBackupID);

		if (FALSE == ExecuteQuery(strQuery))
		{
			return -1;
		}

		CloseDataBase();
	}
	
	return nBackupID;
}

const BOOL CDataBaseProject::WriteBackupData(LPCTSTR lpszSystemName, const INT nBackupID, const INT nMotorID, const INT nIndex, const INT nValue)
{
	CString strQuery;
	strQuery.Format(_T("insert into backup_data(Project_Name, backup_id, motor_id, parameter_index, parameter_value) values('%s', %d, %d, %d, %d)"),
		lpszSystemName,
		nBackupID,
		nMotorID,
		nIndex,
		nValue);

	return ExecuteQuery(strQuery);
}

const BOOL	CDataBaseProject::GetProjectBackupList(LPCTSTR lpszSystemName)
{
	CString strQuery;
	strQuery.Format(_T("select * from [backup_list] where [Project_Name] = '%s'"), lpszSystemName);

	return ExecuteQuery(strQuery);
}

const INT CDataBaseProject::GetProjectBackupListCount(LPCTSTR lpszSystemName)
{
	CString strQuery, strCount(_T("count('project_name')"));
	strQuery.Format(_T("select %s from [backup_list] where [Project_Name] = '%s'"), strCount, lpszSystemName);

	if (FALSE == ExecuteQuery(strQuery))
	{
		return FALSE;
	}

	if (0 == m_lstRecord.GetCount())
	{
		return 0;
	}

	INT nResult = 0;
	CDataBaseRecordData* pRecord = m_lstRecord.GetHead();
	pRecord->GetIntergerData(strCount, nResult);

	return nResult;
}


const BOOL CDataBaseProject::GetProjectName(INT nRowid, CString& strName)
{
	CString strQuery;
	BOOL bResult = FALSE;
	//strQuery.Format(_T("Select [Backup_Name] from [Backup_List] where rowid = %d"), nRowid);

	//if (FALSE == ExecuteQuery(strQuery))
	//{
	//	return FALSE;
	//}

	if (true)
	{
	/*	CDataBaseRecordData* pRecord = GetRecord();
		pRecord->GetTextData(_T("Backup_Name"), strName);*/
		/*CDataBaseRecordData* pRecord = GetRecord();
		if (pRecord->GetTextData(_T("Backup_Name"), strName))
		{
			bResult = TRUE;
		}
		else
		{
			bResult = FALSE;
		}*/
	}

	return bResult;
}


const BOOL CDataBaseProject::GetMotorCount(LPCTSTR lpszSystemName, INT& nCount)
{

	CString strQuery, strColumnName(_T("Motor_Count"));
	strQuery.Format(_T("Select %s from [Project] where [Project_Name] = '%s'"), strColumnName, lpszSystemName);

	if (FALSE == ExecuteQuery(strQuery))
	{
		return FALSE;
	}

	if (0 == m_lstRecord.GetCount())
	{
		nCount = 0;
		return TRUE;
	}

	CDataBaseRecordData* pRecord = m_lstRecord.GetHead();
	pRecord->GetIntergerData(strColumnName, nCount);

	return TRUE;
}

const BOOL CDataBaseProject::LoadMotorParameter(LPCTSTR lpszProjectName, const INT nBackupID, const INT nMotorIndex)
{
	CString strQuery;
	if (-1 != nMotorIndex)
	{
		strQuery.Format(_T("Select [Parameter_Index], [Parameter_value] from [Backup_Data] where [Project_Name] = '%s' and [Backup_ID] = %d and [Motor_ID] = %d"), lpszProjectName, nBackupID, nMotorIndex);
	}
	else
	{
		strQuery.Format(_T("Select [Motor_ID], [Parameter_Index], [Parameter_value] from [Backup_Data] where [Project_Name] = '%s' and [Backup_ID] = %d"), lpszProjectName, nBackupID);
	}

	return ExecuteQuery(strQuery);
}

const BOOL CDataBaseProject::GetBackupName(LPCTSTR lpszProjectName, const INT nBackupID)
{
	CString strQuery;
	strQuery.Format(_T("Select [Backup_Name] from [Backup_List] where [Project_Name] = '%s' and [Backup_ID] = %d"), lpszProjectName, nBackupID);

	return ExecuteQuery(strQuery);
}

const BOOL CDataBaseProject::GetDefaultLoadParameterParameter(CDeviceMotionGate::MOTOR_PARAMETER_DATA* pParameter)
{
	memcpy(pParameter, &m_stDefaultMotorParameter, sizeof(CDeviceMotionGate::MOTOR_PARAMETER_DATA));
	return TRUE;
}

const BOOL CDataBaseProject::LoadMotorDefaultParameter()
{
	if (FALSE == ExecuteQuery(_T("Select [Parameter_Index], [Parameter_value] from [Default_data]")))
	{
		return FALSE;
	}

	if (this->SeekToBegin())
	{
		Lock();

		while (CDataBaseRecordData* pData = this->SeekToNext())
		{
			INT nParamIndex = -1, nParamValue = -1;
			if (pData->GetIntergerData(_T("Parameter_Index"), nParamIndex) &&
				pData->GetIntergerData(_T("Parameter_Value"), nParamValue))
			{
				const INT nTableIndex = nParamIndex / 1000;
				const INT nIndex = nParamIndex % 1000;

				switch (nTableIndex)
				{
				case 0:
				{
					m_stDefaultMotorParameter.lParameter[nIndex] = static_cast<LONG>(nParamValue);
				}	break;

				default:
				{
					m_stDefaultMotorParameter.nodeItem[nTableIndex - 1].wBuffer[nIndex] = static_cast<WORD>(nParamValue);
				}	break;
				}
			}
			else
			{
				return FALSE;
			}
		}

		UnLock();
	}
	

	return TRUE;
}

const BOOL CDataBaseProject::SetHomingTimeout(LPCTSTR lpszSystemName, const INT nTimeout)
{
	CString strQuery;
	strQuery.Format(_T("insert or replace into [Project_data](Project_Name, Homing_timeout) values('%s', %d)"), lpszSystemName, nTimeout);

	return ExecuteQuery(strQuery);
}

const BOOL	CDataBaseProject::GetHomingTimeout(LPCTSTR lpszSystemName, INT& nTimeout)
{
	CString strQuery;
	strQuery.Format(_T("Select [Homing_Timeout] from [Project] where [Project_Name] = '%s'"), lpszSystemName);

	if (FALSE == ExecuteQuery(strQuery) || FALSE == SeekToBegin())
	{
		return FALSE;
	}

	Lock();

	CDataBaseRecordData* pData = m_lstRecord.GetAt(m_cursor);
	const BOOL bResult = pData->GetIntergerData(_T("Homing_Timeout"), nTimeout);

	UnLock();

	return bResult;
}

const BOOL CDataBaseProject::ChangeRegisterName(LPCTSTR lpszSystemName, LPCTSTR lpszNewProjectName)
{
	CString strQuery;
	strQuery.Format(_T("update [Project] set Project_name = '%s', Time_Modify = DATETIME('now', 'localtime') where [Project_Name] = '%s'"), lpszNewProjectName, lpszSystemName);

	return ExecuteQueryEx(strQuery);
}

const BOOL CDataBaseProject::ChangeBackupKey(LPCTSTR lpszCurKeyName, LPCTSTR lpszNewKeyName)
{
	CString strQuery;
	strQuery.Format(_T("update [Backup_list] set [Project_Name] = '%s' where [Project_Name] = '%s'"), lpszNewKeyName, lpszCurKeyName);

	if (FALSE == ExecuteQueryEx(strQuery))
	{
		return FALSE;
	}

	strQuery.Format(_T("update [Backup_Data] set [Project_Name] = '%s' where [Project_Name] = '%s'"), lpszNewKeyName, lpszCurKeyName);

	return ExecuteQueryEx(strQuery);
}

const BOOL CDataBaseProject::DeleteBackupData(const INT nBackupID, LPCTSTR lpszProjectName)
{
	CString strQuery;
	strQuery.Format(
		/*_T("delete from [Backup_List] where [Project_Name] = '%s' and [Backup_ID] = %d;  vacuum;")*/
		_T("delete from[Backup_Data] where[Project_Name] = '%s' and [Backup_ID] = %d; vacuum;"),
		lpszProjectName, nBackupID, lpszProjectName, nBackupID);

	return ExecuteQueryEx(strQuery);
}


const BOOL CDataBaseProject::DeleteBackupList(const INT nBackupID, LPCTSTR lpszProjectName)
{
	CString strQuery;
	strQuery.Format(
		_T("delete from [Backup_List] where [Project_Name] = '%s' and [Backup_ID] = %d;  vacuum;"),
		//_T("delete from[Backup_Data] where[Project_Name] = '%s' and [Backup_ID] = %d; vacuum;"),
		lpszProjectName, nBackupID, lpszProjectName, nBackupID);

	return ExecuteQueryEx(strQuery);
}


const BOOL CDataBaseProject::DeleteBackupReservationDataAll(LPCTSTR lpszSystemName)
{
	CString strQuery;
	strQuery.Format(_T("delete from [Schedule] where [Project_Name] = '%s'"), lpszSystemName);

	return ExecuteQueryEx(strQuery);
}

const BOOL CDataBaseProject::DeletePassedBackupReservationData(LPCTSTR lpszSystemName)
{
	CString strQuery;
	strQuery.Format(_T("delete from [Schedule] where [Project_Name] = '%s' and [Week_Code] = '' and [Schedule_DateTime] <= datetime('now', 'localtime')"), lpszSystemName);
	
	return ExecuteQueryEx(strQuery);
}

const BOOL CDataBaseProject::InsertBackupReservationData(LPCTSTR lpszSystemName, COleDateTime dateTime, LPCTSTR lpszWeekCode)
{
	CString strQuery;
	strQuery.Format(_T("insert into [Schedule](Project_Name, Schedule_DateTime, Week_Code) values('%s', '%s', '%s')"),
		lpszSystemName, dateTime.Format(_T("%Y-%m-%d %H:%M:%S")), lpszWeekCode);

	return ExecuteQueryEx(strQuery);
}

const BOOL CDataBaseProject::LoadBackupReservation(LPCTSTR lpszSystemName)
{
	CString strQuery;
	strQuery.Format(_T("Select * from [Schedule] where [Project_Name] = '%s';"), lpszSystemName);

	return ExecuteQuery(strQuery);
}

const BOOL CDataBaseProject::GetSingleReservationTime(LPCTSTR lpszSystemName, COleDateTime& dateTime)
{
	if (FALSE == OpenDataBase())
	{
		return FALSE;
	}

	BOOL bResult = FALSE;

	CString strQuery;
	strQuery.Format(_T("select [Schedule_DateTime] from [Schedule] where [Project_Name] = '%s' and [Week_Code] = '' and [Schedule_DateTime] > datetime('now', 'localtime') order by [Schedule_DateTime] limit 1"), lpszSystemName);

	if (ExecuteQuery(strQuery) && SeekToBegin())
	{
		CString strDateTime;
		CDataBaseRecordData* pRecord = GetRecord();
		if (pRecord->GetTextData(_T("Schedule_DateTime"), strDateTime))
		{
			bResult = dateTime.ParseDateTime(strDateTime);
		}		
	}

	CloseDataBase();

	return bResult;
}


const BOOL CDataBaseProject::GetRepeatReservationTime(LPCTSTR lpszSystemName, COleDateTime& dateTime)
{
	if (FALSE == OpenDataBase())
	{
		return FALSE;
	}

	COleDateTime now(COleDateTime::GetCurrentTime());
	now -= COleDateTimeSpan(0, 0, 0, now.GetSecond());

	const INT nWeekCode = now.GetDayOfWeek() - 1;

	CString strQuery, strDateTime, strWeekCode;

	BOOL bResult = FALSE;

	strQuery.Format(_T("select time([Schedule_DateTime]), [Week_Code] from [Schedule] where [Project_Name] = '%s'"), lpszSystemName);
	if (ExecuteQuery(strQuery) && SeekToBegin())
	{
		BOOL bSelected = FALSE;
		COleDateTime timeSelected = now + COleDateTimeSpan(8, 0, 0, 0);
		COleDateTime timeTemp;

		while (CDataBaseRecordData* pRecord = SeekToNext())
		{
			if (pRecord->GetTextData(_T("time([Schedule_DateTime])"), strDateTime) &&
				pRecord->GetTextData(_T("Week_Code"), strWeekCode) &&
				timeTemp.ParseDateTime(strDateTime))
			{
				for (INT nIndex = 0; nIndex < 7; nIndex++)
				{
					if (-1 != strWeekCode.Find(_T('A') + nIndex))
					{
						timeTemp = COleDateTime(now.GetYear(), now.GetMonth(), now.GetDay(), timeTemp.GetHour(), timeTemp.GetMinute(), timeTemp.GetSecond());

						if (nWeekCode != nIndex)
						{
							if (nWeekCode > nIndex)
							{
								timeTemp += COleDateTimeSpan(7 - (nWeekCode - nIndex), 0, 0, 0);
							}
							else
							{
								timeTemp += COleDateTimeSpan(nIndex - nWeekCode, 0, 0, 0);
							}
						}
						else if (timeTemp <= now)
						{
							timeTemp += COleDateTimeSpan(7, 0, 0, 0);
						}

						if (timeTemp < timeSelected)
						{
							timeSelected = timeTemp;

							bResult = TRUE;
						}
					}
				}
			}
		}

		if (bResult)
		{
			dateTime = timeSelected;
		}
	}

	CloseDataBase();

	return bResult;
}


const BOOL CDataBaseProject::GetSystemImage(LPCTSTR lpszSystemName, CBitmap* pBitmap)
{
	if (NULL == lpszSystemName)
	{
		return FALSE;
	}

	if (FALSE == OpenDataBase())
	{
		return FALSE;
	}

	BOOL bResult = FALSE;

	CString strQuery;
	strQuery.Format(_T("select [Image] from [Project] where [Project_Name] = '%s'"), lpszSystemName);
	if (ExecuteQuery(strQuery) && SeekToBegin())
	{
		CDataBaseRecordData* pRecord = GetRecord();
		if (pRecord->GetImageData(_T("Image"), pBitmap))
		{
			bResult = TRUE;
		}
	}

	CloseDataBase();

	return CDataBaseConfig::GetInstance()->LoadNoImage(pBitmap);
}

const BOOL CDataBaseProject::ChangeNominalBackupID(const INT nNominalBackupID, LPCTSTR lpszSystemName)
{
	CString strQuery;
	strQuery.Format(_T("update [project] set [Nominal_ID] = %d where [Project_Name] = '%s'"), nNominalBackupID, lpszSystemName);
	return ExecuteQuery(strQuery);
}

const BOOL CDataBaseProject::RemoveLastestBackupID(LPCTSTR lpszSystemName)
{
	CString strQuery;
	strQuery.Format(_T("update [project] set [Restored_ID] = -1 where [Project_Name] = '%s'"), lpszSystemName);
	return ExecuteQuery(strQuery);
}

const BOOL CDataBaseProject::ChangeIpAddress(const DWORD dwCurrentIpAddress, const DWORD dwNextIpAddress)
{
	CString strCurAddress, strNextAddress;
	GetStringAddress(strCurAddress, dwCurrentIpAddress);
	GetStringAddress(strNextAddress, dwNextIpAddress);

	CString strQuery;
	strQuery.Format(_T("update [Project] set [Address] = '%s' where [Address] = '%s'"), strNextAddress, strCurAddress);
	if (FALSE == ExecuteQuery(strQuery))
	{
		return FALSE;
	}

	strQuery.Format(_T("update [Backup_List] set [Address] = '%s' where [Address] = '%s'"), strNextAddress, strCurAddress);
	if (FALSE == ExecuteQuery(strQuery))
	{
		strQuery.Format(_T("update [Project] set [Address] = '%s' where [Address] = '%s'"), strCurAddress, strNextAddress );
		ExecuteQuery(strQuery);
		
		return FALSE;
	}
	
	strQuery.Format(_T("update [Backup_Data] set [Address] = '%s' where [Address] = '%s'"), strNextAddress, strCurAddress);
	if (FALSE == ExecuteQuery(strQuery))
	{
		strQuery.Format(_T("update [Backup_List] set [Address] = '%s' where [Address] = '%s'"), strNextAddress, strNextAddress);
		ExecuteQuery(strQuery);

		strQuery.Format(_T("update [Project] set [Address] = '%s' where [Address] = '%s'"), strNextAddress, strCurAddress);
		ExecuteQuery(strQuery);

		return FALSE;
	}

	return TRUE;
}

const BOOL CDataBaseProject::GetDeviceList(LPCTSTR lpszProjectName)
{
	BOOL bResult = FALSE;

	if (NULL == lpszProjectName)
	{
		bResult = ExecuteQuery(_T("select * from [project] order by [Priority] asc"));
	}
	else
	{
		CString strQuery;
		strQuery.Format(_T("select * from [project] where [Project_Name] = '%s'"), lpszProjectName);

		bResult = ExecuteQuery(strQuery);
	}

	return bResult && SeekToBegin();
}

const BOOL CDataBaseProject::GetBackupFileMotorIndex(LPCTSTR lpszProjectName, INT nBackupID)
{
	BOOL bResult = FALSE;
	CString strQuery;
	strQuery.Format(_T("select [Motor_ID] from [Backup_Data] where [Backup_ID] = '%d' and [Project_Name] = '%s' group by [Motor_ID] having count([Motor_ID]) > 1"), nBackupID, lpszProjectName);
	if (true)
	{
		bResult = ExecuteQuery(strQuery);
	}

	return bResult && SeekToBegin();
}

const BOOL CDataBaseProject::GetBackupMotorCount(LPCTSTR strName, INT nBackupID, INT& nCnt)
{
	CString strQuery;
	strQuery.Format(_T("select count(Motor_ID) from (select [Motor_ID], count(Motor_ID) from [Backup_Data] where ([Backup_ID] = '%d' and [Project_Name] = '%s')group by [Motor_ID] having count([Motor_ID]) > 1)"), nBackupID, strName);

	if (FALSE == ExecuteQuery(strQuery) || FALSE == SeekToBegin())
	{
		return FALSE;
	}

	Lock();

	CDataBaseRecordData* pData = m_lstRecord.GetAt(m_cursor);
	const BOOL bResult = pData->GetIntergerData(_T("count(Motor_ID)"), nCnt);

	UnLock();

	return bResult;
}

const BOOL CDataBaseProject::GetBackupMotorEndNumber(LPCTSTR strName, INT nBackupID, INT& nIndex)
{
	CString strQuery;
	strQuery.Format(_T("select [Motor_ID] from [backup_data] where [Project_Name] = '%s' and [Backup_ID] = '%d' order by [Motor_ID] desc limit 1"), strName, nBackupID);

	if (FALSE == ExecuteQuery(strQuery) || FALSE == SeekToBegin())
	{
		return FALSE;
	}

	Lock();

	CDataBaseRecordData* pData = m_lstRecord.GetAt(m_cursor);
	const BOOL bResult = pData->GetIntergerData(_T("Motor_ID"), nIndex);

	UnLock();

	return bResult;
}

const BOOL CDataBaseProject::GetBackupList()
{
	BOOL bResult = FALSE;

	if (1)
	{ // Project List에 등록되어 있는 Project Name만 추출하여 현시한다.
		 bResult = ExecuteQuery(_T("select [Backup_List].[Project_Name], count([Backup_List].[Project_Name]) from [Backup_List] inner join [Project] on [Backup_List].[Project_Name] = [Project].[Project_Name] group by [Backup_List].[Project_Name]"));
	}
	return bResult && SeekToBegin();
}

const BOOL CDataBaseProject::GetBackupListDetailData()
{
	BOOL bResult = FALSE;
	if (1)
	{ // Project List에 등록되어 있는 항목의 Nomianal 값과 Nominal TimeStamp값을 가져온다.
		bResult = ExecuteQuery(_T("select [Backup_List].[Project_Name] ,[Backup_List].[Backup_Name], [Backup_List].[TimeStamp] from [Backup_List] inner join [Project] on [Backup_List].[Project_Name] = [Project].[Project_Name] and [Backup_List].[Backup_ID] = [Project].Nominal_ID"));
	}

	return bResult && SeekToBegin();
}

const BOOL CDataBaseProject::DeleteAddressIndexAll()
{
	return ExecuteQuery(_T("delete from [IP_Address];"));
}

const BOOL CDataBaseProject::InsertAddressIndex(const DWORD dwAddress, const INT nPriority)
{
	CString strAddress;	GetStringAddress(strAddress, dwAddress);

	CString strQuery;
	strQuery.Format(_T("insert into [IP_Address](Address, Priority) values( '%s', %d );"), strAddress, nPriority);

	return ExecuteQuery(strQuery);
}

const INT CDataBaseProject::GetMaxPriority()
{
	INT nResult = -1;

	if (OpenDataBase())
	{
		if (ExecuteQuery(_T("select [Priority] from [Project] order by [Priority] desc limit 1")) && SeekToBegin() )
		{
			CDataBaseRecordData* pRecord = SeekToNext();
			pRecord->GetIntergerData(_T("Priority"), nResult);
		}

		CloseDataBase();
	}

	return nResult;
}



const BOOL CDataBaseProject::CheckOverlapDeviceID(INT nID, CString& strGetName, CString strCurrenteName)
{
	CString strQuery;
	//CString strResult = L"NULL!";
	//strQuery.Format(_T(" Select exists (Select [Project_Name] from [Project] where [Device_ID] in ('%d'))"), nID);
	strQuery.Format(_T(" Select [Project_Name] from [Project] where [Device_ID] in ('%d')"), nID);
	
	if (FALSE == ExecuteQuery(strQuery))
	{
		return FALSE;
	}

	if (0 == m_lstRecord.GetCount())
	{
		//nCount = 0;
		return TRUE;
	}

	CDataBaseRecordData* pRecord = m_lstRecord.GetHead();
	pRecord->GetTextData(L"Project_Name", strGetName);
	
	if (strGetName == L"" || strGetName == strCurrenteName)
	{
		return TRUE;
	}

	return FALSE;
 }


////////////////////////////////////////
//	CDataBaseConfig
CDataBaseConfig::CDataBaseConfig() : CDataBaseManager(_T("Config"))
{

	Initialize();

	m_strFileName += _T("Config.db");

	LoadInterface();

	LoadNoImage();

	LoadSerialSystemInfo();

	LoadTotalSystemInform();

	ChangeDefaultUser();
}

CDataBaseConfig::~CDataBaseConfig()
{
	if (m_stSerialDeviceInfo.bmpSystemImage.GetSafeHandle())
	{
		m_stSerialDeviceInfo.bmpSystemImage.Detach();
	}
}

CDataBaseConfig* CDataBaseConfig::GetInstance()
{
	static CDataBaseConfig _instance;
	return &_instance;
}

const BOOL CDataBaseConfig::ChangeUser(LPCTSTR lpszName, LPCTSTR lpszPassword)
{
	CString strQuery;
	strQuery.Format(_T("Select [Password] from [User] where [Name] = '%s' and [activate] = true"), lpszName);
	if (ExecuteQuery(strQuery) && SeekToBegin())
	{
		Lock();

		CDataBaseRecordData* pRecord = SeekToNext();
		CString strPassword;
		if (pRecord->GetTextData(_T("Password"), strPassword))
		{
			if (strPassword == lpszPassword)
			{
				strQuery.Format(_T("Select * from [User] where [Name] = '%s' and [activate] = true"), lpszName);
				if (ExecuteQuery(strQuery) && SeekToBegin())
				{
					CDataBaseRecordData* pRecord = SeekToNext();

					const BOOL bResult = pRecord->GetTextData(_T("Name"), m_strName) &&
						pRecord->GetIntergerData(_T("Default_User"), m_bDefault) &&
						pRecord->GetIntergerData(_T("Grant_Admin"), m_bAdmin) &&
						pRecord->GetIntergerData(_T("Grant_Engineer"), m_bEngineer) &&
						pRecord->GetIntergerData(_T("Grant_Operator"), m_bOperator) &&
						pRecord->GetIntergerData(_T("Grant_Backup"), m_bBackup) &&
						pRecord->GetIntergerData(_T("Grant_Backup_Manage"), m_bBackupManage) &&
						pRecord->GetIntergerData(_T("Grant_Restore"), m_bRestore) &&
						pRecord->GetIntergerData(_T("Grant_Compare"), m_bCompare) &&
						pRecord->GetIntergerData(_T("Grant_Configuration"), m_bConfiguration) &&
						pRecord->GetIntergerData(_T("Grant_Backup_Schedule"), m_bBackupSchedule) &&
						pRecord->GetIntergerData(_T("Grant_Reset"), m_bReset) &&
						pRecord->GetIntergerData(_T("Grant_IdAssignment"), m_bIdAssignment) &&
						pRecord->GetIntergerData(_T("Grant_ID_Reset"), m_bIDReset) &&
						pRecord->GetIntergerData(_T("Grant_Control"), m_bControl );

					CDataBaseData* pData = pRecord->FindData(_T("User_Image"));
					if (pData && pData->GetSize())
					{
						const size_t cbSize = pData->GetSize();
						HGLOBAL hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, cbSize);
						if (hBuffer)
						{
							HANDLE hLock = ::GlobalLock(hBuffer);
							if (hLock)
							{
								CopyMemory(hLock, pData->GetData(), cbSize);
								::GlobalUnlock(hBuffer);

								IStream* pStream = NULL;
								if (S_OK == ::CreateStreamOnHGlobal(hBuffer, FALSE, &pStream))
								{
									if (FALSE == m_picture.IsNull())
									{
										m_picture.Detach();
									}

									m_picture.Load(pStream);
									pStream->Release();
								}
							}

							::GlobalFree(hBuffer);
						}
					}

					UnLock();
					return bResult;
				}
			}
		}

		UnLock();
	}

	return FALSE;
}


const BOOL CDataBaseConfig::ChangeDefaultUser()
{
	CString strQuery(_T("select * from [user] where [default_user] = true and [activate] = true"));

	if (ExecuteQuery(strQuery) && SeekToBegin())
	{
		Lock();

		CDataBaseRecordData* pRecord = SeekToNext();

		const BOOL bResult = pRecord->GetTextData(_T("Name"), m_strName) &&
			pRecord->GetIntergerData(_T("Default_User"), m_bDefault) &&
			pRecord->GetIntergerData(_T("Grant_Admin"), m_bAdmin) &&
			pRecord->GetIntergerData(_T("Grant_Engineer"), m_bEngineer) &&
			pRecord->GetIntergerData(_T("Grant_Operator"), m_bOperator) &&
			pRecord->GetIntergerData(_T("Grant_Backup"), m_bBackup) &&
			pRecord->GetIntergerData(_T("Grant_Backup_Manage"), m_bBackupManage) &&
			pRecord->GetIntergerData(_T("Grant_Restore"), m_bRestore) &&
			pRecord->GetIntergerData(_T("Grant_Compare"), m_bCompare) &&
			pRecord->GetIntergerData(_T("Grant_Configuration"), m_bConfiguration) &&
			pRecord->GetIntergerData(_T("Grant_Backup_Schedule"), m_bBackupSchedule) &&
			pRecord->GetIntergerData(_T("Grant_Reset"), m_bReset) &&
			pRecord->GetIntergerData(_T("Grant_IdAssignment"), m_bIdAssignment) &&
			pRecord->GetIntergerData(_T("Grant_ID_Reset"), m_bIDReset) &&
			pRecord->GetIntergerData(_T("Grant_Control"), m_bControl);

		CDataBaseData* pData = pRecord->FindData(_T("User_Image"));
		if (pData && pData->GetSize())
		{
			const size_t cbSize = pData->GetSize();
			HGLOBAL hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, cbSize);
			if (hBuffer)
			{
				HANDLE hLock = ::GlobalLock(hBuffer);
				if (hLock)
				{
					CopyMemory(hLock, pData->GetData(), cbSize);
					::GlobalUnlock(hBuffer);

					IStream* pStream = NULL;
					if (S_OK == ::CreateStreamOnHGlobal(hBuffer, FALSE, &pStream))
					{
						if (FALSE == m_picture.IsNull())
						{
							m_picture.Detach();
						}

						m_picture.Load(pStream);
						pStream->Release();
					}
				}

				::GlobalFree(hBuffer);
			}
		}

		ASSERT(bResult);

		UnLock();

		return TRUE;
	}
	else
	{
		ASSERT(0);
	}

	return FALSE;
}


WORD CDataBaseConfig::GetAuthorizeControl(LPCTSTR lpszName)
{
	WORD wResult = 0;
	CString strQuery;
	strQuery.Format(_T("Select * from [User] where [Name] = '%s' and [activate] = true"), lpszName);
	
	if (ExecuteQuery(strQuery) && SeekToBegin())
	{
		Lock();
		CDataBaseRecordData* pRecord = SeekToNext();

		const BOOL bResult =
			pRecord->GetIntergerData(_T("Grant_Backup"), authorize.bBackup) &&
			pRecord->GetIntergerData(_T("Grant_Backup_Manage"), authorize.bBackupManage) &&
			pRecord->GetIntergerData(_T("Grant_Restore"), authorize.bRestore) &&
			pRecord->GetIntergerData(_T("Grant_Compare"), authorize.bCompare) &&
			pRecord->GetIntergerData(_T("Grant_Configuration"), authorize.bConfiguration) &&
			pRecord->GetIntergerData(_T("Grant_Backup_Schedule"), authorize.bBackupSchedule) &&
			pRecord->GetIntergerData(_T("Grant_Reset"), authorize.bReset) &&
			pRecord->GetIntergerData(_T("Grant_IdAssignment"), authorize.bIdAssign) &&
			pRecord->GetIntergerData(_T("Grant_ID_Reset"), authorize.bIdReset);

		wResult = ((authorize.bBackup ? 1 << BTN_BACKUP : 0) +
			(authorize.bBackupManage ? 1 << BTN_BACKUP_MANAGE : 0) +
			(authorize.bRestore ? 1 << BTN_RESOTRE : 0) +
			(authorize.bCompare ? 1 << BTN_COMPARE : 0) +
			(authorize.bConfiguration ? 1 << BTN_COMFIGURATION : 0) +
			(authorize.bBackupSchedule ? 1 << BTN_BACKUP_SCHEDULE : 0) +
			(authorize.bReset ? 1 << BTN_RESET : 0) +
			(authorize.bIdAssign ? 1 << BTN_ID_ASSIGN : 0) +
			(authorize.bIdReset ? 1 << BTN_ID_RESET : 0));

		UnLock();
	}

	return wResult;
}


const BOOL CDataBaseConfig::ReLoadAuthorizeControl(LPCTSTR lpszName)
{
	CString strQuery;
	strQuery.Format(_T("Select * from [User] where [Name] = '%s' and [activate] = true"), lpszName);

	if (ExecuteQuery(strQuery) && SeekToBegin())
	{
		Lock();

		CDataBaseRecordData* pRecord = SeekToNext();

		const BOOL bResult = 
			pRecord->GetIntergerData(_T("Grant_Backup"), m_bBackup) &&
			pRecord->GetIntergerData(_T("Grant_Backup_Manage"), m_bBackupManage) &&
			pRecord->GetIntergerData(_T("Grant_Restore"), m_bRestore) &&
			pRecord->GetIntergerData(_T("Grant_Compare"), m_bCompare) &&
			pRecord->GetIntergerData(_T("Grant_Configuration"), m_bConfiguration) &&
			pRecord->GetIntergerData(_T("Grant_Backup_Schedule"), m_bBackupSchedule) &&
			pRecord->GetIntergerData(_T("Grant_Reset"), m_bReset) &&
			pRecord->GetIntergerData(_T("Grant_IdAssignment"), m_bIdAssignment) &&
			pRecord->GetIntergerData(_T("Grant_ID_Reset"), m_bIDReset) &&
			pRecord->GetIntergerData(_T("Grant_Control"), m_bControl);

		UnLock();

		return TRUE;
	}
	else
	{
		ASSERT(0);
	}

	return FALSE;
}



LPCTSTR CDataBaseConfig::GetOperatorName()
{
	return m_strName;
}

const BOOL CDataBaseConfig::IsDefaultOperator()
{
	return m_bDefault;
}

CImage* CDataBaseConfig::GetUserPicture()
{
	return &m_picture;
}

const BOOL CDataBaseConfig::IsAdministrator()
{
	return m_bAdmin;
}

const BOOL CDataBaseConfig::IsEngineer()
{
	return m_bEngineer;
}


const BOOL CDataBaseConfig::IsOperator()
{
	return m_bOperator;
}

const BOOL CDataBaseConfig::IsDeviceBackupGranted()
{
	return m_bBackup;
}

const BOOL CDataBaseConfig::IsDeviceRestorationGranted()
{
	return m_bRestore;
}

const BOOL CDataBaseConfig::IsDeviceFactoryResetGranted()
{
	return m_bReset;
}

const BOOL CDataBaseConfig::IsDeviceIdAssignmentGranted()
{
	return  m_bIdAssignment;
}


const BOOL CDataBaseConfig::IsDeviceBackupManageGranted()
{
	return m_bBackupManage;
}


const BOOL CDataBaseConfig::IsDeviceCompareGranted()
{
	// TODO: 여기에 구현 코드 추가.
	return m_bCompare;
}


const BOOL CDataBaseConfig::IsDeviceConfigGranted()
{
	// TODO: 여기에 구현 코드 추가.
	return m_bConfiguration;
}


const BOOL CDataBaseConfig::IsDeviceBackupScheduleGranted()
{
	// TODO: 여기에 구현 코드 추가.
	return m_bBackupSchedule;
}


const BOOL CDataBaseConfig::IsDeviceIdResetGranted()
{
	return m_bIDReset;
}

const BOOL	CDataBaseConfig::IsControlGranted()
{
	return m_bControl;
}

void CDataBaseConfig::LoadInterface()
{
	CString strQuery;

#define QUERY_SELECT_INTERFACE		_T("Select [Value] from [Global_Data] where [Name] = '%s';")
	
	strQuery.Format(QUERY_SELECT_INTERFACE, STRING_INTERFACE_TYPE);
	m_stInferface.nInterface = interface_TCP;
	if (ExecuteQuery(strQuery) && SeekToBegin())
	{
		CDataBaseRecordData* pRecord = GetRecord();
		pRecord->GetIntergerData(_T("Value"), m_stInferface.nInterface);
	}

	strQuery.Format(QUERY_SELECT_INTERFACE, STRING_INTERFACE_SERIAL_PORT);
	m_stInferface.nSerialPort = -1;
	if (ExecuteQuery(strQuery) && SeekToBegin())
	{
		CDataBaseRecordData* pRecord = GetRecord();
		pRecord->GetIntergerData(_T("Value"), m_stInferface.nSerialPort);
	}

	strQuery.Format(QUERY_SELECT_INTERFACE, STRING_INTERFACE_MOTIONGATE);
	m_stInferface.bEnableMotionGate = TRUE;
	if (ExecuteQuery(strQuery) && SeekToBegin())
	{
		CDataBaseRecordData* pRecord = GetRecord();
		pRecord->GetIntergerData(_T("Value"), m_stInferface.bEnableMotionGate);
	}

	strQuery.Format(QUERY_SELECT_INTERFACE, STRING_INTERFACE_CONFIRM);
	m_stInferface.bConfirmToContinue = FALSE;
	if (ExecuteQuery(strQuery) && SeekToBegin())
	{
		CDataBaseRecordData* pRecord = GetRecord();
		pRecord->GetIntergerData(_T("Value"), m_stInferface.bConfirmToContinue);
	}
#undef QUERY_SELECT_INTERFACE
}

void CDataBaseConfig::LoadNoImage()
{
	if (ExecuteQuery(_T("Select [Image_Source] from [Image] where Name = 'No_Project_Image'")) && SeekToBegin())
	{
		Lock();

		CDataBaseRecordData* pRecord = GetRecord();

		CBitmap bitmap;
		pRecord->GetImageData(_T("Image_Source"), &bitmap);
		
		if (m_bmpNoImage.GetSafeHandle())
		{
			m_bmpNoImage.Detach();
		}

		m_bmpNoImage.Attach(::CopyImage(bitmap, IMAGE_BITMAP, 0, 0, 0));

		UnLock();
	}
}

HBITMAP CDataBaseConfig::SetNoImage(HBITMAP hBitmap)
{
	if (m_bmpNoImage.GetSafeHandle())
	{
		m_bmpNoImage.Detach();
	}
	
	m_bmpNoImage.Attach(::CopyImage(hBitmap, IMAGE_BITMAP, 0, 0, 0));

	if (FALSE == OpenDataBase())
	{
		return m_bmpNoImage;
	}

	CString strQuery;
	strQuery.Format(_T("insert or replace into [Image](Name, Image_Source) values( 'No_Project_Image', ?); "));

	sqlite3_stmt* pStmt = NULL;
	if (ExecuteQuery(strQuery, &pStmt))
	{
		CImage image;
		image.Attach(static_cast<HBITMAP>(::CopyImage(m_bmpNoImage, IMAGE_BITMAP, 0, 0, 0)));
		IStream* pStream = NULL;
		HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, &pStream);
		if (S_OK != image.Save(pStream, Gdiplus::ImageFormatBMP))
		{
			int i = 0;
		}

		ULARGE_INTEGER cbStream;
		IStream_Size(pStream, &cbStream);
		BYTE* pbtBuffer = new BYTE[cbStream.LowPart];
		IStream_Reset(pStream);
		IStream_Read(pStream, pbtBuffer, cbStream.LowPart);
		pStream->Release();

		INT nIndex = 1;
		if (BindBlob(pStmt, nIndex++, pbtBuffer, cbStream.LowPart) &&
			StepQuery(pStmt) &&
			FinalizeQuery(pStmt))
		{
			
		}
		else
		{
			int i = 0;
		}

		delete[] pbtBuffer;
	}

	CloseDataBase();

	return m_bmpNoImage;
}

HBITMAP CDataBaseConfig::GetNoImage()
{
	return static_cast<HBITMAP>(m_bmpNoImage.GetSafeHandle());
}

const BOOL CDataBaseConfig::LoadNoImage(CBitmap* pBitmap)
{
	if (NULL == pBitmap)
	{
		return FALSE;
	}

	if (pBitmap->GetSafeHandle())
	{
		pBitmap->Detach();
	}

	pBitmap->Attach(::CopyImage(m_bmpNoImage, IMAGE_BITMAP, 0, 0, 0));

	return TRUE;
}

const BOOL CDataBaseConfig::IsNoImage(CBitmap* pBitmap)
{
	if (NULL == pBitmap || FALSE == pBitmap->GetSafeHandle())
	{
		return TRUE;
	}

	BITMAP bitmapTarget, bitmapSource;
	
	pBitmap->GetBitmap(&bitmapTarget);
	m_bmpNoImage.GetBitmap(&bitmapSource);

	if (0 != memcmp(&bitmapTarget, &bitmapSource, sizeof(bitmapTarget)))
	{
		return FALSE;
	}

	DWORD dwSize = bitmapTarget.bmWidth * bitmapTarget.bmHeight * bitmapTarget.bmPlanes * (bitmapTarget.bmBitsPixel >> 3);	// 2^3 = 8 Bit

	BYTE* pbtDataTarget = NULL;
	if (1)
	{		
		pbtDataTarget = new BYTE[dwSize];
		const DWORD dwBuffer = pBitmap->GetBitmapBits(dwSize, pbtDataTarget);
	}

	BYTE* pbtDataSource = NULL;
	if (1)
	{
		pbtDataSource = new BYTE[dwSize];
		const DWORD dwBuffer = m_bmpNoImage.GetBitmapBits(dwSize, pbtDataSource);
	}

	const BOOL bResult = (0 == memcmp(pbtDataTarget, pbtDataSource, dwSize));
	
	delete[] pbtDataTarget;
	delete[] pbtDataSource;

	return bResult;
}

const BOOL CDataBaseConfig::LoadUserImage(LPCTSTR lpszName, CBitmap* pBitmap)
{
	CString strQuery;
	strQuery.Format(_T("Select [User_Image] from [User] where Name = '%s'"), lpszName);

	if ((ExecuteQuery(strQuery) && SeekToBegin()) ||
		(ExecuteQuery(_T("Select [User_Image] from [User] where Name = 'UnKnown'")) && SeekToBegin()))
	{
		Lock();

		CDataBaseRecordData* pRecord = GetRecord();
		const BOOL bResult = pRecord->GetImageData(_T("User_Image"), pBitmap);

		UnLock();

		return bResult;
	}

	return FALSE;
}


const BOOL CDataBaseConfig::LoadUserPassword(LPCTSTR lpszName, CString& strPassword)
{
	BOOL bResult;
	CString strQuery;
	
	strQuery.Format(_T("Select [Password] from [User] where Name = '%s'"), lpszName);

	if ((ExecuteQuery(strQuery) && SeekToBegin()))
	{
		Lock();

		CDataBaseRecordData* pRecord = GetRecord();

		bResult = pRecord->GetTextData(_T("Password"), strPassword);

		UnLock();

		return bResult;
	}

	return FALSE;
}

const BOOL CDataBaseConfig::ChangePassword(PASSWORD_NODE node)
{
	CString strQuery;
	strQuery.Format(_T("update [User] set Password = '%s' where Name = '%s'"), node.strPassword, node.strName);
	return ExecuteQuery(strQuery);
}

void CDataBaseConfig::SetInterface( const INT nType)
{
	if (nType != m_stInferface.nInterface)
	{
		m_stInferface.nInterface = nType;

		CString strQuery;

		strQuery.Format(_T("insert or replace into [Global_Data] values( '%s', '%d' );"), STRING_INTERFACE_TYPE, nType);

		const BOOL bResult = ExecuteQuery(strQuery);
		
		ASSERT(bResult);
	}
}

const INT CDataBaseConfig::GetInterface()
{
	return m_stInferface.nInterface;
}

void CDataBaseConfig::SetSerialPort(const INT nPortNo)
{
	if (nPortNo != m_stInferface.nSerialPort )
	{
		m_stInferface.nSerialPort = nPortNo;

		CString strQuery;

		strQuery.Format(_T("insert or replace into [Global_Data] values( '%s', '%d' );"), STRING_INTERFACE_SERIAL_PORT, nPortNo);

		const BOOL bResult = ExecuteQuery(strQuery);

		ASSERT(bResult);
	}
}

const INT CDataBaseConfig::GetSerialPort()
{
	return m_stInferface.nSerialPort;
}

void CDataBaseConfig::EnableMotionGate(const BOOL bEnable)
{
	if (bEnable != m_stInferface.bEnableMotionGate )
	{
		m_stInferface.bEnableMotionGate = bEnable;

		CString strQuery;

		strQuery.Format(_T("insert or replace into [Global_Data] values( '%s', '%d' );"), STRING_INTERFACE_MOTIONGATE, bEnable);

		const BOOL bResult = ExecuteQuery(strQuery);

		ASSERT(bResult);
	}
}

const BOOL CDataBaseConfig::IsMotionGateEnabled()
{
	return m_stInferface.bEnableMotionGate;
}

void CDataBaseConfig::ConfirmToContinue(const BOOL bEnable)
{
	if (bEnable != m_stInferface.bConfirmToContinue)
	{
		m_stInferface.bConfirmToContinue = bEnable;

		CString strQuery;

		strQuery.Format(_T("insert or replace into [Global_Data] values( '%s', '%d' );"), STRING_INTERFACE_CONFIRM, bEnable);

		const BOOL bResult = ExecuteQuery(strQuery);

		ASSERT(bResult);
	}
}

const BOOL CDataBaseConfig::IsConfirmToContinueEnabled()
{
	return m_stInferface.bConfirmToContinue;
}


void CDataBaseConfig::LoadTotalSystemInform()
{
	if (1)
	{ // Standard Life Count
		CString strQuery;
		CString strValue;

		m_stTotalDeviceConfig.nLifeCountCycle = 0;
		strQuery.Format(_T("Select [Value] from [Global_Data] where Name = '%s'"), STRING_TOTAL_LIFECOUNT_STAND);

		if (ExecuteQuery(strQuery) && SeekToBegin())
		{
			CDataBaseRecordData* pRecord = GetRecord();
			if (pRecord->GetTextData(_T("Value"), strValue))
			{
				m_stTotalDeviceConfig.nLifeCountStandard = _ttoi(strValue);
			}
		}
	}

	if (1)
	{ // Cycle Life Count
		CString strQuery;
		CString strValue;

		m_stTotalDeviceConfig.nLifeCountCycle = 0;
		strQuery.Format(_T("Select [Value] from [Global_Data] where Name = '%s'"), STRING_TOTAL_LIFECOUNT_CYCLE);

		if (ExecuteQuery(strQuery) && SeekToBegin())
		{
			CDataBaseRecordData* pRecord = GetRecord();
			if (pRecord->GetTextData(_T("Value"), strValue))
			{
				m_stTotalDeviceConfig.nLifeCountCycle = _ttoi(strValue);
			}
		}
	}
}


INT CDataBaseConfig::GetLifeCountStandardValue()
{
	return m_stTotalDeviceConfig.nLifeCountStandard;
}


INT CDataBaseConfig::GetLifeCountCycleValue()
{
	return m_stTotalDeviceConfig.nLifeCountCycle;
}


void CDataBaseConfig::LoadSerialSystemInfo()
{	
	if (1)
	{	//	1. Address
		m_stSerialDeviceInfo.dwAddress = 0;

		CString strQuery;
		strQuery.Format(_T("Select [Value] from [Global_Data] where Name = '%s'"), STRING_SERIAL_ADDRESS);

		if (ExecuteQuery(strQuery) && SeekToBegin())
		{
			CDataBaseRecordData* pRecord = GetRecord();
			CString strValue;
			if (pRecord->GetTextData(_T("Value"), strValue))
			{
				m_stSerialDeviceInfo.dwAddress = StringToAddress(strValue);
			}
		}
	}

	if (1)
	{	//	2. Name
		m_stSerialDeviceInfo.strSystemName = _T("None");

		CString strQuery;
		strQuery.Format(_T("Select [Value] from [Global_Data] where Name = '%s'"), STRING_SERIAL_NAME);

		if (ExecuteQuery(strQuery) && SeekToBegin())
		{
			CDataBaseRecordData* pRecord = GetRecord();
			pRecord->GetTextData(_T("Value"), m_stSerialDeviceInfo.strSystemName);
		}
	}

	if (1)
	{	//	3. Motor Count
		m_stSerialDeviceInfo.nMotorCount = 1;

		CString strQuery;
		strQuery.Format(_T("Select [Value] from [Global_Data] where Name = '%s'"), STRING_SERIAL_MOTOR_COUNT);

		if (ExecuteQuery(strQuery) && SeekToBegin())
		{
			CDataBaseRecordData* pRecord = GetRecord();
			CString strValue;
			if (pRecord->GetTextData(_T("Value"), strValue))
			{
				m_stSerialDeviceInfo.nMotorCount = _ttoi(strValue);
			}
		}
	}
	
	if (1)
	{	//	4. Homing Timeout
		m_stSerialDeviceInfo.nHomingTimeout = 10000;

		CString strQuery;
		strQuery.Format(_T("Select [Value] from [Global_Data] where Name = '%s'"), STRING_SERIAL_HOMING_TIMEOUT);

		if (ExecuteQuery(strQuery) && SeekToBegin())
		{
			CDataBaseRecordData* pRecord = GetRecord();
			CString strValue;
			if (pRecord->GetTextData(_T("Value"), strValue) )
			{
				m_stSerialDeviceInfo.nHomingTimeout = _ttoi(strValue);
			}
		}
	}
	
	if (1)
	{	// 5. Modbus_WriteAddress
		CString strQuery;
		strQuery.Format(_T("Select [Value] from [Global_Data] where Name = '%s'"), STRING_MODBUS_WRITEADDRESS);

		if (ExecuteQuery(strQuery) && SeekToBegin())
		{
			CDataBaseRecordData* pRecord = GetRecord();
			CString strValue;
			if (pRecord->GetTextData(_T("Value"), strValue))
			{
				m_stSerialDeviceInfo.nModbusWriteAddress = _ttoi(strValue);
				//m_stSerialDeviceInfo.wModbusWirteAddress = GetStringToWord(strValue);
				m_stSerialDeviceInfo.wModbusWirteAddress = _ttoi(strValue);
			}
		}
	}

	if (1)
	{	//	6. Image
		if (m_stSerialDeviceInfo.bmpSystemImage.GetSafeHandle())
		{
			m_stSerialDeviceInfo.bmpSystemImage.Detach();
		}

		CString strQuery;
		strQuery.Format(_T("Select[Image_Source] from[Image] where Name = '%s'"), IMAGE_SERIAL_SYSTEM);

		if (ExecuteQuery(strQuery) && SeekToBegin())
		{
			CDataBaseRecordData* pRecord = GetRecord();
			const BOOL bResult = pRecord->GetImageData(_T("Image_Source"), &m_stSerialDeviceInfo.bmpSystemImage);
		}
		else
		{
			m_stSerialDeviceInfo.bmpSystemImage.Attach(static_cast<HBITMAP>(::CopyImage(m_bmpNoImage, IMAGE_BITMAP, 0, 0, 0)));
		}
	}
}

void CDataBaseConfig::SetSerialAddress(const DWORD dwAddress)
{
	if (dwAddress != m_stSerialDeviceInfo.dwAddress)
	{
		m_stSerialDeviceInfo.dwAddress = dwAddress;

		CString strAddress;
		GetStringAddress(strAddress, dwAddress);

		CString strQuery;
		strQuery.Format(_T("Insert Into[Global_Data] Values('%s', '%s');"), STRING_SERIAL_ADDRESS, strAddress);

		const BOOL bResult = ExecuteQuery(strQuery);
		
		ASSERT(bResult);
	}
}

const DWORD	CDataBaseConfig::GetSerialAddress()
{
	return m_stSerialDeviceInfo.dwAddress;
}

void CDataBaseConfig::SetSerialName(LPCTSTR lpszName)
{
	if (m_stSerialDeviceInfo.strSystemName != lpszName)
	{
		m_stSerialDeviceInfo.strSystemName = lpszName;

		CString strQuery;
		strQuery.Format(_T("Insert Into[Global_Data] Values('%s', '%s');"), STRING_SERIAL_NAME, m_stSerialDeviceInfo.strSystemName);

		const BOOL bResult = ExecuteQuery(strQuery);

		ASSERT(bResult);
	}
}

LPCTSTR		CDataBaseConfig::GetSerialName()
{
	return m_stSerialDeviceInfo.strSystemName;
}

void		CDataBaseConfig::SetSerialMotorCount(const INT nCount)
{
	if (nCount != m_stSerialDeviceInfo.nMotorCount)
	{
		m_stSerialDeviceInfo.nMotorCount = nCount;

		CString strQuery;
		strQuery.Format(_T("Insert Into [Global_Data] Values('%s', %d);"), STRING_SERIAL_MOTOR_COUNT, m_stSerialDeviceInfo.nMotorCount);

		const BOOL bResult = ExecuteQuery(strQuery);

		ASSERT(bResult);
	}
}

const INT	CDataBaseConfig::GetSerialMotorCount()
{
	return m_stSerialDeviceInfo.nMotorCount;
}

void CDataBaseConfig::SetSerialHomingTimeout(const INT nTimeout, const BOOL bSecond)
{
	if (nTimeout != m_stSerialDeviceInfo.nHomingTimeout)
	{
		m_stSerialDeviceInfo.nHomingTimeout = bSecond ? nTimeout * 1000 : nTimeout;

		CString strQuery;
		strQuery.Format(_T("Insert Into [Global_Data] Values('%s', %d);"), STRING_SERIAL_HOMING_TIMEOUT, m_stSerialDeviceInfo.nHomingTimeout);

		const BOOL bResult = ExecuteQuery(strQuery);

		ASSERT(bResult);
	}
}

const INT	CDataBaseConfig::GetSerialHomingTimeout(const BOOL bSecond)
{
	return bSecond ? m_stSerialDeviceInfo.nHomingTimeout / 1000 : m_stSerialDeviceInfo.nHomingTimeout;
}


HBITMAP CDataBaseConfig::SetSerialSystemImage(HBITMAP hBitmap)
{
	if (m_stSerialDeviceInfo.bmpSystemImage.GetSafeHandle())
	{
		m_stSerialDeviceInfo.bmpSystemImage.Detach();
	}

	m_stSerialDeviceInfo.bmpSystemImage.Attach(static_cast<HBITMAP>(::CopyImage(hBitmap, IMAGE_BITMAP, 0, 0, 0)));

	if (FALSE == OpenDataBase())
	{
		return m_stSerialDeviceInfo.bmpSystemImage;
	}

	CString strQuery;
	strQuery.Format(_T("insert or replace into [Image] values( '%s', ? );"), IMAGE_SERIAL_SYSTEM );

	sqlite3_stmt* pStmt = NULL;
	if (ExecuteQuery(strQuery, &pStmt))
	{
		CImage image;
		image.Attach(static_cast<HBITMAP>(::CopyImage(hBitmap, IMAGE_BITMAP, 0, 0, 0)));
		IStream* pStream = NULL;
		HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, &pStream);
		if (S_OK != image.Save(pStream, Gdiplus::ImageFormatBMP))
		{
			int i = 0;
		}

		ULARGE_INTEGER cbStream;
		IStream_Size(pStream, &cbStream);
		BYTE* pbtBuffer = new BYTE[cbStream.LowPart];
		IStream_Reset(pStream);
		IStream_Read(pStream, pbtBuffer, cbStream.LowPart);
		pStream->Release();

		INT nIndex = 1;
		if (BindBlob(pStmt, nIndex++, pbtBuffer, cbStream.LowPart) &&
			StepQuery(pStmt) &&
			FinalizeQuery(pStmt))
		{
			
		}
		else
		{
			ASSERT(0);
		}

		delete[] pbtBuffer;
	}

	CloseDataBase();

	return m_stSerialDeviceInfo.bmpSystemImage;
}

HBITMAP CDataBaseConfig::GetSerialSystemImage()
{
	return m_stSerialDeviceInfo.bmpSystemImage;
}


const INT CDataBaseConfig::GetModBusWriteAddress()
{
	return m_stSerialDeviceInfo.nModbusWriteAddress;
}



const BOOL CDataBaseConfig::DeleteModBusWirteAddress(const INT nLatestAddress)
{
	CString strQuery;
	strQuery.Format(_T("update [Global_Data] set [Value] = %d where [Name] = '%s'"), nLatestAddress, _T("Modbus_WriteAddress"));

	return ExecuteQuery(strQuery);
}

const BOOL CDataBaseConfig::SetModBusWriteAddress(const INT nWriteAddress)
{
	CString strQuery;
	strQuery.Format(_T("update [Global_Data] set [Value] = %d where [Name] = '%s'"), nWriteAddress, _T("Modbus_WriteAddress"));
	m_stSerialDeviceInfo.nModbusWriteAddress = nWriteAddress;
	return ExecuteQuery(strQuery);
}


BOOL CDataBaseConfig::SetLifeCountStandardAlarmValue(INT nStandard)
{
	CString strQuery;
	strQuery.Format(_T("update [Global_Data] Set [Value] = %d where Name = 'Life_Count_Alarm_Standard'"), nStandard);
	return ExecuteQuery(strQuery);
}

BOOL CDataBaseConfig::SetLifeCountCycleAlarmValue(INT nCycle)
{
	CString strQuery;
	strQuery.Format(_T("update [Global_Data] Set [Value] = %d where Name = 'Life_Count_Alarm_Cycle'"), nCycle);
	return ExecuteQuery(strQuery);
}




void CDataBaseConfig::SetModBusWriteAddressTypeWORD(WORD wValue)
{
	m_stSerialDeviceInfo.wModbusWirteAddress = wValue;
}


WORD CDataBaseConfig::GetModBusWriteAddressTypeWORD()
{
	// TODO: 여기에 구현 코드 추가.
	return m_stSerialDeviceInfo.wModbusWirteAddress;
}



WORD CDataBaseConfig::GetStringToWord(CString str)
{
	WORD wResult = 0;
	
	if (2 > str.GetLength())
	{
		wResult = _ttoi(str);
	}

	else
	{
		INT nLength = str.GetLength() / 2;
		for (INT i = 0; i < nLength; i++)
		{
			wResult |= GetState(str.Mid(2 * i, 2)) << (nLength == 1 ? 0 : (8 * (1 - i)));
		}
	}

	return wResult;
}



unsigned char CDataBaseConfig::GetState(CString str)
{
	//CString str;
	CString strReturnValue;
	CString strValue;
	unsigned char cVal = 0;
	//CEdit* pEdit = (CEdit*)GetDlgItemText(id, str);
	str.MakeUpper();
	for (int i = 0; i < str.GetLength(); i++)
	{
		strReturnValue = str[i];
		if (strReturnValue >= L"A" && strReturnValue <= L"Z")
		{
			cVal += (i == 0 ? (str[i] - 55) << 4 : (str[i] - 55));
		}
		else if (strReturnValue >= L"0" && strReturnValue <= L"9")
		{
			cVal += (i == 0 ? (str[i] - 48) << 4 : (str[i] - 48));
		}
	}
	return cVal;
}



const BOOL CDataBaseConfig::SetAuthorizeControl(LPCTSTR lpszName, CONFIG_NODE wData)
{
	CString strQuery;
	strQuery.Format(_T("update [User] set Grant_Backup =  %d, Grant_Backup_Manage = %d, Grant_Restore = %d, Grant_Compare = %d, Grant_Configuration = %d, Grant_Backup_Schedule = %d, Grant_Reset = %d, Grant_IdAssignment = %d, Grant_ID_Reset = %d where Name = '%s'"), 
						wData.backup, wData.backupManage, wData.restore, wData.compare, wData.configuration, wData.backupSchedule, wData.reset, wData.idAssign, wData.idReset, lpszName);
	
	return ExecuteQuery(strQuery);
}


AUTHORIZE_CONTROL_INFO CDataBaseConfig::GetAuthorizeState()
{
	return authorize;
}
