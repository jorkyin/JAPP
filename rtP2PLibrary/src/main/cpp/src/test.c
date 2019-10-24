
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>



#include "rt_p2p_log.h"
#include "rt_p2p_types.h"
#include "rt_object_api.h"
#include "rt_p2p_api_define.h"
#include "../include/rtlib/rt_p2p_types.h"

static RT_APP_INT32 g_stop = 0;
static RT_APP_INT32 g_onLine = 0;
static RT_APP_INT32 g_sendTalk = 0;
static RT_APP_CHAR  g_willDownLoadFile[128];


#define RT_CALENDAR_BIT_CHECK(a,b) ((a) & (1<<(b)))

RT_APP_INT32 urlEncode(RT_APP_CHAR *pInEncode,RT_APP_CHAR *pOutEncode,RT_APP_INT32 size)
{
	if(pInEncode == NULL || pOutEncode == NULL)
	{
		return -1;
	}
	
	char szTemp[512] = {0};		
	char *pSrc = pInEncode;
	
	int i = 0;;
	int nCount = (int)strlen(pInEncode);
	for (i = 0; i < nCount; i++)
	{
		if ((pSrc[i] >= 'a' && pSrc[i] <= 'z')
			|| (pSrc[i] >= 'A' && pSrc[i] <= 'Z')
			|| (pSrc[i] >= '0' && pSrc[i] <= '9'))
		{
			if (strlen(szTemp) == 0)
			{
				strncpy(szTemp, (const char*)&pSrc[i], 1);
			}
			else
			{
				strncat(szTemp, (const char*)&pSrc[i], 1);
			}
		}
		else
		{
			char szTmp[32] = {0} ;
			sprintf(szTmp, "%%%x", pSrc[i]);
			strcat(szTemp, szTmp);			  
		}
	}
	
	strncpy(pOutEncode,szTemp,size);	
}

/* 内网搜索设备UID返回回调*/
RT_APP_INT32 rtSearchDataCallBack(RT_APP_CHAR *pbuf)
{
	STRU_SEARCH_PARAMS *pSearchDev = (STRU_SEARCH_PARAMS *)pbuf;
	printf("uid=%s,ip=%s\n",pSearchDev->szDevUID,pSearchDev->szIpAddr);
	return 0;
}


RT_APP_INT32 rtConnectCallBack(const RT_APP_CHAR* uid,RT_APP_INT32 msgType, RT_APP_INT32 msgValue)
{
	/************************************************************
	    //连接超时 --> 重连一定的次数后，如果还不成功，则停止p2p
	    ERROR_PPPP_TIME_OUT 

	    //ID号无效 --> 停止p2p
	    ERROR_PPPP_INVALID_ID
	    ERROR_PPPP_INVALID_PREFIX

	    //设备不在线 -->停止p2p
	    ERROR_PPPP_DEVICE_NOT_ONLINE

	    //连接失败 -->停止p2p
	    ERROR_PPPP_NOT_INITIALIZED
	    ERROR_PPPP_NO_RELAY_SERVER_AVAILABLE
	    ERROR_PPPP_MAX_SESSION
	    ERROR_PPPP_UDP_PORT_BIND_FAILED
	    ERROR_PPPP_USER_CONNECT_BREAK
    *************************************************************/
	if(msgType == MSG_NOTIFY_TYPE_PPPP_STATUS)
	{
		switch(msgValue)
		{
			case PPPP_STATUS_CONNECTING: /* connecting */
				printf( "\033[0;32;31m uid[%s] connecting \033[0m\n",uid);
				break;
			case PPPP_STATUS_INITIALING: /* initialing */
				printf("\033[32;49;1m uid[%s] initialing \033[39;49;0m\n",uid);
				break;
			case PPPP_STATUS_ON_LINE: /* on line */
				printf("\033[32;49;1m uid[%s] on line \033[39;49;0m\n",uid);
				g_onLine = 1;
				break;
			case PPPP_STATUS_CONNECT_FAILED: /* connect failed */
				printf("\033[32;49;1m uid[%s] connecting failed \033[39;49;0m\n",uid);
				break;
			case PPPP_STATUS_DISCONNECT: /*connect is off*/
				printf("\033[32;49;1m uid[%s] disconnect \033[39;49;0m\n",uid);
				break;
			case PPPP_STATUS_INVALID_ID: /* invalid id */
				printf("\033[32;49;1m uid[%s] invalid uid \033[39;49;0m\n",uid);
				break;
			case PPPP_STATUS_DEVICE_NOT_ON_LINE:
				printf("\033[32;49;1m uid[%s] no on line \033[39;49;0m\n",uid);
				break;
			case PPPP_STATUS_CONNECT_TIMEOUT: /* connect timeout */
				printf("\033[32;49;1m uid[%s] connecting time out \033[39;49;0m\n",uid);
				break;
			case PPPP_STATUS_INVALID_USER_PWD: /* user or pwd is invalid */
				printf("\033[32;49;1m uid[%s] pwd error \033[39;49;0m\n",uid);
				break;
			case PPPP_STATUS_MAX_CONNECT_CNT: /* 超过设备最大连接数 */
				printf("\033[32;49;1m uid[%s] More than the max number of connections \033[39;49;0m\n",uid);
				break;
			default:
				printf( "\033[0;32;31m uid[%s] unknown\033[0m\n",uid);
				return;
		}
	}

	if(msgType == MSG_NOTIFY_TYPE_PPPP_MODE)
	{
		switch(msgValue)
		{
			case PPPP_MODE_P2P:
				break;
			case PPPP_MODE_RELAY:
				break;
		}
	}

	if(msgType == MSG_NOTIFY_TYPE_TALK_STATUS) /*当msgValue的值为PPPP_STATUS_SEND_TALK_DATA时表示可以发送对讲数据*/
	{
		switch(msgValue)
		{
			case PPPP_STATUS_SEND_TALK_DATA:
				printf("you can send talk data\n");
				g_sendTalk = 1;
				break;
			default:
				break;
		}
	}

	if(msgType == MSG_NOTIFY_TYPE_STREAM_TYPE)
	{
		printf("video stream\n");
	}
	
	return 0;
}

