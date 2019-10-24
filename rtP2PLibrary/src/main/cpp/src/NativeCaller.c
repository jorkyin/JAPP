//
// Created by yinjian on 2017/11/11.
//



#include <rt_object_api.h>
#include <memory.h>
#include <rt_p2p_api_define.h>
#include <log.h>
#include "NativeCaller.h"
#include "NativeCallBack.h"
#include "RTMap.h"
#include "rt_p2p_live.h"

#include <android/native_window_jni.h>
/*

pthread_t audio_decode_thread = NULL;
pthread_t audio_player_thread = NULL;
*/



//设置回调的类

JNIEXPORT jint JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_setCallBackContext(JNIEnv *env, jclass type,
                                                                   jobject object) {

//初始化java回调中的方法，用于将消息已反向调用的方式返回回去
    if (NativeCallBack_init(env, type, object) != 0) {
        return -1;
    }

    return 0;
}

//初始化SDK
JNIEXPORT jint JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_RTSDKInit(JNIEnv *env, jclass type) {

    RT_APP_Params appInitParams;
    memset(&appInitParams, 0, sizeof(RT_APP_Params));

    /*回调注册区*/
    //获取信息回调
    appInitParams.getMsgDataCallBack = rtGetMsgDataCallBack;
    //设置信息回调
    appInitParams.setMsgReturnCallBack = rtSetMsgReturnCallBack;
    //连接回调
    appInitParams.connectStatusCallBack = rtConnectCallBack;
    //搜索回调
    appInitParams.searchDataCallBack = rtSearchDataCallBack;
    //直播回调
    appInitParams.h264VideoPlayerCallBack = rtH264VideoPlayerCallBack;
    //回播回调
    appInitParams.h264PlayBackCallBack = rtH264PlayBackCallBack;
    //其他流
    //appInitParams.otherVideoPlayerCallBack = rtOtherVideoPlayerCallBack;
    //音频
    appInitParams.audioPlayerCallBack = audioPlayerCallBack;
    //告警
    appInitParams.alarmCallBack = rtAlarmBackCallBack;
    //下载图片回调
    appInitParams.loadPicCallBack = rtDownLoadPictureCallBack;
    //下载视频回调
    appInitParams.loadVidCallBack = rtDownLoadVideoCallBack;
    //日志回调
    //RT_LOG_REGISTER_CALL_Back(showLog1);

    if (RT_APP_API_Initial(&appInitParams) != 0) {
        LOGD("-----------RT_APP_API_Initial failed-------------\n");
        return -1;
    }

    //初始化map
    RT_InitMap();

    return 0;
}


//开始搜索局域网内部的UID
JNIEXPORT void JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_StartSearchDev(JNIEnv *env, jclass type) {

    RT_APP_API_SEARCH_DEV_Start();

}
//结束搜索局域网内部的UID
JNIEXPORT void JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_StopSearchDev(JNIEnv *env, jclass type) {

    RT_APP_API_SEARCH_DEV_Stop();

}
//链接设备
JNIEXPORT void JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_StartConnectDev(JNIEnv *env, jclass type,
                                                                jstring UID_,
                                                                jstring user_, jstring password_,
                                                                jstring userData_) {
    const char *UID = (*env)->GetStringUTFChars(env, UID_, 0);
    const char *user = (*env)->GetStringUTFChars(env, user_, 0);
    const char *password = (*env)->GetStringUTFChars(env, password_, 0);
    const char *userData = (*env)->GetStringUTFChars(env, userData_, 0);
    RT_APP_UPTR u32AppHandle = 0;
    RT_APP_API_Start(&u32AppHandle, (RT_APP_CHAR *) UID, (RT_APP_CHAR *) user,
                     (RT_APP_CHAR *) password, (void *) userData);

    //将handle保存到Map中
    RT_DeleteHanleFromMap(UID);
    RT_AddHandleToMap(UID, u32AppHandle);

    (*env)->ReleaseStringUTFChars(env, UID_, UID);
    (*env)->ReleaseStringUTFChars(env, user_, user);
    (*env)->ReleaseStringUTFChars(env, password_, password);
    (*env)->ReleaseStringUTFChars(env, userData_, userData);
}

