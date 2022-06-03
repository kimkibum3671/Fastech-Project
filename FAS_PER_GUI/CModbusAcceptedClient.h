#pragma once
#pragma pack(1)

#include <winsock2.h>
#include "..\Common_Define\Define_Structure.h"

class CModbusAcceptedClient
{
public:
	CModbusAcceptedClient(	const INT		nPortNo,
							SOCKET			socket,
							SOCKADDR_IN&	address );
	
	~CModbusAcceptedClient();

protected:
	INT			m_nPortNo;	// Port number of server
	SOCKET		m_socket;
	SOCKADDR_IN	m_address;
	CWinThread*	m_pThread;
	BOOL		m_bRun;

protected:
	CHAR* m_revBuffer[1024];
	INT m_nRevSize;

	CHAR* m_sndBuffer[1024];
	INT m_nSndSize;

	//	юс╫ц
	WORD	m_wData[255];

protected:
	static UINT ThreadClient(LPVOID pParam);

	typedef struct _MBAP_HEADER
	{
		WORD	wTransactionID;
		WORD	wProtocolID;
		WORD	wDataSize;
		BYTE	btStationID;
		BYTE	btCommand;
	}	MBAP_HEADER, *PMBAP_HEADER;

	typedef struct _MODBUS_PROTOCOL
	{
		MBAP_HEADER		header;
		CHAR			data;
	}	MODBUS_PROTOCOL, *PMODBUS_PROTOCOL;

	typedef struct _MODBUS_PROTOCOL_REQ_READ_REGISTER
	{
		WORD	wAddress;
		WORD	wLength;
	}	MODBUS_PROTOCOL_REQ_READ_REGISTER, *PMODBUS_PROTOCOL_REQ_READ_REGISTER,
		MODBUS_PROTOCOL_RES_WRITE_MULTIPLE_REGISTER, *PMODBUS_PROTOCOL_RES_WRITE_MULTIPLE_REGISTER;

	typedef struct _MODBUS_PROTOCOL_RES_READ_REGISTER
	{
		BYTE	btCount;
		WORD	wData;
	}	MODBUS_PROTOCOL_RES_READ_REGISTER, * PMODBUS_PROTOCOL_RES_READ_REGISTER;

	typedef struct _MODBUS_PROTOCOL_REQ_WRITE_REGISTER
	{
		WORD	wAddress;
		WORD	wData;
	}	MODBUS_PROTOCOL_REQ_WRITE_REGISTER, *PMODBUS_PROTOCOL_REQ_WRITE_REGISTER,
		MODBUS_PROTOCOL_RES_WRITE_SINGLE_REGISTER, *PMODBUS_PROTOCOL_RES_WRITE_SINGLE_REGISTER;

	typedef struct _MODBUS_PROTOCOL_WRITE_MULTIPLE_REGISTER
	{
		WORD wAddress;
		WORD wLength;
		BYTE btCount;
		WORD wData;
	}	MODBUS_PROTOCOL_WRITE_MULTIPLE_REGISTER, * PMODBUS_PROTOCOL_WRITE_MULTIPLE_REGISTER;

public:
	const INT GetPortNo();
	SOCKET GetSocket();
	const BOOL IsRun();
	
	void ChangeEndian(WORD* pwData);

	void BuildRequest(LPCVOID lpBuffer, const INT cbBuffer);
	void BuildResponse(LPCVOID lpBuffer, const INT cbBuffer);

	void Close();

public:
	virtual const INT OnReceive( LPCVOID lpBuffer, const INT cbBuffer );
	virtual void OnClose();
	

public:
	typedef struct _REQUIRE_WIRTE_DATA
	{
		WORD wEENo;
		BYTE byPTNo;
		BYTE byMotorNo;
	} REQUIRE_WIRTE_DATA;

	REQUIRE_WIRTE_DATA m_ReqData;

	typedef struct _RESPONSE_DATA
	{
		WORD wEENo;
		BYTE byPTNo;
		BYTE byMotorNo;
		BYTE byMotionErr;
		BYTE byMotorErr;
		LONG lPos;
		WORD wSpd;
	}	RESPONSE_DATA;

	RESPONSE_DATA m_ResData;

	enum _RESPONSE_STRUCT
	{
		REQ0,
		REQ1,
		EE_NO,
		PT_MOTOR_NO,
		ERR_CODE,
		POS_LO,
		POS_HI,
		SPEED,
		MAX
	};
	
	const BOOL RequireWriteCmd(WORD wAddress, WORD wValue, WORD wStandardAddr);
	const WORD ResponseCmd(WORD wAddress);
	
	::ITEM_NODE m_node;
	WORD MergByte(BYTE byte1, BYTE byte2);
	const BOOL ReadyResponseData();

	BOOL m_bResMotorSetting;
};
