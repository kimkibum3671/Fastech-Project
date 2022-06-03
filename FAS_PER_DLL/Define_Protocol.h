#pragma once

#include "Windows.h"

//------------------------------------------------------------------
//                 Command Protocol Defines.
//------------------------------------------------------------------

static const BYTE	FRAME_GETSLAVEINFO						= (0x01);
static const BYTE	FRAME_GETENCODER						= (0x06);
static const BYTE	FRAME_SETIDVALUE						= (0x08);

// Step Commands
static const BYTE	FRAME_STEPGETCURRENT					= (0x0B);		// Factory, Distributor only
static const BYTE	FRAME_STEPSETCURRENT					= (0x0C);		// Factory, Distributor only
static const BYTE	FRAME_STEPVERSION						= (0x0D);		// Factory, Distributor only
static const BYTE	FRAME_STEPGETMOTORDB					= (0x0E);		// Factory, Distributor only
static const BYTE	FRAME_STEPSETMOTORDB					= (0x0F);		// Factory, Distributor only

// Parameters Commands
static const BYTE	FRAME_FAS_SAVEALLPARAM					= (0x10);
static const BYTE	FRAME_FAS_GETROMPARAM					= (0x11);
static const BYTE	FRAME_FAS_SETPARAMETER					= (0x12);
static const BYTE	FRAME_FAS_GETPARAMETER					= (0x13);
static const BYTE	FRAME_FAS_RESETPARAM					= (0x16);

// ROM Writing Commands
static const BYTE	FRAME_ISROMERASED						= (0x1A);
static const BYTE	FRAME_ISRUNROMEXIST						= (0x1B);
static const BYTE	FRAME_ERASEFLASH						= (0x1C);
static const BYTE	FRAME_WRITEFLASH						= (0x1D);
static const BYTE	FRAME_REBOOTBOOTROM						= (0x1E);
static const BYTE	FRAME_REBOOTRUNROM						= (0x1F);

// I/O Commands
static const BYTE	FRAME_FAS_SETIO_OUTPUT					= (0x20);
static const BYTE	FRAME_FAS_SETIO_INPUT					= (0x21);
static const BYTE	FRAME_FAS_GETIO_INPUT					= (0x22);
static const BYTE	FRAME_FAS_GETIO_OUTPUT					= (0x23);

static const BYTE	FRAME_FAS_SET_INPUT_ASSGN_MAP			= (0x24);
static const BYTE	FRAME_FAS_SET_OUTPUT_ASSGN_MAP			= (0x25);
static const BYTE	FRAME_FAS_IO_ASSGN_MAP_READROM			= (0x26);
static const BYTE	FRAME_FAS_GET_INPUT_ASSGN_MAP			= (0x27);
static const BYTE	FRAME_FAS_GET_OUTPUT_ASSGN_MAP			= (0x28);

static const BYTE	FRAME_FAS_SERVOENABLE					= (0x2A);
static const BYTE	FRAME_FAS_ALARMRESET					= (0x2B);
static const BYTE	FRAME_FAS_STEPALARMRESET				= (0x2C);	// Step Alarm Reset function.

static const BYTE	FRAME_FAS_BRAKESET						= (0x2D);
static const BYTE	FRAME_FAS_GETALARMTYPE					= (0x2E);	// Alarm Type

// Motion Commands
static const BYTE	FRAME_FAS_MOVESTOP						= (0x31);
static const BYTE	FRAME_FAS_EMERGENCYSTOP					= (0x32);

static const BYTE	FRAME_FAS_MOVEORIGIN					= (0x33);
static const BYTE	FRAME_FAS_MOVESINGLEABS					= (0x34);
static const BYTE	FRAME_FAS_MOVESINGLEINC					= (0x35);
static const BYTE	FRAME_FAS_MOVETOLIMIT					= (0x36);
static const BYTE	FRAME_FAS_MOVEVELOCITY					= (0x37);

static const BYTE	FRAME_FAS_POSABSOVERRIDE				= (0x38);
static const BYTE	FRAME_FAS_POSINCOVERRIDE				= (0x39);
static const BYTE	FRAME_FAS_VELOVERRIDE					= (0x3A);

static const BYTE	FRAME_FAS_ALLMOVESTOP					= (0x3B);
static const BYTE	FRAME_FAS_ALLEMERGENCYSTOP				= (0x3C);
static const BYTE	FRAME_FAS_ALLMOVEORIGIN					= (0x3D);
static const BYTE	FRAME_FAS_ALLMOVESINGLEABS				= (0x3E);
static const BYTE	FRAME_FAS_ALLMOVESINGLEINC				= (0x3F);

// Motion Status Commands
static const BYTE	FRAME_FAS_GETAXISSTATUS					= (0x40);
static const BYTE	FRAME_FAS_GETIOSTATUS					= (0x41);
static const BYTE	FRAME_FAS_GETMOTIONSTATUS				= (0x42);
static const BYTE	FRAME_FAS_GETALLSTATUS					= (0x43);