//断开设备链接
JNIEXPORT void JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_StopConnectDev(JNIEnv *env, jclass type,
                                                               jstring UID_) {
    const char *UID = (*env)->GetStringUTFChars(env, UID_, 0);

    RT_APP_UPTR u32AppHandle = 0;
    RT_GetHandleFromMap(UID, &u32AppHandle);
    if (u32AppHandle != 0) {
        RT_APP_API_Stop(u32AppHandle, (RT_APP_CHAR *) UID);
        //删除map中的handle
        RT_DeleteHanleFromMap(UID);
    }

    (*env)->ReleaseStringUTFChars(env, UID_, UID);
}

//开启视频流
JNIEXPORT jint JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_StartLiveStream(JNIEnv *env, jclass type,
                                                                jstring UID_,jint streamid) {
    int ret=-1 ;
    const char *UID = (*env)->GetStringUTFChars(env, UID_, 0);
    RT_APP_UPTR u32AppHandle = 0;
    RT_GetHandleFromMap(UID, &u32AppHandle);//通过map获取设备句柄

    if (u32AppHandle != 0) {
        //开始播放视频流
        //ret=start_play_video(u32AppHandle, (RT_APP_CHAR *) UID, (RT_APP_UPTR) streamid);
        ret=start_video_play(u32AppHandle, (RT_APP_CHAR *) UID,(RT_APP_UPTR) streamid);
    }

    (*env)->ReleaseStringUTFChars(env, UID_, UID);
    return ret;
}

//停止播放视频流
JNIEXPORT jint JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_StopLiveStream(JNIEnv *env, jclass type,
                                                               jstring UID_) {
    const char *UID = (*env)->GetStringUTFChars(env, UID_, 0);
    int ret =-1;
    RT_APP_UPTR u32AppHandle = 0;
    RT_GetHandleFromMap(UID, &u32AppHandle);//通过map获取设备句柄

    if (u32AppHandle != 0) {
        //停止播放视频流
       // ret=stop_play_video(u32AppHandle, (RT_APP_CHAR *) UID);
        ret=stop_video_play(u32AppHandle, (RT_APP_CHAR *) UID);
    }

    (*env)->ReleaseStringUTFChars(env, UID_, UID);
    return ret;
}

/*码流切换API*/
JNIEXPORT jint JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_SwitchLiveStream(JNIEnv *env, jclass type,
                                                                 jstring UID_,
                                                                 jint streamid) {
    const char *UID = (*env)->GetStringUTFChars(env, UID_, 0);
int ret=-1;
    RT_APP_UPTR u32AppHandle = 0;
    RT_GetHandleFromMap(UID, &u32AppHandle);//通过map获取设备句柄

    if (u32AppHandle != 0) {
        ret=switcht_video_play(u32AppHandle, (RT_APP_CHAR *) UID, (RT_APP_UPTR) streamid);
    }

    (*env)->ReleaseStringUTFChars(env, UID_, UID);
    return ret;
}

/*实时音频监听开始*/
JNIEXPORT jint JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_StartLiveAudio(JNIEnv *env, jclass type,
                                                               jstring UID_,
                                                               jint audioType) {
    int ret = -1;
    const char *UID = (*env)->GetStringUTFChars(env, UID_, 0);
    RT_APP_UPTR u32AppHandle = 0;
    RT_GetHandleFromMap(UID, &u32AppHandle);//通过map获取设备句柄
    if (u32AppHandle != 0) {
        ret = start_audio_play(u32AppHandle, (RT_APP_CHAR *) UID, (RT_APP_UCHAR) audioType);
    }
    (*env)->ReleaseStringUTFChars(env, UID_, UID);
    return ret;
}

