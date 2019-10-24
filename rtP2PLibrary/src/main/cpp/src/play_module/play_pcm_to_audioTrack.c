//
// Created by yinjian on 2018/11/23/023.
//

#include <stdint.h>
#include <log.h>
#include <libswresample/swresample.h>
#include <stdbool.h>
#include "play_pcm_to_audioTrack.h"
#include "NativeCallBack.h"

//音频播放
typedef struct _PCM_TO_AUDIOTRACK_HANDLE_ {
    struct SwrContext *swr_ctx;//重采样上下文

    uint64_t ch_layout;//输出的声道布局
    enum AVSampleFormat sample_fmt;//输出的采样格式 16bit PCM
    int sample_rate;//输出的采样率
    int nb_samples;//每个通道的采样个数
    int channel_nb;//通道数

    uint8_t *audio_buffer;
} PCM_TO_AUDIOTRACK_HANDLE;

int ffmpeg_pcm_swr_open(long *handle,
                          uint64_t ch_layout, enum AVSampleFormat sample_fmt,int sample_rate, int nb_samples,
                          uint64_t in_ch_layout,enum AVSampleFormat in_sample_fmt, int in_sample_rate,int in_nb_samples) {
    LOGI("初始化重采样上下文");
    int ret = -1;
    //重采样设置选项-----------------------------------------------------------start
    //frame.16bit 44100 PCM 统一音频采样格式与采样率
    struct SwrContext *swr_ctx = swr_alloc();//开辟一块内存空间
    swr_ctx = swr_alloc_set_opts(swr_ctx,
                                 ch_layout,sample_fmt,sample_rate,
                                 in_ch_layout,in_sample_fmt,in_sample_rate,
                                 0, NULL);
    ret = swr_init(swr_ctx);
    if (ret < 0) {
        swr_ctx = NULL;
        LOGE("swr_init(swrCtx) Error ");
        return -1;
    }
    //获取输出的声道个数
    int channel_nb = av_get_channel_layout_nb_channels(ch_layout);
    LOGI("out_channel_nb  ret:%d", channel_nb);
    //创建buffer
    int buffer_size = av_samples_get_buffer_size(NULL, channel_nb,nb_samples,sample_fmt,1);
    LOGI("av_samples_get_buffer_size  buffer_size:%d", buffer_size);
    uint8_t *audio_buffer = (uint8_t *) av_malloc((size_t) (buffer_size));

    PCM_TO_AUDIOTRACK_HANDLE *pHandle = (PCM_TO_AUDIOTRACK_HANDLE *) malloc(sizeof(PCM_TO_AUDIOTRACK_HANDLE));
    if (!pHandle) {
        LOGE("AUDIO_DECODER_HANDLE malloc error\n");
        return -1;
    }

    memset(pHandle, 0, sizeof(PCM_TO_AUDIOTRACK_HANDLE));

    pHandle->swr_ctx = swr_ctx;//重采样上下文
    pHandle->ch_layout = ch_layout;//输出的声道布局
    pHandle->sample_fmt = sample_fmt;//输出的采样格式 16bit PCM
    pHandle->sample_rate = sample_rate;//输出的采样率
    pHandle->nb_samples = nb_samples;//每个通道的采样个数
    pHandle->channel_nb = channel_nb;//通道数

    pHandle->audio_buffer = audio_buffer;

    *handle = (long) pHandle;
    return 0;
}

int ffmpeg_pcm_swr_put(long handle, char *data, int nb_samples) {
    PCM_TO_AUDIOTRACK_HANDLE *pHandle = (PCM_TO_AUDIOTRACK_HANDLE *) handle;
    if (pHandle == NULL) {
        LOGE("PCM_SWR_HANDLE *pHandle is NULL");
        return -1;
    }

    int ret = swr_convert(pHandle->swr_ctx,
                          &pHandle->audio_buffer, pHandle->nb_samples,
                          (const uint8_t **) data, nb_samples);
    if (ret < 0) {
        LOGE("swr_convert %d", ret);
        return -1;
    }
    int size = av_samples_get_buffer_size(NULL, pHandle->channel_nb, ret,
                                          pHandle->sample_fmt, 1);
    LOGE("av_samples_get_buffer_size %d", size);
    rtPcm2AudioTrack(pHandle->audio_buffer, size);//发给AudioTrack

    //如果还有缓存在swrcontext中,第二个参数要填写0才能获取到，缓存数据
    while (true) {
        if (swr_get_out_samples(pHandle->swr_ctx, 0) >= pHandle->nb_samples) {
            ret = swr_convert(pHandle->swr_ctx,
                              &pHandle->audio_buffer, pHandle->nb_samples,
                              NULL, 0);

            size = av_samples_get_buffer_size(NULL, pHandle->channel_nb,
                                              ret,
                                              pHandle->sample_fmt, 1);

            rtPcm2AudioTrack(pHandle->audio_buffer, size);//发给AudioTrack

            LOGE("av_samples_get_buffer_size %d", size);
        } else {
            break;
        }
    }

    return 0;
}

int ffmpeg_pcm_swr_close(long handle) {
    PCM_TO_AUDIOTRACK_HANDLE *pHandle = (PCM_TO_AUDIOTRACK_HANDLE *) handle;
    if (pHandle == NULL) {
        return -1;
    }
    if (pHandle->swr_ctx!=NULL){
        swr_free(&pHandle->swr_ctx);
    }

    free(pHandle->audio_buffer);
    free(pHandle);

    return 0;
}