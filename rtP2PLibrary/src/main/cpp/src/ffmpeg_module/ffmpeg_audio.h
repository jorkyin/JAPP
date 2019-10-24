//
// Created by yinjian on 2019/4/8.
//

#ifndef RTP2PDEMO_FFMPEG_AUDIO_H
#define RTP2PDEMO_FFMPEG_AUDIO_H

//sample_rate:采样率
//channels:通道数
//sample_fmt:音频格式,AVSampleFormat
//samples:采样数，比如AAC是1024，GXX是320
int ffmpeg_audio_open(long *handle,
                      int64_t out_ch_layout, enum AVSampleFormat out_sample_fmt, int out_sample_rate);

int ffmpeg_audio_put(long handle, uint8_t *data, int data_len, int audioType);

int ffmpeg_audio_get(long handle, uint8_t *data, int *data_len);

int ffmpeg_audio_close(long handle);

#endif //RTP2PDEMO_FFMPEG_AUDIO_H
