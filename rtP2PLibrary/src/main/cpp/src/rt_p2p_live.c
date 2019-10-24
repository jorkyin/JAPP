//
// Created by yinjian on 2018/12/20/020.
//

#include "rt_p2p_live.h"
#include "NativeCallBack.h"
#include "packet_queue.h"

#include <stdbool.h>
#include <stdio.h>

#include <rt_object_api.h>
#include <record_picture.h>

#include <ffmpeg_pcm_swr.h>
#include <ffmpeg_audio_decoder.h>
#include <ffmpeg_parser.h>
#include <ffmpeg_video_decoder.h>
#include <ffmpeg_video_sws.h>
#include <play_rgba_to_nativeWindow.h>
#include <libavformat/avformat.h>
#include <pthread.h>
#include <play_pcm_to_opensl_io.h>
#include <ffmpeg_module/ffmpeg_audio.h>


bool is_video_stream_receive = false; //true:正在接收视频流 flase:停止接收视频流
bool is_audio_stream_receive = false; //true:正在接收音频流 flase:停止音频流
bool is_video_player = false; //true:正在播放视频 flase:停止播放视频
bool is_voide_record = false;//true:正在录像  false:停止录像
bool is_audio_palyer = false;//true:正在监听  false:停止监听
bool is_picture_get = false;//true:正在截图  false:停止截图

FILE *record_video_file = NULL;//录像文件
char *record_picture_path = NULL;//获取图片

//视频播放
typedef struct _PLAY_VIDEO_HANDLE_ {
    ANativeWindow *native_window;
    int totalTime;//视频总时长
    int video_width;
    int video_height;
    int window_width;
    int window_height;
    int video_stream_id; //正在点播的码流
    long decoder_handle;
    long sws_handle;
    long nativeWindow_handle;
} PLAY_VIDEO_HANDLE;

PLAY_VIDEO_HANDLE *v_handle;

int videoPlayer_close() {
    if (v_handle->decoder_handle != 0) {
        ffmpeg_video_decoder_close(v_handle->decoder_handle);//销毁视频变量
    }
    if (v_handle->sws_handle != 0) {
        ffmpeg_video_frame_sws_close(v_handle->sws_handle);
    }
    if (v_handle->nativeWindow_handle != 0) {
        native_window_video_close(v_handle->nativeWindow_handle);
    }
    ANativeWindow_release(v_handle->native_window);
    free(v_handle);
    v_handle = NULL;
    return 0;
}

int set_windows_size(int width, int height) {
    if (v_handle == NULL) {
        LOGE("[%s   %d]video_play_handle is NULL error", __FUNCTION__, __LINE__);
        return -1;
    }

    v_handle->window_width = width;
    v_handle->window_height = height;
    // LOGI("width=%d height=%d",video_play_handle->window_width,video_play_handle->window_height);
    return 0;
}

int set_native_window(ANativeWindow *native_window, int width, int height) {
    if (v_handle == NULL) {
        LOGE("[%s   %d]video_play_handle is null", __FUNCTION__, __LINE__);
        v_handle = (PLAY_VIDEO_HANDLE *) malloc(sizeof(PLAY_VIDEO_HANDLE));
        if (!v_handle) {
            LOGE("[%s   %d] set_native_window malloc error", __FUNCTION__, __LINE__);
            return -1;
        }
        memset(v_handle, 0, sizeof(PLAY_VIDEO_HANDLE));
        v_handle->video_width = 0;
        v_handle->video_height = 0;
        v_handle->video_stream_id = 0;
        v_handle->decoder_handle = 0;
        v_handle->sws_handle = 0;
        v_handle->nativeWindow_handle = 0;
    }
    v_handle->native_window = native_window;
    v_handle->window_width = width;
    v_handle->window_height = height;

    rtVideoViewState(true);
    // LOGI("width=%d height=%d",video_play_handle->window_width,video_play_handle->window_height);
    return 0;
}

//开始直播视频流
int start_video_play(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *UID, RT_APP_UPTR streamid) {
    LOGI("UID=%s streamid=%d ", UID, streamid);
    if (v_handle == NULL) {
        LOGE("[%s   %d]video_play_handle is null", __FUNCTION__, __LINE__);
        return -1;
    }
    v_handle->video_stream_id = streamid;
    is_video_player = true;
    if (u32AppHandle != 0) {
        //开始播放视频流
        RT_APP_API_START_LIVE_Stream(u32AppHandle, UID, v_handle->video_stream_id);
    }
    //返回正在点播的码流
    rtVideoStreamType(v_handle->video_stream_id);
    return 0;
}

