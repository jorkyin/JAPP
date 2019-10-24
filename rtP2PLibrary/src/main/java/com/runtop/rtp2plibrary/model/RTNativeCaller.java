package com.runtop.rtp2plibrary.model;

import android.view.Surface;

public class RTNativeCaller {
    static {
        System.loadLibrary("native");
    }
    //设置回调的类
    public static native int setCallBackContext(Object object);
    //初始化SDK
    public static native int RTSDKInit();
    //开始搜索局域网内部的UID
    public static native void StartSearchDev();
    //结束搜索局域网内部的UID
    public static native void StopSearchDev();
    //连接设备
    public static native void StartConnectDev(String UID, String user, String password, String userData);//userData 由用户输入,从实时码流回调进行返回);
    //断开设备链接
    public static native void StopConnectDev(String UID);
    //开启视频流
    public static native int StartLiveStream(String UID,int streamid);// 3:720P   1:QVGA		 0:VGA
    //停止视频流
    public static native int StopLiveStream(String UID);
    //切换视频流
    public static native int SwitchLiveStream(String UID, int streamid);// 3:720P   1:QVGA		 0:VGA
    //开启监听
    public static native int StartLiveAudio(String UID, int audioType);// 3:720P   1:QVGA		 0:VGA
    //停止监听
    public static native int StopLiveAudio(String UID);
    //开启音频对讲
    public static native int StartLiveTalk(String UID);
    //发送对讲音频数据
    public static native int StartLiveTalkAudioData(String UID, int streamid);
    //停止音频对讲
    public static native int StopLiveTalk(String UID);

    //设置本地播放器窗口，用于在C层写入视频数据
    public static native int SetNativeWindow(Surface surface,int width,int height);
    //设置实现窗口大小
    public static native int SetWindowsSize(int width,int height);





    /**
     * 获取摄像机系统参数
     */
    //获取能力级参数
    public static native int GetP2PCapabilityParam(String UID);

    /*
     * 获取SD录像文件信息
     *@param：UID
     * @param：starttime 搜索开始时间  "20190125000000"
     * @param：endtime   搜索结束时间 "20190125235959"
     * @param：page      翻页值
     * @param：fileType 文件类型 :1-常规录像,2-报警录像,3-所有录像
     * @param：pageSize 自定义页文件大小 100
     * @param：showType 0:文件夹方式显示 1:时间轴方式显示(设备不会传文件名过来)
     * @return： int        0：成功  -1：失败
     * */
    public static native int GetP2PGetP2PSDNewRecordList(String UID,String starttime,String endtime,int page,int fileType,int pageSize,int showType);

    /* api参数解析
     ** fileName:文件的名称
     ** offset  :文件的偏移时间(一种是播放文件时间秒数的偏移,另外一种是播放文件的具体时间偏移)
     ** 参数的组合填法如下
     ** fileName为NULL offset为文件的具体时间,则设备按时间轴的方式播放
     ** fileName不为NULL offse为0则设备按文件名的方式播放, offse>0则使用拖放方式播放
     ** 若参数不按此方式填,则导致设备播放文件失败
     */
    //开始回放录像
    public static native int StartPlayBack(String UID,String url,int totalTime,int play_back_type,long offset);
    //停止回放录像
    public static native int StopPlayBack(String UID,String url,int play_back_type);

    //开始录像
    public static native int StartRecordVideo(String UID, int audioType,String VideoFlieName);
    //停止录像
    public static native int StopRecordVideo(String UID);
    //截取视频流成图片
    public static native int GetPicture(String PictureFilePath);
}
