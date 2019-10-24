//
// Created by Administrator on 2018/12/13.
//



#include <libavformat/avformat.h>
#include <rt_p2p_types.h>
#include "ffmpeg_video_decoder.h"

typedef struct _VIDEO_DECODER_HANDLE_ {
    AVCodec *pAVCodec;
    AVCodecContext *pAVCodecCtx;
    AVFrame *pAVFrame;
    int codecid;
    char *date;
    int date_len;
    int got_frame;

} VIDEO_DECODER_HANDLE;

int get_codec_id(long handle){
    VIDEO_DECODER_HANDLE *pHandle = (VIDEO_DECODER_HANDLE *) handle;
    if (pHandle == NULL) { return -1; }
    return pHandle->codecid;
}


int get_frame_codec_id(RT_APP_UCHAR *date){
  //查看视频编码释放变化（AV_CODEC_ID_H264 ，AV_CODEC_ID_H265）
        AVProbeData pd = {0};
        pd.buf_size = 1024;
        pd.buf = date;
        pd.filename = "";
        AVInputFormat *fmt = NULL;
        while ((fmt = av_iformat_next(fmt))) {
            if (fmt->flags & AVFMT_NOFILE) {
                continue;
            }
            if ((strcmp(fmt->name, "h264") != 0) && (strcmp(fmt->name, "hevc") != 0)) {
                continue;
            }
            if (fmt->read_probe) {
                if (fmt->read_probe(&pd) > 0) {
                    return  fmt->raw_codec_id;
                }
            }
        }
    return -1;
}

