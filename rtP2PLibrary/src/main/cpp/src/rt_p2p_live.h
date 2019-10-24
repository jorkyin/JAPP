//
// Created by yinjian on 2018/12/20/020.
//

#ifndef RTP2PDEMO_LIVE_VIDEO_H
#define RTP2PDEMO_LIVE_VIDEO_H


#include <android/native_window.h>
#include <rt_p2p_types.h>

int set_windows_size(int width, int height);
int set_native_window(ANativeWindow *native_window,int width, int height);
int videoPlayer(RT_APP_CHAR *uid,RT_APP_UCHAR *date, RT_APP_INT32 len, RT_APP_INT32 frame_type, RT_APP_UINT32 pts);
int start_video_play(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *UID,RT_APP_UPTR streamid);//开始视频直播
int switcht_video_play(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *UID, RT_APP_UPTR streamid);//开始视频直播
int stop_video_play(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *UID);//结束视频直播

int start_audio_play(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *UID, RT_APP_UCHAR audioType);//开始播放音频
int stop_audio_play(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *UID);//停止播放音频
int live_audio_player(RT_APP_CHAR *uid, RT_APP_UCHAR *data, RT_APP_INT32 data_len);
int playback_audio(RT_APP_CHAR *uid, RT_APP_UCHAR *data, RT_APP_INT32 size, RT_APP_INT32 frametype,
                   RT_APP_UINT32 timestamp);
int getPcmData(uint8_t *pcm_buffer,int *size);
int start_record_mp4(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *UID, RT_APP_UCHAR audioType,char * file_name);//开始录像
int stop_record_mp4(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *UID);//停止录像

int start_playback(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *UID,RT_APP_CHAR *url,int totalTime, RT_APP_INT32 play_back_type,RT_APP_LONG offset);//开始回放录像
int stop_playback(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *UID,RT_APP_INT32 play_back_type);//停止回放录像

int record_picture(char *PictureFilePath);
//int get_picture(char *PictureFilePath);//获取图片
#endif //RTP2PDEMO_LIVE_VIDEO_H
