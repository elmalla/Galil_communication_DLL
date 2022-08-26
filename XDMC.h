
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the XDMC_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// XDMC_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

//#define STRICT

#include <windows.h>
//#define WINAPI __stdcall
#define DllExport __declspec(dllexport)

//extern "C" DllExport BSTR Serial_Get_COM_Ports(void);
//extern "C" DllExport LONG Serial_Get_Error(void *objptr);
extern "C" DllExport void * WINAPI XDMC_Init(void * objptr,HWND hWnd,USHORT usController,BSTR bstrDMCFile = NULL ,BSTR bstrCommand = NULL);
extern "C" DllExport BSTR   WINAPI XDMC_SendCmd (void * objptr, BSTR bstrCommand);
extern "C" DllExport long   WINAPI XDMC_GetAllMotorsBusy(void * objptr, int iNoOfAxis );
extern "C" DllExport long   WINAPI XDMC_GetMotorBusy(void * objptr, int iAxisNo );//, BSTR bstrVarName = NULL);
extern "C" DllExport long   WINAPI XDMC_MotorGetPos(void * objptr, int iAxisNo );
extern "C" DllExport long   WINAPI XDMC_GetMotorCycleCount(void * objptr);
extern "C" DllExport long   WINAPI XDMC_SetMotorCycleCount(void * objptr,long lCount);
extern "C" DllExport long   WINAPI XDMC_MotorInit(void * objptr, int iAxisNo );
extern "C" DllExport long   WINAPI XDMC_MotorEnable(void * objptr, int iAxisNo, bool EnableDisable, bool bAllAxis = FALSE);
extern "C" DllExport long   WINAPI XDMC_MotorSetSpd(void * objptr, int iAxisNo, long lMotorSpeed);
extern "C" DllExport long   WINAPI XDMC_MotorSetAC(void * objptr, int iAxisNo, long lMotorAcc);
extern "C" DllExport long   WINAPI XDMC_MotorSetDC(void * objptr, int iAxisNo, long lMotorDec);
extern "C" DllExport long   WINAPI XDMC_MotorSetStartPos(void * objptr, int iAxisNo,double lMotorPos);
extern "C" DllExport long   WINAPI XDMC_MotorSetStartPosArray(void * objptr, int iAxisNo,double *lMotorPosArray, long lArraySize, long iRecipeCount);
extern "C" DllExport long   WINAPI XDMC_MotorSetStandbyPos(void * objptr, int iAxisNo, long lStandbyPos);
extern "C" DllExport long   WINAPI XDMC_MotorSetStandbyMotion(void * objptr, int iAxisNo, long lStandbyAcc, long lStandbyDec, long lStandbySpd);
extern "C" DllExport long   WINAPI XDMC_MotorSetStartMotion(void * objptr, int iAxisNo,long lStartAcc, long lStartDec, long lStartSpd,bool bFlagCut = FALSE);
extern "C" DllExport long   WINAPI XDMC_STOP(void * objptr, int iAxisNo, bool bAllAxis = FALSE);
extern "C" DllExport long   WINAPI XDMC_MoveStandby(void * objptr, int iAxisNo, int iDmcThread = 0);
extern "C" DllExport long   WINAPI XDMC_MoveSTARDBY(void * objptr);
extern "C" DllExport long   WINAPI XDMC_GetModLive(void * objptr);
extern "C" DllExport long   WINAPI XDMC_SetModLive(void * objptr, bool OnOff);
extern "C" DllExport long   WINAPI XDMC_DownloadSystemFile(void * objptr, BSTR bstrDMCFile);
extern "C" DllExport long   WINAPI XDMC_ModReset(void * objptr);
extern "C" DllExport long   WINAPI XDMC_Close(void * objptr);


extern "C" DllExport long WINAPI XDMC_JOG(void * objptr,long iAxisNo, double dSpeed);
extern "C" DllExport long WINAPI XDMC_GetMotionStatus(void * objptr, int iAxisNo);

//extern "C" DllExport BSTR   WINAPI XDMC_TroubleShoot(void * objptr, int iTraceFlag , long lLogData );//, BSTR bstrVarName = NULL);

//*****************************************************************************************************
//---------------------------------DSP 620 Application specific Functions-------------------------------*
//                                             Galil IO is used
//*****************************************************************************************************


