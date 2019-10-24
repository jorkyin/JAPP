//
// Created by yinjian on 2018/12/21/021.
//

#ifndef RTP2PDEMO_FFMPEG_VIDEO_SWS_H
#define RTP2PDEMO_FFMPEG_VIDEO_SWS_H

#include <libavutil/frame.h>
int ffmpeg_video_frame_sws_open(long *handle,enum AVPixelFormat srcFormat, int srcWidth, int srcHeight,int windowWidth,int windowHeight);
int ffmpeg_video_frame_sws(long handle,AVFrame src_frame,AVFrame *RGBA_frame);
int ffmpeg_video_frame_sws_close(long handle);
#endif //RTP2PDEMO_FFMPEG_VIDEO_SWS_H