/* 设置回调返回值 */
/**pbuf中的值为1时表示用户设置到设备的值是成功,0表示设置失败 */
RT_APP_INT32 rtSetMsgReturnCallBack(const RT_APP_CHAR* uid,RT_APP_INT32 msg, RT_APP_CHAR *pbuf, RT_APP_INT32 len)
{
	switch(msg)
	{
		case MSG_TYPE_DECODER_CONTROL:
			printf("[PTZ] ret=%d\n",(RT_APP_INT32)(*pbuf));
			break;
		case MSG_TYPE_SET_WIFI:
			printf("[SET WIFI] ret=%d\n",(RT_APP_INT32)(*pbuf));
			break;
		case MSG_TYPE_SET_NETWORK:
			printf("[SET NETWORK] ret=%d\n",(RT_APP_INT32)(*pbuf));
			break;
		case MSG_TYPE_SET_USER:
			printf("[SET USER] ret=%d\n",(RT_APP_INT32)(*pbuf));
			break;
		case MSG_TYPE_SET_DATETIME:
			printf("[SET TIME] ret=%d\n",(RT_APP_INT32)(*pbuf));
			break;
		case MSG_TYPE_SET_MAIL:
			printf("[SET EMAIL] ret=%d\n",(RT_APP_INT32)(*pbuf));
			break;
		case MSG_TYPE_SET_FTP:
			printf("[SET FTP] ret=%d\n",(RT_APP_INT32)(*pbuf));
			break;
		case MSG_TYPE_SET_PTZ:
			printf("[SET PTZ] ret=%d\n",(RT_APP_INT32)(*pbuf));
			break;
		case MSG_TYPE_SET_RECORD_SCH:
			printf("[SET SD RECOED] ret=%d\n",(RT_APP_INT32)(*pbuf));
			break;
		case MSG_TYPE_SET_ALARM:
			printf("[SET ALARM] ret=%d\n",(RT_APP_INT32)(*pbuf));
			break;
		case MSG_TYPE_FORMAT_SD:
			printf("[FORMAT SD] ret=%d\n",(RT_APP_INT32)(*pbuf));
			break;
		case MSG_TYPE_UNMOUNT_SD:
			printf("[UNMOUNT SD] ret=%d\n",(RT_APP_INT32)(*pbuf));
			break;
		default:
			break;
	}
	return 0;
}
RT_APP_ULONG g_u32AppHandle = 0;
RT_APP_CHAR g_uid[32] = {0};
/* 数据获取回调 */
RT_APP_INT32 rtGetMsgDataCallBack(const RT_APP_CHAR *uid,RT_APP_INT32 msg, RT_APP_CHAR *pbuf, RT_APP_INT32 len)
{
	switch(msg)
	{
		case MSG_TYPE_GET_P2P_CAPABILITY: //获取p2p设备的功能集
		{
			/*解析json数据时先解析isshowall,如果为1表示app所有功能都显示,如果为0则解析后面的具体参数*/
			
			/*
	         显示部分功能  1:显示 0:不显示  ,
	        {"isshowall":0,"basefunc":{"realTimeVideo":1,"voiceSpeak":1,"voiceListen":1,"remotePlayback":1,
	        "ptzControl":1,"parameter":1},"baseparam":{"networkParam":1,"ftpParam":1,"emailParam":1,"userParam":1,
	        "alarmParam":1,"timeParam":1,"tfParam":1,"wifiParam":1},"videoAttr":{"colorAttr":1,"powerAttr":0,
	        "zoomAttr":0,"focusAttr":0,"streamAttr":1,"flipMirror":1},"cxtfunc":{"reboot":0,"factory":0,"format":1}}
			备注:
				如果parameter的值为0,则baseparam中的所有值不管是0还是1,都将视为0
	        */

			/*
			 显示所有功能
			 {"isshowall":1}
			*/
			
			RT_APP_CHAR *buf = NULL;
			buf = (RT_APP_CHAR *)malloc(len);
			memset(buf,0,sizeof(len));
			strcpy(buf,pbuf);
			printf("[GET P2P CAPABILITY] value=%s\n",buf);
			free(buf);
			buf = NULL;
		}
			break;
		case MSG_TYPE_GET_STATUS:
		{
			PSTRU_CAMERA_STATUS pCameraStatus = NULL;
			pCameraStatus = (PSTRU_CAMERA_STATUS)pbuf;
			if(pCameraStatus == NULL)
			{
				return -1;
			}
			printf("[GET STATUS] devid=%s,devname=%s,mac=%s,wifimac=%s,onlineNum=%d\n",pCameraStatus->devid,pCameraStatus->devname,pCameraStatus->mac,pCameraStatus->wifimac,pCameraStatus->onlineNum);
		}
			break;
		case MSG_TYPE_GET_CAMERA_PARAMS:
		{
			PSTRU_CAMERA_PARAMS pCameraParams = NULL;
    		pCameraParams = (PSTRU_CAMERA_PARAMS)(pbuf);
			printf("[GET CAMERA PARAM]contrast=%d,brightness=%d,flip=%d,mainFram=%d,subFram=%d\n",pCameraParams->contrast,pCameraParams->brightness,pCameraParams->flip,pCameraParams->mainFram,pCameraParams->subFram);
		}
			break;
		case MSG_TYPE_WIFI_SCAN:
		{
			if(pbuf == NULL)
			{
				return -1;
			}
			STRU_WIFI_SEARCH_RESULT_LIST wifiList;
			memset(&wifiList,0,sizeof(STRU_WIFI_SEARCH_RESULT_LIST));
			memcpy(&wifiList,(STRU_WIFI_SEARCH_RESULT_LIST *)pbuf,sizeof(STRU_WIFI_SEARCH_RESULT_LIST));
			RT_APP_INT32 count = wifiList.nResultCount;
			RT_APP_INT32 i = 0;
			for(i = 0; i < count; i++)
			{
				printf("[WIFI SCAN] ssid[%d]=%s,security=%d\n",i,wifiList.wifi[i].ssid,wifiList.wifi[i].security);
			}
		}
			break;
		case MSG_TYPE_GET_NEW_RECORD_FILE: //新协议和老协议的录像文件数据获取来自一个结构体
		case MSG_TYPE_GET_RECORD_FILE:
		{
			PSTRU_SDCARD_RECORD_FILE pRecordFile = NULL;
			pRecordFile = (PSTRU_SDCARD_RECORD_FILE)pbuf;
			if(pRecordFile == NULL)
			{
				return -1;
			}
			printf("[RECORD] filetime[%02d:%02d:%02d]-",pRecordFile->fileDayStartTime/3600,(pRecordFile->fileDayStartTime/60)%60,pRecordFile->fileDayStartTime%60);
			printf("[%02d:%02d:%02d]\n",pRecordFile->fileDayEndTime/3600,(pRecordFile->fileDayEndTime/60)%60,pRecordFile->fileDayEndTime%60);
		}
			break;
		case MSG_TYPE_GET_NETWORK:
		{
			PSTRU_NETWORK_PARAMS pNetworkParams = NULL;
			pNetworkParams = (PSTRU_NETWORK_PARAMS)pbuf;
			printf("[NETWORK] ip=%s,netmask=%s\n",pNetworkParams->ipaddr,pNetworkParams->netmask);
		}
			break;
		case MSG_TYPE_GET_USER_INFO:
		{
			PSTRU_USER_INFO pUserInfo = NULL;
			pUserInfo = (PSTRU_USER_INFO)pbuf;
			printf("[USER] user1=%s,pwd1=%s\n",pUserInfo->user1,pUserInfo->pwd1);
		}
			break;
		case MSG_TYPE_GET_WIFI:
		{
			PSTRU_WIFI_PARAMS pWifiParams = NULL;
			pWifiParams = (PSTRU_WIFI_PARAMS)pbuf;
			printf("[WIFI] ssid=%s\n",pWifiParams->ssid);
		}
			break;
		case MSG_TYPE_GET_FTP:
		{
			PSTRU_FTP_PARAMS PFtpParams = NULL;
			PFtpParams = (PSTRU_FTP_PARAMS)pbuf;
			printf("[FTP] serverFtp=%s,user=%s,pwd=%s\n",PFtpParams->svr_ftp,PFtpParams->user,PFtpParams->pwd);
		}
			break;
		case MSG_TYPE_GET_EMAIL:
		{
			PSTRU_MAIL_PARAMS pMailParams = NULL;
			pMailParams = (PSTRU_MAIL_PARAMS)pbuf;
			printf("[EMAIL] server=%s,user=%s\n",pMailParams->svr,pMailParams->user);
		}
			break;
		case MSG_TYPE_GET_DDNS: //协议暂未实现
		{
			PSTRU_DDNS_PARAMS pDdnsParams = NULL;
			pDdnsParams = (PSTRU_DDNS_PARAMS)pbuf;
			if(pDdnsParams == NULL)
			{
				return -1;
			}
			printf("[DDNS] host=%s\n",pDdnsParams->host);
		}
			break;
		case MSG_TYPE_GET_TIME:
		{
			PSTRU_DATETIME_PARAMS pDatetimeParams = NULL;
			pDatetimeParams = (PSTRU_DATETIME_PARAMS)pbuf;	
			printf("[TIME] ntpServer=%s,dstMode=%s\n",pDatetimeParams->ntp_svr,pDatetimeParams->dstMode);
		}
			break;
		case MSG_TYPE_GET_ALARM_PARAM:
		{
			PSTRU_ALARM_PARAMS pAlarmParams = NULL;
			pAlarmParams = (PSTRU_ALARM_PARAMS)pbuf;
			printf("[ALARM] motionArmed=%d,mail=%d\n",pAlarmParams->motion_armed,pAlarmParams->mail);
		}
			break;
		case MSG_TYPE_GET_PTZ_PARAMS:
		{
			PSTRU_PTZ_PARAMS pPtzParams = NULL;
			pPtzParams = (PSTRU_PTZ_PARAMS)pbuf;
			printf("[PTZ] downRate=%d\n",pPtzParams->ptz_patrol_down_rate);
		}
			break;
		case MSG_TYPE_GET_RECORD:
		{
			PSTRU_SD_RECORD_PARAM pRecordParam = NULL;
			pRecordParam = (PSTRU_SD_RECORD_PARAM)pbuf;
            printf("[TF INFO] enable=%d,sdtotal=%d,sdfree=%d\n",pRecordParam->record_time_enable,pRecordParam->sdtotal,pRecordParam->sdfree);
		}
			break;
		case MSG_TYPE_GET_DOWNLOAD_FILE: //下载文件
		{
			PSTRU_MEDIA_DATA pstuDownLoadData = NULL;
			pstuDownLoadData = (PSTRU_MEDIA_DATA)pbuf;

			/*备注:返回下载文件在TF中的状态,详情见:STRU_MEDIA_DATA_STATE*/
			printf("[RETURN DOWNLOAD FILE ATTR] fileName=%s,fileState=%d,fileSize=%u\n",pstuDownLoadData->szFileName,pstuDownLoadData->dataState,pstuDownLoadData->fileSize);
		}
			break;
		case MSG_TYPE_GET_SNAP_UPLOAD_FTP: //获取ftp抓拍的路径
		{
			PSTRU_SNAP_UPLOAD_FTP pSnapUploadFtp = NULL;
			pSnapUploadFtp = (PSTRU_SNAP_UPLOAD_FTP)pbuf;

			printf("[SNAP UPLOAD FTP] ftpPath=%s\n",pSnapUploadFtp->ftpPath);
		}
			break;
		case MSG_TYPE_GET_REC_MODE: // 获取录像模式
		{
			PSTRU_REC_MODE pGetRecMode = NULL;
			pGetRecMode = (PSTRU_REC_MODE)pbuf;
			printf("[GET REC MODE] timeRecEnable=%d,motionRecEnable=%d\n",pGetRecMode->timeRecEnable,pGetRecMode->motionRecEnable);
		}
			break;
		case MSG_TYPE_GET_REC_CALENDAR: //获取该月中那些天有录像文件
		{
			PSTRU_REC_CALENDAR pGetRecCalendar = NULL;
			pGetRecCalendar = (PSTRU_REC_CALENDAR)pbuf;
			printf("The month of the day for on video [%d][",pGetRecCalendar->count);
			int pos = 0;
			for(pos=0;pos < 32;pos++)
			{
				int val = RT_CALENDAR_BIT_CHECK(pGetRecCalendar->calendar,pos);
				if(val > 0)
				{
					printf(" %d",pos+1); /*提示:pos后面得加1因为下标是从0开始算的*/
				}	
			}
			printf("]\n");
		}
			break;
		case MSG_TYPE_GET_ALARM_EX:
		{
			PSTRU_ALARM_PARAMS_EX pGetAlarmEx = NULL;
			pGetAlarmEx = (PSTRU_ALARM_PARAMS_EX)pbuf;

			printf("mdSensitive=%d\n",pGetAlarmEx->mdSensitive);
			printf("mdInterval=%d\n",pGetAlarmEx->mdInterval);
			printf("mdEmailSnap=%d\n",pGetAlarmEx->mdEmailSnap);
			printf("mdFtpSnap=%d\n",pGetAlarmEx->mdFtpSnap);
			printf("mdFtpRec=%d\n",pGetAlarmEx->mdFtpRec);
			printf("ioEnable=%d\n",pGetAlarmEx->ioEnable);
			printf("ioInterval=%d\n",pGetAlarmEx->ioInterval);
			printf("ioEmailSnap=%d\n",pGetAlarmEx->ioEmailSnap);
			printf("ioFtpSnap=%d\n",pGetAlarmEx->ioFtpSnap);
			printf("ioFtpRec=%d\n",pGetAlarmEx->ioFtpRec);
		}
			break;
		case MSG_TYPE_GET_SD_REC_EX:
		{
			PSTRU_SD_RECORD_EX pGetSdRecEx = NULL;
			pGetSdRecEx = (PSTRU_SD_RECORD_EX)pbuf;
			printf("sdRecMode=%d,fullRecTime=%d,alarmRecTime=%d,timelapseRecEnable=%d,timelapseRecTime=%d\n",
				pGetSdRecEx->sdRecMode,pGetSdRecEx->fullRecTime,pGetSdRecEx->alarmRecTime,pGetSdRecEx->timelapseRecEnable,pGetSdRecEx->timelapseRecTime);

			printf("timePlanEnable=%d,timePlanStartTime=%d,timePlanEndTime=%d,loopRecEnable=%d,resolutionRec=%d,audioRecEnable=%d\n",
				pGetSdRecEx->timePlanEnable,pGetSdRecEx->timePlanStartTime,pGetSdRecEx->timePlanEndTime,pGetSdRecEx->loopRecEnable,pGetSdRecEx->resolutionRec,pGetSdRecEx->audioRecEnable);
			
		}
			break;
		case MSG_TYPE_GET_OSD:
		{
			PSTRU_OSD_ATTRS pGetOsd = NULL;
			pGetOsd = (PSTRU_OSD_ATTRS)pbuf;
			printf("osdEnable=%d,osdName=%s\n",pGetOsd->osdEnable,pGetOsd->osdName);
		}
			break;
		case MSG_TYPE_GET_IRCUT:
		{
			PSTRU_IRCUT_ATTR pGetIrcut = NULL;
			pGetIrcut = (PSTRU_IRCUT_ATTR)pbuf;
			printf("ircutDelayTime=%d\n",pGetIrcut->ircutDelayTime);
		}
			break;
		default:
			break;
	
	}
	return 0;
}

