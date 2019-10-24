#ifndef __FFMPEG_AUDIO_DECODER_H__
#define __FFMPEG_AUDIO_DECODER_H__

int ffmpeg_audio_decoder_open(long *handle, int sample_rate, int channels, int codecid);
int ffmpeg_audio_decoder_put(long handle, uint8_t *data, int data_len);
int ffmpeg_audio_decoder_get(long handle, AVFrame *frame);
int ffmpeg_audio_decoder_close(long handle);

#endif
