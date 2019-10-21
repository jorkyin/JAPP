package com.jorkyin.jphotolibrary.base;

public class JPhoto {
    public static final int PHOTO_TYPE_PHOTO = 0;//图片
    public static final int PHOTO_TYPE_VIDEO = 1;//视频

    private String mNmae;
    private String mSrc;//路径
    private boolean isSelect=false;
    private boolean isVisibility=false;
    private String mTitleName;

    public JPhoto(String path) {
        this.mSrc = path;
        this.mNmae = this.mSrc.substring(this.mSrc.lastIndexOf(".") + 1).toLowerCase();;

    }

    public String getFilenNmae() {
        return mNmae;
    }

    public void setFilenNmae(String filenNmae) {
        this.mNmae = filenNmae;
    }

    public String getFilePath() {
        return mSrc;
    }

    public void setFilePath(String filePath) {
        this.mSrc = filePath;
    }

    public boolean isSelect() {
        return isSelect;
    }

    public void setSelect(boolean select) {
        isSelect = select;
    }

    public boolean isVisibility() {
        return isVisibility;
    }

    public void setVisibility(boolean visibility) {
        isVisibility = visibility;
    }

    public String getTitleName() {
        return mTitleName;
    }

    public void setTitleName(String titleName) {
        this.mTitleName = titleName;
    }

    public int getFileType() {
        // 获取扩展名
        String FileEnd = mNmae.substring(mNmae.lastIndexOf(".") + 1).toLowerCase();
        if (FileEnd.equals("jpg")) {
            return PHOTO_TYPE_PHOTO;
        } else if (FileEnd.equals("mp4") || FileEnd.equals("avi")) {
            return PHOTO_TYPE_VIDEO;
        }
        return -1;
    }
}