static const BYTE	FRAME_FAS_GETRUNPTSTATUS				= (0x44);

static const BYTE	FRAME_FAS_SETCMDPOS						= (0x50);
static const BYTE	FRAME_FAS_GETCMDPOS						= (0x51);
static const BYTE	FRAME_FAS_SETACTPOS						= (0x52);
static const BYTE	FRAME_FAS_GETACTPOS						= (0x53);
static const BYTE	FRAME_FAS_GETPOSERR						= (0x54);
static const BYTE	FRAME_FAS_GETACTVEL						= (0x55);
static const BYTE	FRAME_FAS_CLEARPOS						= (0x56);

static const BYTE	FRAME_FAS_MOVEPAUSE						= (0x58);

// Position Table specific commands.
static const BYTE	FRAME_FAS_POSTAB_READ_ITEM				= (0x60);
static const BYTE	FRAME_FAS_POSTAB_WRITE_ITEM				= (0x61);
static const BYTE	FRAME_FAS_POSTAB_READ_ROM				= (0x62);
static const BYTE	FRAME_FAS_POSTAB_WRITE_ROM				= (0x63);
static const BYTE	FRAME_FAS_POSTAB_RUN_ITEM				= (0x64);
static const BYTE	FRAME_FAS_POSTAB_IS_DATA				= (0x65);

static const BYTE	FRAME_FAS_POSTAB_RUN_ONEITEM			= (0x68);
static const BYTE	FRAME_FAS_POSTAB_CHECK_STOPMODE			= (0x69);

// Hidden Parameter commands.
static const BYTE	FRAME_FAS_GET_HIDDEN_PARAM				= (0x66);
static const BYTE	FRAME_FAS_SET_HIDDEN_PARAM				= (0x67);

// Position Table specific commands. (2nd)
static const BYTE	FRAME_FAS_POSTAB_READ_ONEITEM			= (0x6A);
static const BYTE	FRAME_FAS_POSTAB_WRITE_ONEITEM			= (0x6B);

// Linear Motion commands.
static const BYTE	FRAME_FAS_SETLINEARINFO					= (0x70);
static const BYTE	FRAME_FAS_MOVELINEARINC					= (0x71);

// Ex-Motion Commands
static const BYTE	FRAME_FAS_MOVESINGLEABS_EX				= (0x80);
static const BYTE	FRAME_FAS_MOVESINGLEINC_EX				= (0x81);
static const BYTE	FRAME_FAS_MOVEVELOCITY_EX				= (0x82);

// Calibration Commands.
static const BYTE	FRAME_FAS_STARTCALIBRATION				= (0x90);
static const BYTE	FRAME_FAS_GETCALIBRATIONRESULT			= (0x91);
static const BYTE	FRAME_FAS_SETCALIBRATIONDATA			= (0x92);

// BLDC Drive Commands.
static const BYTE	FRAME_FAS_BLDC_READ_PARAMETER			= (0x84);
static const BYTE	FRAME_FAS_BLDC_WRITE_PARAMETER			= (0x85);
static const BYTE	FRAME_FAS_BLDC_READ_ROM					= (0x86);
static const BYTE	FRAME_FAS_BLDC_WRITE_ROM				= (0x87);

// BLDC Tuning Commands.
static const BYTE	FRAME_MONITORING_CHECKFINISH			= (0x8A);
static const BYTE	FRAME_MONITORING_CANCEL					= (0x8B);
static const BYTE	FRAME_MONITORING_READDATA				= (0x8C);
static const BYTE	FRAME_MONITORING_SETCONFIG				= (0x8D);
static const BYTE	FRAME_MONITORING_GETCONFIG				= (0x8E);
static const BYTE	FRAME_MONITORING_START					= (0x8F);

// Driver Functions
static const BYTE	FRAME_DRIVER_GETVERSION					= (0xB0);
static const BYTE	FRAME_DRIVER_GETPARAM					= (0xB1);
static const BYTE	FRAME_DRIVER_SETPARAM					= (0xB2);
static const BYTE	FRAME_DRIVER_FUNCTION					= (0xB3);

//	Operation Mode
static const BYTE	FRAME_MOTIONGATE_SETMODE				= (0xC0);
static const BYTE	FRAME_MOTIONGATE_GETMODE				= (0xC1);
static const BYTE	FRAME_MOTIONGATE_RESCAN					= (0xC4);
static const BYTE	FRAME_MOTIONGATE_GETINFO				= (0xC7);
static const BYTE	FRAME_MOTIONGATE_GETAXISINFO			= (0xC8);
static const BYTE	FRAME_MOTIONGATE_REBOOT					= (0xC9);
static const BYTE	FRAME_MOTIONGATE_ALARMRESET				= (0xCA);
static const BYTE	FRAME_MOTIONGATE_ALARMCHECK				= (0xCB);
static const BYTE	FRAME_MOTIONGATE_GET_SYSTEMSTATUS		= (0xCD);
