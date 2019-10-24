
#ifndef __RT_OBJECT_API_H__
#define __RT_OBJECT_API_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include "rt_p2p_log.h"
#include "rt_p2p_types.h"

typedef struct _RT_APP_NETWORK_Info_s{	
	RT_APP_CHAR bFlagInternet;		// Internet Reachable? 1: YES, 0: NO
	RT_APP_CHAR bFlagHostResolved;	// P2P Server IP resolved? 1: YES, 0: NO
	RT_APP_CHAR bFlagServerHello;	// P2P Server Hello? 1: YES, 0: NO
	RT_APP_CHAR NAT_Type;			// NAT type, 0: Unknow, 1: IP-Restricted Cone type,   2: Port-Restricted Cone type, 3: Symmetric 
	RT_APP_CHAR MyLanIP[16];		// My LAN IP. If (bFlagInternet==0) || (bFlagHostResolved==0) || (bFlagServerHello==0), MyLanIP will be "0.0.0.0"
	RT_APP_CHAR MyWanIP[16];		// My Wan IP. If (bFlagInternet==0) || (bFlagHostResolved==0) || (bFlagServerHello==0), MyWanIP will be "0.0.0.0"
}RT_APP_NETWORK_Info;

typedef struct _RT_APP_Params_s 
{
	RT_APP_INT32 (*setMsgReturnCallBack)(const RT_APP_CHAR *uid, RT_APP_INT32 msg, RT_APP_CHAR *pbuf, RT_APP_INT32 len);
	RT_APP_INT32 (*getMsgDataCallBack)(const RT_APP_CHAR *uid, RT_APP_INT32 msg, RT_APP_CHAR *pbuf, RT_APP_INT32 len);
	RT_APP_INT32 (*connectStatusCallBack)(const RT_APP_CHAR *uid, RT_APP_INT32 msgType, RT_APP_INT32 msgValue);
	RT_APP_INT32 (*searchDataCallBack)(RT_APP_CHAR *pbuf);
	RT_APP_INT32 (*h264VideoPlayerCallBack)(const RT_APP_CHAR *uid, RT_APP_UCHAR *pbuf, RT_APP_INT32 frametype, RT_APP_UINT32 timestamp, RT_APP_INT32 len, RT_APP_VOID *pOutUserData);
	RT_APP_INT32 (*h264PlayBackCallBack)(const RT_APP_CHAR *uid, RT_APP_UCHAR *pbuf, RT_APP_INT32 frametype, RT_APP_UINT32 timestamp, RT_APP_INT32 len);
	RT_APP_INT32 (*otherVideoPlayerCallBack)(const RT_APP_CHAR *uid, RT_APP_UCHAR *pbuf, RT_APP_INT32 frametype, RT_APP_INT32 len);
	RT_APP_INT32 (*audioPlayerCallBack)(const RT_APP_CHAR *uid, RT_APP_UCHAR *pbuf, RT_APP_INT32 len);
	RT_APP_INT32 (*alarmCallBack)(const RT_APP_CHAR *uid, RT_APP_INT32 alarmType, RT_APP_VOID *pData);
	RT_APP_INT32 (*loadPicCallBack)(const RT_APP_CHAR *uid, const RT_APP_CHAR *pFileName, RT_APP_UCHAR *pbuf, RT_APP_INT32 len, RT_APP_UCHAR endFlag);
	RT_APP_INT32 (*loadVidCallBack)(const RT_APP_CHAR *uid, const RT_APP_CHAR *pFileName, RT_APP_UCHAR *pbuf, RT_APP_INT32 len, RT_APP_UCHAR endFlag);
	RT_APP_VOID  (*logCallBack)(RT_APP_INT32 logLevel,const RT_APP_CHAR *format);
}RT_APP_Params;


RT_APP_INT32 RT_APP_API_Initial(RT_APP_Params *pAppParams);

RT_APP_INT32 RT_APP_API_DeInitial();

RT_APP_INT32 RT_APP_API_NETWORK_Detect(RT_APP_NETWORK_Info *pNetInfo);

RT_APP_INT32 RT_APP_API_Start(RT_APP_UPTR* pu32AppHandle, RT_APP_CHAR *szDID, RT_APP_CHAR *szUser, RT_APP_CHAR *szPwd, RT_APP_VOID *pUserData); //pUserData 由用户输入,从实时码流回调进行返回);

RT_APP_INT32 RT_APP_API_Stop(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *szDID);

RT_APP_INT32 RT_APP_API_SEARCH_DEV_Start();

RT_APP_INT32 RT_APP_API_SEARCH_DEV_Stop();

RT_APP_INT32 RT_APP_API_START_LIVE_Stream(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *szDID, RT_APP_INT32 streamid);

RT_APP_INT32 RT_APP_API_STOP_LIVE_Stream(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *szDID);

RT_APP_INT32 RT_APP_API_SWITCH_Stream(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *szDID, RT_APP_INT32 streamid);

RT_APP_INT32 RT_APP_API_START_Audio(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *szDID, RT_APP_UCHAR audioFormat);

RT_APP_INT32 RT_APP_API_STOP_Audio(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *szDID);

RT_APP_INT32 RT_APP_API_START_Talk(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *szDID);

RT_APP_INT32 RT_APP_API_STOP_Talk(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *szDID);

RT_APP_INT32 RT_APP_API_TALK_AUDIO_Data(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *szDID, RT_APP_CHAR *audioPcmData, RT_APP_INT32 len);

RT_APP_INT32 RT_APP_API_START_PlayBack(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *szDID, RT_APP_CHAR *fileName, RT_APP_INT32 offset);

RT_APP_INT32 RT_APP_API_STOP_PlayBack(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *szDID);

RT_APP_INT32 RT_APP_API_GET_SYSTEM_Params(RT_APP_UPTR u32AppHandle, RT_APP_CHAR * szDID, RT_APP_INT32 type, RT_APP_VOID *pData, RT_APP_UINT32 len);

RT_APP_INT32 RT_APP_API_SET_SYSTEM_Params(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *szDID, RT_APP_INT32 type, RT_APP_VOID *pData, RT_APP_UINT32 len);

RT_APP_INT32 RT_APP_API_START_DOWNLOAD_File(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *szDID, RT_APP_VOID *pData, RT_APP_INT32 len);

RT_APP_INT32 RT_APP_API_STOP_DOWNLOAD_File(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *szDID, RT_APP_CHAR *pFileName);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif //__RT_OBJECT_API_H__

