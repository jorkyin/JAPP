#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include "libavformat/avformat.h"
#include "ffmpeg_aac_encoder.h"


typedef struct __AAC_ENCODER__{
    AVCodec *pAudioCodec;
    AVCodecContext *pAudioCodecCtx;
    AVFrame *pAVFrame;
    int frame_size;
    char *frame_buf;
}AAC_ENCODER;

int ffmpeg_aac_endoder_open(long *handle, int sample_rate, int channels){

    int ret = 0;

    avcodec_register_all();

    AVCodec *pAudioCodec = NULL;
    AVCodecContext *pAudioCodecCtx = NULL;

    pAudioCodec = avcodec_find_encoder_by_name("libfdk_aac");
    if(NULL == pAudioCodec){
        av_log(NULL, AV_LOG_ERROR, "[%s    %d] cannot find audio error\n",__FUNCTION__,__LINE__); 
        return -1;
    }

    pAudioCodecCtx = avcodec_alloc_context3(pAudioCodec);
    if(NULL == pAudioCodecCtx){
        av_log(NULL, AV_LOG_ERROR, "[%s    %d] avcodec_alloc_context3 error \n",__FUNCTION__,__LINE__);
        return -1;
    }

    pAudioCodecCtx->frame_size = 1024;//AAC固定1024个采样点
    pAudioCodecCtx->codec_id = pAudioCodec->id;
    pAudioCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
    pAudioCodecCtx->sample_fmt = AV_SAMPLE_FMT_S16;
    pAudioCodecCtx->sample_rate = sample_rate;
    pAudioCodecCtx->channel_layout = (channels==1)?AV_CH_LAYOUT_MONO:AV_CH_LAYOUT_STEREO;
    pAudioCodecCtx->channels = av_get_channel_layout_nb_channels(pAudioCodecCtx->channel_layout);

    if ((ret = avcodec_open2(pAudioCodecCtx, pAudioCodec, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "avcodec_open2 error %x\n", ret);
        return -1;
    }

    //根据音频属性，分配AVFrame空间大小
    AVFrame *pAVFrame = av_frame_alloc();
	pAVFrame->nb_samples= pAudioCodecCtx->frame_size;
	pAVFrame->format= pAudioCodecCtx->sample_fmt;

    int frame_size = av_samples_get_buffer_size(NULL, pAudioCodecCtx->channels, pAudioCodecCtx->frame_size, pAudioCodecCtx->sample_fmt, 0);	
    char *frame_buf = (char *)malloc(frame_size);
    ret = avcodec_fill_audio_frame(pAVFrame, pAudioCodecCtx->channels, pAudioCodecCtx->sample_fmt, (const uint8_t*)frame_buf, frame_size, 0);
    if (ret < 0){
        av_log(NULL, AV_LOG_ERROR, "avcodec_fill_audio_frame error %s\n", av_err2str(ret));
        return -1;
    }

    AAC_ENCODER *pAACEncoder = (AAC_ENCODER *)malloc(sizeof(AAC_ENCODER));
    pAACEncoder->pAudioCodec = pAudioCodec;
    pAACEncoder->pAudioCodecCtx = pAudioCodecCtx;
    pAACEncoder->pAVFrame = pAVFrame;
    pAACEncoder->frame_size = frame_size;
    pAACEncoder->frame_buf = frame_buf;

    *handle = (long)pAACEncoder;

    return 0;
}

int ffmpeg_aac_endoder_close(long handle){

    if(handle == 0){
        return -1;
    }
    AAC_ENCODER *pAACEncoder = (AAC_ENCODER *)handle;

    free(pAACEncoder->frame_buf);

    av_frame_free(&pAACEncoder->pAVFrame);

    avcodec_close(pAACEncoder->pAudioCodecCtx);

    avcodec_free_context(&pAACEncoder->pAudioCodecCtx);

    free(pAACEncoder);

    return 0;
}

int ffmpeg_aac_endoder_encode(long handle, char *pcm_buf, int pcm_size, char *aac_buf, int aac_size){

    int ret = 0;
    if(handle == 0){
        return -1;
    }
    AAC_ENCODER *pAACEncoder = (AAC_ENCODER *)handle;

    if(pAACEncoder->frame_size != pcm_size){
        av_log(NULL, AV_LOG_ERROR, "frame size error %d %d\n", pAACEncoder->frame_size, pcm_size);
        return -1;
    }

    memcpy(pAACEncoder->frame_buf, pcm_buf, pcm_size);

    ret = avcodec_send_frame(pAACEncoder->pAudioCodecCtx, pAACEncoder->pAVFrame);
    if(ret != 0){
        av_log(NULL, AV_LOG_ERROR, "avcodec_send_frame error");
        return -1;
    }

    AVPacket avPacket;
    av_init_packet(&avPacket);
    avPacket.data = NULL;    // packet data will be allocated by the encoder
    avPacket.size = 0;
    ret = avcodec_receive_packet(pAACEncoder->pAudioCodecCtx, &avPacket);
    if(ret != 0){
        av_log(NULL, AV_LOG_ERROR, "avcodec_receive_packet error");
        return -1;
    }


    if(avPacket.size > aac_size){
        av_log(NULL, AV_LOG_ERROR, "aac size not enough\n");
    }else{
        aac_size = avPacket.size;
        memcpy(aac_buf, avPacket.data, aac_size);
    }

    av_packet_unref(&avPacket);

    return aac_size;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

