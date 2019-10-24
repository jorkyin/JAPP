//
//  Rt_stream_out.c
//  RTP2PApp
//
//  Created by 杨青远 on 2017/5/17.
//  Copyright ? 2017年 杨青远. All rights reserved.
//

/*
 *将视频存储为mp4格式
 */
//#include <sys/timeb.h>

#include <memory.h>
#include <libavcodec/avcodec.h>
#include <log.h>
#include "Rt_stream_out.h"
#include "Rt_pub_def.h"

#include "ffmpeg_mp4.h"

//开始视频录制，调用改方法后不回真正的录像，真正的视频保存还是要调用write的方法
int rt_stream_out_start(RT_STRAM_OUT *stRtStreamOut, int videoid, int width, int height, int fps,
                        RT_AUDIO_TYPE audioType, int s32Sample_rate, int s32Channel,
                        const char*filename){
    long handle = 0;
    MP4_VIDEO_ATTR stVideoAttr;
    MP4_AUDIO_ATTR stAudioAttr;
    char *mp4Path = filename;

    memset(&stVideoAttr, 0, sizeof(stVideoAttr));
    stVideoAttr.width = width;
    stVideoAttr.height = height;
    stVideoAttr.codeid = videoid;
    stVideoAttr.fps = 0;

    memset(&stAudioAttr, 0, sizeof(stAudioAttr));
    if(RT_AUDIO_TYPE_PCM == audioType){
        stAudioAttr.codeid = AV_CODEC_ID_PCM_S16LE;
        stAudioAttr.bitWidth = 16;
        stAudioAttr.channels = 1;
        stAudioAttr.sampleRate = 8000;
    }else{
        stAudioAttr.codeid = AV_CODEC_ID_AAC;
        stAudioAttr.bitWidth = 16;
        stAudioAttr.channels = 1;
        stAudioAttr.sampleRate = 16000;
    }

    LOGD("mp4 file video codeid:%s width:%d height:%d  fps:%d\n", avcodec_get_name(stVideoAttr.codeid), stVideoAttr.width, stVideoAttr.height, stVideoAttr.fps);
    LOGD("mp4 file audio codeid:%s channels:%d sampleRate:%d\n",  avcodec_get_name(stAudioAttr.codeid), stAudioAttr.channels, stAudioAttr.sampleRate);

    ffmpeg_mp4_write_open(&handle, &stVideoAttr, &stAudioAttr, mp4Path);
    //ffmpeg_mp4_write_open(&handle, NULL, &stAudioAttr, mp4Path);

    //LOGD("mp4 file handle %lld filename:%s\n", handle, filename);

    stRtStreamOut->mp4Handle = handle;

    stRtStreamOut->enRtVideoRecState = RT_VIDEO_REC;



    return 0;
}

//写视频数据
int rt_stream_out_write_video_h264(RT_STRAM_OUT *stRtStreamOut,AVPacket *stAVPacket){
    ffmpeg_mp4_write_video(stRtStreamOut->mp4Handle, stAVPacket->data, stAVPacket->size, stAVPacket->pts);
    return 0;
}


//写音频数据
int rt_stream_out_write_audio_aac(RT_STRAM_OUT *stRtStreamOut,AVPacket *stAVPacket){
    ffmpeg_mp4_write_audio(stRtStreamOut->mp4Handle, stAVPacket->data, stAVPacket->size, stAVPacket->pts);
    return 0;
}

int rt_stream_out_write_audio_pcm(RT_STRAM_OUT *stRtStreamOut,AVPacket *stAVPacket){
    ffmpeg_mp4_write_audio(stRtStreamOut->mp4Handle, stAVPacket->data, stAVPacket->size, stAVPacket->pts);
    return 0;
}


//停止录像
int rt_stream_out_stop(RT_STRAM_OUT *stRtStreamOut){
    stRtStreamOut->enRtVideoRecState = RT_VIDEO_ENREC;
    ffmpeg_mp4_write_close(stRtStreamOut->mp4Handle);
    stRtStreamOut->mp4Handle = 0;
    return 0;
}
