//
// Created by yinjian on 2018/11/23/023.
//

#ifndef RTP2PDEMO_FFMPEGAUDIO_H
#define RTP2PDEMO_FFMPEGAUDIO_H



struct audio_swr {
    struct SwrContext *swr_ctx;
    AVFrame *frame_src;
    AVFrame *frame_pcm;
    uint8_t *audio_buffer;

    //输出的采样格式
    uint64_t out_ch_layout;//输出的声道布局
    enum AVSampleFormat out_sample_fmt ;//输出的采样格式 16bit PCM
    int out_sample_rate;//输出的采样率
    int nb_samples ;

} audioSwr;
int audio_pcm_resample(AVFrame *frame);
int destroy_audio_swr();
#endif //RTP2PDEMO_FFMPEGAUDIO_H