/*实时音频监听结束*/
JNIEXPORT jint JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_StopLiveAudio(JNIEnv *env, jclass type,
                                                              jstring UID_) {
    int ret = -1;
    const char *UID = (*env)->GetStringUTFChars(env, UID_, 0);
    //LOGI("StopLiveAudio UID=%s ", UID);
    RT_APP_UPTR u32AppHandle = 0;
    RT_GetHandleFromMap(UID, &u32AppHandle);//通过map获取设备句柄
    if (u32AppHandle != 0) {
        ret = stop_audio_play(u32AppHandle, (RT_APP_CHAR *) UID);
    }
    (*env)->ReleaseStringUTFChars(env, UID_, UID);
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_StartLiveTalk(JNIEnv *env, jclass type,
                                                              jstring UID_) {
    const char *UID = (*env)->GetStringUTFChars(env, UID_, 0);

    RT_APP_UPTR u32AppHandle = 0;
    RT_GetHandleFromMap(UID, &u32AppHandle);//通过map获取设备句柄

    if (u32AppHandle != 0) {
        RT_APP_API_START_Talk(u32AppHandle, (RT_APP_CHAR *) UID);
    }

    (*env)->ReleaseStringUTFChars(env, UID_, UID);
    return 0;
}

JNIEXPORT jint JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_StartLiveTalkAudioData(JNIEnv *env, jclass type,
                                                                       jstring UID_,
                                                                       jint streamid) {
    const char *UID = (*env)->GetStringUTFChars(env, UID_, 0);

    // TODO

    (*env)->ReleaseStringUTFChars(env, UID_, UID);
    return 0;
}

JNIEXPORT jint JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_StopLiveTalk(JNIEnv *env, jclass type,
                                                             jstring UID_) {
    const char *UID = (*env)->GetStringUTFChars(env, UID_, 0);

    RT_APP_UPTR u32AppHandle = 0;
    RT_GetHandleFromMap(UID, &u32AppHandle);//通过map获取设备句柄

    if (u32AppHandle != 0) {
        RT_APP_API_STOP_Talk(u32AppHandle, (RT_APP_CHAR *) UID);
    }
    (*env)->ReleaseStringUTFChars(env, UID_, UID);
    return 0;
}


JNIEXPORT jint JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_SetNativeWindow(JNIEnv *env, jclass type,
                                                                jobject surface, jint width,
                                                                jint height) {

    return set_native_window(ANativeWindow_fromSurface(env, surface),width, height);
}

JNIEXPORT jint JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_SetWindowsSize(JNIEnv *env, jclass type, jint width,
                                                               jint height) {
    //return setWindowsSize(width, height);
    return set_windows_size(width, height);
}

JNIEXPORT jint JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_GetP2PCapabilityParam(JNIEnv *env, jclass type,
                                                                      jstring UID_) {
    const char *UID = (*env)->GetStringUTFChars(env, UID_, 0);

    RT_APP_UPTR u32AppHandle = 0;
    RT_GetHandleFromMap(UID, &u32AppHandle);//通过map获取设备句柄

    if (u32AppHandle != 0) {
        //开始播放视频流
        RT_APP_API_GET_SYSTEM_Params(u32AppHandle, (RT_APP_CHAR *) UID, MSG_TYPE_GET_P2P_CAPABILITY,
                                     NULL, 0);
    } else {
        (*env)->ReleaseStringUTFChars(env, UID_, UID);
        return -1;
    }

    (*env)->ReleaseStringUTFChars(env, UID_, UID);
    return 0;
}
JNIEXPORT jint JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_GetP2PGetP2PSDNewRecordList(JNIEnv *env,
                                                                            jclass type,
                                                                            jstring UID_,
                                                                            jstring starttime_,
                                                                            jstring endtime_,
                                                                            jint page,
                                                                            jint fileType,
                                                                            jint pageSize,
                                                                            jint showType) {
    const char *UID = (*env)->GetStringUTFChars(env, UID_, 0);
    const char *starttime = (*env)->GetStringUTFChars(env, starttime_, 0);
    const char *endtime = (*env)->GetStringUTFChars(env, endtime_, 0);

    RT_APP_UPTR u32AppHandle = 0;
    RT_GetHandleFromMap(UID, &u32AppHandle);//通过map获取设备句柄

    if (u32AppHandle != 0) {
        //获取SD卡录像文件列表
        STRU_SEARCH_SDCARD_RECORD_FILE sdcardsearch;
        memset(&sdcardsearch, 0, sizeof(sdcardsearch));

        /* 搜索开始时间 */
        strcpy(sdcardsearch.starttime,starttime);

        /* 搜索结束时间 */
        strcpy(sdcardsearch.endtime,endtime);

        sdcardsearch.page = page;   /* 翻页值 */
        sdcardsearch.fileType = fileType; /* 文件类型 :1-常规录像,2-报警录像,3-所有录像 */
        sdcardsearch.pageSize = pageSize; /* 自定义页文件大小 */
        sdcardsearch.showType = showType; /*0:文件夹方式显示 1:时间轴方式显示(设备不会传文件名过来)*/

        RT_APP_API_GET_SYSTEM_Params(u32AppHandle, (RT_APP_CHAR *) UID, MSG_TYPE_GET_NEW_RECORD_FILE,
                                     (RT_APP_VOID *)&sdcardsearch, sizeof(STRU_SEARCH_SDCARD_RECORD_FILE));
    } else {
        (*env)->ReleaseStringUTFChars(env, UID_, UID);
        (*env)->ReleaseStringUTFChars(env, starttime_, starttime);
        (*env)->ReleaseStringUTFChars(env, endtime_, endtime);
        return -1;
    }

    (*env)->ReleaseStringUTFChars(env, UID_, UID);
    (*env)->ReleaseStringUTFChars(env, starttime_, starttime);
    (*env)->ReleaseStringUTFChars(env, endtime_, endtime);
    return 0;
}

//开始录像
JNIEXPORT jint JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_StartRecordVideo(JNIEnv *env, jclass type,
                                                                 jstring UID_, jint audioType,
                                                                 jstring VideoFlieName_) {
    int ret = -1;
    const char *UID = (*env)->GetStringUTFChars(env, UID_, 0);
    const char *VideoFlieName = (*env)->GetStringUTFChars(env, VideoFlieName_, 0);
  //  LOGI("RT_APP_API_START_Audio UID=%s audioType=%d", UID, audioType);

    RT_APP_UPTR u32AppHandle = 0;
    RT_GetHandleFromMap(UID, &u32AppHandle);//通过map获取设备句柄
    if (u32AppHandle != 0) {
        ret = start_record_mp4(u32AppHandle, (RT_APP_CHAR *) UID, (RT_APP_UCHAR) audioType,
                                 (char *) VideoFlieName);
    }
    (*env)->ReleaseStringUTFChars(env, UID_, UID);
    (*env)->ReleaseStringUTFChars(env, VideoFlieName_, VideoFlieName);
    return ret;
}
//停止录像
JNIEXPORT jint JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_StopRecordVideo(JNIEnv *env, jclass type,
                                                                jstring UID_) {
    int ret = -1;
    const char *UID = (*env)->GetStringUTFChars(env, UID_, 0);
    RT_APP_UPTR u32AppHandle = 0;
    RT_GetHandleFromMap(UID, &u32AppHandle);//通过map获取设备句柄
    if (u32AppHandle != 0) {
      //  LOGI("RT_APP_API_STOP_Audio UID=%s ", UID);
        ret = stop_record_mp4(u32AppHandle, (RT_APP_CHAR *) UID);
    }
    (*env)->ReleaseStringUTFChars(env, UID_, UID);
    return ret;
}
//截取视频流成图片
JNIEXPORT jint JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_GetPicture(JNIEnv *env, jclass type,
                                                           jstring PictureFilePath_) {
    const char *PictureFilePath = (*env)->GetStringUTFChars(env, PictureFilePath_, 0);

    int ret= record_picture((char *) PictureFilePath);

    (*env)->ReleaseStringUTFChars(env, PictureFilePath_, PictureFilePath);
    return ret;
}
//开始回放录像
JNIEXPORT jint JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_StartPlayBack(JNIEnv *env, jclass type,
                                                                jstring UID_, jstring url_,
                                                                jint totalTime, jint play_back_type,
                                                                jlong offset) {
    const char *UID = (*env)->GetStringUTFChars(env, UID_, 0);
    const char *url = (*env)->GetStringUTFChars(env, url_, 0);

    int ret=-1;

    RT_APP_UPTR u32AppHandle = 0;
    RT_GetHandleFromMap(UID, &u32AppHandle);//通过map获取设备句柄
    if (u32AppHandle != 0) {
        ret = start_playback(u32AppHandle, (RT_APP_CHAR *) UID, (RT_APP_CHAR *) url,totalTime, play_back_type,
                             (RT_APP_LONG) offset);
    }

    (*env)->ReleaseStringUTFChars(env, UID_, UID);
    (*env)->ReleaseStringUTFChars(env, url_, url);
    return ret;
}
//停止回放录像
JNIEXPORT jint JNICALL
Java_com_runtop_rtp2plibrary_model_RTNativeCaller_StopPlayBack(JNIEnv *env, jclass type,
                                                               jstring UID_, jstring url_,
                                                               jint play_back_type) {
    const char *UID = (*env)->GetStringUTFChars(env, UID_, 0);
    const char *url = (*env)->GetStringUTFChars(env, url_, 0);
    int ret=-1;

    RT_APP_UPTR u32AppHandle = 0;
    RT_GetHandleFromMap(UID, &u32AppHandle);//通过map获取设备句柄
    if (u32AppHandle != 0) {
        ret = stop_playback(u32AppHandle, (RT_APP_CHAR *) UID, play_back_type);
    }
    (*env)->ReleaseStringUTFChars(env, UID_, UID);
    (*env)->ReleaseStringUTFChars(env, url_, url);
    return ret;
}
