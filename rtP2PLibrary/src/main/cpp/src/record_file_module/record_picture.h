//
// Created by yinjian on 2018/12/22/022.
//

#ifndef RTP2PDEMO_RECORD_PICTURE_H
#define RTP2PDEMO_RECORD_PICTURE_H

#include <libavutil/frame.h>
#include <rt_p2p_types.h>

int write_yuv_to_jpeg(RT_APP_CHAR *uid,AVFrame *pFrame, char *url);
#endif //RTP2PDEMO_RECORD_PICTURE_H
