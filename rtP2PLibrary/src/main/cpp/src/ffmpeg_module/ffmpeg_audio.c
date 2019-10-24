//
// Created by yinjian on 2019/4/8.
//

#include <stdint.h>
#include <libavutil/frame.h>
#include <log.h>
#include <libavcodec/avcodec.h>
#include <stdbool.h>
#include "ffmpeg_audio.h"
#include "ffmpeg_audio_decoder.h"
#include "ffmpeg_parser.h"
#include "ffmpeg_pcm_swr.h"

typedef struct _PCM_HANDLE_ {
    // int audioType;//支持的音频类型0:G711a 1:AAC

    int64_t out_ch_layout;//输出的声道布局
    enum AVSampleFormat out_sample_fmt;//输出的采样格式 16bit PCM
    int out_sample_rate;//输出的采样率
    int out_channel_nb;//通道数
    int out_nb_samples;//通道采样数

    long decoder_handle;
    long swr_handle;

    uint8_t *swr_buffer;//重采样之后的数据储存
    uint8_t *pcm_buffer;//重采样之后的数据储存
    int pcm_buffer_size;

    AVFrame *pcm_frame;//解码之后的frame

} PCM_HANDLE;

int ffmpeg_audio_open(long *handle, int64_t out_ch_layout, enum AVSampleFormat out_sample_fmt,
                      int out_sample_rate) {
    PCM_HANDLE *pHandle = (PCM_HANDLE *) malloc(sizeof(PCM_HANDLE));
    if (!pHandle) {
        LOGE("AUDIO_DECODER_HANDLE malloc error\n");
        return -1;
    }
    memset(pHandle, 0, sizeof(PCM_HANDLE));
    pHandle->out_ch_layout = out_ch_layout;//输出的声道布局
    pHandle->out_sample_fmt = out_sample_fmt;//输出的采样格式 16bit PCM
    pHandle->out_sample_rate = out_sample_rate;//输出的采样率
    pHandle->pcm_frame = av_frame_alloc();

    pHandle->swr_buffer = NULL;
    pHandle->pcm_buffer = NULL;

    pHandle->swr_handle = 0;
    pHandle->decoder_handle=0;
    *handle = (long) pHandle;
    return 0;
}

/**
 * uid
 * data     pcm数据
 * data_len  pcm数据大小
 * channel_layout= AV_CH_LAYOUT_MONO;//输入的声道布局
 * format=AV_SAMPLE_FMT_S16;    //输入的采样格式
 * sample_rate= 8000;//输入的采样率
 * timestamp
 * **/
int pcm_player(PCM_HANDLE *pHandle, AVFrame *frame) {
    if (pHandle->swr_handle == 0) {//创建重采样参数
        if (pcm_swr_open(&pHandle->swr_handle, frame->channel_layout, frame->format,
                         frame->sample_rate) != 0) {
            LOGE("[%s %d] pcm_swr_open is error", __FUNCTION__, __LINE__);
            return -1;
        }
    }
    int ret = pcm_swr_put(pHandle->swr_handle, frame);//获取重采样之后的数据
    return ret;
}

int aac_player(PCM_HANDLE *pHandle, uint8_t *data, int data_len, int timestamp) {
    int ret;
    if (pHandle->decoder_handle == 0) {
        if (ffmpeg_audio_decoder_open(&(pHandle->decoder_handle), 16000, 2, AV_CODEC_ID_AAC) != 0) {
            pHandle->decoder_handle = 0;
            LOGE("[%s   %d] ffmpeg_audio_decoder_open error", __FUNCTION__, __LINE__);
            return -1;
        }
    }
    if (pHandle->pcm_frame == NULL) {
        return -1;
    }
    av_frame_unref(pHandle->pcm_frame);

    ffmpeg_audio_decoder_put(pHandle->decoder_handle, data, data_len);
    ffmpeg_audio_decoder_get(pHandle->decoder_handle, pHandle->pcm_frame);
    pcm_player(pHandle, pHandle->pcm_frame);//音频帧率重采样之后发给播放器
    return 0;
}

int ffmpeg_audio_put(long handle, uint8_t *data, int data_len, int audioType) {
    PCM_HANDLE *pHandle = (PCM_HANDLE *) handle;
    if (pHandle == NULL) {
        return -1;
    }
    if (pHandle->pcm_frame == NULL) {
        return -1;
    }
    if (data == NULL) {
        return -1;
    }int ret=-1;
    if (audioType == 0) {//0:G711a
        //LOGI("audio_play_handle->audioType is pcm");
        pHandle->pcm_frame->data[0] = data;
        pHandle->pcm_frame->linesize[0] = data_len;
        pHandle->pcm_frame->nb_samples = data_len / 2;
        pHandle->pcm_frame->channel_layout = AV_CH_LAYOUT_MONO;//输入的声道布局
        pHandle->pcm_frame->format = AV_SAMPLE_FMT_S16;    //输入的采样格式
        pHandle->pcm_frame->sample_rate = 8000;   //输入的采样率

        ret=pcm_player(pHandle, pHandle->pcm_frame);
        if ( ret< 1) { return -1; }
    } else if (audioType == 1) {//1:AAC
        //if (aac_player(pHandle, data,data_len,0) != 0) { return -1; }
    }
    return ret;
}

int ffmpeg_audio_get(long handle, uint8_t *data, int *data_len) {
    PCM_HANDLE *pHandle = (PCM_HANDLE *) handle;
    if (pHandle == NULL) {
        return -1;
    }
    return pcm_swr_get(pHandle->swr_handle,data,data_len);
}

int ffmpeg_audio_close(long handle) {
    PCM_HANDLE *pHandle = (PCM_HANDLE *) handle;
    if (pHandle == NULL) {
        return -1;
    }
    if (pHandle->decoder_handle != 0) {
        ffmpeg_audio_decoder_close(pHandle->decoder_handle);
    }

    if (pHandle->swr_handle != 0) {
        pcm_swr_close(pHandle->swr_handle);
    }
    if (pHandle->swr_buffer != NULL) {
        av_freep(pHandle->swr_buffer);
        pHandle->swr_buffer = NULL;
    }
    if (pHandle->pcm_frame != NULL) {
        av_frame_free(&pHandle->pcm_frame);
        pHandle->pcm_frame = NULL;
    }

    av_freep(pHandle);
    pHandle = NULL;
    return 0;
}