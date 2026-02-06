/*
** Minimal J2534 VBATT Shim - DLL Loader
**
** Loads the real PCANPT32.dll from the same directory as this shim DLL
** and resolves all 14 J2534 v04.04 function pointers.
*/

#pragma once

#include <windows.h>
#include "j2534_v0404.h"

extern HMODULE g_hModule;

bool shim_loadLibrary();
void shim_unloadLibrary();
bool shim_isLoaded();

extern PTOPEN _PassThruOpen;
extern PTCLOSE _PassThruClose;
extern PTCONNECT _PassThruConnect;
extern PTDISCONNECT _PassThruDisconnect;
extern PTREADMSGS _PassThruReadMsgs;
extern PTWRITEMSGS _PassThruWriteMsgs;
extern PTSTARTPERIODICMSG _PassThruStartPeriodicMsg;
extern PTSTOPPERIODICMSG _PassThruStopPeriodicMsg;
extern PTSTARTMSGFILTER _PassThruStartMsgFilter;
extern PTSTOPMSGFILTER _PassThruStopMsgFilter;
extern PTSETPROGRAMMINGVOLTAGE _PassThruSetProgrammingVoltage;
extern PTREADVERSION _PassThruReadVersion;
extern PTGETLASTERROR _PassThruGetLastError;
extern PTIOCTL _PassThruIoctl;
