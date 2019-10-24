//
// Created by yinjian on 2018/12/21/021.
//

#include <log.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include "ffmpeg_video_sws.h"

//视频帧转码
typedef struct _VIDEO_SWS_HANDLE_{
    struct SwsContext *sws_ctx;
    AVFrame *frame_RGBA;
}VIDEO_SWS_HANDLE;

int ffmpeg_video_frame_sws_open(long *handle,enum AVPixelFormat srcFormat, int srcWidth, int srcHeight,int windowWidth,int windowHeight) {

    enum AVPixelFormat av_pixel_format = AV_PIX_FMT_RGBA;

    //创建一个yuv420视频像素数据格式缓冲区(一帧数据)
    AVFrame *frame_RGBA = av_frame_alloc();// 用于渲染
    if (frame_RGBA == NULL) {
        LOGE("无法分配视频帧内存。");
        return -1;
    }

    // 确定所需的缓冲区大小和分配缓冲区
    int numBytes = av_image_get_buffer_size(av_pixel_format,
                                            windowWidth,
                                            windowHeight,
                                            1);
    uint8_t *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    //p_image_decoder.frame_RGBA->填充数据
    /*  参数一：目标->填充数据(avframe_yuv420p)
        参数二：目标->每一行大小
        参数三：原始数据
        参数四：目标->格式类型
        参数五：宽
        参数六：高
        参数七：字节对齐方式*/
    int ret = av_image_fill_arrays(frame_RGBA->data,
                                   frame_RGBA->linesize,
                                   buffer,
                                   av_pixel_format,
                                   windowWidth, windowHeight,
                                   1);
    if (ret < 0) {
        LOGE("av_image_fill_arrays init error! %d", ret);
        return -1;
    }


    //4、注意：在这里我们不能够保证解码出来的一帧视频像素数据格式是yuv格式
    /** 参数一：源文件->原始视频像素数据格式宽
        参数二：源文件->原始视频像素数据格式高
        参数三：源文件->原始视频像素数据格式类型
        参数四：目标文件->目标视频像素数据格式宽
        参数五：目标文件->目标视频像素数据格式高
        参数六：目标文件->目标视频像素数据格式类型*/

    // 由于解码出来的帧格式不是RGBA的,在渲染之前需要进行格式转换
    struct SwsContext *sws_ctx = sws_getContext(srcWidth, srcHeight, srcFormat,
                                         windowWidth, windowHeight, av_pixel_format,
                                         SWS_BILINEAR, NULL, NULL, NULL);
    if (sws_ctx == NULL) {
        LOGE("p_image_decoder.sws_ctx init error!");
        return -1;
    }

    LOGI("init Show ok");


    VIDEO_SWS_HANDLE *pHandle = (VIDEO_SWS_HANDLE *)malloc(sizeof(VIDEO_SWS_HANDLE));
    if(!pHandle){
        av_log(NULL, AV_LOG_ERROR, "AUDIO_DECODER_HANDLE malloc error\n");
        return -1;
    }
    memset(pHandle, 0, sizeof(VIDEO_SWS_HANDLE));

    pHandle->sws_ctx = sws_ctx;
    pHandle->frame_RGBA = frame_RGBA;

    *handle = (long)pHandle;
    return 0;
}

int ffmpeg_video_frame_sws(long handle,AVFrame src_frame,AVFrame *RGBA_frame) {
    VIDEO_SWS_HANDLE *pHandle = (VIDEO_SWS_HANDLE *)handle;
    if(pHandle == NULL){
        return -1;
    }

    // 4、注意：在这里我们不能够保证解码出来的一帧视频像素数据格式是yuv格式
    //视频像素数据格式很多种类型: yuv420P、yuv422p、yuv444p等等...
    // 保证：我的解码后的视频像素数据格式统一为yuv420P->通用的格式
    //进行类型转换: 将解码出来的视频像素点数据格式->统一转类型为yuv420P
    //sws_scale作用：进行类型转换的
    /** 参数一：视频像素数据格式上下文
     参数二：原来的视频像素数据格式->输入数据
     参数三：原来的视频像素数据格式->输入画面每一行大小
     参数四：原来的视频像素数据格式->输入画面每一行开始位置(填写：0->表示从原点开始读取)
     参数五：原来的视频像素数据格式->输入数据行数
     参数六：转换类型后视频像素数据格式->输出数据
     参数七：转换类型后视频像素数据格式->输出画面每一行大小*/
    int h = sws_scale(pHandle->sws_ctx,
                      (const uint8_t *const *) src_frame.data,
                      src_frame.linesize, 0,
                      src_frame.height,
                      pHandle->frame_RGBA->data,
                      pHandle->frame_RGBA->linesize);

    if (h>0){
        pHandle->frame_RGBA->height=h;
        //*RGBA_frame=*(pHandle->frame_RGBA);

        memcpy(RGBA_frame, pHandle->frame_RGBA, sizeof(*(pHandle->frame_RGBA)));
    } else{
        LOGE("sws_scale is Error");
        return -1;
    }

    return 0;
}


int ffmpeg_video_frame_sws_close(long handle) {
    VIDEO_SWS_HANDLE *pHandle = (VIDEO_SWS_HANDLE *)handle;
    if(pHandle == NULL){
        return -1;
    }

    if (pHandle->frame_RGBA != NULL) {
        av_frame_free(&(pHandle->frame_RGBA));
        pHandle->frame_RGBA = NULL;
    }
    if (pHandle->sws_ctx != NULL) {
        sws_freeContext(pHandle->sws_ctx);
        pHandle->sws_ctx = NULL;
    }
    return 0;
}