/* 仅供内部测试使用 */
#define TEST_GET_P2P_CAPABILITY  0 /*获取p2p的能力集,动态显示APP界面*/
#define TEST_GET_PARAM           1 /*获取时钟参数/网络参数/wifi参数/用户信息/email参数/告警参数/ftp参数/DDNS*/
#define TEST_GET_NETWORK         2
#define TEST_GET_USER            3
#define TEST_GET_WIFI            4
#define TEST_GET_FTP             5
#define TEST_GET_EMAIL           6
#define TEST_GET_DDNS            7
#define TEST_GET_DATE            8
#define TEST_GET_ALARM           9
#define TEST_GET_STATUS          10
#define TEST_GET_CAMARE_PARAM    11
#define TEST_PTZ_CONTROL         12
#define TEST_GET_NEW_RECORD_LIST 13 /*新协议搜索,根据开始和结束时间进行录像文件搜索*/
#define TEST_GET_RECORD_LIST     14 /*旧协议搜索,录像最多搜索出450个文件*/
#define TEST_CAMERA_CONTROL      15
#define TEST_REBOOT_DEVICLLE     16
#define TEST_SET_WIFI            17
#define TEST_SET_NETWORK         18
#define TEST_SET_USER_INFO       19
#define TEST_SET_TIME            20
#define TEST_SET_DDNS            21
#define TEST_SET_EMAIL           22
#define TEST_SET_FTP             23
#define TEST_SET_PTZ             24
#define TEST_SET_TF_RECORD       25
#define TEST_SET_ALARM           26
#define TEST_FORMAT_TF           27
#define TEST_WIFI_SCAN           28  
#define TEST_GET_TF_Info         29
#define TEST_GET_PTZ             30   
#define TEST_SET_SNAP_UPLOAD_FTP 31 /*抓拍上传FTP*/
#define TEST_SET_REC_MODE        32 
#define TEST_GET_REC_MODE        33
#define TEST_SET_MOBILE_TIME     34
#define TEST_GET_REC_CALENDAR    35
#define TEST_UNMOUNT_SD          36
#define TEST_GET_ALARM_EX        37
#define TEST_SET_ALARM_EX        38
#define TEST_GET_SD_REC_EX       39
#define TEST_SET_SD_REC_EX       40
#define TEST_FACTORY_RESET       41
#define TEST_GET_OSD             42
#define TEST_SET_OSD             43
#define TEST_SET_GEOG            44 /*人图天气设备使用*/
#define TEST_SET_IRCUT           45 /*设置IRCUT属性*/
#define TEST_GET_IRCUT           46 /*获取IRCUT属性*/






