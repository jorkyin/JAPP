//
// Created by jorkyin on 18-3-1.
//


#include <memory.h>
#include <rt_p2p_types.h>
#include <log.h>
#include <rt_p2p_api_define.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include "NativeCallBack.h"
#include "rt_p2p_live.h"
#include "NativeCaller.h"

RT_APP_INT32 g_onLine = 0;
RT_APP_INT32 g_sendTalk = 0;
//全局变量
CALLBACK_HANDLE g_callBackHandle;
jmethodID g_CallBack_SearchResult = NULL;//搜索摄像头，会将搜索到的摄像头回调回来
jmethodID g_CallBack_ConnectDev = NULL;//连接设备回调
jmethodID g_CallBack_GetP2PCapability = NULL;//数据获取回调
jmethodID g_CallBack_VideoWindowsSize = NULL;//返回视频显示窗口大小
jmethodID g_CallBack_pcmData = NULL;//返回PCM音频数据用于AudioTrack播放
jmethodID g_CallBack_AudioState = NULL;//返回监听是声音是否开启
jmethodID g_CallBack_RTVideoViewState = NULL;//返回视频播放控件准备状态是否开启
jmethodID g_CallBack_VideoStreamType = NULL;//返回正在点播的码流
jmethodID g_CallBack_SDRecordList = NULL;//返回搜索到的sd卡录像文件信息
jmethodID g_CallBack_PlayBackSeek=NULL; //返回正在回放的进度
jmethodID g_CallBack_pictureData=NULL;//从c层返回图片数据给java层
/**
* 通过全局变量g_JavaVM获取JNIEnv
*/
int RT_GetEnv(JNIEnv **env) {
    int cRet = 0;

    if (g_callBackHandle.javaVM == NULL) {
        //LOGD("[%s	%d] javaVM NULL\n",__FUNCTION__,__LINE__);
        return -1;
    }

    jint status = (*g_callBackHandle.javaVM)->GetEnv(g_callBackHandle.javaVM, (void **) env,
                                                     JNI_VERSION_1_4);

    if (status < 0) {
        //将当前的线程绑定到jvm虚拟机线程当中，并获取env
        status = (*g_callBackHandle.javaVM)->AttachCurrentThread(g_callBackHandle.javaVM, env,
                                                                 NULL);
        if (status < 0) {
            //LOGD("[%s	%d] AttachCurrentThread Faile\n",__FUNCTION__,__LINE__);
            return -1;
        }
        cRet = 1;
    }
    return cRet;
}

