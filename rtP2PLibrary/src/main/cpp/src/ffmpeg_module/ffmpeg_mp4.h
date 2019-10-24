#ifndef _FFMPEG_MP4_
#define _FFMPEG_MP4_

typedef struct _MP4_VIDEO_ATTR_{
	unsigned int width;
	unsigned int height;
	int fps;//注意:这个值如果大于0，MP4录像就按个FPS来，从而会忽略写帧时的PTS
	int codeid;//编码ID，目前只支持AV_CODEC_ID_H264 AV_CODEC_ID_HEVC
}MP4_VIDEO_ATTR;

typedef struct _MP4_AUDIO_ATTR_{
	unsigned int sampleRate;//采样率
	unsigned int channels;//通道数 1:单通道 2:双通道
	unsigned int bitWidth;//位宽，一般是16
	int codeid;//编码ID，目前只支持AV_CODEC_ID_AAC
}MP4_AUDIO_ATTR;

typedef struct _MP4_FILE_INFO_{
	unsigned int u32VideoDuration;//视频时长，单位毫秒
	unsigned int u32AudioDuration;//音频时长，单位毫秒
}MP4_FILE_INFO;


int ffmpeg_mp4_write_open(long *handle, MP4_VIDEO_ATTR *pVideoAttr, MP4_AUDIO_ATTR *pAudioAttr, char *mp4Path);
int ffmpeg_mp4_write_video(long handle, char *data, int data_len, unsigned long long u64PTS);
int ffmpeg_mp4_write_audio(long handle, char *data, int data_len, unsigned long long u64PTS);
int ffmpeg_mp4_write_close(long handle);
int ffmpeg_mp4_write_fileinfo(long handle, MP4_FILE_INFO *pFileInfo);

#endif 
