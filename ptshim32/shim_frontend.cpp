/*
** Minimal J2534 VBATT Shim
**
** Pure passthrough for all 14 J2534 v04.04 API functions, except:
**   - PassThruIoctl: intercepts READ_VBATT (0x03) and returns a
**     spoofed 13.5V (13500 mV) regardless of ChannelID.
**
** The real PCANPT32.dll is auto-loaded from the same directory
** as this shim DLL on the first API call.
*/

#include <windows.h>
#include "j2534_v0404.h"
#include "shim_loader.h"
#include "shim_frontend.h"

// Auto-load the real DLL on first call; return ERR_FAILED if it can't be loaded
#define ENSURE_LOADED() \
    do { if (!shim_loadLibrary()) return ERR_FAILED; } while(0)

extern "C" long J2534_API PassThruOpen(void *pName, unsigned long *pDeviceID)
{
    ENSURE_LOADED();
    if (!_PassThruOpen) return ERR_FAILED;
    return _PassThruOpen(pName, pDeviceID);
}

extern "C" long J2534_API PassThruClose(unsigned long DeviceID)
{
    ENSURE_LOADED();
    if (!_PassThruClose) return ERR_FAILED;
    return _PassThruClose(DeviceID);
}

extern "C" long J2534_API PassThruConnect(unsigned long DeviceID, unsigned long ProtocolID,
    unsigned long Flags, unsigned long Baudrate, unsigned long *pChannelID)
{
    ENSURE_LOADED();
    if (!_PassThruConnect) return ERR_FAILED;
    return _PassThruConnect(DeviceID, ProtocolID, Flags, Baudrate, pChannelID);
}

extern "C" long J2534_API PassThruDisconnect(unsigned long ChannelID)
{
    ENSURE_LOADED();
    if (!_PassThruDisconnect) return ERR_FAILED;
    return _PassThruDisconnect(ChannelID);
}

extern "C" long J2534_API PassThruReadMsgs(unsigned long ChannelID, PASSTHRU_MSG *pMsg,
    unsigned long *pNumMsgs, unsigned long Timeout)
{
    ENSURE_LOADED();
    if (!_PassThruReadMsgs) return ERR_FAILED;
    return _PassThruReadMsgs(ChannelID, pMsg, pNumMsgs, Timeout);
}

extern "C" long J2534_API PassThruWriteMsgs(unsigned long ChannelID, PASSTHRU_MSG *pMsg,
    unsigned long *pNumMsgs, unsigned long Timeout)
{
    ENSURE_LOADED();
    if (!_PassThruWriteMsgs) return ERR_FAILED;
    return _PassThruWriteMsgs(ChannelID, pMsg, pNumMsgs, Timeout);
}

extern "C" long J2534_API PassThruStartPeriodicMsg(unsigned long ChannelID, PASSTHRU_MSG *pMsg,
    unsigned long *pMsgID, unsigned long TimeInterval)
{
    ENSURE_LOADED();
    if (!_PassThruStartPeriodicMsg) return ERR_FAILED;
    return _PassThruStartPeriodicMsg(ChannelID, pMsg, pMsgID, TimeInterval);
}

extern "C" long J2534_API PassThruStopPeriodicMsg(unsigned long ChannelID, unsigned long MsgID)
{
    ENSURE_LOADED();
    if (!_PassThruStopPeriodicMsg) return ERR_FAILED;
    return _PassThruStopPeriodicMsg(ChannelID, MsgID);
}

extern "C" long J2534_API PassThruStartMsgFilter(unsigned long ChannelID, unsigned long FilterType,
    PASSTHRU_MSG *pMaskMsg, PASSTHRU_MSG *pPatternMsg,
    PASSTHRU_MSG *pFlowControlMsg, unsigned long *pFilterID)
{
    ENSURE_LOADED();
    if (!_PassThruStartMsgFilter) return ERR_FAILED;
    return _PassThruStartMsgFilter(ChannelID, FilterType, pMaskMsg, pPatternMsg, pFlowControlMsg, pFilterID);
}

extern "C" long J2534_API PassThruStopMsgFilter(unsigned long ChannelID, unsigned long FilterID)
{
    ENSURE_LOADED();
    if (!_PassThruStopMsgFilter) return ERR_FAILED;
    return _PassThruStopMsgFilter(ChannelID, FilterID);
}

extern "C" long J2534_API PassThruSetProgrammingVoltage(unsigned long DeviceID,
    unsigned long PinNumber, unsigned long Voltage)
{
    ENSURE_LOADED();
    if (!_PassThruSetProgrammingVoltage) return ERR_FAILED;
    return _PassThruSetProgrammingVoltage(DeviceID, PinNumber, Voltage);
}

extern "C" long J2534_API PassThruReadVersion(unsigned long DeviceID,
    char *pFirmwareVersion, char *pDllVersion, char *pApiVersion)
{
    ENSURE_LOADED();
    if (!_PassThruReadVersion) return ERR_FAILED;
    return _PassThruReadVersion(DeviceID, pFirmwareVersion, pDllVersion, pApiVersion);
}

extern "C" long J2534_API PassThruGetLastError(char *pErrorDescription)
{
    ENSURE_LOADED();
    if (!_PassThruGetLastError) return ERR_FAILED;
    return _PassThruGetLastError(pErrorDescription);
}

extern "C" long J2534_API PassThruIoctl(unsigned long ChannelID, unsigned long IoctlID,
    void *pInput, void *pOutput)
{
    // Intercept READ_VBATT: spoof 13.5V regardless of ChannelID
    // This works around the Subaru dealer tool passing DeviceID (0x51) as ChannelID
    if (IoctlID == READ_VBATT)
    {
        if (pOutput != NULL)
            *(unsigned long*)pOutput = 13500;
        return STATUS_NOERROR;
    }

    // Everything else passes through to the real PCAN DLL
    ENSURE_LOADED();
    if (!_PassThruIoctl) return ERR_FAILED;
    return _PassThruIoctl(ChannelID, IoctlID, pInput, pOutput);
}
