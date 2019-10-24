//
//  Rt_stream_in.h
//  RTP2PApp
//
//  Created by 杨青远 on 2017/5/17.
//  Copyright ? 2017年 杨青远. All rights reserved.
//

#ifndef Rt_stream_in_h
#define Rt_stream_in_h
#include "Rt_pub_def.h"
#include <stdio.h>

#ifdef __cplusplus
#if _cplusplus
extern "C"{
#endif
#endif /* __plusplus*/

int rt_stream_in_init(RT_IPCAM_INFO *stRtIpcamInfo,const char *pFilePath);

int rt_stream_in_open(RT_STRAM_IN *stRtStreamIn);

long long rt_stream_in_find_info(RT_STRAM_IN *stRtStreamIn);

int rt_stream_in_relase(RT_STRAM_IN *stRtStreamIn);
    
#ifdef __cplusplus
#if _cplusplus
}
#endif
#endif /* __plusplus*/

#endif /* Rt_stream_in_h */


