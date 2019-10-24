package com.runtop.rtp2plibrary.model;

import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;

import org.greenrobot.eventbus.EventBus;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;

public class RTNativeCallBack {

    private final static String TAG = "RTNativeCallBack";
    public final static String CALLBACKTYPE = "CallBackType";

    public final static class CallBackType {
        public final static String CALL_BACK_TYPE_CAMERA_SEARCH = "CALL_BACK_TYPE_CAMERA_SEARCH";
        public final static String CALL_BACK_TYPE_P2P_STATUS = "CALL_BACK_TYPE_P2P_STATUS";
        public final static String CALL_BACK_TYPE_P2P_MODE = "CALL_BACK_TYPE_P2P_MODE";
        public final static String CALL_BACK_TYPE_CAMERA_STREAM_TYPE = "CALL_BACK_TYPE_CAMERA_STREAM_TYPE";
        public final static String CALL_BACK_TYPE_CAMERA_TALK_STATUS = "CALL_BACK_TYPE_CAMERA_TALK_STATUS";
        public final static String CALL_BACK_TYPE_CAMERA_GET_P2P_CAPABILITY = "CALL_BACK_TYPE_CAMERA_GET_P2P_CAPABILITY";
        public final static String CALL_BACK_TYPE_VIDEO_WINDOWS_SIZE = "CALL_BACK_TYPE_VIDEO_WINDOWS_SIZE";
        public final static String CALL_BACK_TYPE_Audio_State = "CALL_BACK_TYPE_Audio_State";
        public final static String CALL_BACK_TYPE_Video_Stream_Type = "CALL_BACK_TYPE_Video_Stream_Type";
        public final static String CALL_BACK_TYPE_PCM_DATE = "CALL_BACK_TYPE_PCM_DATE";
        public final static String CALL_BACK_TYPE_Picture_Data = "CALL_BACK_TYPE_Picture_Data";
        public static final String CALL_BACK_TYPE_RTVideoView_State = "CALL_BACK_TYPE_RTVideoView_State";
        public static final String CALL_BACK_TYPE_SDRecordList = "CALL_BACK_TYPE_SDRecordList";
        public static final String CallBack_PlayBackSeek = "CallBack_PlayBackSeek";
    }

    /*p2p msgtype*/
    public static final int MSG_NOTIFY_TYPE_PPPP_STATUS = 0;   /* p2p连接状态*/
    public static final int MSG_NOTIFY_TYPE_PPPP_MODE = 1;   /* p2p类型*/
    public static final int MSG_NOTIFY_TYPE_STREAM_TYPE = 2;   //视频流类型
    public static final int MSG_NOTIFY_TYPE_TALK_STATUS = 3;  /*当msgValue的值为PPPP_STATUS_SEND_TALK_DATA时表示可以发送对讲数据*/
    /*p2p连接状态*/
    public static final int PPPP_STATUS_CONNECTING = 0; /* 正在连接 */
    public static final int PPPP_STATUS_INITIALING = 1; /* 启动连接 */
    public static final int PPPP_STATUS_ON_LINE = 2; /* 设备在线 */
    public static final int PPPP_STATUS_CONNECT_FAILED = 3; /* 连接失败 */
    public static final int PPPP_STATUS_DISCONNECT = 4; /* 连接断开*/
    public static final int PPPP_STATUS_INVALID_ID = 5; /* 无效UID*/
    public static final int PPPP_STATUS_DEVICE_NOT_ON_LINE = 6; /*设备不在线*/
    public static final int PPPP_STATUS_CONNECT_TIMEOUT = 7; /* 连接超时 */
    public static final int PPPP_STATUS_INVALID_USER_PWD = 8; /* 用户名或者密码错误 */
    public static final int PPPP_STATUS_MAX_CONNECT_CNT = 9; /*超过设备端的最大连接数*/
    /*p2p类型*/
    public static final int PPPP_MODE_P2P = 1;//p2p直连
    public static final int PPPP_MODE_RELAY = 2;//转发
    /*talk status*/
    public static final int PPPP_STATUS_SEND_TALK_DATA = 0; /* user or pwd is invalid */

    private static RTNativeCallBack mRTNativeCallBack;

    public static RTNativeCallBack getInstance() {
        if (mRTNativeCallBack == null) {
            mRTNativeCallBack = new RTNativeCallBack();
        }
        return mRTNativeCallBack;
    }

    public static void destoryCallBack() {
        mRTNativeCallBack = null;
    }

    public RTNativeCallBack() {
        RTNativeCaller.setCallBackContext(this);
    }