RT_APP_VOID testParam(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *pUid, RT_APP_INT32 paramVal)
{
	switch(paramVal)
	{
		case TEST_GET_P2P_CAPABILITY: /*获取p2p能力集*/
		{
			printf("[TEST GET P2P CAPABILITY]\n");
			RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_P2P_CAPABILITY, NULL, 0);
		}
			break;
		case TEST_GET_PARAM: /*获取时钟参数/网络参数/wifi参数/用户信息/email参数/告警参数/ftp参数/DDNS */
		{
			printf("[TEST GET PARAMS]\n");
			RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_PARAMS, NULL, 0);
		}
			break;
		case TEST_GET_NETWORK: // 获取网络属性
		{
			printf("[TEST GET NETWORK]\n");
			RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_NETWORK, NULL, 0);
		}
			break;
		case TEST_GET_USER: // 获取用户名和密码
		{
			printf("[TEST GET USER INFO]\n");
			RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_USER_INFO, NULL, 0);
		}
			break;
		case TEST_GET_WIFI: // 获取设置连接的wifi名称
		{
			printf("[TEST GET WIFI]\n");
			RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_WIFI, NULL, 0);
		}
			break;
		case TEST_GET_FTP: // 获取FTP属性
		{
			printf("[TEST GET FTP]\n");
			RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_FTP, NULL, 0);
		}
			break;
		case TEST_GET_EMAIL: // 获取EMAIL
		{
			printf("[TEST GET EMAIL]\n");
			RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_EMAIL, NULL, 0);
		}
			break;
		case TEST_GET_DDNS: // 暂未实现
		{
			printf("[TEST GET DDNS]\n");
			RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_DDNS, NULL, 0);
		}
			break;
		case TEST_GET_DATE: // 获取设备时间
		{
			printf("[TEST GET DATE]\n");
			RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_TIME, NULL, 0);
		}
			break;
		case TEST_GET_TF_Info: // 获取TF属性
		{
			printf("[TEST GET TF INFO]\n");
			RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_RECORD, NULL, 0);
		}
			break;
		case TEST_GET_ALARM: // 获取移动侦测属性
		{
			printf("[TEST GET ALARM]\n");
			RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_ALARM_PARAM, NULL, 0);
		}
			break;
		case TEST_GET_PTZ: // 获取云台速度
		{
			printf("[TEST GET PTZ]\n");
			RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_PTZ_PARAMS, NULL, 0);	
		}
			break;
		case TEST_GET_STATUS: // 获取设备的一些状态属性
		{
			printf("[TEST GET STATUS]\n");
			RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_STATUS, NULL, 0);
		}
			break;
		case TEST_GET_CAMARE_PARAM: // 获取设备视频属性
		{	
			printf("[TEST GET CAMERA PARAM]\n");
			RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_CAMERA_PARAMS, NULL, 0);
		}
			break;
		case TEST_PTZ_CONTROL: //云台控制
		{
			printf("[TEST PTZ CONTROL]\n");
			
			STRU_PTZ_CONTROL setCmdValut;
			memset(&setCmdValut,0,sizeof(STRU_PTZ_CONTROL));

			/*用户如果想自定义输入要设置的预置点使用命令CMD_PTZ_PREFAB_BIT_SET_EXT  extParam为要输入的值*/
			/*用户如果想自定义输入要调用的预置点使用命令CMD_PTZ_PREFAB_BIT_RUN_EXT  extParam为要输入的值*/
			//setCmdValut.command = CMD_PTZ_PREFAB_BIT_RUN_EXT; 
			//setCmdValut.extParam = 200; /*自定义值*/

			setCmdValut.command = CMD_PTZ_LEFT; 
			setCmdValut.extParam = 1; /*0 非单步,1 单步*/
			
			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_DECODER_CONTROL, (RT_APP_VOID *)&setCmdValut, sizeof(STRU_PTZ_CONTROL));
			
			sleep(2);
			setCmdValut.command = CMD_PTZ_UP_DOWN_STOP;
			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_DECODER_CONTROL, (RT_APP_VOID *)&setCmdValut, sizeof(STRU_PTZ_CONTROL));
		}
			break;
		case TEST_GET_NEW_RECORD_LIST: /*新协议搜索,根据开始和结束时间进行录像文件搜索*/
		{
			printf("[TEST GET NEW RECORD LIST]\n");
			STRU_SEARCH_SDCARD_RECORD_FILE sdcardsearch;
		    memset(&sdcardsearch, 0, sizeof(sdcardsearch));
			RT_APP_INT32 i = 0;
			RT_APP_INT32 count = 0;
			while(1)
			{
				if(count >= 2)
				{
					i = 0;
					count = 0;
				}
				
				/* 搜索开始时间 */
			    strcpy(sdcardsearch.starttime,"20170818000000");
		
				/* 搜索结束时间 */
				strcpy(sdcardsearch.endtime,"20170818235959");
				
				sdcardsearch.page = ++i;   /* 翻页值 */
				sdcardsearch.fileType = 3; /* 文件类型 :1-常规录像,2-报警录像,3-所有录像 */
				sdcardsearch.pageSize = 100; /* 自定义页文件大小 */
				sdcardsearch.showType = 0; /*0:文件夹方式显示 1:时间轴方式显示(设备不会传文件名过来)*/
				printf("sdcardsearch.page=%d\n",sdcardsearch.page);
				RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_NEW_RECORD_FILE, (RT_APP_VOID *)&sdcardsearch, sizeof(STRU_SEARCH_SDCARD_RECORD_FILE));
				count++;
				sleep(8);
			}
		}
			break;
		case TEST_GET_RECORD_LIST: /*旧协议搜索,录像最多搜索出450个文件*/
		{
			printf("[TEST GET OLD RECORD LIST]\n");
			RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_RECORD_FILE, NULL, 0);
			sleep(10);
		}
			break;
		case TEST_CAMERA_CONTROL: /*设置亮度/设置对比度/设置POWER/设置镜像和翻转/视频帧率码率等*/
		{
			/*
			param	            value
			1：亮度	            1~255
			2：对比度	        1~255
			3：power模式	    0：50hz	1：60hz
			5：旋转	        0：原始   1：垂直翻转   2：水平镜像  3：垂直翻转 + 水平镜像
			*/
			printf("[TEST CAMERA CONTROL]\n");
			STRU_CAMERA_CONTROL cameraControl;
		    memset(&cameraControl, 0, sizeof(cameraControl));
		    cameraControl.param = 1; //EX:设置亮度
		    cameraControl.value = 255; //设备端的范围是0-100,所以如果要将设备的值设置为70应该自行转换，255对应设备的100
			
			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_CAMERA_CONTROL, (RT_APP_VOID *)&cameraControl, sizeof(STRU_CAMERA_CONTROL));
		}
			break;
		case TEST_REBOOT_DEVICLLE: // 重启设备
		{
			printf("[TEST REBOOT]\n");
			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_REBOOT_DEVICE, NULL, 0);
		}
			break;
		case TEST_SET_WIFI: // 设置设备wifi
		{
			printf("[TEST SET WIFI]\n");
			STRU_WIFI_PARAMS wifiParams;
			memset(&wifiParams,0,sizeof(wifiParams));
			strcpy(wifiParams.ssid, "TEST");
		    //strcpy(wifiParams.wpa_psk, "1234567890"); /*wpa2*/
			 strcpy(wifiParams.key1, "1234567890");     /*wep*/
		    wifiParams.authtype = 2; /*2:wpa 1:wep 0:none 备注如果是wep,密码使用key1存储*/
		    wifiParams.enable = 1;
			
		 	RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_SET_WIFI, (RT_APP_VOID *)&wifiParams, sizeof(STRU_WIFI_PARAMS));   
		}
			break;
		case TEST_SET_NETWORK: // 设置网络属性
		{
			printf("[TEST SET NETWORK]\n");
			STRU_NETWORK_PARAMS networkParams;
		    memset(&networkParams, 0, sizeof(networkParams));
		    strcpy(networkParams.ipaddr, "192.168.0.146");
		    strcpy(networkParams.netmask, "255.255.255.0");
		    strcpy(networkParams.gateway, "192.168.0.1");
		    networkParams.dhcp = 0; /*dhcp 1: 开启 0: 不开启*/
		    networkParams.port = 80;
			
		    RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_SET_NETWORK, (RT_APP_VOID *)&networkParams, sizeof(STRU_NETWORK_PARAMS));
		}
			
			break;
		case TEST_SET_USER_INFO: // 设置用户名和密码
		{
			printf("[TEST SET USER]\n");
			STRU_USER_INFO userInfo;
		    memset(&userInfo, 0, sizeof(userInfo));
		    strcpy(userInfo.user1, "TEST1");
		    strcpy(userInfo.pwd1, "TEST1");
		    strcpy(userInfo.user2, "TEST2");
		    strcpy(userInfo.pwd2, "TEST2");
		    strcpy(userInfo.user3, "TEST3");
		    strcpy(userInfo.pwd3, "TEST4");

			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_SET_USER, (RT_APP_VOID *)&userInfo, sizeof(STRU_USER_INFO));
			
		}
			break;
		case TEST_SET_TIME: // 设置time
		{
			printf("[TEST SET TIME]\n");
			tzset();
			STRU_DATETIME_PARAMS datetimeParams;
		    memset(&datetimeParams, 0, sizeof(datetimeParams));
		    datetimeParams.now = 0; // now大于0使用手机时间对时,now=0使用ntp对应,目前支持ntp对时
		    datetimeParams.ntp_enable = 1;
		    datetimeParams.tz = timezone+3600;
			datetimeParams.timeMode = 0;
			strcpy(datetimeParams.dstMode,"on");
		    strcpy(datetimeParams.ntp_svr, "time.nist.gov");

			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_SET_DATETIME, (RT_APP_VOID *)&datetimeParams, sizeof(STRU_DATETIME_PARAMS));
		}
			break;
		case TEST_SET_DDNS: //目前设备程序未实现此功能
		{
			printf("[TEST SET DDNS]\n");
			STRU_DDNS_PARAMS ddnsParams;
		    memset(&ddnsParams, 0, sizeof(ddnsParams));
		    strcpy(ddnsParams.user, "DDNS");
		    strcpy(ddnsParams.pwd, "DDNS");
		    strcpy(ddnsParams.host, "8.8.8.8");
		    strcpy(ddnsParams.proxy_svr, "www.baidu.com");
		    
			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_SET_DDNS, (RT_APP_VOID *)&ddnsParams, sizeof(STRU_DDNS_PARAMS));
		}
			break;
		case TEST_SET_EMAIL: // 设置EMAIL
		{
			printf("[TEST SET EMAIL]\n");
			STRU_MAIL_PARAMS mailParams;
		    memset(&mailParams, 0, sizeof(mailParams));
		    strcpy(mailParams.svr, "smtp.server.com");
		    strcpy(mailParams.user, "TEST");
		    strcpy(mailParams.pwd, "TEST");
		    strcpy(mailParams.sender, "SENDER");
		    strcpy(mailParams.receiver1, "RECEVICE");
		    mailParams.port = 25;
		    mailParams.ssl = 1; 

			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_SET_MAIL, (RT_APP_VOID *)&mailParams, sizeof(STRU_MAIL_PARAMS));
		}
			break;
		case TEST_SET_FTP: // 设置FTP属性
		{
			printf("[TEST SET FTP]\n");
			STRU_FTP_PARAMS ftpParams;
		    memset(&ftpParams, 0, sizeof(ftpParams));
		    strcpy(ftpParams.svr_ftp, "192.168.0.199");
		    strcpy(ftpParams.user, "USER");
		    strcpy(ftpParams.pwd, "PWD");
		    strcpy(ftpParams.dir, "/FTP");
		    ftpParams.port = 21;
		    ftpParams.mode = 0;
		    ftpParams.upload_interval = 0;

			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_SET_FTP, (RT_APP_VOID *)&ftpParams, sizeof(STRU_FTP_PARAMS));
		}
			break;
		case TEST_SET_PTZ: // 设置云台速度
		{
			printf("[TEST SET PTZ]\n");
			STRU_PTZ_PARAMS ptzParams;
		    memset(&ptzParams, 0, sizeof(ptzParams));
		    ptzParams.led_mode = 0;
		    ptzParams.ptz_patrol_down_rate = 10;
			
			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_SET_PTZ, (RT_APP_VOID *)&ptzParams, sizeof(STRU_PTZ_PARAMS));
		}
			break;
		case TEST_SET_TF_RECORD: // 设置录像(定时)
		{
			printf("[TEST SET TF RECORD]\n");
			STRU_SD_RECORD_PARAM recordParams;
		    memset(&recordParams, 0, sizeof(recordParams));
		    recordParams.record_cover_enable = 0;
		    recordParams.record_time_enable = 1;

			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_SET_RECORD_SCH, (RT_APP_VOID *)&recordParams, sizeof(STRU_SD_RECORD_PARAM));
		}
			break;
		case TEST_SET_ALARM: // 移动侦测报警设置
		{
			printf("[TEST SET ALARM]\n");
			STRU_ALARM_PARAMS alarmParams;
		    memset(&alarmParams, 0, sizeof(alarmParams));
		    alarmParams.motion_armed = 1 ;
		    alarmParams.motion_sensitivity = 1; /*1-10 */
			alarmParams.mail = 1;
			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_SET_ALARM, (RT_APP_VOID *)&alarmParams, sizeof(STRU_ALARM_PARAMS));
		}
			break;
		case TEST_FORMAT_TF: // 格式化TF
		{
			printf("[TEST FORMAT SD]\n");
			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_FORMAT_SD, NULL, 0);
		}
			break;
		case TEST_WIFI_SCAN: // wifi list 搜索
		{
			printf("[TEST WIFI SCAN]\n");
			RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_WIFI_SCAN, NULL, 0);
		}
			break;
		case TEST_SET_SNAP_UPLOAD_FTP: // 触发设置抓拍并上传FTP
		{
			printf("[TEST] SET SNAP UPLOAD FTP\n");
			STRU_SNAP_UPLOAD_FTP stuSnapUploadFtp;
			memset(&stuSnapUploadFtp,0,sizeof(stuSnapUploadFtp));
			stuSnapUploadFtp.snapStreamVal = ENUM_SNAP_STREAM_VGA;
			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_SNAP_UPLOAD_FTP, (RT_APP_VOID *)&stuSnapUploadFtp, sizeof(STRU_SNAP_UPLOAD_FTP));
		}
			break;
		case TEST_SET_REC_MODE: //设置录像模式(目前可以设置定时录像和移动侦测录像)
		{
			printf("[TEST] SET REC MODE\n");
			STRU_REC_MODE stuSetRecMode;
			memset(&stuSetRecMode,0,sizeof(stuSetRecMode));
			stuSetRecMode.timeRecEnable = 0;
			stuSetRecMode.motionRecEnable = 0; 
			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_SET_REC_MODE, (RT_APP_VOID *)&stuSetRecMode, sizeof(STRU_REC_MODE));
		}
			break;
		case TEST_GET_REC_MODE: //获取录像模式
		{
			printf("[TEST GET REC MODE]\n");
			RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_REC_MODE, NULL, 0);
		}
			break;
		case TEST_SET_MOBILE_TIME: //同步手机时间
		{
			printf("[TEST] SET MOBILE TIME\n");
			STRU_TIME_ATTR stuSetMobileTime;
			memset(&stuSetMobileTime,0,sizeof(stuSetMobileTime));
			stuSetMobileTime.year = 2017;
			stuSetMobileTime.month = 7;
			stuSetMobileTime.day = 18;
			stuSetMobileTime.hour = 14;
			stuSetMobileTime.min = 10;
			stuSetMobileTime.sec = 20;
			strcpy(stuSetMobileTime.timezone,"Atlantic/Cape_Verde");
			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_SET_MOBILE_TIME, (RT_APP_VOID *)&stuSetMobileTime, sizeof(STRU_TIME_ATTR));
		}
			break;
		case TEST_GET_REC_CALENDAR:
		{
			printf("[TEST] GET REC CALENDAR\n");
			STRU_REC_CALENDAR stuGetRecCalendar;
			memset(&stuGetRecCalendar,0,sizeof(stuGetRecCalendar));
			stuGetRecCalendar.calendar = 11; /*获取11月*/
			/*如果不想填参数四和五,则传入NULL和0*/
			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_REC_CALENDAR, (RT_APP_VOID *)&stuGetRecCalendar, sizeof(STRU_REC_CALENDAR));
		}
			break;
		case TEST_UNMOUNT_SD:
		{
			printf("[TEST UNMOUNT SD]\n");
			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_UNMOUNT_SD, NULL, 0);
		}
			break;
		case TEST_GET_ALARM_EX:
		{
			printf("[TEST GET ALARM EX PARAM]\n");
			RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_ALARM_EX, NULL, 0);
		}
			break;
		case TEST_SET_ALARM_EX:
		{
			printf("[TEST] SET ALARM EX\n");
			STRU_ALARM_PARAMS_EX stuSetAlarmEx;
			memset(&stuSetAlarmEx,0,sizeof(stuSetAlarmEx));

			stuSetAlarmEx.mdSensitive = 1;
			stuSetAlarmEx.mdInterval = 23;
			stuSetAlarmEx.mdEmailSnap = 1;
			stuSetAlarmEx.mdFtpSnap = 1;
			stuSetAlarmEx.mdFtpRec = 1;
			stuSetAlarmEx.ioEnable = 0; /*io告警开关 0-常开，1-常关，2-关闭*/
			stuSetAlarmEx.ioInterval = 15;
			stuSetAlarmEx.ioEmailSnap = 1;
			stuSetAlarmEx.ioFtpSnap = 1;
			stuSetAlarmEx.ioFtpRec = 1;
 
			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_SET_ALARM_EX, (RT_APP_VOID *)&stuSetAlarmEx, sizeof(STRU_ALARM_PARAMS_EX));
		}
			break;
		case TEST_GET_SD_REC_EX:
		{	
			printf("[TEST GET SD REC EX PARAM]\n");
			RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_SD_REC_EX, NULL, 0);
		}
	
			break;
		case TEST_SET_SD_REC_EX:
		{
			printf("[TEST] SET SD REC EX\n");
			STRU_SD_RECORD_EX stuSetSdRecEx;
			memset(&stuSetSdRecEx,0,sizeof(stuSetSdRecEx));

			stuSetSdRecEx.resolutionRec = 0;  /*录像的分辨率 0:主码流 1:副码流*/
			stuSetSdRecEx.sdRecMode = 0;     /*sd卡录像模式 0:不录像 1:全时录像 2:报警录像 */
			stuSetSdRecEx.fullRecTime = 120;   /*全时录像打包时间(单位秒)*/
			stuSetSdRecEx.alarmRecTime = 15;  /*告警录像打包时间(单位秒)*/
			stuSetSdRecEx.timelapseRecEnable = 0;  /*缩时录像开关 0:关 1:开*/
			stuSetSdRecEx.timelapseRecTime = 8;  /*缩时录像打包时间(单位小时)*/
			stuSetSdRecEx.timePlanEnable = 1;  /*定时录像计划开关 1:打开 0:关闭 (备注关闭的话默认为走7*24小时录像)*/
			stuSetSdRecEx.timePlanStartTime = 0; /*定时录像计划开始时间 (时间精度为15分钟 例如03:30  3*(60/15)+30/15=14) 备注:如果*/
			stuSetSdRecEx.timePlanEndTime = 96; /*定时录像计划结束时间 (时间精度为15分钟 例如20:45  20*(60/15)+45/15=83)*/
			stuSetSdRecEx.loopRecEnable = 1; /*循环录像 1:循环录像 0:不循环录像*/
			stuSetSdRecEx.audioRecEnable = 1; /*录像是否录制声音 1:录制声音 0:不录像声音*/
			
			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_SET_SD_REC_EX, (RT_APP_VOID *)&stuSetSdRecEx, sizeof(STRU_SD_RECORD_EX));
		}
			break;
		case TEST_FACTORY_RESET:
		{
			printf("[TEST FACTORY RESET]\n"); 
			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_FACTORY_RESET, NULL, 0);
		}
			break;
		case TEST_GET_OSD:
		{
			printf("[TEST GET OSD PARAM]\n");
			RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_OSD, NULL, 0);
		}
			break;
		case TEST_SET_OSD:
		{
			printf("[TEST SET OSD PARAM]\n");
			STRU_OSD_ATTRS stuSetOsd;
			memset(&stuSetOsd,0,sizeof(stuSetOsd));
			stuSetOsd.osdEnable = 1;
			strcpy(stuSetOsd.osdName,"runtop");
			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_SET_OSD, (RT_APP_VOID *)&stuSetOsd, sizeof(STRU_OSD_ATTRS));
		}
			break;
		case TEST_SET_GEOG:
		{
			printf("[TEST SET GEOG PARAM]\n");
			STRU_GEOG_ATTRS stuGeogAttr;
			memset(&stuGeogAttr,0,sizeof(STRU_GEOG_ATTRS));
			
			strcpy(stuGeogAttr.longitude,"42.35"); /*经度*/
			strcpy(stuGeogAttr.latitude,"87.39"); /*纬度*/
			stuGeogAttr.isRefresh = 1;
			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_SET_GEOG, (RT_APP_VOID *)&stuGeogAttr, sizeof(STRU_GEOG_ATTRS));
		}
			break;
		case TEST_SET_IRCUT:
		{
			printf("[TEST SET IRCUT PARAM]\n");
			STRU_IRCUT_ATTR stuIrcutAttr;
			memset(&stuIrcutAttr,0,sizeof(STRU_IRCUT_ATTR));
			stuIrcutAttr.ircutDelayTime = 1000*2; /*单位毫秒 2000ms*/
			RT_APP_API_SET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_SET_IRCUT, (RT_APP_VOID *)&stuIrcutAttr, sizeof(STRU_IRCUT_ATTR));
		}
			break;
		case TEST_GET_IRCUT:
		{
			printf("[TEST GET IRCUT PARAM]\n");
			RT_APP_API_GET_SYSTEM_Params(u32AppHandle, pUid, MSG_TYPE_GET_IRCUT, NULL, 0);
		}
		default:
			break;
	}
	
	sleep(5);
	g_stop = 1;
}