int ffmpeg_video_decoder_open(long *handle, int codecid) {

    AVCodec *pAVCodec = NULL;
    AVCodecContext *pAVCodecCtx = NULL;
    AVFrame *pAVFrame = NULL;

    //1.注册所有编解码器，注册后才能使用
    avcodec_register_all();
    av_register_all();
    //2.从注册的解码器里找到H264解码器
    pAVCodec = avcodec_find_decoder((enum AVCodecID) codecid);
    //pAVCodec = avcodec_find_decoder(AV_CODEC_ID_H265);
    if (pAVCodec == NULL) {
        LOGE("[%s %d] 创建解码器失败", __FUNCTION__, __LINE__);
        return -1;
    }

    //3. 初始化解码的上下文，上下文很关键，包含了解码所需要的信息
    pAVCodecCtx = avcodec_alloc_context3(pAVCodec);
    if (pAVCodecCtx == NULL) {
        LOGE("[%s %d] 创建解码器上下文失败", __FUNCTION__, __LINE__);
        return -1;
    }
    pAVCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;

    //第五步：打开解码器
    if (avcodec_open2(pAVCodecCtx, pAVCodec, NULL) < 0) {
        LOGE("[%s %d] 打开解码器失败", __FUNCTION__, __LINE__);
        return -1;
    }

    pAVFrame = av_frame_alloc();
    if (!pAVFrame) {
        LOGE("[%s %d] av_frame_alloc error", __FUNCTION__, __LINE__);
        return -1;
    }

    VIDEO_DECODER_HANDLE *pHandle = (VIDEO_DECODER_HANDLE *) malloc(sizeof(VIDEO_DECODER_HANDLE));
    if (!pHandle) {
        LOGE("[%s %d] AUDIO_DECODER_HANDLE malloc error", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(pHandle, 0, sizeof(VIDEO_DECODER_HANDLE));
    pHandle->codecid = codecid;
    pHandle->pAVCodecCtx = pAVCodecCtx;
    pHandle->pAVCodec = pAVCodec;
    pHandle->pAVFrame = pAVFrame;

    *handle = (long) pHandle;

    return 0;
}

int ffmpeg_video_decoder(long handle, char *data, int data_len, AVFrame *frame) {
    int ret = -1;
    if (data_len < 1 || frame == NULL) {
        return -1;
    }
    VIDEO_DECODER_HANDLE *pHandle = (VIDEO_DECODER_HANDLE *) handle;
    if (pHandle == NULL) { return -1; }

    AVPacket pkt;
    av_init_packet(&pkt);

    pkt.data = (uint8_t *) data;
    pkt.size = data_len;
    pkt.pts = AV_NOPTS_VALUE;
    pkt.dts = AV_NOPTS_VALUE;

    ret = avcodec_send_packet(pHandle->pAVCodecCtx, &pkt);
    if (ret != 0) {
        if (AVERROR(EAGAIN) == ret) {
            LOGE("avcodec_send_packet  AVERROR(EAGAIN) %s\n", av_err2str(ret));
        } else if (AVERROR_EOF == ret) {
            LOGE("avcodec_send_packet AVERROR_EOF %s\n", av_err2str(ret));
        } else if (AVERROR(EINVAL) == ret) {
            LOGE("avcodec_send_packet AVERROR(EINVAL) %s\n", av_err2str(ret));
        } else if (AVERROR(ENOMEM) == ret) {
            LOGE("avcodec_send_packet AVERROR(ENOMEM) %s\n", av_err2str(ret));
        }

        return -1;

    }

    ret = avcodec_receive_frame(pHandle->pAVCodecCtx, pHandle->pAVFrame);
    if (ret != 0) {
        av_frame_unref(pHandle->pAVFrame);
        if (AVERROR(EAGAIN) == ret) {
            LOGE("avcodec_receive_frame  AVERROR(EAGAIN) %s\n", av_err2str(ret));
        } else if (AVERROR_EOF == ret) {
            LOGE("avcodec_receive_frame AVERROR_EOF %s\n", av_err2str(ret));
        } else if (AVERROR(EINVAL) == ret) {
            LOGE("avcodec_receive_frame AVERROR(EINVAL) %s\n", av_err2str(ret));
        } else if (AVERROR(ENOMEM) == ret) {
            LOGE("avcodec_receive_frame AVERROR(ENOMEM) %s\n", av_err2str(ret));
        }


        return -1;
    }
    //  LOGI("pHandle->pAVFrame height=%d  ",pHandle->pAVFrame->height);

    memcpy(frame, pHandle->pAVFrame, sizeof(*(pHandle->pAVFrame)));
    //LOGI("frame height=%d  ",frame->height);
    return 0;
}

int ffmpeg_video_decoder_put(long handle, char *data, int data_len) {
    int ret = -1;
    VIDEO_DECODER_HANDLE *pHandle = (VIDEO_DECODER_HANDLE *) handle;
    if (pHandle == NULL) { return -1; }
    pHandle->got_frame = 0;
    AVPacket pkt;
    av_init_packet(&pkt);

    pkt.data = (uint8_t *) data;
    pkt.size = data_len;
    pkt.pts = AV_NOPTS_VALUE;
    pkt.dts = AV_NOPTS_VALUE;

    ret = avcodec_send_packet(pHandle->pAVCodecCtx, &pkt);
    if (ret != 0) {
        av_log(NULL, AV_LOG_ERROR, "avcodec_send_packet error %s\n", av_err2str(ret));
        return -1;

    }

    ret = avcodec_receive_frame(pHandle->pAVCodecCtx, pHandle->pAVFrame);
    if (ret != 0) {
        av_frame_unref(pHandle->pAVFrame);
        av_log(NULL, AV_LOG_ERROR, "avcodec_receive_frame error %s\n", av_err2str(ret));
        return -1;
    }
    pHandle->date_len = pHandle->pAVFrame->linesize[0];
    if (pHandle->date_len == 0) {
        pHandle->date = (char *) malloc((size_t) pHandle->date_len);
    }

    memcpy(pHandle->date, pHandle->pAVFrame->data[0], (size_t) pHandle->date_len);

    av_frame_unref(pHandle->pAVFrame);

    pHandle->got_frame = 1;

    return pHandle->date_len;
}

int ffmpeg_video_decoder_get(long handle, char *data, int data_len) {
    VIDEO_DECODER_HANDLE *pHandle = (VIDEO_DECODER_HANDLE *) handle;
    if (pHandle == NULL) {
        return -1;
    }

    if (pHandle->got_frame == 0) {
        av_log(NULL, AV_LOG_ERROR, "got_frame %d\n", pHandle->got_frame);
        return -1;
    }

    if (data_len < pHandle->date_len) {
        av_log(NULL, AV_LOG_ERROR, "data not enough %d %d\n", data_len, pHandle->date_len);
        return -1;
    }

    memcpy(data, pHandle->date, (size_t) pHandle->date_len);

    return pHandle->date_len;
}

int ffmpeg_video_decoder_close(long handle) {

    VIDEO_DECODER_HANDLE *pHandle = (VIDEO_DECODER_HANDLE *) handle;
    if (pHandle == NULL) {
        return -1;
    }

    avcodec_close(pHandle->pAVCodecCtx);

    avcodec_free_context(&pHandle->pAVCodecCtx);

    if (pHandle->pAVFrame) {
        av_frame_unref(pHandle->pAVFrame);
        av_frame_free(&pHandle->pAVFrame);
    }

    free(pHandle);
    return 0;
}