    /**
     * 搜索摄像头，会将搜索到的摄像头回调回来
     */
    private void CallBack_SearchResult(String strMac, String strName, String strDeviceID, String strIpAddr, int port) {
        EventBus mEventBus = EventBus.getDefault();
        Bundle bundle = new Bundle();
        bundle.putString(CALLBACKTYPE, CallBackType.CALL_BACK_TYPE_CAMERA_SEARCH);
        Log.i(TAG, "CallBack_SearchResult: Mac=" + strMac + " strName=" + strName
                + " strDeviceID=" + strDeviceID + " strIpAddr=" + strIpAddr + " port=" + port);
        bundle.putString("mac", strMac);
        bundle.putString("devName", strName);
        bundle.putString("devDid", strDeviceID);
        bundle.putString("ipAddr", strIpAddr);
        bundle.putInt("port", port);
        mEventBus.post(bundle);//将搜索回来的摄像头信息发送给AddCameraPresenter
    }

    //连接设备回调
    private void CallBack_ConnectDev(String uid, int msgTye, int msgValue) {
        //Log.i(TAG, "CallBack_SearchResult: uid="+uid+" msgTye="+msgTye+" msgValue="+msgValue);
        EventBus mEventBus = EventBus.getDefault();
        Bundle bundle = new Bundle();
        bundle.putString("uid", uid);
        bundle.putInt("msgValue", msgValue);
        switch (msgTye) {
            case MSG_NOTIFY_TYPE_PPPP_STATUS: /* p2p连接状态*/
                bundle.putString(CALLBACKTYPE, CallBackType.CALL_BACK_TYPE_P2P_STATUS);
                break;
            case MSG_NOTIFY_TYPE_PPPP_MODE:   /* p2p类型*/
                bundle.putString(CALLBACKTYPE, CallBackType.CALL_BACK_TYPE_P2P_MODE);
                break;
            case MSG_NOTIFY_TYPE_STREAM_TYPE:  //视频流类型
                bundle.putString(CALLBACKTYPE, CallBackType.CALL_BACK_TYPE_CAMERA_STREAM_TYPE);
                break;
            case MSG_NOTIFY_TYPE_TALK_STATUS:  /*音频对讲*/
                bundle.putString(CALLBACKTYPE, CallBackType.CALL_BACK_TYPE_CAMERA_TALK_STATUS);
                break;
        }
        mEventBus.post(bundle);
    }

    /**
     * 数据获取回调
     */
    //P2P能力级回调
    private void CallBack_GetP2PCapability(String uid, String CapabilityMsg) {
        //Log.i(TAG, "CallBack_GetP2PCapability: UID="+uid+" CapabilityMsg="+CapabilityMsg);

        EventBus mEventBus = EventBus.getDefault();
        Bundle bundle = new Bundle();
        bundle.putString(CALLBACKTYPE, CallBackType.CALL_BACK_TYPE_CAMERA_GET_P2P_CAPABILITY);
        bundle.putString("uid", uid);
        bundle.putString("CapabilityMsg", CapabilityMsg);
        mEventBus.post(bundle);
    }

    //返回视频显示窗口大小
    private void CallBack_VideoWindowsSize(int width, int height) {
        // Log.i(TAG, "CallBack_VideoWindowsSize: width="+width+" height="+height);

        EventBus mEventBus = EventBus.getDefault();
        Bundle bundle = new Bundle();
        bundle.putString(CALLBACKTYPE, CallBackType.CALL_BACK_TYPE_VIDEO_WINDOWS_SIZE);
        bundle.putInt("width", width);
        bundle.putInt("height", height);
        mEventBus.post(bundle);
    }

    //返回正在回放的进度
    private void CallBack_PlayBackSeek(int player_time,int total_time) {
        // Log.i(TAG, "CallBack_VideoWindowsSize: width="+width+" height="+height);

        EventBus mEventBus = EventBus.getDefault();
        Bundle bundle = new Bundle();
        bundle.putString(CALLBACKTYPE, CallBackType.CallBack_PlayBackSeek);
        bundle.putInt("player_time", player_time);
        bundle.putInt("total_time", total_time);
        mEventBus.post(bundle);
    }

    //返回监听是声音是否开启
    private void CallBack_RTVideoViewState(int RTVideoViewState) {
        Log.i(TAG, "CallBack_RTVideoViewState:" + RTVideoViewState);

        EventBus mEventBus = EventBus.getDefault();
        Bundle bundle = new Bundle();
        bundle.putString(CALLBACKTYPE, CallBackType.CALL_BACK_TYPE_RTVideoView_State);
        bundle.putInt("RTVideoViewState", RTVideoViewState);
        mEventBus.post(bundle);
    }