/*实时码流回调函数*/
/* frametype:0 表示I帧,1 表示P帧*/
RT_APP_INT32 rtH264VideoPlayerCallBack(const RT_APP_CHAR *uid,RT_APP_UCHAR *pbuf, RT_APP_INT32 frametype, RT_APP_UINT32 timestamp, RT_APP_INT32 len, RT_APP_VOID *pOutUserData)
{
	static RT_APP_INT32 pframe = 1;
	static RT_APP_INT32 saveTime = 0;
	
	if(frametype == 1) // P
	{
		printf("\033[32;49;1m  video[P] uid=%s,pbuf=%x,len=%d P[%02d] time[%d] \033[39;49;0m \n", uid,pbuf[6],len,pframe++,timestamp-saveTime);
		
	}
	else // I
	{
		printf( "\033[0;32;31m      video[I] uid=%s,pbuf=%x,len=%d \033[0m time[%d]\n", uid,pbuf[6],len,timestamp-saveTime);
		pframe = 1;
	}
	saveTime = timestamp;
	
	return 0;
}

#if 0
//暂时用不到,可以不用注册
RT_APP_INT32 rtOtherVideoPlayerCallBack(const RT_APP_CHAR *uid, RT_APP_UCHAR *pbuf, RT_APP_INT32 frametype, RT_APP_INT32 len)
{
	
	return 0;
}
#endif

