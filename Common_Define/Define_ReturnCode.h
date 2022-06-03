#pragma once

#ifndef FMM_RETURN_CODE
#define FMM_RETURN_CODE

/**
* --------------------------------------------------------------------------------
*                         Motor Return Code Defines.
* --------------------------------------------------------------------------------
*/	

typedef enum _FMM_ERROR
{
	FMM_OK = 0,

	FMM_NOT_OPEN,
	FMM_INVALID_DEVICE,
	FMM_INVALID_PORT_NUM,
	FMM_INVALID_SLAVE_NUM,

	FMC_DISCONNECTED = 5,
	FMC_TIMEOUT_ERROR,				//	1ȸ �ۼ��� �������� ������ ���� ���
	FMC_CRCFAILED_ERROR,
	FMC_RECVPACKET_ERROR,			// PACKET SIZE ERROR

	FMM_POSTABLE_ERROR,

	FMP_FRAMETYPEERROR = 0x80,
	FMP_DATAERROR,
	FMP_PACKETERROR,
	FMP_RUNFAIL = 0x85,
	FMP_RESETFAIL,
	FMP_SERVOONFAIL1,
	FMP_SERVOONFAIL2,
	FMP_SERVOONFAIL3,
	FMP_SERVOOFF_FAIL,
	FMP_ROMACCESS,

	FMP_PACKETCRCERROR = 0xAA,

	FMM_MOTION_GATE_BUSY = 0xE2,

	FMM_UNKNOWN_ERROR = 0xFF,


	FMC_UNKNOWN_ERROR = 0x1000,
	FMM_INVALID_PORT,
	FMC_NO_RESPONSE,				//	TCP ������ �̷�������� ����̽��κ��� ������ ���� ��
	FMC_REFUSED,					//	���濡�� ������ �ź��� ��
	FMC_SOCKET_ERROR,				//	Socket�� �����ϴ� �������� Error�� �߻�
	FMC_NOT_COMPLPETE,				//	FMC_NOT_COMPLPETE
	FMC_INSUFFICIENT_BUFFER,		//	���� ���۰� �����ϴ�
	FMC_INSUFFICIENT_BUFFER2,		//	������ ���� �뷮�� �����ϴ�
//	FMC_INVALID_DATASIZE,			//	��û�� �����Ϳ� ũ�Ⱑ �ٸ���
	FMC_DATA_ERROR,					//	�������� Data�� �ƴ�
	FMC_DATA_COMPARE_ERROR,			//	���� �����Ͱ� ������ �ƴ�.

}	FMM_ERROR;
#endif