extern "C" DllExport long WINAPI XDSP_GetAppFlags(void * objptr,long lFlagNo );
extern "C" DllExport long WINAPI XDSP_GetInputFlag(void * objptr,long lFlagNo);
//extern "C" DllExport long WINAPI XDSP_GetMotionStatus(void * objptr, int iAxisNo );

extern "C" DllExport long WINAPI XDSP_StartIndexer(void * objptr, int iAxisNo,bool bFlagCut = FALSE, long iDmcThread =0);

extern "C" DllExport long WINAPI XDSP_SetAppFlag(void * objptr,long lFlagNo );
extern "C" DllExport long WINAPI XDSP_SetOutput(void * objptr,long lModuleNo, long lONOFF );

extern "C" DllExport long WINAPI XDSP_SetPunchArray(void * objptr,long *lPunchRecipe, int lRecipeCount);

extern "C" DllExport long WINAPI XDSP_SetCutterCount(void * objptr,long lCutCount);

extern "C" DllExport long WINAPI XDSP_ManualMove(void * objptr, int iAxisNo, long iDmcThread);




extern "C" DllExport long   WINAPI XNWC_Start(void * objptr, int iFnRef, long lDmcThread=0);
extern "C" DllExport double WINAPI XNWC_GetData(void * objptr, int lDataRef );
extern "C" DllExport long   WINAPI XNWC_SetData(void * objptr,int iDataRef, double dValue );
extern "C" DllExport long   WINAPI XNWC_GetAppFlags(void * objptr,int iFlagNo );
extern "C" DllExport long   WINAPI XNWC_SetAppFlag(void * objptr,int iFlagNo, long iValue );
extern "C" DllExport long   WINAPI XNWC_SetMotion(void * objptr,long lRef,long lAcc, long lDec,long lSpd);

extern "C" DllExport double WINAPI XNWC_TroubleShoot (void * objptr,long lDataRef);
extern "C" DllExport long   WINAPI XNWC_GetArray (void * objptr,char *buffer,short start,short length,long lDataRef);
extern "C" DllExport long   WINAPI XNWC_SetArray(void * objptr,double *buffer, long lArraySize,long lDataRef);
extern "C" DllExport long   WINAPI XNWC_SetOutput(void * objptr,long lModuleNo, long lONOFF );



//extern "C" DllExport long   WINAPI XDMC_GetDMCHandle(void * objptr);
/*extern "C" DllExport void Serial_Close(void * objptr);
extern "C" DllExport int Serial_Initialize_Setting(void * objptr,int nBaudrateIndex,int nDataIndex, int nParityIndex,int nStopIndex, DWORD nHandshakingIndex);
extern "C" DllExport int  Serial_Send(void * objptr,BSTR sendData);
extern "C" DllExport BSTR Serial_Read(void * objptr, int Terminator);
extern "C" DllExport LONG Serial_GetError(void *objptr);
extern "C" DllExport BSTR Serial_Send_n_Recieve(void * objptr,BSTR sendData,int Terminator);

/*
XMotorInit
XHome
XEStop
XStop
XMotorReset
XMotorEnable
XMotorSetPos
XMotorGetPos
XMotorSetStartPos
XMotorGetStartPos
XMotorSetStandbyPos
XMotorGetStandbyPos
XMotorSetOffsetPos
XMotorGetOffsetPos
XMotorSetEmbAC
XMotorGetEmbAC
XMotorSetEmbDC
XMotorGetEmbDC
XMotorSetEmbSpd
XMotorGetEmbSpd
XMotorSetSpd
XMotorGetSpd
XMotorSetAC
XMotorGetAC
XMotorSetDC
XMotorGetDC
XMotorSetHMAC
XMotorGetHMAC
XMotorSetHMDC
XMotorGetHMDC
XMotorSetHMSpd
XMotorGetHMSpd
XMotorSetUSPow
XMotorGetUSPow
XMotorSetEmbZ
XMotorGetEmbZ
XMotorMoveAbs
XMotorMoveJogP
XMotorMoveJogN
XMotorMoveRel
XGetMotorBusy
XGetModReady
XMoveStandby
XMoveAllStandby
XMoveStartPos
XMoveAllStartPos
XModInit*/