/*实时码流测试函数*/
RT_APP_VOID testVideo(RT_APP_UPTR u32AppHandle,RT_APP_CHAR *pUid)
{
	while(1)
	{
		printf("\033[32;49;1m %s %s %d -----testVideo-----\033[39;49;0m\n", __FILE__, __FUNCTION__, __LINE__);
		RT_APP_API_START_LIVE_Stream(u32AppHandle, pUid, 3); // 3:720P	 0:VGA   1:QVGA	
		
		/*码流切换API*/
		//RT_APP_API_SWITCH_Stream(u32AppHandle, pUid, 0); // 3:720P	 0:VGA   1:QVGA	
		sleep(10000);
		RT_APP_API_STOP_LIVE_Stream(u32AppHandle, pUid);
		break;
	}
	g_stop = 1;
}

/*实时音频回调函数*/
RT_APP_INT32 audioPlayerCallBack(const RT_APP_CHAR *uid, RT_APP_UCHAR *pbuf, RT_APP_INT32 len)
{
	printf("-----callback audio-----%d\n",len);

	return 0;
}


/*实时音频测试函数*/
RT_APP_VOID testAudio(RT_APP_UPTR u32AppHandle,RT_APP_CHAR *pUid)
{
	static RT_APP_INT32 audioCount = 0;
	while(audioCount < 5)
	{
		printf("\033[32;49;1m %s %s %d -----testAudio-----\033[39;49;0m\n", __FILE__, __FUNCTION__, __LINE__);
		RT_APP_API_START_Audio(u32AppHandle, pUid, ENUM_AUDIO_TYPE_AAC);
		sleep(5);
		RT_APP_API_STOP_Audio(u32AppHandle, pUid);
		audioCount++;
	}
	g_stop = 1;
	
}