//初始化回调方法
int NativeCallBack_init(JNIEnv *env, jclass cls, jobject context) {
    memset(&g_callBackHandle, 0, sizeof(CALLBACK_HANDLE));
    if (context == NULL) {
        g_CallBack_SearchResult = NULL;
        g_CallBack_ConnectDev = NULL;
        g_CallBack_GetP2PCapability = NULL;
        g_CallBack_VideoWindowsSize = NULL;
        g_CallBack_pcmData = NULL;
        g_CallBack_pictureData=NULL;
        g_CallBack_AudioState = NULL;
        g_CallBack_RTVideoViewState = NULL;
        g_CallBack_VideoStreamType = NULL;
        g_CallBack_SDRecordList = NULL;
        g_CallBack_PlayBackSeek=NULL;
        return -1;
    }

    //获取jvm(虚拟机)
    (*env)->GetJavaVM(env, &g_callBackHandle.javaVM);
    //新建全局变量
    g_callBackHandle.callBackObj = (*env)->NewGlobalRef(env, context);
    //获取当前类对象
    jclass clazz = (*env)->GetObjectClass(env, context);

    //jni反向调用
    g_CallBack_SearchResult = (*env)->GetMethodID(env, clazz, "CallBack_SearchResult",
                                                  "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
    g_CallBack_ConnectDev = (*env)->GetMethodID(env, clazz, "CallBack_ConnectDev",
                                                "(Ljava/lang/String;II)V");
    g_CallBack_GetP2PCapability = (*env)->GetMethodID(env, clazz, "CallBack_GetP2PCapability",
                                                      "(Ljava/lang/String;Ljava/lang/String;)V");
    g_CallBack_VideoWindowsSize = (*env)->GetMethodID(env, clazz, "CallBack_VideoWindowsSize",
                                                      "(II)V");
    g_CallBack_PlayBackSeek= (*env)->GetMethodID(env, clazz, "CallBack_PlayBackSeek",
                                                 "(II)V");
    g_CallBack_pcmData = (*env)->GetMethodID(env, clazz, "CallBack_pcmData", "([BI)V");
    g_CallBack_pictureData=(*env)->GetMethodID(env, clazz, "CallBack_pictureData", "(Ljava/lang/String;[BI)V");
    g_CallBack_VideoStreamType = (*env)->GetMethodID(env, clazz, "CallBack_VideoStreamType",
                                                     "(I)V");
    g_CallBack_AudioState = (*env)->GetMethodID(env, clazz, "CallBack_AudioState", "(I)V");
    g_CallBack_RTVideoViewState = (*env)->GetMethodID(env, clazz, "CallBack_RTVideoViewState",
                                                      "(I)V");
    g_CallBack_SDRecordList = (*env)->GetMethodID(env, clazz, "CallBack_SDRecordList",
                                                  "(Ljava/lang/String;Ljava/lang/String;IIIIII)V");
    return 0;
}


//销毁回调方法
void RT_Callback_free(JNIEnv *env) {

    //删除全局变量
    (*env)->DeleteGlobalRef(env, g_callBackHandle.callBackObj);
    g_callBackHandle.callBackObj = NULL;

    //销毁虚拟机
    g_callBackHandle.javaVM = NULL;

    g_CallBack_SearchResult = NULL;
    g_CallBack_ConnectDev = NULL;
    g_CallBack_GetP2PCapability = NULL;
    g_CallBack_VideoWindowsSize = NULL;
    g_CallBack_VideoStreamType = NULL;
    g_CallBack_AudioState = NULL;
    g_CallBack_RTVideoViewState = NULL;
    g_CallBack_pcmData = NULL;
    g_CallBack_pictureData=NULL;
    g_CallBack_SDRecordList = NULL;
    g_CallBack_PlayBackSeek=NULL;
}


/* 内网搜索设备UID返回回调*/
RT_APP_INT32 rtSearchDataCallBack(RT_APP_CHAR *pbuf) {
    LOGD("[%s   %d]\n", __FUNCTION__, __LINE__);
    JNIEnv *env;
    int cRet = RT_GetEnv(&env);
    if (cRet == -1) {
        return -1;
    }

    //转为jstring
    STRU_SEARCH_PARAMS *pSearchDev = (STRU_SEARCH_PARAMS *) pbuf;
    jstring macAddr = (*env)->NewStringUTF(env, pSearchDev->szMacAddr);
    jstring devName = (*env)->NewStringUTF(env, pSearchDev->szDevName);
    jstring devDid = (*env)->NewStringUTF(env, pSearchDev->szDevUID);
    jstring ipAddr = (*env)->NewStringUTF(env, pSearchDev->szIpAddr);
    jint port = pSearchDev->nPort;

    //LOGD("%s,%s,%s,%s",pSearchDev->szMacAddr,pSearchDev->szDevName,pSearchDev->szDevUID,pSearchDev->szIpAddr);


    (*env)->CallVoidMethod(env, g_callBackHandle.callBackObj, g_CallBack_SearchResult,
                           macAddr, devName, devDid, ipAddr, port);


    //释放临时变量
    (*env)->DeleteLocalRef(env, devDid);
    (*env)->DeleteLocalRef(env, ipAddr);
    (*env)->DeleteLocalRef(env, macAddr);
    (*env)->DeleteLocalRef(env, devName);

    //如果是绑定的方式，要解绑
    if (cRet == 1) {
        (*g_callBackHandle.javaVM)->DetachCurrentThread(g_callBackHandle.javaVM);
    }

    return 0;
}

/* 数据获取回调 */
RT_APP_INT32 rtGetMsgDataCallBack(const RT_APP_CHAR *uid, RT_APP_INT32 msg, RT_APP_CHAR *pbuf,
                                  RT_APP_INT32 len) {
    JNIEnv *env;
    int cRet = RT_GetEnv(&env);
    if (cRet == -1) {
        return -1;
    }
    jstring UID = (*env)->NewStringUTF(env, uid);
    switch (msg) {
        case MSG_TYPE_GET_P2P_CAPABILITY: //获取p2p设备的功能集
            LOGD("[%s   %d] rtGetMsgDataCallBack uid=%s pbuf=%s\n", __FUNCTION__, __LINE__, uid,
                 pbuf);
            jstring CapabilityMsg = (*env)->NewStringUTF(env, pbuf);

            (*env)->CallVoidMethod(env, g_callBackHandle.callBackObj, g_CallBack_GetP2PCapability,
                                   UID, CapabilityMsg);

            //释放临时变量
            (*env)->DeleteLocalRef(env, CapabilityMsg);

            break;
        case MSG_TYPE_GET_STATUS:;
            LOGI("rtGetMsgDataCallBack uid=%s msg=%d pbuf=%s len=%d", uid, msg, pbuf, len);
            break;
        case MSG_TYPE_GET_NEW_RECORD_FILE: //新协议和老协议的录像文件数据获取来自一个结构体
        case MSG_TYPE_GET_RECORD_FILE:
            LOGD("[%s   %d] uid=%s pbuf=%s\n", __FUNCTION__, __LINE__, uid, pbuf);
            PSTRU_SDCARD_RECORD_FILE pRecordFile = NULL;
            pRecordFile = (PSTRU_SDCARD_RECORD_FILE) pbuf;
            if (pRecordFile == NULL) {
                return -1;
            }
            jstring fileName = (*env)->NewStringUTF(env, pRecordFile->szFileName);
            (*env)->CallVoidMethod(env, g_callBackHandle.callBackObj, g_CallBack_SDRecordList,
                                   UID, fileName, pRecordFile->nFileSize,
                                   pRecordFile->fileDayStartTime, pRecordFile->fileDayEndTime,
                                   pRecordFile->fileTotalTime,
                                   pRecordFile->currRecordCount, pRecordFile->nFileSize);
            //释放临时变量
            (*env)->DeleteLocalRef(env, fileName);
            break;
        default:
            break;
    }
    //释放临时变量
    (*env)->DeleteLocalRef(env, UID);
    //如果是绑定的方式，要解绑
    if (cRet == 1) {
        (*g_callBackHandle.javaVM)->DetachCurrentThread(g_callBackHandle.javaVM);
    }

    return 0;
}

/* 设置回调返回值 */
/**pbuf中的值为1时表示用户设置到设备的值是成功,0表示设置失败 */
RT_APP_INT32 rtSetMsgReturnCallBack(const RT_APP_CHAR *uid, RT_APP_INT32 msg, RT_APP_CHAR *pbuf,
                                    RT_APP_INT32 len) {

    LOGI("rtSetMsgReturnCallBack uid=%s msg=%d pbuf=%s len=%d", uid, msg, pbuf, len);
    return 0;
}


//连接回调
RT_APP_INT32
rtConnectCallBack(const RT_APP_CHAR *uid, RT_APP_INT32 msgType, RT_APP_INT32 msgValue) {
    //LOGD("[%s   %d]\n", __FUNCTION__, __LINE__);
    JNIEnv *env;
    int cRet = RT_GetEnv(&env);
    if (cRet == -1) {
        return -1;
    }
    jstring UID = (*env)->NewStringUTF(env, uid);


    (*env)->CallVoidMethod(env, g_callBackHandle.callBackObj, g_CallBack_ConnectDev, UID, msgType,
                           msgValue);
    //释放临时变量
    (*env)->DeleteLocalRef(env, UID);
    //如果是绑定的方式，要解绑
    if (cRet == 1) {
        (*g_callBackHandle.javaVM)->DetachCurrentThread(g_callBackHandle.javaVM);
    }
    return 0;
}

/*实时码流回调函数*/
/* frametype:0 表示I帧,1 表示P帧*/
RT_APP_INT32
rtH264VideoPlayerCallBack(const RT_APP_CHAR *uid, RT_APP_UCHAR *pbuf, RT_APP_INT32 frametype,
                          RT_APP_UINT32 timestamp, RT_APP_INT32 len,
                          RT_APP_VOID *pOutUserData) {
    static RT_APP_INT32 pframe = 1;
    static RT_APP_INT32 saveTime = 0;

    if (frametype == 1) // P
    {
        pframe++;
        // LOGI("video[P] uid=%s,pbuf=%x,len=%d P[%02d] time[%d] ", uid, pbuf[6], len, pframe, timestamp - saveTime);

    } else // I
    {

        //LOGI("video[I] uid=%s,pbuf=%x,len=%d time[%d]", uid, pbuf[6], len, timestamp - saveTime);

        pframe = 1;
    }

    //将返回的视频数据返回给播放器
    videoPlayer((RT_APP_CHAR *) uid, pbuf, len, frametype, timestamp);
    old_speed += len;

    saveTime = timestamp;
    return 0;
}

/*TF远程回放回调函数*/
/* frametype:0表示视频I帧 1表示视频P帧,6代表pcm 7表示音频AAC帧*/
RT_APP_INT32
rtH264PlayBackCallBack(const RT_APP_CHAR *uid, RT_APP_UCHAR *pbuf, RT_APP_INT32 frametype,
                       RT_APP_UINT32 timestamp, RT_APP_INT32 len) {

    LOGI("frametype =%d", frametype);

    static RT_APP_INT32 pframe = 1;
    static RT_APP_INT32 saveTime = 0;


    if (frametype == 1 || frametype == 0) // P
    {
        // LOGI("video uid=%s,pbuf=%x,len=%d timestamp[%d] ", uid, pbuf[6], len, timestamp);
        //将返回的视频数据返回给播放器
        videoPlayer((RT_APP_CHAR *) uid, pbuf, len, frametype, timestamp);
    } else if (frametype==6)// I
    {
        playback_audio((RT_APP_CHAR *) uid, pbuf, len, 0, timestamp);
    } else if (frametype==7){
        playback_audio((RT_APP_CHAR *) uid, pbuf, len, 1, timestamp);
    }


    old_speed += len;

    saveTime = timestamp;
    return 0;
}

/*实时音频回调函数*/
RT_APP_INT32
audioPlayerCallBack(const RT_APP_CHAR *uid, RT_APP_UCHAR *pbuf, RT_APP_INT32 len) {
   // LOGI("audio uid=%s,pbuf=%x,len=%d ", uid, pbuf[6], len);
    if (len > 0) {
        live_audio_player((RT_APP_CHAR *) uid, pbuf, len);
    }
    return 0;
}

/*告警回调函数*/
RT_APP_INT32 rtAlarmBackCallBack(const RT_APP_CHAR *uid, RT_APP_INT32 alarmType,
                                 RT_APP_VOID *pData) { return 0; }

//下载照片回调
RT_APP_INT32
rtDownLoadPictureCallBack(const RT_APP_CHAR *uid, const RT_APP_CHAR *pFileName, RT_APP_UCHAR *pbuf,
                          RT_APP_INT32 len, RT_APP_UCHAR endFlag) { return 0; }

//下载视频回调
RT_APP_INT32
rtDownLoadVideoCallBack(const RT_APP_CHAR *uid, const RT_APP_CHAR *pFileName, RT_APP_UCHAR *pbuf,
                        RT_APP_INT32 len, RT_APP_UCHAR endFlag) { return 0; }

//返回视频播放器窗口大小
RT_APP_INT32
rtVideoSize(int width, int height) {
    LOGD("[%s   %d]\n", __FUNCTION__, __LINE__);
    JNIEnv *env;
    int cRet = RT_GetEnv(&env);
    if (cRet == -1) {
        return -1;
    }

    (*env)->CallVoidMethod(env, g_callBackHandle.callBackObj, g_CallBack_VideoWindowsSize, width,
                           height);


    //如果是绑定的方式，要解绑
    if (cRet == 1) {
        (*g_callBackHandle.javaVM)->DetachCurrentThread(g_callBackHandle.javaVM);
    }
    return 0;
}

//返回PCM音频数据用于AudioTrack播放
RT_APP_INT32
rtPcm2AudioTrack(uint8_t *buffer, int size) {
    LOGD("[%s   %d]\n", __FUNCTION__, __LINE__);
    JNIEnv *env;
    int cRet = RT_GetEnv(&env);
    if (cRet == -1) {
        return -1;
    }

    //buffer缓冲数据，转成byte数组
    jbyteArray data_array = (*env)->NewByteArray(env, size);
    jbyte *sample_byte = (*env)->GetByteArrayElements(env, data_array, NULL);
    //赋值
    memcpy(sample_byte, buffer, (size_t) size);
    //同步
    (*env)->ReleaseByteArrayElements(env, data_array, sample_byte, 0);
    //将pcm的数据写入AudioTrack
    (*env)->CallVoidMethod(env, g_callBackHandle.callBackObj, g_CallBack_pcmData, data_array, size);


    //如果是绑定的方式，要解绑
    if (cRet == 1) {
        (*g_callBackHandle.javaVM)->DetachCurrentThread(g_callBackHandle.javaVM);
    }
    return 0;
}

//从c层返回图片数据给java层，
RT_APP_INT32
rtPictureData2java(const RT_APP_CHAR *uid,uint8_t *buffer, int size) {
    LOGD("[%s   %d]\n", __FUNCTION__, __LINE__);
    JNIEnv *env;
    int cRet = RT_GetEnv(&env);
    if (cRet == -1) {
        return -1;
    }
    jstring UID = (*env)->NewStringUTF(env, uid);
    //buffer缓冲数据，转成byte数组
    jbyteArray data_array = (*env)->NewByteArray(env, size);
    jbyte *sample_byte = (*env)->GetByteArrayElements(env, data_array, NULL);
    //赋值
    memcpy(sample_byte, buffer, (size_t) size);
    //同步
    (*env)->ReleaseByteArrayElements(env, data_array, sample_byte, 0);
    //将pcm的数据写入AudioTrack
    (*env)->CallVoidMethod(env, g_callBackHandle.callBackObj, g_CallBack_pictureData,UID, data_array, size);
    //释放临时变量
    (*env)->DeleteLocalRef(env, UID);

    //如果是绑定的方式，要解绑
    if (cRet == 1) {
        (*g_callBackHandle.javaVM)->DetachCurrentThread(g_callBackHandle.javaVM);
    }
    return 0;
}



//返回监听是声音是否开启
RT_APP_INT32
rtVideoViewState(int rtVideoViewState) {
    LOGD("[%s   %d]\n", __FUNCTION__, __LINE__);
    JNIEnv *env;
    int cRet = RT_GetEnv(&env);
    if (cRet == -1) {
        return -1;
    }

    (*env)->CallVoidMethod(env, g_callBackHandle.callBackObj, g_CallBack_RTVideoViewState,
                           rtVideoViewState);


    //如果是绑定的方式，要解绑
    if (cRet == 1) {
        (*g_callBackHandle.javaVM)->DetachCurrentThread(g_callBackHandle.javaVM);
    }
    return 0;
}

//返回监听是声音是否开启
RT_APP_INT32
rtAudioState(int audioState) {
    LOGD("[%s   %d]\n", __FUNCTION__, __LINE__);
    JNIEnv *env;
    int cRet = RT_GetEnv(&env);
    if (cRet == -1) {
        return -1;
    }

    (*env)->CallVoidMethod(env, g_callBackHandle.callBackObj, g_CallBack_AudioState, audioState);


    //如果是绑定的方式，要解绑
    if (cRet == 1) {
        (*g_callBackHandle.javaVM)->DetachCurrentThread(g_callBackHandle.javaVM);
    }
    return 0;
}


//返回正在点播的码流
RT_APP_INT32
rtVideoStreamType(int videoStreamType) {
   LOGD("[%s   %d]\n", __FUNCTION__, __LINE__);
    JNIEnv *env;
    int cRet = RT_GetEnv(&env);
    if (cRet == -1) {
        return -1;
    }

    (*env)->CallVoidMethod(env, g_callBackHandle.callBackObj, g_CallBack_VideoStreamType,
                           videoStreamType);


    //如果是绑定的方式，要解绑
    if (cRet == 1) {
        (*g_callBackHandle.javaVM)->DetachCurrentThread(g_callBackHandle.javaVM);
    }
    return 0;
}


//返回正在回放的进度
RT_APP_INT32
rtPlayBackSeek(int player_time,int total_time) {
    //LOGD("[%s   %d]\n", __FUNCTION__, __LINE__);
    JNIEnv *env;
    int cRet = RT_GetEnv(&env);
    if (cRet == -1) {
        return -1;
    }

    (*env)->CallVoidMethod(env, g_callBackHandle.callBackObj, g_CallBack_PlayBackSeek,
                           player_time,total_time);


    //如果是绑定的方式，要解绑
    if (cRet == 1) {
        (*g_callBackHandle.javaVM)->DetachCurrentThread(g_callBackHandle.javaVM);
    }
    return 0;
}