package com.runtop.rtp2plibrary;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.SurfaceTexture;
import android.media.AudioTrack;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import com.runtop.rtp2plibrary.model.RTNativeCallBack;
import com.runtop.rtp2plibrary.model.RTNativeCaller;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

/**
 * 自定义播放器
 * Created by rhm on 2018/1/23.
 */

public class RTVideoView extends FrameLayout implements TextureView.SurfaceTextureListener {
    private static final String TAG = "RTP2PVideoView";

    private Context mContext;

    private RTVideoViewPresenter presenter;

    private boolean stateVideo = false;//视频状态
    private FrameLayout mContainer;//设置布局容器
    private SurfaceTexture mSurfaceTexture;
    public static final int STREAM_MAIN = 3; //主码流
    public static final int STREAM_SUB = 0;  //辅码流
    private int stateVideoType = STREAM_SUB;
    private View mVideoView;

    private boolean stateAudioMonitor = false;//音频监听状态
    //private AudioTrack mAudioTrack;
    private TextureView mTextureView;

    public RTVideoView(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        mContext = context;
        //注册EventBus事件
        EventBus.getDefault().register(this);
        presenter = new RTVideoViewPresenter();
        onCreate();
    }

    private void onCreate() {
        mVideoView = LayoutInflater.from(mContext).inflate(R.layout.rt_video_view, null);
        mTextureView = mVideoView.findViewById(R.id.texture_view);
        mTextureView.setSurfaceTextureListener(this);//设置

        mContainer = new FrameLayout(mContext);//创建容器布局
        mContainer.setBackgroundColor(Color.BLACK);//设置背景色为黑色
        LayoutParams params = new LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);//设置铺满全屏
        this.addView(mContainer, params);
        mContainer.addView(mVideoView, 0, params);
        mContainer.setKeepScreenOn(true);//设置屏幕常亮
    }

    public void play(String uid, int videoStream) {
        Log.i(TAG, "startVideoStream");
        RTNativeCaller.StartLiveStream(uid, videoStream);//开始点播视频流
    }

    public void play(String uid, String url, int totalTime, int playBackType, long offset) {
        Log.i(TAG, "startVideoStream");
        RTNativeCaller.StartPlayBack(uid, url, totalTime, playBackType, offset);//开始点播视频流
    }

    public void release(String uid) {
        if (stateVideo) {
            RTNativeCaller.StopLiveStream(uid);
            ; //停止视频流，销毁解码器
        }

        if (stateAudioMonitor) {
            stopAudioStream(uid);
        }
        //释放播放器
        mContainer.removeView(mVideoView);
        if (mSurfaceTexture != null) {
            mSurfaceTexture.release();
            mSurfaceTexture = null;
        }
        //取消注册EventBus事件
        EventBus.getDefault().unregister(this);
    }


    //切换视频直播流
    public void switchVideoStream(String uid, int streamType) {
        Log.i(TAG, "switchVideoStream:stateVideoType =" + stateVideoType + " streamType=" + streamType);
        if (stateVideoType != streamType) {
            RTNativeCaller.SwitchLiveStream(uid, streamType);//切换视频流
        }
    }

    //设置音频监听
    public void setAudio(String uid, int audioType) {
        if (!stateAudioMonitor) {
            statcAudioStream(uid, audioType);
        } else {
            stopAudioStream(uid);
        }
    }

    //开始音频流
    private void statcAudioStream(String uid, int audioType) {
        Log.i(TAG, "startAudioMonitor: ");
        /*if (mAudioTrack == null) {
            Log.i(TAG, "AudioTrack init: ");
            mAudioTrack = presenter.startAudioTrackplayer();
            mAudioTrack.play();
        }*/
        RTNativeCaller.StartLiveAudio(uid, audioType);
    }

    //结束音频流
    private void stopAudioStream(String uid) {
        Log.i(TAG, "stopAudioMonitor: ");
        RTNativeCaller.StopLiveAudio(uid);
        /*if (mAudioTrack != null) {
            Log.i(TAG, "mAudioTrack release: ");
            mAudioTrack.stop();
            mAudioTrack.release();
            mAudioTrack = null;
        }*/
    }

    //事件订阅者处理事件
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void videoStatusCallBack(Bundle bundle) {
        String callBackType = bundle.getString(RTNativeCallBack.CALLBACKTYPE);
        if (callBackType == null) {
            return;
        }
        switch (callBackType) {
            case RTNativeCallBack.CallBackType.CALL_BACK_TYPE_VIDEO_WINDOWS_SIZE://返回窗口大小
                int width = bundle.getInt("width");
                int height = bundle.getInt("height");
                if (mOnVideoSizeChangedListener != null) {
                    mOnVideoSizeChangedListener.onVideoSizeChanged(width, height);
                }
                break;
            case RTNativeCallBack.CallBackType.CALL_BACK_TYPE_Video_Stream_Type://返回正在点播的码流
                stateVideoType = bundle.getInt("videoStreamType");
                if (mOnVideoStateChangedListener != null) {
                    mOnVideoStateChangedListener.onVideoState(stateVideoType);
                }
                stateVideo = true;
                break;
            case RTNativeCallBack.CallBackType.CALL_BACK_TYPE_PCM_DATE://返回PCM音频数据用于AudioTrack播放
                byte[] audioData = bundle.getByteArray("audioData");
                int sizeInBytes = bundle.getInt("sizeInBytes");

              /*  if (mAudioTrack == null) {
                    Log.i(TAG, "AudioTrack is null: ");
                    break;
                }

                if (audioData != null) {
                    mAudioTrack.write(audioData, 0, sizeInBytes);
                }*/

                break;
            case RTNativeCallBack.CallBackType.CALL_BACK_TYPE_Audio_State://返回监听是声音是否开启
                int audioState = bundle.getInt("audioState");
                //C中 true 1  false 0
                stateAudioMonitor = audioState == 1;
                if (mOnVoiceStateChangedListener != null) {
                    mOnVoiceStateChangedListener.onVoiceStateChanged(stateAudioMonitor);
                }
                break;
            case RTNativeCallBack.CallBackType.CALL_BACK_TYPE_RTVideoView_State://返回视频准备开启
                int RTVideoViewState = bundle.getInt("RTVideoViewState");
                //C中 true 1  false 0
                if (mOnPreparedListener != null) {
                    mOnPreparedListener.onPrepared(RTVideoViewState == 1);
                }

                break;
            case RTNativeCallBack.CallBackType.CallBack_PlayBackSeek://返回正在回放的进度
                int player_time = bundle.getInt("player_time");
                int total_time = bundle.getInt("total_time");
                if (mOnPlayBackSeekChangedListener != null) {
                    mOnPlayBackSeekChangedListener.PlayBackSeekChanged(player_time, total_time);
                }
        }
    }

    /*********************
     * SurfaceTexture监听
     *********************/
    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        mSurfaceTexture = surface;
        //设置渲染
        RTNativeCaller.SetNativeWindow(new Surface(mSurfaceTexture), width, height);
    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
        Log.i(TAG, "onSurfaceTextureSizeChanged: width=" + width + " height" + height);
    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        return false;
    }

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
    }

    //截屏
    public Bitmap getBitmap(int w, int h) {
        return mTextureView.getBitmap(w, h);
    }

    //准备播放
    public interface OnPreparedListener {
        void onPrepared(boolean rtVideoViewState);
    }

    public void setOnPreparedListener(OnPreparedListener onPreparedListener) {
        mOnPreparedListener = onPreparedListener;
    }

    private OnPreparedListener mOnPreparedListener = null;

    //视频大小相关
    public interface OnVideoSizeChangedListener {
        void onVideoSizeChanged(int width, int height);
    }

    public void setOnVideoSizeChangedListener(OnVideoSizeChangedListener onVideoSizeChangedListener) {
        mOnVideoSizeChangedListener = onVideoSizeChangedListener;
    }

    private OnVideoSizeChangedListener mOnVideoSizeChangedListener = null;//视频大小相关

    //当前点播的视频流状态
    public interface OnVideoStateChangedListener {
        void onVideoState(int videoState);
    }

    public void setOnVideoStateChangedListener(OnVideoStateChangedListener onVideoStateChangedListener) {
        mOnVideoStateChangedListener = onVideoStateChangedListener;
    }

    private OnVideoStateChangedListener mOnVideoStateChangedListener = null;

    //回放进度
    public interface OnPlayBackSeekChangedListener {
        void PlayBackSeekChanged(int player_time,
                                 int total_time);
    }

    public void setOnPlayBackSeekChangedListenerr(OnPlayBackSeekChangedListener onPlayBackSeekChangedListener) {
        mOnPlayBackSeekChangedListener = onPlayBackSeekChangedListener;
    }

    private OnPlayBackSeekChangedListener mOnPlayBackSeekChangedListener = null;


    //当前音频开关状态
    public interface OnVoiceStateChangedListener {
        void onVoiceStateChanged(boolean voiceState);
    }

    public void setOnVoiceStateChangedListener(OnVoiceStateChangedListener onVoiceStateChangedListener) {
        mOnVoiceStateChangedListener = onVoiceStateChangedListener;
    }

    private OnVoiceStateChangedListener mOnVoiceStateChangedListener = null;

}