/*实时对讲测试函数*/
RT_APP_VOID testTalk(RT_APP_UPTR u32AppHandle,RT_APP_CHAR *pUid,RT_APP_INT32 talkAudioType)
{
		printf("\033[32;49;1m %s %s %d =====testTalk=====talkAudioType=%d\033[39;49;0m\n", __FILE__, __FUNCTION__, __LINE__,talkAudioType);
		RT_APP_API_START_Talk(u32AppHandle, pUid);
		
		#define SIZE (1024)
		while(1)
		{
			if(g_sendTalk != 1) //等待返回可以发送对讲数据的状态
			{
				usleep(1000*10);
				continue;
			}

			static FILE *pf = NULL;
			RT_APP_CHAR buf[SIZE * 2] = {0};
			int readSize = 0;
			if(pf == NULL)
			{
				/*从配置文件中读取音频文件发送给设备*/
				if(talkAudioType == 0) /*PCM音频文件*/
				{
					pf = fopen("./rt.pcm","rb");
				}
				else if(talkAudioType == 1) /*AAC音频文件*/
				{
					pf = fopen("./rt.aac","rb");
				}
				else
				{
					return ;
				}
				
			}
			if(pf)
			{
				RT_APP_INT32 readRet = 0;
				int audioType = 6;
				memset(buf,0,sizeof(buf));
				if(talkAudioType == 0)
				{
					readRet = fread(buf,1,SIZE,pf);
					audioType = ENUM_AUDIO_TYPE_PCM;
				}
				else if(talkAudioType == 1) /*aac这样读取的原因是保存aac文件时先存储了它的数据长度再存数据*/
				{
					readRet = fread(&readSize,1,sizeof(readSize),pf);
					memset(buf,0,sizeof(buf));
					readRet = fread(buf,1,readSize,pf);
					audioType = ENUM_AUDIO_TYPE_AAC;
				}
				if(readRet > 0)
				{
					#if 0
					printf("pcm readRet=%d\n",readRet);

					static FILE *pf = NULL;
					if(pf == NULL)
					{
						pf = fopen("/home/gbh/22p.pcm","wb");
					}
					if(pf)
					{
						fwrite(buf,1,readRet,pf);
					}
					#endif
					
					RT_APP_INT32 ret = -2;
					ret = RT_APP_API_TALK_AUDIO_Data(u32AppHandle, pUid, buf, readRet,audioType); /*6 pcm 7 aac*/
					usleep(1000*35);
					if(ret != 0)
					{
						fclose(pf);
						pf = NULL;
						break;
					}
				}
				else
				{
					fclose(pf);
					pf = NULL;
					break;
				}
			}
		
		}
		printf("------stop talk-------\n");
		RT_APP_API_STOP_Talk(u32AppHandle, pUid);
		g_stop = 1;
		g_sendTalk = 0;
}

/*TF远程回放回调函数*/
/* frametype:0 表示I帧,1 表示P帧*/
RT_APP_INT32 rtH264PlayBackCallBack(const RT_APP_CHAR *uid, RT_APP_UCHAR *pbuf, RT_APP_INT32 frametype, RT_APP_UINT32 timestamp, RT_APP_INT32 len)
{
	printf("-----palyback video-----len=%d,timestamp=%d\n",len,timestamp);
	return 0;
}

/*TF远程测试函数*/
//#define FILE_URL "20170812/rec001/rec011_20170812152250_01_0620_0060.avi"
#define FILE_URL "20170812/rec001/rec011_20170812150402_01_0346_0062.mp4"

RT_APP_INT32 testPlayback(RT_APP_UPTR u32AppHandle,RT_APP_CHAR *pUid)
{
	/* api参数解析 
	** FILE_URL:文件的名称
	** offset  :文件的偏移时间(一种是播放文件时间秒数的偏移,另外一种是播放文件的具体时间偏移)
	** 参数的组合填法如下 
	** fileName为NULL offset为文件的具体时间,则设备按时间轴的方式播放
	** fileName不为NULL offse为0则设备按文件名的方式播放, offse>0则使用拖放方式播放
	** 若参数不按此方式填,则导致设备播放文件失败
	*/
	sleep(5);
	RT_APP_API_START_PlayBack(u32AppHandle, pUid, (RT_APP_CHAR *)FILE_URL, 0);
	sleep(10);
	RT_APP_API_STOP_PlayBack(u32AppHandle, pUid);
	g_stop = 1;
	return 0;
}

/*告警回调函数*/
RT_APP_INT32 rtAlarmBackCallBack(const RT_APP_CHAR *uid, RT_APP_INT32 alarmType, RT_APP_VOID *pData)
{
	switch(alarmType)
	{
		case MOTION_ALARM: // 移动侦测
			printf("MD ALARM\n");
			break;
		case GPIO_ALARM: // IO 联动
			printf("IO ALARM\n");
			break;
		case SCHDEULE_ALARM: // 布防
			printf("SCHDEULE ALARM\n");
			break;
		case CONTROL_ALARM: // 遥控器
			printf("CONTROL ALARM\n");
			break;
		case HUMI_ALARM: // 温湿度
			printf("HUMI ALARM\n");
			break;
		case PM25_ALARM: //  PM25
			printf("PM25 ALARM\n");
			break;
		case SMOKE_ALARM: // 烟感
			printf("SMOKE ALARM\n");
			break;
		case SOUND_ALARM: // 声音
			printf("SOUND ALARM\n");
    		break;
		case MOTION_ALARM_EX: // 移动侦测告警扩展
			printf("MOTION ALARM EX\n");
			break;
		case GPIO_ALARM_EX: //GPIO告警扩展
			printf("GPIO ALARM EX\n");
			break;
    	default:
			break;
	}
	
	if(pData)
	{
		PSTRU_ALARM_EX_INFO pAlarmInfo = (PSTRU_ALARM_EX_INFO)pData;
		printf("alarmInfo=%s\n",pAlarmInfo->alarmInfo);
	}

	return 0;
}

RT_APP_INT32 rtDownLoadPictureCallBack(const RT_APP_CHAR *uid, const RT_APP_CHAR *pFileName, RT_APP_UCHAR *pbuf, RT_APP_INT32 len, RT_APP_UCHAR endFlag)
{
	if(pbuf == NULL || len <= 0)
	{
		return -1;
	}
	
	static FILE *picturePf = NULL;
	if(picturePf == NULL)
	{
		char saveBuf[256] = {0};
		char *start = strrchr(pFileName,'/');
		sprintf(saveBuf,"./%s",start+1);
		picturePf = fopen(saveBuf,"w+");
	}
	if(picturePf)
	{
		fwrite(pbuf,1,len,picturePf);
		if(endFlag == 0)
		{
			fclose(picturePf);
			picturePf = NULL;
		}
		
	}

	printf("rtPictureCallBack pFileName=%s,len=%d,endFlag=%d\n",pFileName,len,endFlag);
	return 0;
}

