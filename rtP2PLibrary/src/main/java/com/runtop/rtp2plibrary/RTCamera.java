package com.runtop.rtp2plibrary;

/**
 * Created by yinjian on 2018/2/26.
 */

public class RTCamera {
    public static final String TAG = "RTCamera";
    /**
     * mUID : PPRT-012345-XXXXXX
     * nUser : admin
     * mPassword : admin
     */
    private String mCamName = "";
    private RTCapability mRTCapability = new RTCapability();
    private RTP2P mRTP2P = new RTP2P();
    private RTNetInfo mRTNetInfo = new RTNetInfo();

    public RTCamera(String name) {
        this.mCamName = name;
    }

    public String getCamName() {
        return mCamName;
    }

    public void setCaName(String camName) {
        this.mCamName = camName;
    }

    public RTCapability getCapability() {
        return mRTCapability;
    }

    public void setCapability(RTCapability RTCapability) {
        this.mRTCapability = RTCapability;
    }

    public RTP2P getRTP2P() {
        return mRTP2P;
    }

    public void setRTP2P(RTP2P mRTP2P) {
        this.mRTP2P = mRTP2P;
    }

    public RTNetInfo getRTNetInfo() {
        return mRTNetInfo;
    }

    public void setRTNetInfo(RTNetInfo mRTNetInfo) {
        this.mRTNetInfo = mRTNetInfo;
    }
}
