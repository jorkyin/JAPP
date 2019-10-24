//
// Created by yinjian on 2018/12/26/026.
//

#include <stdint.h>
#include <log.h>
#include <libswresample/swresample.h>
#include <stdbool.h>
#include "ffmpeg_pcm_swr.h"

//音频播放
typedef struct _PCM_SWR_HANDLE_ {
    struct SwrContext *swr_ctx;//重采样上下文
    enum AVSampleFormat out_sample_fmt;//输出的采样格式 16bit PCM
    int out_channel_nb;//通道数
    int out_nb_samples;//通道采样数
    uint64_t out_ch_layout;//输出的声道布局
    int out_sample_rate;//输出的采样率

    uint8_t *pcm_buffer;//重采样之后的数据储存
    int pcm_buffer_size;
    uint8_t *swr_buffer;//重采样之后的数据
    int swr_buffer_size;
} PCM_SWR_HANDLE;

int pcm_swr_open(long *handle,
                 uint64_t in_ch_layout, int in_sample_fmt,
                 int in_sample_rate) {
    //输出的采样格式
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;//输出的采样格式 16bit PCM
    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;//输出的声道布局
    int out_sample_rate = 44100;//输出的采样率
    int out_nb_samples = 1024;

    LOGI("[%s %d] 初始化重采样上下文", __FUNCTION__, __LINE__);
    LOGI("[%s %d] in_ch_layout=%llu", __FUNCTION__, __LINE__, in_ch_layout);
    LOGI("[%s %d] in_sample_fmt=%d", __FUNCTION__, __LINE__, in_sample_fmt);
    LOGI("[%s %d] in_sample_rate=%d", __FUNCTION__, __LINE__, in_sample_rate);

    int ret = -1;
    //重采样设置选项-----------------------------------------------------------start
    //frame.16bit 44100 PCM 统一音频采样格式与采样率
    struct SwrContext *swr_ctx = swr_alloc();//开辟一块内存空间
    swr_ctx = swr_alloc_set_opts(swr_ctx,
                                 out_ch_layout, out_sample_fmt, out_sample_rate,
                                 in_ch_layout, (enum AVSampleFormat) in_sample_fmt, in_sample_rate,
                                 0, NULL);
    LOGE("[%s %d] 0", __FUNCTION__, __LINE__);
    ret = swr_init(swr_ctx);
    if (ret < 0) {
        swr_ctx = NULL;
        LOGE("[%s %d] swr_init(swrCtx) Error ", __FUNCTION__, __LINE__);
        return -1;
    }
    LOGE("[%s %d] 1", __FUNCTION__, __LINE__);
    PCM_SWR_HANDLE *pHandle = (PCM_SWR_HANDLE *) malloc(sizeof(PCM_SWR_HANDLE));
    if (!pHandle) {
        LOGE("[%s %d] AUDIO_DECODER_HANDLE malloc error ", __FUNCTION__, __LINE__);
        return -1;
    }
    LOGE("[%s %d] 2", __FUNCTION__, __LINE__);
    memset(pHandle, 0, sizeof(PCM_SWR_HANDLE));
    LOGE("[%s %d] 1", __FUNCTION__, __LINE__);
    pHandle->swr_ctx = swr_ctx;//重采样上下文

    pHandle->out_sample_fmt = out_sample_fmt;//输出的采样格式 16bit PCM
    pHandle->out_channel_nb = av_get_channel_layout_nb_channels(out_ch_layout);//通道数
    pHandle->out_nb_samples=out_nb_samples;
    pHandle->pcm_buffer = (uint8_t *) av_malloc((size_t) 44100 * 2 * 2);
    pHandle->swr_buffer_size=av_samples_get_buffer_size(NULL,
                               pHandle->out_channel_nb,
                               pHandle->out_nb_samples,
                               pHandle->out_sample_fmt,
                               1) * 2;
    LOGI("[%s %d] swr_buffer_size %d", __FUNCTION__, __LINE__,pHandle->swr_buffer_size);
    pHandle->swr_buffer = (uint8_t *) av_malloc((size_t) pHandle->swr_buffer_size);
    *handle = (long) pHandle;
    LOGI("[%s %d] OK", __FUNCTION__, __LINE__);
    return 0;
}

int pcm_swr_put(long handle, AVFrame *frame) {
    PCM_SWR_HANDLE *pHandle = (PCM_SWR_HANDLE *) handle;
    if (pHandle == NULL) {
        LOGE("[%s %d] PCM_SWR_HANDLE *pHandle is NULL", __FUNCTION__, __LINE__);
        return -1;
    }
    pHandle->pcm_buffer_size =0;
    int   ret = swr_convert(pHandle->swr_ctx, &pHandle->swr_buffer, pHandle->out_nb_samples,
                            (const uint8_t **)frame->data, frame->nb_samples);
    //LOGI("[%s %d]  ret=%d", __FUNCTION__, __LINE__, ret);
    if (ret > 0) {
        memcpy(&pHandle->pcm_buffer[pHandle->pcm_buffer_size], pHandle->swr_buffer, (size_t) ret);
        pHandle->pcm_buffer_size = pHandle->pcm_buffer_size + ret;
       // LOGE("[%s %d] pcm_buffer_size=%d", __FUNCTION__, __LINE__,pHandle->pcm_buffer_size);

    }
    while (true) {
        ret = swr_get_out_samples(pHandle->swr_ctx, 0);
        if (ret < pHandle->out_nb_samples) {
            //LOGI("[%s %d]  nb_samples=%d", __FUNCTION__, __LINE__,ret);
            break;
        }
        ret = swr_convert(pHandle->swr_ctx, &pHandle->swr_buffer, pHandle->out_nb_samples,NULL, 0);
        if (ret > 0) {
            memcpy(&pHandle->pcm_buffer[pHandle->pcm_buffer_size], pHandle->swr_buffer, (size_t) ret);
            pHandle->pcm_buffer_size = pHandle->pcm_buffer_size + ret;
            //LOGE("[%s %d] pcm_buffer_size=%d", __FUNCTION__, __LINE__,pHandle->pcm_buffer_size);
        }
    }
    LOGE("[%s %d] pcm_buffer_size=%d", __FUNCTION__, __LINE__,pHandle->pcm_buffer_size);
    return pHandle->pcm_buffer_size;
}

int pcm_swr_get(long handle, uint8_t *out_pcm, int *out_pcm_size){
    PCM_SWR_HANDLE *pHandle = (PCM_SWR_HANDLE *) handle;
    if (pHandle == NULL) {
        return -1;
    }
    memcpy(out_pcm, pHandle->pcm_buffer, (size_t) pHandle->pcm_buffer_size);
    *out_pcm_size = pHandle->pcm_buffer_size;
    return pHandle->pcm_buffer_size;
}

int pcm_swr_close(long handle) {
    PCM_SWR_HANDLE *pHandle = (PCM_SWR_HANDLE *) handle;
    if (pHandle == NULL) {
        return -1;
    }
    if (pHandle->swr_ctx != NULL) {
        swr_free(&pHandle->swr_ctx);
        pHandle->swr_ctx = NULL;
    }
    if (pHandle->pcm_buffer != NULL) {
        av_freep(pHandle->pcm_buffer);
        pHandle->pcm_buffer = NULL;
    }
    av_freep(pHandle);

    return 0;
}