RT_APP_INT32 rtDownLoadVideoCallBack(const RT_APP_CHAR *uid, const RT_APP_CHAR *pFileName, RT_APP_UCHAR *pbuf, RT_APP_INT32 len, RT_APP_UCHAR endFlag)
{
	if(pbuf == NULL || len <= 0)
	{
		return -1;
	}

	static FILE *videoPf = NULL;
	if(videoPf == NULL)
	{
		char saveBuf[256] = {0};
		char *start = strrchr(pFileName,'/');
		sprintf(saveBuf,"./%s",start+1);
		videoPf = fopen(saveBuf,"w+");
	}
	if(videoPf)
	{
		fwrite(pbuf,1,len,videoPf);
		if(endFlag == 0)
		{
			fclose(videoPf);
			videoPf = NULL;
		}
		
	}
	printf("rtDownLoadVideoCallBack pFileName=%s,len=%d,endFlag=%d\n",pFileName,len,endFlag);
	
	return 0;
}

RT_APP_INT32 testDownLoadFile(RT_APP_UPTR u32AppHandle,RT_APP_CHAR *pUid)

{
	/*备注:调用RT_APP_API_START_DOWNLOAD_File的前提条件是MSG_TYPE_GET_DOWNLOAD_FILE中的
	 *文件状态为ENUM_MEDIA_DATA_EXIST表示当前下载文件存在于TF卡中.
	 *提示:可以调用RT_APP_API_START_DOWNLOAD_File中断文件下载,需传入参数见具体函数
	 */
	STRU_MEDIA_DATA stuDownLoadData;
	memset(&stuDownLoadData,0,sizeof(stuDownLoadData));
	
	strcpy(stuDownLoadData.szFileName,g_willDownLoadFile);

	if(strstr(stuDownLoadData.szFileName,".jpg"))
	{
		stuDownLoadData.mediaType = ENUM_MEDIA_TYPE_PICTURE;
	}
	else
	{
		stuDownLoadData.mediaType = ENUM_MEDIA_TYPE_VIDEO;
	}
	
	printf("\033[0;32;31m will start download file\n");
	RT_APP_API_START_DOWNLOAD_File(u32AppHandle, pUid, (RT_APP_VOID *)&stuDownLoadData, sizeof(STRU_MEDIA_DATA));

	#if 0
	sleep(5);
	memset(&stuDownLoadData,0,sizeof(stuDownLoadData));
	
	strcpy(stuDownLoadData.szFileName,"/bin/vs/sd/20170711/IMG001/IMG_chn0_md_20170715152847_001.jpg");
	stuDownLoadData.mediaType = ENUM_MEDIA_TYPE_PICTURE;
	
	RT_APP_API_START_DOWNLOAD_File(u32AppHandle, pUid, (RT_APP_VOID *)&stuDownLoadData, sizeof(STRU_MEDIA_DATA));
	#endif

	#if 0
	sleep(5);
	printf("\033[0;32;31m will stop download file\n");
	RT_APP_API_STOP_DOWNLOAD_File(u32AppHandle, pUid, stuDownLoadData.szFileName);
	#endif
	
	return 0;
}

/*日志打印函数有如下几个级别LOG_LEVEl*/
RT_APP_VOID showLog1(RT_APP_INT32 logLevel,const RT_APP_CHAR *format)
{
	//if(logLevel == ENUM_LOGD || ) //选择打印级别
	{
		printf("%s\n",format);
	}
}

RT_APP_VOID usage(RT_APP_VOID)
{
	printf("appTest <UID> <user> <name> <TestOption>\n");
	printf("TestOption:1  video    test\n");
	printf("TestOption:2  audio    test\n");
	printf("TestOption:3  talk(0:PCM 1:AAC)test\n");
	printf("TestOption:4  playback test\n");
	printf("TestOption:5  download test\n");
	printf("TestOption:6  param    test\n");
}

RT_APP_INT32 main(RT_APP_INT32 argc,RT_APP_CHAR *argv[])
{
	RT_LOG_REGISTER_CALL_Back(showLog1);

	if(argc < 5)
	{
		usage();
		return -1;
	}

	RT_APP_INT32 type = 0;
	RT_APP_CHAR chUid[64]={0};
	RT_APP_CHAR chUser[64]={0};
	RT_APP_CHAR chPwd[64]={0};
	strcpy(chUid,argv[1]);
	strcpy(chUser,argv[2]);
	strcpy(chPwd,argv[3]);
	type = atoi(argv[4]);

	RT_APP_Params appInitParams;
	memset(&appInitParams,0,sizeof(RT_APP_Params));
	
	RT_APP_UPTR u32AppHandle = 0;
	RT_APP_INT32 ret = 0;

	/*回调注册区*/
	appInitParams.getMsgDataCallBack = rtGetMsgDataCallBack;
	appInitParams.setMsgReturnCallBack = rtSetMsgReturnCallBack;
	appInitParams.connectStatusCallBack = rtConnectCallBack;
	appInitParams.searchDataCallBack = rtSearchDataCallBack;
	appInitParams.h264VideoPlayerCallBack = rtH264VideoPlayerCallBack;
	appInitParams.h264PlayBackCallBack = rtH264PlayBackCallBack;
	//appInitParams.otherVideoPlayerCallBack = rtOtherVideoPlayerCallBack;
	appInitParams.audioPlayerCallBack = audioPlayerCallBack;
	appInitParams.alarmCallBack = rtAlarmBackCallBack;
	appInitParams.loadPicCallBack = rtDownLoadPictureCallBack;
	appInitParams.loadVidCallBack = rtDownLoadVideoCallBack;
	ret = RT_APP_API_Initial(&appInitParams);
	if(ret != 0)
	{
		printf("init failed\n");
		return -1;
	}
	g_stop = 0;
	
	/*开始连接设备*/
	RT_APP_API_Start(&u32AppHandle,chUid,chUser,chPwd,NULL);
	g_u32AppHandle = u32AppHandle;
	strcpy(g_uid,chUid);
	
	while(1)
	{

		if(g_onLine == 1)
		{
			switch(type)
			{
				case 1: // video
					printf("[TEST VIDEO......]\n");
					testVideo(u32AppHandle,chUid);
					break;
				case 2: // audio
					printf("[TEST AUDIO......]\n");
					testAudio(u32AppHandle,chUid);
					break;
				case 3: // talk
					printf("[TEST TALK 0:PCM 1:AAC...]\n");
					RT_APP_INT32 talkType = atoi(argv[5]);
					testTalk(u32AppHandle,chUid,talkType);
					break;
				case 4: // playback
					printf("[TEST PLAYBACK......]\n");
					testPlayback(u32AppHandle,chUid);
					break;
				case 5: // download file
					printf("[TEST DOWNLOAD FILE......]\n");
					if(argc != 6)
					{
						printf("appTest <UID> <user> <name> 5 <absolute path download file>\n");
						exit(0);
					}
					strcpy(g_willDownLoadFile,argv[5]);
					testDownLoadFile(u32AppHandle,chUid);
					break;
				case 6: // param
				{
					if(argc != 6)
					{
						printf("appTest <UID> <user> <name> 6 <paramVale>\n");
						exit(0);
					}
					printf("[TEST PARAM......]\n");
					RT_APP_INT32 paramValue = atoi(argv[5]);
					testParam(u32AppHandle,chUid,paramValue);
				}
					break;
				default: // unknown
					break;
			}
			
			break;
		}
		else
		{
			continue;
		}
	}

	while(1)
	{
		
		if(g_stop == 1)
		{
			printf("======stop======\n");
			ret += RT_APP_API_Stop(u32AppHandle,chUid);
			u32AppHandle = 0;
			RT_APP_API_DeInitial();
			exit(0);
		}
		
		usleep(1000*100);
	}
	return 0;
}
