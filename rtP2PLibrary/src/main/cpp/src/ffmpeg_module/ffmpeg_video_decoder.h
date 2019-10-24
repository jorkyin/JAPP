//
// Created by Administrator on 2018/12/13.
//

#ifndef RTP2PDEMO_FFMPEG_VIDEO_DECODER_H
#define RTP2PDEMO_FFMPEG_VIDEO_DECODER_H
#include <libavcodec/avcodec.h>
#include <log.h>
int ffmpeg_video_decoder_open(long *handle,int codecid);
int get_codec_id(long handle);
int get_frame_codec_id(RT_APP_UCHAR *date);//通过I帧判断当前解码器的id

int ffmpeg_video_decoder(long handle, char *data, int data_len,AVFrame *frame);
int ffmpeg_video_decoder_put(long handle, char *data, int data_len);

int ffmpeg_video_decoder_get(long handle, char *data, int data_len);

int ffmpeg_video_decoder_close(long handle);
#endif //RTP2PDEMO_FFMPEG_VIDEO_DECODER_H
