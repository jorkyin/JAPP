#ifndef _FFMPEG_PCMSKIN_H_
#define _FFMPEG_PCMSKIN_H_

//这个PCM缓冲池设计出来是为了可以按指定的数据量来
//获取到PCM的数据大小，主要应用来比如AAC编码需要的
//数据量是1024个采样点，但摄像机音频解码出来的数据
//有可能是320个采样点的，使用bufferskin之后，可以保证
//每一次从bufferskin取出来的数据都是1024

//sample_rate:采样率
//channels:通道数
//sample_fmt:音频格式,AVSampleFormat
//samples:采样数，比如AAC是1024，GXX是320
int ffmpeg_pcmskin_open(long *handle, int sample_rate, int channels, int sample_fmt, int samples);

int ffmpeg_pcmskin_put(long handle, char *data, int data_len);

int ffmpeg_pcmskin_get(long handle, char *data, int data_len);

int ffmpeg_pcmskin_close(long handle);
#endif
