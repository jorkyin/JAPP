//
//  Rt_stream_out.h
//  RTP2PApp
//
//  Created by 杨青远 on 2017/5/17.
//  Copyright �0�8 2017年 杨青远. All rights reserved.
//

#ifndef Rt_stream_out_h
#define Rt_stream_out_h

#include "Rt_pub_def.h"

#ifdef __cplusplus
#if _cplusplus
extern "C"{
#endif
#endif /* __plusplus*/
    
    
#include <stdio.h>

int rt_stream_out_init(RT_STRAM_OUT *stRtStreamOut,RT_AUDIO_TYPE audioType,int width,int height,int fps,int s32SynTime);

int rt_stream_out_start(RT_STRAM_OUT *stRtStreamOut, int videoid, int width, int height, int fps,
                        RT_AUDIO_TYPE audioType, int s32Sample_rate, int s32Channel,
                        const char*filename);

int rt_stream_out_write_video_h264(RT_STRAM_OUT *stRtStreamOut,AVPacket *stAVPacket);

int rt_stream_out_write_audio_aac(RT_STRAM_OUT *stRtStreamOut,AVPacket *stAVPacket);

int rt_stream_out_write_video_h265(RT_STRAM_OUT *stRtStreamOut,AVPacket *stAVPacket);

int rt_stream_out_write_audio_pcm(RT_STRAM_OUT *stRtStreamOut,AVPacket *stAVPacket);

/*
pFrame:即解码好后的数据,格式可以为yuv420p,rgb格式
srcW:源视频的宽
srcH:源视频的高
dstW:得到的图片的宽
dstH:得到的图片的高
filePath:图标保存的路径，全程
在没有特别需求的情况下srcW 跟 dstW ,srcH 跟 dstH的值是相同的
当720p截图得到1080p的时候，srcW 跟 dstW的值就要对应的设置
*/
int rt_stream_out_write_pic_jpeg(AVFrame* pFrame, int srcW, int srcH,int dstW,int dstH, const char*filePath);

/*重置输入流，调用该方法后，视频会重新开始从I帧开始录像,注意录像会存储为同一个视频文件*/
int rt_stream_out_reset(RT_STRAM_OUT *stRtStreamOut);

int rt_stream_out_stop(RT_STRAM_OUT *stRtStreamOut);

//int rt_stream_out_relase(RT_STRAM_OUT *stRtStreamOut);
#ifdef __cplusplus
#if _cplusplus
}
#endif
#endif /* __plusplus*/

#endif /* Rt_stream_out_h */


