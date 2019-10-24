#ifndef __FFMPEG_AAC_ENCODER__
#define __FFMPEG_AAC_ENCODER__

typedef struct _AAC_ENCODER_ATTR_{
	unsigned int sampleRate;//采样率
	unsigned int channels;//通道数 1:单通道 2:双通道
	unsigned int bitWidth;//位宽，一般是16
	int codeid;//编码ID，目前只支持AV_CODEC_ID_AAC
}AAC_ENCODER_ATTR;

int ffmpeg_aac_endoder_open(long *handle, int sample_rate, int channels);

int ffmpeg_aac_endoder_close(long handle);

int ffmpeg_aac_endoder_encode(long handle, char *pcm_buf, int pcm_size, char *aac_buf, int aac_size);

#endif