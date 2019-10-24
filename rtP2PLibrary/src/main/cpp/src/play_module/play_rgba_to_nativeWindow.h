//
// Created by yinjian on 2018/12/22/022.
//

#ifndef RTP2PDEMO_PALY_NATIVE_WINDOWS_VIDEO_H
#define RTP2PDEMO_PALY_NATIVE_WINDOWS_VIDEO_H

#include <android/native_window.h>
#include <libavutil/frame.h>

int native_window_video_open(long *handle, ANativeWindow *native_window, int  window_width, int  window_height);
int native_window_video_show(long handle,AVFrame *frame);
int native_window_video_close(long handle);
#endif //RTP2PDEMO_PALY_NATIVE_WINDOWS_VIDEO_H
