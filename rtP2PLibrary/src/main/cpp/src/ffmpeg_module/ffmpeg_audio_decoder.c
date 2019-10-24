#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <log.h>
#include <libavformat/avformat.h>

#include "libavcodec/avcodec.h"

#include "ffmpeg_audio_decoder.h"
#include "ffmpeg_parser.h"

typedef struct _AUDIO_DECODER_HANDLE_ {
    AVCodec *pAVCodec;
    AVCodecContext *pAVCodecCtx;
    int channels;
    int codecid;

    uint64_t channel_layout;
    int format;
    int sample_rate;
    int pcm_nb_samples;

    long parser_handle;
    AVPacket *pkt;
    AVFrame *frame;//aac解码之前的frame

    uint8_t *aac_buffer;//aac解码之前的pcm数据
    int aac_buffer_size;
    uint8_t *pcm_buffer;//aac解码之后的数据储存池
    int pcm_buffer_size;

} AUDIO_DECODER_HANDLE;

int ffmpeg_audio_decoder_open(long *handle, int sample_rate, int channels, int codecid) {
    AVCodec *pAVCodec = NULL;
    AVCodecContext *pAVCodecCtx = NULL;

    int ret = 0;

    av_register_all();
    avformat_network_init();
    avcodec_register_all();
#if 0
    if((enum AVCodecID)codecid== AV_CODEC_ID_AAC){
        pAVCodec = avcodec_find_decoder_by_name("libfdk_aac");
    }else{
        pAVCodec = avcodec_find_decoder((enum AVCodecID)codecid);
    }
#else
    pAVCodec = avcodec_find_decoder((enum AVCodecID) codecid);
#endif

    if (pAVCodec == NULL) {
        LOGE("[%s   %d]avcodec_find_decoder %s error",__FUNCTION__,__LINE__,avcodec_get_name((enum AVCodecID) codecid));
        return -1;
    }

    pAVCodecCtx = avcodec_alloc_context3(pAVCodec);
    if (pAVCodecCtx == NULL) {
        av_log(NULL, AV_LOG_ERROR, "avcodec_alloc_context3 error\n");
        LOGE("[%s   %d]avcodec_alloc_context3 error",__FUNCTION__,__LINE__);
        return -1;
    }

    pAVCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
    pAVCodecCtx->sample_rate = sample_rate;
    pAVCodecCtx->channels = channels;
    ret = avcodec_open2(pAVCodecCtx, pAVCodec, NULL);
    if (ret != 0) {
        LOGE("[%s   %d]avcodec_open2 error %s", __FUNCTION__, __LINE__, av_err2str(ret));
        return -1;
    }

    AUDIO_DECODER_HANDLE *pHandle = (AUDIO_DECODER_HANDLE *) malloc(sizeof(AUDIO_DECODER_HANDLE));
    if (!pHandle) {
        LOGE("[%s   %d]AUDIO_DECODER_HANDLE malloc error", __FUNCTION__, __LINE__);
        return -1;
    }
    memset(pHandle, 0, sizeof(AUDIO_DECODER_HANDLE));
    pHandle->channels = channels;
    pHandle->codecid = codecid;
    pHandle->pAVCodecCtx = pAVCodecCtx;
    pHandle->pAVCodec = pAVCodec;
    pHandle->frame = NULL;
    LOGE("[%s   %d] 1", __FUNCTION__, __LINE__);
    if (ffmpeg_parser_open(&pHandle->parser_handle, AV_CODEC_ID_AAC) != 0) {
        pHandle->parser_handle = 0;
        LOGE("[%s   %d] ffmpeg_parser_open error", __FUNCTION__, __LINE__);
        return -1;
    }
    LOGE("[%s   %d] 2", __FUNCTION__, __LINE__);
    pHandle->frame = av_frame_alloc();
    pHandle->aac_buffer_size = 1024;
    pHandle->aac_buffer = av_malloc((size_t) pHandle->aac_buffer_size);
    av_init_packet(pHandle->pkt);
    *handle = (long) pHandle;

    return 0;
}

