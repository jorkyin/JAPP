//
// Created by yinjian on 2017/11/24.
//

#ifndef BARLEY_OPENSL_IO1_H
#define BARLEY_OPENSL_IO1_H

int opensl_pcm_open(long *handle, int sample_rate, int channels);

int opensl_pcm_close(long handle);

#endif //BARLEY_OPENSL_IO1_H
