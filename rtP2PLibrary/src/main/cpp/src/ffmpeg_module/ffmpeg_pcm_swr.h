//
// Created by yinjian on 2018/12/26/026.
//

#ifndef RTP2PDEMO_FFMPEG_AAC_SWR_H
#define RTP2PDEMO_FFMPEG_AAC_SWR_H

#include <rt_p2p_types.h>
/**
 * swr_handle,
 * in_ch_layout,     //输入的声道布局
 * in_sample_fmt,    //输入的采样格式
 * in_sample_rate    //输入的采样率
 * */
int pcm_swr_open(long *handle, uint64_t in_ch_layout, int in_sample_fmt, int in_sample_rate);
int pcm_swr_put(long handle,AVFrame *frame);
int pcm_swr_get(long handle, uint8_t *out_pcm, int *out_pcm_size);
int pcm_swr_close(long handle);
#endif //RTP2PDEMO_FFMPEG_AAC_SWR_H