//选择直播视频流
int switcht_video_play(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *UID, RT_APP_UPTR streamid) {
    LOGI("UID=%s streamid=%d ", UID, streamid);
    if (v_handle == NULL) {
        return -1;
    }
    if (v_handle->video_stream_id == streamid) {
        return -1;
    }
    v_handle->video_stream_id = streamid;
    if (is_video_player) {
        if (u32AppHandle != 0) {
            //选择播放视频流
            RT_APP_API_SWITCH_Stream(u32AppHandle, (RT_APP_CHAR *) UID,
                                     v_handle->video_stream_id);
        }
    }
    rtVideoStreamType(v_handle->video_stream_id);
    LOGI("UID=%s ok ", UID);
    return 0;
}

//结束直播视频流
int stop_video_play(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *UID) {
    LOGI("UID=%s ", UID);
    if (v_handle == NULL || !is_video_player) {
        is_video_player = false;
        return -1;
    }

    is_video_player = false;

    if (u32AppHandle != 0) {
        //停止播放视频流
        RT_APP_API_STOP_LIVE_Stream(u32AppHandle, UID);
    }

    videoPlayer_close();

    LOGI("UID=%s ok", UID);
    return 0;
}

//将返回的视频数据返回给播放器
int videoPlayer(RT_APP_CHAR *uid, RT_APP_UCHAR *date, RT_APP_INT32 len, RT_APP_INT32 frame_type,
                RT_APP_UINT32 pts) {
    // LOGI("len=%d frame_type=%d pts=%d",len,frame_type,pts);
    if (v_handle == NULL) {
        return -1;
    }
    int ret = -1;
    // LOGI("  len = %d frame_type=%d pts=%d",len,frame_type,pts);
    if (!is_video_player) {
        LOGE("is_video_player   error");
        return -1;
    }
/***********************************视频解码*************************************************/
    if (v_handle->decoder_handle == 0) {//视频解码器是否初始化
        if (ffmpeg_video_decoder_open(&(v_handle->decoder_handle), AV_CODEC_ID_H264) != 0) {
            LOGE("[%s %d] ffmpeg_video_decoder_open   error ", __FUNCTION__,
                 __LINE__);
            v_handle->decoder_handle = 0;
            return -1;
        }
    }
    if (frame_type == 0) {//如果是I帧
        int codec_id = get_frame_codec_id(date);//获取frame的解码器id
        if (codec_id < 0) {
            return -1;
        } else {
            if (codec_id != get_codec_id(v_handle->decoder_handle)) {//视频解码器id是否和frame中的id相同
                ffmpeg_video_decoder_close(v_handle->decoder_handle);
                if (ffmpeg_video_decoder_open(&(v_handle->decoder_handle), codec_id) != 0) {
                    LOGE("[%s %d] ffmpeg_video_decoder_open   error codec_id=%d", __FUNCTION__,
                         __LINE__, codec_id);
                    v_handle->decoder_handle = 0;
                    return -1;
                }
            }
        }
    }
    AVFrame frame;
    ret = ffmpeg_video_decoder(v_handle->decoder_handle, (char *) date, len, &frame);
    if (ret != 0 || frame.width < 1 || frame.height < 1) {
        //LOGE("ffmpeg_video_decoder   error");
        return -1;
    }
   // LOGI("width=%d height=%d",frame.width,frame.height);
/*******************************************************************************************/

    if (is_picture_get == true && record_picture_path != NULL) {
        is_picture_get = false;
        if (write_yuv_to_jpeg(uid, &frame, record_picture_path) != 0) {
            LOGE("write_yuv_to_jpeg error");
        }
    }

    /* LOGI("video_width=%d video_height=%d window_width=%d window_height=%d",video_play_handle->video_width,
          video_play_handle->video_height,
          video_play_handle->window_width,
          video_play_handle->window_height);*/

/***********************************视频转码*************************************************/
    if (v_handle->video_width != frame.width ||
        v_handle->video_height != frame.height) {
        v_handle->video_width = frame.width;
        v_handle->video_height = frame.height;
        v_handle->window_height =
                v_handle->window_width * frame.height / frame.width;

        rtVideoSize(v_handle->window_width, v_handle->window_height);

        if (ffmpeg_video_frame_sws_open(&(v_handle->sws_handle),
                                        (enum AVPixelFormat) frame.format,
                                        v_handle->video_width,
                                        v_handle->video_height,
                                        v_handle->window_width,
                                        v_handle->window_height) != 0) {
            LOGE("ffmpeg_video_frame_sws_open   error");
            return -1;
        }
    }

    AVFrame RGBA_frame;
    if (ffmpeg_video_frame_sws(v_handle->sws_handle, frame, &RGBA_frame) != 0) {
        LOGE("ffmpeg_video_frame_sws   error");
        return -1;
    }
/*******************************************************************************************/

/***********************************视频显示*************************************************/
    if (v_handle->nativeWindow_handle == 0) {
        if (native_window_video_open(&v_handle->nativeWindow_handle,
                                     v_handle->native_window,
                                     v_handle->window_width,
                                     v_handle->window_height) != 0) {
            LOGE("native_window_video_open   error");
            return -1;
        }
    }

    if (native_window_video_show(v_handle->nativeWindow_handle, &RGBA_frame) != 0) {
        LOGE("native_window_video_show   error");
    }
    rtPlayBackSeek(pts, v_handle->totalTime * 1000);
/*******************************************************************************************/
    return 0;
}

