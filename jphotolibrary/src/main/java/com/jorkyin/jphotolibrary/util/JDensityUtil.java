package com.jorkyin.jphotolibrary.util;

import android.content.Context;
import android.util.Log;

import java.util.HashMap;

/**
 * 作用：单位转换工具
 * px和dp互相转换工具
 */
public class JDensityUtil {
    /**
     * 根据手机的分辨率从 dip 的单位 转成为 px(像素)
     */
    public static int dip2px(Context context, float dpValue) {
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int) (dpValue * scale + 0.5f);
    }

    /**
     * 根据手机的分辨率从 px(像素) 的单位 转成为 dp
     */
    public static int px2dip(Context context, float pxValue) {
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int) (pxValue / scale + 0.5f);
    }

    public static int getPlayTime(String mUri) {
        android.media.MediaMetadataRetriever mmr = new android.media.MediaMetadataRetriever();
        int duration = 0;
        try {
            if (mUri != null) {
                HashMap<String, String> headers = null;
                headers = new HashMap<String, String>();
                headers.put("User-Agent", "Mozilla/5.0 (Linux; U; Android 4.4.2; zh-CN; MW-KW-001 Build/JRO03C) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 UCBrowser/1.0.0.001 U4/0.8.0 Mobile Safari/533.1");
                mmr.setDataSource(mUri, headers);
            }

            String strDuration = mmr.extractMetadata(android.media.MediaMetadataRetriever.METADATA_KEY_DURATION);//时长(毫秒)
            // String width = mmr.extractMetadata(android.media.MediaMetadataRetriever.METADATA_KEY_VIDEO_WIDTH);//宽
            // String height = mmr.extractMetadata(android.media.MediaMetadataRetriever.METADATA_KEY_VIDEO_HEIGHT);//高
            duration = Integer.parseInt(strDuration) / 1000;

        } catch (Exception ex) {
            Log.e("TAG", "MediaMetadataRetriever exception " + ex);
        } finally {
            mmr.release();
        }
        return duration;
    }

    //时间戳转时间 单位s 返回格式00：00
    public static String timestamp2Time(int time) {
        int m = time / 60;
        int s = time % 60;
        String s_m;
        String s_s;
        if (m < 10) {
            s_m = "0" + m;
        } else {
            s_m = m + "";
        }
        if (s < 10) {
            s_s = "0" + s;
        } else {
            s_s = "" + s;
        }
        return s_m + ":" + s_s;
    }

}