#pragma once

// Pos. Table Item structure.
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