//音频解码器 G711A的解码ID为AV_CODEC_ID_PCM_ALAW
//a_audio_decoder.codec = avcodec_find_decoder(AV_CODEC_ID_AAC);

//音频播放
typedef struct _PLAY_AUDIO_HANDLE_ {
    int audioType;//支持的音频类型0:G711a 7:AAC

    long queue_handle;
    long SLES_handle;
    long pcm_handle;

    uint8_t *src_audio_buffer;//从队列中取的音频数据

    pthread_mutex_t mutex;//互斥锁
    pthread_cond_t has_product;//条件变量 判断是否有产品

    bool is_opensles_state;//true:可用  false:不可用

} PLAY_AUDIO_HANDLE;

//音频播放
PLAY_AUDIO_HANDLE *a_handle;


//开始播放音频
int start_audio_play(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *UID, RT_APP_UCHAR audioType) {

    LOGE("[%s %d]UID=%s audioType=%d", __FUNCTION__, __LINE__, UID, audioType);
    if (is_audio_palyer) {
        return -1;
    }

    if (a_handle == NULL) {
        a_handle = (PLAY_AUDIO_HANDLE *) malloc(sizeof(PLAY_AUDIO_HANDLE));
        if (!a_handle) {
            LOGE("[%s %d] set_windows_size malloc error", __FUNCTION__, __LINE__);
            return -1;
        }
        memset(a_handle, 0, sizeof(PLAY_AUDIO_HANDLE));
        a_handle->queue_handle = 0;
        a_handle->pcm_handle = 0;
        a_handle->SLES_handle=0;
        a_handle->is_opensles_state = false;
    }
    pthread_cond_init(&a_handle->has_product, NULL);//条件变量初始化
    if (queue_open(&a_handle->queue_handle, &a_handle->mutex) != 0) {//初始化队列
        LOGE("[%s %d] queue_open is error", __FUNCTION__, __LINE__);
        a_handle->queue_handle = 0;
        return -1;
    }
    //初始化音频 解码-》重采样
    if (ffmpeg_audio_open(&a_handle->pcm_handle, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, 44100) !=
        0) {
        LOGE("[%s %d] ffmpeg_audio_open is error", __FUNCTION__, __LINE__);
        return -1;
    }

    a_handle->audioType = audioType;

    if (!is_voide_record) {
        if (u32AppHandle != 0) {
            RT_APP_API_START_Audio(u32AppHandle, UID, (RT_APP_UCHAR) a_handle->audioType);
        }
    }

    is_audio_palyer = true;
    rtAudioState(is_audio_palyer);//放回监听状态

    LOGI("ok");
    return 0;
}

int audioPlayer_close() {
    if (!is_audio_palyer || a_handle == NULL) {
        return -1;
    }
    if (a_handle->queue_handle != 0) {
        queue_close(a_handle->queue_handle, &a_handle->mutex);
    }
    if (a_handle->SLES_handle != 0) {
        opensl_pcm_close(a_handle->SLES_handle);
    }
    if (a_handle->pcm_handle != 0) {
        ffmpeg_audio_close(a_handle->pcm_handle);
    }

    return 0;
}

//停止播放音频
int stop_audio_play(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *UID) {

    if (!is_audio_palyer) { return -1; }
    if (!is_voide_record) {
        // destroy_audio_decode();//销毁音频参数
        if (u32AppHandle != 0) {
            LOGI("RT_APP_API_STOP_Audio UID=%s ", UID);
            RT_APP_API_STOP_Audio(u32AppHandle, UID);
        }
    }
    //realseResource();
    is_audio_palyer = false;
    rtAudioState(is_audio_palyer);
    audioPlayer_close();
    LOGI("ok ");
    return 0;
}


