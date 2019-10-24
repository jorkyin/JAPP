//
// Created by yinjian on 2018/12/22/022.
//


#include <log.h>
#include <libavformat/avformat.h>
#include <libavutil/file.h>
#include <libavutil/imgutils.h>
#include "record_picture.h"
#include "../src/NativeCallBack.h"

/**
 * 将AVFrame(YUV420格式)保存为JPEG格式的图片
 *
 * @param width YUV420的宽
 * @param height YUV42的高
 *
 */
int write_yuv_to_jpeg(RT_APP_CHAR *uid, AVFrame *pFrame, char *url) {

    LOGI("start");
    if (url == NULL || pFrame == NULL) {
        LOGE("url is NULL");
        return -1;
    }
    av_register_all();
    avformat_network_init();
    avcodec_register_all();
    // 分配AVFormatContext对象
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    if (pFormatCtx == NULL) {
        LOGE("avformat_alloc_context is NULL");
        return -1;
    }
    // 设置输出文件格式
    pFormatCtx->oformat = av_guess_format("mjpeg", NULL, NULL);
    if (pFormatCtx->oformat == NULL) {
        LOGE("av_guess_format is NULL");
        return -1;
    }

    pFormatCtx->oformat->video_codec = AV_CODEC_ID_MJPEG;
    // 构建一个新stream
    AVStream *pAVStream = avformat_new_stream(pFormatCtx, 0);
    if (pAVStream == NULL) {
        LOGE("avformat_new_stream open output file.");
        return -1;
    }
    // 查找解码器
    AVCodec *pCodec = avcodec_find_encoder(pFormatCtx->oformat->video_codec);
    //AVCodec *pCodec = avcodec_find_encoder( pFormatCtx->);
    if (pCodec == NULL) {
        LOGE("Codec not found.");
        return -1;
    }
    // 设置该stream的信息
    AVCodecContext *pCodecCtx = avcodec_alloc_context3(pCodec);
    if (pCodecCtx == NULL) {
        LOGE("pCodecCtx not found.");
        return -1;
    }
    pCodecCtx->codec_id = pFormatCtx->oformat->video_codec;
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUVJ420P;
    pCodecCtx->width = pFrame->width;
    pCodecCtx->height = pFrame->height;
    pCodecCtx->time_base.num = 1;
    pCodecCtx->time_base.den = 25;

    // 设置pCodecCtx的解码器为pCodec
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("Could not open codec.");
        return -1;
    }

    //Encode
    // 给AVPacket分配足够大的空间
    AVPacket pkt;
    av_new_packet(&pkt, pCodecCtx->width * pCodecCtx->height * 3);

    int ret = avcodec_send_frame(pCodecCtx, pFrame);
    if (ret != 0) {
        LOGE("avcodec_send_frame error");
        return -1;
    }
    ret = avcodec_receive_packet(pCodecCtx, &pkt);
    if (ret != 0) {
        LOGE("avcodec_send_frame error");
        return -1;
    }
    rtPictureData2java(uid, pkt.data, pkt.size);
#if 0
    // 创建并初始化一个和该url相关的AVIOContext
    ret =avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, url);
    if (ret < 0) {
        LOGE("Couldn't open output file. ret=%d  %s", ret, av_err2str(ret));

    } else {
        // 开始输出一些信息
        av_dump_format(pFormatCtx, 0, url, 1);
        // End Output some information

        //Write Header
        ret = avformat_write_header(pFormatCtx, NULL);
        if (ret < 0) {
            LOGE("avformat_write_header. ret=%d  %s", ret, av_err2str(ret));
        }
        ret = av_write_frame(pFormatCtx, &pkt);
        if (ret != 0) {
            LOGE("av_write_frame. ret=%d  %s", ret, av_err2str(ret));
        }
        //Write Trailer
        ret = av_write_trailer(pFormatCtx);
        if (ret != 0) {
            LOGE("av_write_trailer. ret=%d  %s", ret, av_err2str(ret));
        }
    }
#else


    // 创建并初始化一个和该url相关的AVIOContext
    ret = avio_open(&pFormatCtx->pb, url, AVIO_FLAG_WRITE);
    if (ret < 0) {
        LOGE("Couldn't open output file. ret=%d  %s", ret, av_err2str(ret));

    } else {
        // 开始输出一些信息
        av_dump_format(pFormatCtx, 0, url, 1);
        // End Output some information

        //Write Header
        ret = avformat_write_header(pFormatCtx, NULL);
        if (ret < 0) {
            LOGE("avformat_write_header. ret=%d  %s", ret, av_err2str(ret));
        }
        ret = av_write_frame(pFormatCtx, &pkt);
        if (ret != 0) {
            LOGE("av_write_frame. ret=%d  %s", ret, av_err2str(ret));
        }
        //Write Trailer
        ret = av_write_trailer(pFormatCtx);
        if (ret != 0) {
            LOGE("av_write_trailer. ret=%d  %s", ret, av_err2str(ret));
        }
    }
#endif
    LOGI("Encode Successful.\n");

    av_packet_unref(&pkt);




    avcodec_close(pCodecCtx);
    avio_close(pFormatCtx->pb);
    avformat_free_context(pFormatCtx);

    return 0;
}