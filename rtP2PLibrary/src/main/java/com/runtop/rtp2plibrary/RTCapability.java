package com.runtop.rtp2plibrary;

public class RTCapability {
    private String p2pDevVersion;
    private int audioformat;
    private int resolution;
    private int mainStreamFmt;
    private int subStreamFmt;
    private int mobStreamFmt;
    private int onlinenum;
    private int isshowall;

    public String getP2pDevVersion() {
        return p2pDevVersion;
    }

    public void setP2pDevVersion(String p2pDevVersion) {
        this.p2pDevVersion = p2pDevVersion;
    }

    public int getAudioformat() {
        return audioformat;
    }

    public void setAudioformat(int audioformat) {
        this.audioformat = audioformat;
    }

    public int getResolution() {
        return resolution;
    }

    public void setResolution(int resolution) {
        this.resolution = resolution;
    }

    public int getMainStreamFmt() {
        return mainStreamFmt;
    }

    public void setMainStreamFmt(int mainStreamFmt) {
        this.mainStreamFmt = mainStreamFmt;
    }

    public int getSubStreamFmt() {
        return subStreamFmt;
    }

    public void setSubStreamFmt(int subStreamFmt) {
        this.subStreamFmt = subStreamFmt;
    }

    public int getMobStreamFmt() {
        return mobStreamFmt;
    }

    public void setMobStreamFmt(int mobStreamFmt) {
        this.mobStreamFmt = mobStreamFmt;
    }

    public int getOnlinenum() {
        return onlinenum;
    }

    public void setOnlinenum(int onlinenum) {
        this.onlinenum = onlinenum;
    }

    public int getIsshowall() {
        return isshowall;
    }

    public void setIsshowall(int isshowall) {
        this.isshowall = isshowall;
    }


}