int playback_audio(RT_APP_CHAR *uid, RT_APP_UCHAR *data, RT_APP_INT32 size, RT_APP_INT32 frametype,
                   RT_APP_UINT32 timestamp) {
    if (!is_audio_palyer || a_handle == NULL || size < 1) {
        return -1;
    }

    while (true) {
        if (queue_put(a_handle->queue_handle, &a_handle->mutex, data, size, frametype, timestamp) ==
            0) { break; }
    }

    if (a_handle->SLES_handle==0){
        opensl_pcm_open(&a_handle->SLES_handle,44100,2);
    }
    return 0;
}

int live_audio_player(RT_APP_CHAR *uid, RT_APP_UCHAR *data, RT_APP_INT32 data_len) {
    if (!is_audio_palyer || a_handle == NULL || data_len < 1) {
        return -1;
    }

  /*  if (ffmpeg_audio_put(a_handle->pcm_handle, data, data_len, 0) != 0) {
        return -1;
    }
    return 0;*/


    while (true) {
        if (queue_put(a_handle->queue_handle, &a_handle->mutex, data, data_len,
                      a_handle->audioType, 0) == 0) { break; }
    }
    if (a_handle->SLES_handle==0){
        opensl_pcm_open(&a_handle->SLES_handle, 44100, 2);
    }
    return 0;
}

int getPcmData(uint8_t *pcm_buffer, int *size) {
    int type;
    int len;
    int timestamp;
int ret=-1;
    while (true) {
        if (queue_get(a_handle->queue_handle, &a_handle->mutex, &a_handle->src_audio_buffer, &len,
                      &type, &timestamp) > 0) {
            continue;
        }
        ret=ffmpeg_audio_put(a_handle->pcm_handle, a_handle->src_audio_buffer, len, type);
        LOGE("[%s %d] ffmpeg_audio_put len=%d -> size=%d", __FUNCTION__, __LINE__,len,ret);
        if (ret < 1) {
            continue;
        }

        if (ffmpeg_audio_get(a_handle->pcm_handle, pcm_buffer, size) > 0) {
            LOGE("[%s %d] ffmpeg_audio_get size=%d", __FUNCTION__, __LINE__,*size);
            //音频帧率重采样之后发给播放器
             //rtPcm2AudioTrack(pHandle->swr_buffer, ret);//发给AudioTrack
            break;
        }
    }

    return 0;
}


//开始回放录像
int start_playback(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *uid, RT_APP_CHAR *url, int totalTime,
                   RT_APP_INT32 play_back_type, RT_APP_LONG offset) {
    LOGE("[%s   %d]start_playback uid=%s url=%s play_back_type=%d ", __FUNCTION__, __LINE__, uid,
         url, play_back_type);
    if (v_handle == NULL) {
        LOGE("[%s   %d]video_play_handle is null", __FUNCTION__, __LINE__);
        return -1;
    }
    v_handle->totalTime = totalTime;
    is_video_player = true;
    is_audio_palyer = true;
    if (u32AppHandle != 0) {
        LOGE("[%s   %d]RT_APP_API_START_PlayBack uid=%s url=%s play_back_type=%d ", __FUNCTION__,
             __LINE__, uid, url, play_back_type);
        //开始播放视频流
        RT_APP_API_START_PlayBack(u32AppHandle, uid, url, 0);
    }
    return 0;
}

//停止回放录像
int stop_playback(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *uid, RT_APP_INT32 play_back_type) {
    if (u32AppHandle != 0) {
        RT_APP_API_STOP_PlayBack(u32AppHandle, uid);
    }
    return 0;
}

//拍照
int record_picture(char *PictureFilePath) {
    LOGI("record_picture  PictureFilePath=%s", PictureFilePath);
    if (is_picture_get) {
        return -1;
    }
    if (PictureFilePath == NULL) {
        LOGE("录像文件夹名字为空");
        return -1;
    }

    record_picture_path = PictureFilePath;
    is_picture_get = true;

    return 0;
}

//开始录像
int start_record_mp4(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *UID, RT_APP_UCHAR audioType,
                     char *file_name) {
    return 0;
}

//停止录像
int stop_record_mp4(RT_APP_UPTR u32AppHandle, RT_APP_CHAR *UID) { return 0; }
