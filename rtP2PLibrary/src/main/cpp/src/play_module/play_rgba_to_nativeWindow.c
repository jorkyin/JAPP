//
// Created by yinjian on 2018/12/22/022.
//


#include <log.h>
#include "play_rgba_to_nativeWindow.h"



//视频播放
typedef struct _Native_WINDOWS_HANDLE_{
    ANativeWindow *native_window;
    int  window_width;
    int  window_height;
}Native_WINDOWS_HANDLE;

int native_window_video_open(long *handle,ANativeWindow *native_window,int  window_width,int  window_height){

    if (native_window==NULL
        ||window_width==0
        ||window_height==0){
        return -1;
    }
    ANativeWindow *nativeWindow=native_window;
    // 设置native window的buffer大小,可自动拉伸
    int ret = ANativeWindow_setBuffersGeometry(nativeWindow,
                                               window_width,
                                               window_height,
                                               WINDOW_FORMAT_RGBA_8888);
    if (ret != 0) {
        LOGE("ANativeWindow_setBuffersGeometry init error! %d", ret);
        return -1;
    }

    Native_WINDOWS_HANDLE *pHandle = (Native_WINDOWS_HANDLE *) malloc(sizeof(Native_WINDOWS_HANDLE));
    if (!pHandle) {
        LOGE("AUDIO_DECODER_HANDLE malloc error\n");
        return -1;
    }

    memset(pHandle, 0, sizeof(Native_WINDOWS_HANDLE));
    pHandle->native_window = nativeWindow;
    pHandle->window_width = window_width;
    pHandle->window_height = window_height;

    *handle = (long) pHandle;
    return 0;
}

int native_window_video_show(long handle,AVFrame *frame) {
    int ret = -1;
    Native_WINDOWS_HANDLE *pHandle = (Native_WINDOWS_HANDLE *) handle;
    if (pHandle == NULL) { return -1; }

    ANativeWindow_Buffer nativeWindow_buffer;//申明一块缓冲区 用于绘制
    //8.准备绘制
    ANativeWindow_lock(pHandle->native_window, &nativeWindow_buffer, NULL);//锁定画布(outBuffer中将会得到数据)

    //10.绘制
    uint8_t *dst = (uint8_t *) nativeWindow_buffer.bits; //实际的位数
    int destStride = nativeWindow_buffer.stride * 4; //拿到一行有多少个字节 RGBA
    uint8_t *src = frame->data[0];//像素数据的首地址
    int srcStride = frame->linesize[0]; //实际内存一行的数量

    for (int i = 0; i < frame->height; ++i) {
        //将rgb_Frame缓冲区里面的数据一行一行copy到window的缓冲区里面
        //copy到window缓冲区的时候进行一些偏移设置可以将视频播放居中
        memcpy(dst + i * destStride, src + i * srcStride, (size_t) srcStride);
    }

    ANativeWindow_unlockAndPost(pHandle->native_window);//解锁画布
    return 0;
}

int native_window_video_close(long handle){
    Native_WINDOWS_HANDLE *pHandle = (Native_WINDOWS_HANDLE *)handle;
    if(pHandle == NULL){
        return -1;
    }

    if (pHandle->native_window != NULL) {
        ANativeWindow_release(pHandle->native_window);
        pHandle->native_window = NULL;
    }
    free(pHandle);
    return 0;
}