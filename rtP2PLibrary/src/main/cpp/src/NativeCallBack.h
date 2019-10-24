//
// Created by jorkyin on 18-3-1.
//

#ifndef BARLEY_RUNTOPCALLBACK_H
#define BARLEY_RUNTOPCALLBACK_H


#include <jni.h>

#include <rt_p2p_types.h>
typedef struct _CALLBACK_HANDLE_{

    JavaVM* javaVM;
    jobject callBackObj;
    JNIEnv *env;
    int isBand;
    int isStopAudio;
}CALLBACK_HANDLE;

//初始化回调方法
int NativeCallBack_init(JNIEnv *env,jclass cls,jobject context);
//销毁回调方法
void RT_Callback_free(JNIEnv *env);

/* 内网搜索设备UID返回回调*/
RT_APP_INT32 rtSearchDataCallBack(RT_APP_CHAR *pbuf);
/* 数据获取回调 */
RT_APP_INT32 rtGetMsgDataCallBack(const RT_APP_CHAR *uid, RT_APP_INT32 msg, RT_APP_CHAR *pbuf,
                                  RT_APP_INT32 len);
/* 设置回调返回值 */
/**pbuf中的值为1时表示用户设置到设备的值是成功,0表示设置失败 */
RT_APP_INT32 rtSetMsgReturnCallBack(const RT_APP_CHAR *uid, RT_APP_INT32 msg, RT_APP_CHAR *pbuf,
                                    RT_APP_INT32 len);

//连接回调
RT_APP_INT32
rtConnectCallBack(const RT_APP_CHAR *uid, RT_APP_INT32 msgType, RT_APP_INT32 msgValue);

/*实时码流回调函数*/
/* frametype:0 表示I帧,1 表示P帧*/
RT_APP_INT32
rtH264VideoPlayerCallBack(const RT_APP_CHAR *uid, RT_APP_UCHAR *pbuf, RT_APP_INT32 frametype,
                          RT_APP_UINT32 timestamp, RT_APP_INT32 len,
                          RT_APP_VOID *pOutUserData);

/*TF远程回放回调函数*/
/* frametype:0 表示I帧,1 表示P帧*/
RT_APP_INT32
rtH264PlayBackCallBack(const RT_APP_CHAR *uid, RT_APP_UCHAR *pbuf, RT_APP_INT32 frametype,
                       RT_APP_UINT32 timestamp, RT_APP_INT32 len);

/*实时音频回调函数*/
RT_APP_INT32
audioPlayerCallBack(const RT_APP_CHAR *uid, RT_APP_UCHAR *pbuf, RT_APP_INT32 len);

/*告警回调函数*/
RT_APP_INT32 rtAlarmBackCallBack(const RT_APP_CHAR *uid, RT_APP_INT32 alarmType,
                                 RT_APP_VOID *pData);

//下载照片回调
RT_APP_INT32
rtDownLoadPictureCallBack(const RT_APP_CHAR *uid, const RT_APP_CHAR *pFileName, RT_APP_UCHAR *pbuf,
                          RT_APP_INT32 len, RT_APP_UCHAR endFlag);
//下载视频回调
RT_APP_INT32
rtDownLoadVideoCallBack(const RT_APP_CHAR *uid, const RT_APP_CHAR *pFileName, RT_APP_UCHAR *pbuf,
                        RT_APP_INT32 len, RT_APP_UCHAR endFlag);
RT_APP_VOID testVideo(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *pUid);



//返回视频播放器窗口大小
RT_APP_INT32
rtVideoSize(int width,int height);

//返回PCM音频数据用于AudioTrack播放
RT_APP_INT32
rtPcm2AudioTrack(uint8_t *buffer,int size);

//从c层返回图片数据给java层，
RT_APP_INT32
rtPictureData2java(const RT_APP_CHAR *uid,uint8_t *buffer, int size);

RT_APP_INT32
rtVideoViewState(int rtVideoViewState);

//返回监听是声音是否开启
RT_APP_INT32
rtAudioState(int audioState);

//返回正在点播的码流
RT_APP_INT32
rtVideoStreamType(int  videoStreamType);

//返回正在回放的进度
RT_APP_INT32
rtPlayBackSeek(int player_time,int total_time);
#endif //BARLEY_RUNTOPCALLBACK_H
