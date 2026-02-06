/*
** Minimal J2534 VBATT Shim - DLL Loader
**
** Loads the real PCANPT32.dll from the same directory as this shim DLL
** and resolves all 14 J2534 v04.04 function pointers.
*/

#include <windows.h>
#include "j2534_v0404.h"
#include "shim_loader.h"

// Name of the real PCAN J2534 DLL to load from the same directory as this shim
#define REAL_DLL_NAME "PCANPT32.dll"

static HINSTANCE hDLL = NULL;
static bool fLibLoaded = false;

PTOPEN _PassThruOpen = NULL;
PTCLOSE _PassThruClose = NULL;
PTCONNECT _PassThruConnect = NULL;
PTDISCONNECT _PassThruDisconnect = NULL;
PTREADMSGS _PassThruReadMsgs = NULL;
PTWRITEMSGS _PassThruWriteMsgs = NULL;
PTSTARTPERIODICMSG _PassThruStartPeriodicMsg = NULL;
PTSTOPPERIODICMSG _PassThruStopPeriodicMsg = NULL;
PTSTARTMSGFILTER _PassThruStartMsgFilter = NULL;
PTSTOPMSGFILTER _PassThruStopMsgFilter = NULL;
PTSETPROGRAMMINGVOLTAGE _PassThruSetProgrammingVoltage = NULL;
PTREADVERSION _PassThruReadVersion = NULL;
PTGETLASTERROR _PassThruGetLastError = NULL;
PTIOCTL _PassThruIoctl = NULL;

bool shim_loadLibrary()
{
    if (fLibLoaded)
        return true;

    // Build path: same directory as this DLL + REAL_DLL_NAME
    char szPath[MAX_PATH];
    GetModuleFileNameA(g_hModule, szPath, MAX_PATH);

    char *lastSlash = strrchr(szPath, '\\');
    if (lastSlash != NULL)
        *(lastSlash + 1) = '\0';
    else
        szPath[0] = '\0';

    strcat_s(szPath, MAX_PATH, REAL_DLL_NAME);

    hDLL = LoadLibraryA(szPath);
    if (hDLL == NULL)
        return false;

    _PassThruOpen = (PTOPEN)GetProcAddress(hDLL, "PassThruOpen");
    _PassThruClose = (PTCLOSE)GetProcAddress(hDLL, "PassThruClose");
    _PassThruConnect = (PTCONNECT)GetProcAddress(hDLL, "PassThruConnect");
    _PassThruDisconnect = (PTDISCONNECT)GetProcAddress(hDLL, "PassThruDisconnect");
    _PassThruReadMsgs = (PTREADMSGS)GetProcAddress(hDLL, "PassThruReadMsgs");
    _PassThruWriteMsgs = (PTWRITEMSGS)GetProcAddress(hDLL, "PassThruWriteMsgs");
    _PassThruStartPeriodicMsg = (PTSTARTPERIODICMSG)GetProcAddress(hDLL, "PassThruStartPeriodicMsg");
    _PassThruStopPeriodicMsg = (PTSTOPPERIODICMSG)GetProcAddress(hDLL, "PassThruStopPeriodicMsg");
    _PassThruStartMsgFilter = (PTSTARTMSGFILTER)GetProcAddress(hDLL, "PassThruStartMsgFilter");
    _PassThruStopMsgFilter = (PTSTOPMSGFILTER)GetProcAddress(hDLL, "PassThruStopMsgFilter");
    _PassThruSetProgrammingVoltage = (PTSETPROGRAMMINGVOLTAGE)GetProcAddress(hDLL, "PassThruSetProgrammingVoltage");
    _PassThruReadVersion = (PTREADVERSION)GetProcAddress(hDLL, "PassThruReadVersion");
    _PassThruGetLastError = (PTGETLASTERROR)GetProcAddress(hDLL, "PassThruGetLastError");
    _PassThruIoctl = (PTIOCTL)GetProcAddress(hDLL, "PassThruIoctl");

    fLibLoaded = true;
    return true;
}

void shim_unloadLibrary()
{
    if (!fLibLoaded)
        return;

    fLibLoaded = false;

    _PassThruOpen = NULL;
    _PassThruClose = NULL;
    _PassThruConnect = NULL;
    _PassThruDisconnect = NULL;
    _PassThruReadMsgs = NULL;
    _PassThruWriteMsgs = NULL;
    _PassThruStartPeriodicMsg = NULL;
    _PassThruStopPeriodicMsg = NULL;
    _PassThruStartMsgFilter = NULL;
    _PassThruStopMsgFilter = NULL;
    _PassThruSetProgrammingVoltage = NULL;
    _PassThruReadVersion = NULL;
    _PassThruGetLastError = NULL;
    _PassThruIoctl = NULL;

    if (hDLL != NULL)
    {
        FreeLibrary(hDLL);
        hDLL = NULL;
    }
}

bool shim_isLoaded()
{
    return fLibLoaded;
}