    //返回监听是声音是否开启
    private void CallBack_AudioState(int audioState) {
        Log.i(TAG, "CallBack_AudioState: audioState=" + audioState);

        EventBus mEventBus = EventBus.getDefault();
        Bundle bundle = new Bundle();
        bundle.putString(CALLBACKTYPE, CallBackType.CALL_BACK_TYPE_Audio_State);
        bundle.putInt("audioState", audioState);
        mEventBus.post(bundle);
    }

    //返回正在点播的码流
    private void CallBack_VideoStreamType(int videoStreamType) {
        Log.i(TAG, "CallBack_VideoStreamType: videoStreamType=" + videoStreamType);

        EventBus mEventBus = EventBus.getDefault();
        Bundle bundle = new Bundle();
        bundle.putString(CALLBACKTYPE, CallBackType.CALL_BACK_TYPE_Video_Stream_Type);
        bundle.putInt("videoStreamType", videoStreamType);
        mEventBus.post(bundle);
    }

    //从c层返回PCM音频数据给java层，用于AudioTrack播放
    private void CallBack_pcmData(byte[] audioData, int sizeInBytes) {
        // Log.i(TAG, "CallBack_pcmData: sizeInBytes="+sizeInBytes);

        EventBus mEventBus = EventBus.getDefault();
        Bundle bundle = new Bundle();
        bundle.putString(CALLBACKTYPE, CallBackType.CALL_BACK_TYPE_PCM_DATE);
        bundle.putByteArray("audioData", audioData);
        bundle.putInt("sizeInBytes", sizeInBytes);
        mEventBus.post(bundle);
    }

    //从c层返回图片数据给java层，
    private void CallBack_pictureData(String uid,byte[] pictureData, int size) {
        Log.i(TAG, "CallBack_pictureData: size="+size);
        EventBus mEventBus = EventBus.getDefault();
        Bundle bundle = new Bundle();
        bundle.putString(CALLBACKTYPE, CallBackType.CALL_BACK_TYPE_Picture_Data);
        bundle.putString("uid", uid);
        bundle.putByteArray("pictureData", pictureData);
        bundle.putInt("size", size);



        mEventBus.post(bundle);
    }



    /**
     * //返回搜索到的sd卡录像文件信息
     *
     * @param : uid
     * @param ：fileName 录像文件名*
     * @param : fileSize        录像文件大小
     * @param : startTime 录像的开始时间 HH:MM:SS的时间秒的总和,ex: 18:23:56 ---> 18*3600+23*60+56
     * @param : endTime;   录像的结束时间 HH:MM:SS的时间秒的总和,ex: 18:23:56 ---> 18*3600+23*60+56
     * @param : totalTime;    录像文件单个时长
     * @param : currRecordCount;  当前页的录像文件个数
     * @param : fileTotalCount;   当前设备搜索时间中SD总文件个数
     */
    private void CallBack_SDRecordList(String uid, String fileName, int fileSize, int startTime, int endTime, int totalTime, int currRecordCount, int fileTotalCount) {
       /* Log.i(TAG, "CallBack_SDRecordList: uid=" + uid + " fileName=" + fileName + " fileSize=" + fileSize + " startTime=" + startTime + " endTime=" + endTime + " totalTime=" + totalTime
                + " currRecordCount=" + currRecordCount + " fileTotalCount=" + fileTotalCount);*/

        /*LOGI("[RECORD] filetime[%02d:%02d:%02d]-[%02d:%02d:%02d]",pRecordFile->fileDayStartTime/3600,(pRecordFile->fileDayStartTime/60)%60,pRecordFile->fileDayStartTime%60,
                pRecordFile->fileDayEndTime/3600,(pRecordFile->fileDayEndTime/60)%60,pRecordFile->fileDayEndTime%60);*/
        EventBus mEventBus = EventBus.getDefault();
        Bundle bundle = new Bundle();
        bundle.putString(CALLBACKTYPE, CallBackType.CALL_BACK_TYPE_SDRecordList);
        bundle.putString("uid", uid);
        bundle.putString("fileName", fileName);
        bundle.putInt("fileSize", fileSize);
        bundle.putInt("startTime", startTime);
        bundle.putInt("endTime", endTime);
        bundle.putInt("totalTime", totalTime);
        bundle.putInt("currRecordCount", currRecordCount);
        bundle.putInt("fileTotalCount", fileTotalCount);
        mEventBus.post(bundle);

    }
}


