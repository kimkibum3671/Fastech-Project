#pragma once

#pragma pack(1)

//------------------------------------------------------------------
//                 Drive Information Defines.
//------------------------------------------------------------------
typedef struct _DRIVE_INFO
{
	unsigned short nVersionNo[4];	// Drive Version Number (Major Ver/Minor Ver/Bug fix/Build No.) (?)
	char sVersion[30];				// Drive Version string

	unsigned short nDriveType;		// Drive Model
	unsigned short nMotorType;		// Motor Model
	char sMotorInfo[20];			// Motor Info.(?)

	unsigned short nInPinNo;		// Input Pin Number
	unsigned short nOutPinNo;		// Output Pin Number

	unsigned short nPTNum;			// Position Table Item Number

	unsigned short nFirmwareType;	// Firmware Type Information
} DRIVE_INFO;

//------------------------------------------------------------------
//                 Motiongate Information Defines.
//------------------------------------------------------------------
typedef struct _MOTIONGATE_READ_INFO
{
	CHAR chMotiongateInfomation[40];
	CHAR chMotiongateName[20];
	BYTE btMotorCount;
	BYTE btHomingTimeout;	// Unit : Second
	BYTE btReserved[2];
}	MOTIONGATE_READ_INFO;

typedef struct _MOTIONGATE_WRITE_INFO
{
	BYTE btSubCommand;
	CHAR chMotiongateName[20];
	BYTE btMotorCount;
	BYTE btHomingTimeout;	// Unit : Second
	BYTE btReserved[2];
}	MOTIONGATE_WRITE_INFO;

//------------------------------------------------------------------
//                 Position Table Item structure.
//------------------------------------------------------------------
typedef union _ITEM_NODE
{
	WORD	wBuffer[32];		// 64 bytes

	struct
	{
		LONG	lPosition;

		DWORD	dwStartSpd;
		DWORD	dwMoveSpd;

		WORD	wAccelRate;
		WORD	wDecelRate;

		WORD	wCommand;
		WORD	wWaitTime;
		WORD	wContinuous;
		WORD	wBranch;

		WORD	wCond_branch0;
		WORD	wCond_branch1;
		WORD	wCond_branch2;
		WORD	wLoopCount;
		WORD	wBranchAfterLoop;
		WORD	wPTSet;
		WORD	wLoopCountCLR;

		WORD	bCheckInpos;		// 0 : Check Inpos, 1 : Don't Check.
	};
} ITEM_NODE, *LPITEM_NODE;

typedef union
{
	char	chValue[8];
	BYTE	byValue[8];
	long	lValue[2];
	DWORD	dwValue[2];
	ULONGLONG uValue;
} UNION_8DATA;

typedef union
{
	char	chValue[4];
	BYTE	byValue[4];
	long	lValue;
	DWORD	dwValue;
} UNION_4DATA;

typedef union
{
	char	chValue[2];
	BYTE	byValue[2];
	short	shValue;
	WORD	wValue;
} UNION_2DATA;