int ffmpeg_audio_decoder_put(long handle, uint8_t *data, int data_len) {
    AUDIO_DECODER_HANDLE *pHandle = (AUDIO_DECODER_HANDLE *) handle;
    if (pHandle == NULL) {
        return -1;
    }
    if (pHandle->parser_handle == 0) {
        LOGE("[%s   %d] ffmpeg_audio_decoder_open error", __FUNCTION__, __LINE__);
        return -1;
    }
    if (pHandle->frame == NULL) {
        return -1;
    }
    if (pHandle->pkt==NULL){
        return -1;
    }

    int ret = 0;
    int len = -1;
    pHandle->pcm_buffer_size = 0;
    pHandle->pcm_nb_samples=0;
    while (data_len) {
        len = ffmpeg_parser_parse2(pHandle->parser_handle, &pHandle->aac_buffer, &pHandle->aac_buffer_size,
                                   data, data_len);
        data += len;
        data_len -= len;
        // LOGI("[%s %d]  len=%d", __FUNCTION__, __LINE__, len);
        if (len) {
            av_packet_unref(pHandle->pkt);
            pHandle->pkt->data = pHandle->aac_buffer;
            pHandle->pkt->size = len;
            pHandle->pkt->pts = AV_NOPTS_VALUE;
            pHandle->pkt->dts = AV_NOPTS_VALUE;

            av_frame_unref(pHandle->frame);
            ret = avcodec_send_packet(pHandle->pAVCodecCtx, pHandle->pkt);

            if (ret != 0) {
                LOGE("avcodec_send_packet error %s %d", av_err2str(ret), ret);
                return -1;
            }

            ret = avcodec_receive_frame(pHandle->pAVCodecCtx, pHandle->frame);
            if (ret != 0) {
                LOGE("avcodec_receive_frame error %s %d", av_err2str(ret), ret);
                return -1;
            }

            if (ret < 0) {
                //   LOGE("[%s   %d]ffmpeg_audio_decoder_put error", __FUNCTION__, __LINE__);
                continue;
            }
            memcpy(&pHandle->pcm_buffer[pHandle->pcm_buffer_size], pHandle->frame->data[0],
                   (size_t) pHandle->frame->linesize[0]);
            pHandle->pcm_buffer_size = pHandle->pcm_buffer_size + pHandle->frame->linesize[0];
            pHandle->pcm_nb_samples = pHandle->pcm_nb_samples + pHandle->frame->nb_samples;
        }
    }
    return pHandle->pcm_buffer_size;
}

int ffmpeg_audio_decoder_get(long handle, AVFrame *frame){
    AUDIO_DECODER_HANDLE *pHandle = (AUDIO_DECODER_HANDLE *) handle;
    if (pHandle == NULL) {
        return -1;
    }
    frame->channel_layout=pHandle->frame->channel_layout;
    frame->format=pHandle->frame->format;
    frame->sample_rate=pHandle->frame->sample_rate;
    frame->nb_samples=pHandle->pcm_nb_samples;
    frame->data[0]=pHandle->pcm_buffer;
    frame->linesize[0]=pHandle->pcm_buffer_size;
    return pHandle->pcm_buffer_size;
}

int ffmpeg_audio_decoder_close(long handle) {
    AUDIO_DECODER_HANDLE *pHandle = (AUDIO_DECODER_HANDLE *) handle;
    if (pHandle == NULL) {
        return -1;
    }
    if (pHandle->parser_handle != 0) {
        ffmpeg_parser_close(pHandle->parser_handle);
    }
    if (pHandle->pkt!=NULL){
        av_packet_free(&pHandle->pkt);
        pHandle->pkt=NULL;
    }

    if (pHandle->aac_buffer != NULL) {
        av_freep(pHandle->aac_buffer);
        pHandle->aac_buffer = NULL;
    }
    avcodec_close(pHandle->pAVCodecCtx);
    avcodec_free_context(&pHandle->pAVCodecCtx);
    free(pHandle);
    return 0;
}



