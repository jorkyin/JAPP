package com.runtop.rtp2plibrary;

import static com.runtop.rtp2plibrary.model.RTNativeCallBack.PPPP_STATUS_CONNECTING;
import static com.runtop.rtp2plibrary.model.RTNativeCallBack.PPPP_STATUS_CONNECT_FAILED;
import static com.runtop.rtp2plibrary.model.RTNativeCallBack.PPPP_STATUS_CONNECT_TIMEOUT;
import static com.runtop.rtp2plibrary.model.RTNativeCallBack.PPPP_STATUS_DEVICE_NOT_ON_LINE;
import static com.runtop.rtp2plibrary.model.RTNativeCallBack.PPPP_STATUS_DISCONNECT;
import static com.runtop.rtp2plibrary.model.RTNativeCallBack.PPPP_STATUS_INITIALING;
import static com.runtop.rtp2plibrary.model.RTNativeCallBack.PPPP_STATUS_INVALID_ID;
import static com.runtop.rtp2plibrary.model.RTNativeCallBack.PPPP_STATUS_INVALID_USER_PWD;
import static com.runtop.rtp2plibrary.model.RTNativeCallBack.PPPP_STATUS_MAX_CONNECT_CNT;
import static com.runtop.rtp2plibrary.model.RTNativeCallBack.PPPP_STATUS_ON_LINE;

public class RTNetInfo {
    private String mIP = "";
    private String mStateString = "";
    private int mStateValue = -1;
    private String mMac = "";
    private int mPort = 0;

    public RTNetInfo(){}

    public RTNetInfo(String IPAddr, String Mac, int port) {
        this.mIP = IPAddr;
        this.mMac = Mac;
        this.mPort = port;
    }

    public String getIP() {
        return mIP;
    }

    public void setIP(String IP) {
        this.mIP = IP;
    }

    public String getStateString() {
        return mStateString;
    }

    public void setStateString(String state) {
        this.mStateString = state;
    }

    public int getStateValue() {
        return mStateValue;
    }

    public void setStateValue(int state) {
        this.mStateValue = state;

        switch (this.mStateValue) {
            case PPPP_STATUS_CONNECTING:/* 正在连接 */
                mStateString = "正在连接";
                break;
            case PPPP_STATUS_INITIALING:/* 启动连接 */
                mStateString = "启动连接";
                break;
            case PPPP_STATUS_ON_LINE: /* 设备在线 */
                mStateString = "设备在线";
                break;
            case PPPP_STATUS_CONNECT_FAILED:/* 连接失败 */
                mStateString = "连接失败";
                break;
            case PPPP_STATUS_DISCONNECT: /* 连接断开*/
                mStateString = "连接断开";
                break;
            case PPPP_STATUS_INVALID_ID:/* 无效UID*/
                mStateString = "无效UID";
                break;
            case PPPP_STATUS_DEVICE_NOT_ON_LINE:/*设备不在线*/
                mStateString = "设备不在线";
                break;
            case PPPP_STATUS_CONNECT_TIMEOUT:/* 连接超时 */
                mStateString = "连接超时";
                break;
            case PPPP_STATUS_INVALID_USER_PWD:/* 用户名或者密码错误 */
                mStateString = "用户名或者密码错误";
                break;
            case PPPP_STATUS_MAX_CONNECT_CNT:/*超过设备端的最大连接数*/
                mStateString = "超过设备端的最大连接数";
                break;
        }
    }

    public String getMac() {
        return mMac;
    }

    public void setMac(String mMac) {
        this.mMac = mMac;
    }

    public int getPort() {
        return mPort;
    }

    public void setPort(int mPort) {
        this.mPort = mPort;
    }

    public String toString() {
        return "mIP = " + mIP
                + " mStateString = " + mStateString
                + " mStateValue = " + mStateValue
                + " mMac = " + mMac
                + " mPort = " + mPort;
    }
}
