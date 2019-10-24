
//
//  Rt_stream_in.c
//  RTP2PApp
//
//  Created by 杨青远 on 2017/5/17.
//  Copyright ? 2017年 杨青远. All rights reserved.
//
/*
*  负责通过ffmpeg提供的方法对视频流或者音频流的获取，并将其存放在对应的RT_Video的队列中
*/

#include <pthread.h>
#include <log.h>
#include <memory.h>
#include <libavutil/dict.h>
#include <libavformat/avformat.h>
#include "Rt_audio.h"
#include "Rt_video.h"
#include "Rt_stream_in.h"
#include "Rt_stream_out.h"


//注册ffmpeg的回调
static int interruptCallback(void *arg){
    RT_STRAM_IN *stRtStreamIn = (RT_STRAM_IN *)arg;
    
    if(stRtStreamIn == NULL || stRtStreamIn->s32Quit){
        LOGD("[%s   %d] interruptCallback quit \n",__FUNCTION__,__LINE__);
        return 1;
    }
    
    //LOGD("[%s   %d] timeout\n",__FUNCTION__,__LINE__);
    return 0;
}


//初始化参数
int rt_stream_in_init(RT_IPCAM_INFO *stRtIpcamInfo,const char *pFilePath){
    
    RT_STRAM_IN *stRtStreamIn = stRtIpcamInfo->stRtStreamIn;
    RT_SOCKET_TYPE enRtSocketType = stRtIpcamInfo->stRtParams->enRtSocketType;
    RT_LIVE_TYPE enRtLiveType = stRtIpcamInfo->stRtParams->enRtLiveType;

	if(NULL == stRtStreamIn){
		LOGE("[%s   %d] NULL == stRtStreamIn\n",__FUNCTION__,__LINE__);
		return -1;
	}
    rt_stream_in_relase(stRtStreamIn);
	
    stRtStreamIn->enLiveType = enRtLiveType;
    stRtStreamIn->enSocketModel = enRtSocketType;
    stRtStreamIn->s64SeekPosition =-1;
	stRtStreamIn->s32AudioStreamIndex = -1;
	stRtStreamIn->s32VideoStreamIndex = -1;
    memset(stRtStreamIn->acRtspPath, 0, sizeof(stRtStreamIn->acRtspPath));
    strcpy(stRtStreamIn->acRtspPath, pFilePath);
	
	stRtStreamIn->s32Quit = 0;
	stRtStreamIn->s32State = 0;
	
    return 0;
}

int rt_stream_in_open(RT_STRAM_IN *stRtStreamIn){
	
	AVDictionary *options = NULL;
	
	static int readDataErrorCount = 0;
	
	if(RT_LIVE_TYPE_LOCAL != stRtStreamIn->enLiveType){
		if(stRtStreamIn->enSocketModel == RT_SOCKET_TCP){
			av_dict_set(&options, "rtsp_transport", "tcp", 0);
			LOGD("[%s	%d] open with tcp \n",__FUNCTION__,__LINE__);
		}else if(stRtStreamIn->enSocketModel == RT_SOCKET_UDP){
			av_dict_set(&options, "rtsp_transport", "udp", 0);
			LOGD("[%s	%d] open with upd \n",__FUNCTION__,__LINE__);
		}
		
		av_dict_set(&options, "stimeout", "1000000", 0);//设置超时时间，默认一秒
		LOGD("[%s	%d] stimeout 1000000\n",__FUNCTION__,__LINE__);
	}
	
	stRtStreamIn->pformatCtx = avformat_alloc_context();
	stRtStreamIn->pformatCtx->interrupt_callback.callback = interruptCallback;
	stRtStreamIn->pformatCtx->interrupt_callback.opaque = stRtStreamIn;
	
	if(stRtStreamIn->s32Quit){
		LOGD("[%s	%d] quit \n",__FUNCTION__,__LINE__);
		goto ERR_EXIT;
	}
	
	//开启视频流
	if(0 != avformat_open_input(&stRtStreamIn->pformatCtx,
								stRtStreamIn->acRtspPath, NULL,&options)){
		
		LOGE("[%s	%d] open input error filename = %s \n",__FUNCTION__,__LINE__,stRtStreamIn->acRtspPath);

		goto ERR_EXIT;
		
	}else{
		LOGD("[%s	%d]avformat_open_input success \n",__FUNCTION__,__LINE__);
	}

	return 0;
	
ERR_EXIT:
	
	
	return -1;
}

long long rt_stream_in_find_info(RT_STRAM_IN *stRtStreamIn){
	

    /***************如果是本地播放或者视频远程回播，则获取视频的信息，如视频长度等***************/
	long long totalFramePts = 0;
    if(RT_LIVE_TYPE_LIVE != stRtStreamIn->enLiveType){
        
        if(0 > avformat_find_stream_info(stRtStreamIn->pformatCtx, NULL)){
            LOGE("[%s   %d] find stream info error \n",__FUNCTION__,__LINE__);
            return -1;
        }else{
            LOGD("[%s   %d] find stream info success \n",__FUNCTION__,__LINE__);
        }
    
        
        if(stRtStreamIn->pformatCtx->duration != AV_NOPTS_VALUE){
            long long hours,mins,secs,us;
            int64_t duration = stRtStreamIn->pformatCtx->duration + 5000;
            secs = duration / AV_TIME_BASE;
            us = duration % AV_TIME_BASE;
            mins = secs / 60;
            secs %= 60;
            hours = mins/ 60;
            mins %= 60;
            LOGD("[%s   %d] %02lld:%02lld:%02lld.%02lld\n",__FUNCTION__,__LINE__, hours, mins, secs, (100 * us) / AV_TIME_BASE);
            
            totalFramePts=hours*60*60+mins*60+secs;
            
        }else{
            LOGE("[%s   %d] get total pts faile \n",__FUNCTION__,__LINE__);
        }
    
    }
	
    /***************************************查找对应的index***************************************/
    int i=0;
    for(i = 0; i < stRtStreamIn->pformatCtx->nb_streams ;i++){
        if(AVMEDIA_TYPE_VIDEO == stRtStreamIn->pformatCtx->streams[i]->codecpar->codec_type){
            stRtStreamIn->s32VideoStreamIndex = i;
            LOGD("[%s   %d] s32VideoStreamIndex=%d \n",__FUNCTION__,__LINE__,i);
        }else if(AVMEDIA_TYPE_AUDIO == stRtStreamIn->pformatCtx->streams[i]->codecpar->codec_type){
            stRtStreamIn->s32AudioStreamIndex = i;
			LOGD("[%s   %d] s32AudioStreamIndex=%d \n",__FUNCTION__,__LINE__,i);
        }
    }

	//av_dump_format()是一个手工调试的函数，能使我们看到pFormatCtx->streams里面有什么内容
	av_dump_format(stRtStreamIn->pformatCtx,0,stRtStreamIn->acRtspPath,0);
	
	return totalFramePts;
}



//释放资源
int rt_stream_in_relase(RT_STRAM_IN *stRtStreamIn){
    
    LOGD("[%s    %d]\n",__FUNCTION__,__LINE__);
    stRtStreamIn->s32Quit = 1;
    
    if(stRtStreamIn->pformatCtx){
        avformat_close_input(&stRtStreamIn->pformatCtx);
        stRtStreamIn->pformatCtx = NULL;
        LOGD("[%s   %d] free pformatCtx success \n",__FUNCTION__,__LINE__);
    }
    
    return 0;
}
