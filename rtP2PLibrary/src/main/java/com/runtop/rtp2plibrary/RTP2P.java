package com.runtop.rtp2plibrary;

public class RTP2P {
    private String mUID = "";
    private String mUser = "";
    private String mPassword = "";

    public RTP2P() {}
    public RTP2P(String UID, String user, String password) {
        this.mUID = UID;
        this.mUser = user;
        this.mPassword = password;
    }

    public String getUID() {
        return mUID;
    }

    public void setUID(String UID) {
        this.mUID = UID;
    }

    public String getUser() {
        return mUser;
    }

    public void setUser(String user) {
        this.mUser = user;
    }

    public String getPassword() {
        return mPassword;
    }

    public void setPassword(String password) {
        this.mPassword = password;
    }

    public String toString() {
        return " uid = " + mUID + " mUser = " + mUser + " mPassword = " + mPassword;
    }
}
