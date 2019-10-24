#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

#include "ffmpeg_mp4.h"
#include "ffmpeg_audio_decoder.h"
#include "ffmpeg_aac_encoder.h"
#include "ffmpeg_pcmskin.h"

#define H264_Get_NalType(c)  ( (c) & 0x1F )
#define STREAM_HEAD_LEN 4
	
#define NAL_TYPE_SLICE      1
#define NAL_TYPE_IDR        5
#define NAL_TYPE_SEI        6
#define NAL_TYPE_SPS        7
#define NAL_TYPE_PPS        8
#define NAL_TYPE_SEQ_END    9
#define NAL_TYPE_STREAM_END 10
#define MP4INFO_BUFF_LEN  128

#define H265E_NALU_VPS	   32 						/*VPS types*/
#define H265E_NALU_SPS	   33 						/*SPS types*/
#define H265E_NALU_PPS	   34 						/*PPS types*/
#define H265E_NALU_SEI	   39 						/*SEI types*/


#define VIDEO_TIME_BASE	(90000)

#define AAC_FRAME_SIZE (1024)

#define AAC_DATA_LEN	(2048)

typedef struct _MP4_HANDLE_{

	//FFMPEG使用
	AVFormatContext* pMp4Format;

	AVStream *pVideoStream;
	
	AVStream *pAudioStream;

	int s32VideoStreamIndex;

	int s32AudioStreamIndex;

	//当前视频参数
	MP4_VIDEO_ATTR stVideoAttr;

	//当前音频参数
	MP4_AUDIO_ATTR stAudioAttr;

	//是否每一次写入
	int bfirst;
	
	//MP4保存路径
	char mp4Path[512];

	//当前视频PTS
	unsigned long long u64VideoPTS;

	//当前视频帧数
	unsigned int u32VideoFrameCnt;

	unsigned long long u64VideoStartPTS;

	unsigned long long u64VideoEndPTS;

	//当前音频PTS
	unsigned long long u64AudioPTS;


	//当前视音帧数
	unsigned int u32AudioFrameCnt;

	//音频解码句柄
	long s64AudioDecoderHandle;
	char *pcm_data;//解码出来的PCM数据 
	int pcm_data_len;
	
	//AAC编码句柄
	long s64AACEncoderHandle;
	char *aac_data;//编码出来的AAC数据 
	int aac_data_len;
	
	//PCM缓冲池句柄 
	long s64PCMSkinHandle;
	char *pcmskin_data;//从缓冲池获取到的数据
	int pcmskin_data_len;

    //保证不会出现音视频一起写，以免冲突造成各种问题
    pthread_mutex_t writeMutex ;

}MP4_HANDLE;


static int check_keyframe(int codeid, char *data, int data_size){

	if(codeid == AV_CODEC_ID_H264){
		if((data[4]&0X1F) == NAL_TYPE_SPS){
			return 1;
		}else{
			return 0;
		}
	}else if(codeid == AV_CODEC_ID_HEVC){
		if((data[4]>>1) == H265E_NALU_VPS){
			return 1;
		}else{
			return 0;
		}
	}
	
	return 0;
}

static int get_extradata(AVStream *pVideoStream, char *data, int data_size){

	const AVBitStreamFilter *absFilter = NULL;
	AVBSFContext *absCtx = NULL;
	int bFinded = 0;
	const enum AVCodecID *ids = NULL;
	int ret = 0;
	int extradata_size = 0;
	uint8_t *extradata = NULL;
		
	absFilter = av_bsf_get_by_name("extract_extradata");
	if(absFilter == NULL){
		av_log(NULL, AV_LOG_ERROR, "av_bsf_get_by_name extract_extradata error\n");
		return -1;
	}

	//查找过滤器是否支持当前CODEC_ID
	if (!absFilter->codec_ids){
		return -1;
	}

	ret = 0;
	for (ids = absFilter->codec_ids; *ids != AV_CODEC_ID_NONE; ids++){
		if (*ids == pVideoStream->codecpar->codec_id){
			ret = 1;
			break;
		}
	}
	if(ret == 0){
		av_log(NULL, AV_LOG_ERROR, "find  ids error\n");
		return -1;
	}

	//初始化过虑器	
	ret = av_bsf_alloc(absFilter, &absCtx);
	if(ret != 0){
		av_log(NULL, AV_LOG_ERROR, "av_bsf_alloc error\n");
		return -1;
	}
	avcodec_parameters_copy(absCtx->par_in, pVideoStream->codecpar);

	av_bsf_init(absCtx);

	AVPacket avPacket;
	av_init_packet(&avPacket);
	avPacket.data = (uint8_t *)data;
	avPacket.size = data_size;
	
	//获取SPS,PPS
	if(av_bsf_send_packet(absCtx, &avPacket) != 0){
	    return 0;
	}
	
	if(av_bsf_receive_packet(absCtx, &avPacket) == 0){

		extradata = av_packet_get_side_data(&avPacket, AV_PKT_DATA_NEW_EXTRADATA, &extradata_size);
		if(extradata_size > 0){
			pVideoStream->codecpar->extradata = (uint8_t *)malloc(extradata_size);
			memcpy(pVideoStream->codecpar->extradata, extradata, extradata_size);
			pVideoStream->codecpar->extradata_size = extradata_size;
			av_log(NULL, AV_LOG_INFO, "memcpy extradata\n");
			int i = 0;
			for(i=0;i<extradata_size;i++){
				av_log(NULL, AV_LOG_INFO, "%02x ", extradata[i]);
			}
			av_log(NULL, AV_LOG_INFO, "\n");
		}
		
		av_packet_free_side_data(&avPacket);
	}

	//6. 释放资源
	av_bsf_free(&absCtx);
	
	absCtx = NULL;

	return extradata_size;
}




static int ffmpeg_mp4_write_video_nsync(long handle, char *data, int data_len, unsigned long long u64PTS)
{
	int ret = 0;
	int i = 0;
	
	MP4_HANDLE *pHandle =(MP4_HANDLE *)handle;
	if(pHandle == NULL){
		return -1;
	}

	if(!pHandle->pVideoStream){
		return -1;
	}

#if 0
	for(i=0;i<64;i++){
		av_log(NULL, AV_LOG_INFO, "%02X ", (unsigned char)data[i]);
	}
	av_log(NULL, AV_LOG_INFO, "\n");
#endif

	//检查是不是00 00 00 01 或者00 00 01的视频头
	int head = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
	if(head != 0x00000001){
		head = 0;
		head = data[0] << 24 | data[1] << 16 | data[2] << 8;
		if(head != 0x00000100){
			return -1;
		}
	}

	//第一次写入的时候，一定要I帧,不是I帧就返回
	if(pHandle->bfirst && !check_keyframe(pHandle->stVideoAttr.codeid, data, data_len)){
		av_log(NULL, AV_LOG_ERROR, "write need I Frame\n");
		return -1;
	}

	if(pHandle->bfirst== 1){

		//获取SPS，PPS信息
		if(pHandle->pVideoStream->codecpar->extradata == NULL){
			ret = get_extradata(pHandle->pVideoStream, data, data_len);
			if(ret <= 0){
				av_log(NULL, AV_LOG_ERROR, "get_extradata error\n");
				return -1;
			}
		}
		
		ret = avformat_write_header(pHandle->pMp4Format, NULL);
		if(ret != 0){
			av_log(NULL, AV_LOG_ERROR, "avformat_write_header error %s %d\n", av_err2str(ret), ret);
			return -1;
		}
	}

	//如果视频帧率不为0，则以帧率的PTS为准
	//如果视频帧率为0，则以传入的PTS为准
	if(pHandle->stVideoAttr.fps > 0)u64PTS=pHandle->u64VideoPTS;

	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = (uint8_t *)data;
	pkt.size = data_len;
	if(pHandle->stVideoAttr.fps > 0){
		//这里的PTS实际上是由帧率累加起来的，比如25fps，则40000一直加起来
		pkt.pts = u64PTS;
		pkt.dts = u64PTS;
	}else{
		//这里的PTS是由第一帧与当前帧的时间差用来做PTS
		pkt.pts = (pHandle->bfirst==1)?0:u64PTS-pHandle->u64VideoStartPTS;
		pkt.dts = (pHandle->bfirst==1)?0:u64PTS-pHandle->u64VideoStartPTS;
	}

	pkt.stream_index = pHandle->s32VideoStreamIndex;

	//MP4写入
	ret = av_interleaved_write_frame(pHandle->pMp4Format, &pkt);
	if(ret != 0){
		av_log(NULL, AV_LOG_ERROR, "write video error %s\n", av_err2str(ret));
		return -1;
	}
		
	//记录第一次视频的PTS
	if(pHandle->u64VideoStartPTS == AV_NOPTS_VALUE)pHandle->u64VideoStartPTS=u64PTS;
	
	//记录当前视频的PTS
	pHandle->u64VideoEndPTS = u64PTS;	

	//清除第一次写标志位
	if(pHandle->bfirst==1)pHandle->bfirst = 0;

	//按当前帧率递增PTS
	if(pHandle->stVideoAttr.fps > 0)pHandle->u64VideoPTS += (VIDEO_TIME_BASE/pHandle->stVideoAttr.fps);

	pHandle->u32VideoFrameCnt++;
			
	return ret;
}

static int ffmpeg_mp4_write_aac(long handle, char *data, int data_len, unsigned long long u64PTS)
{
	int ret = 0;
	int i = 0;

	MP4_HANDLE *pHandle =(MP4_HANDLE *)handle;
	if(pHandle == NULL){
		return -1;
	}

	if(!pHandle->pAudioStream){
		return -1;
	}

	//如果视频也要写，则让视频先把I帧写下来
	if(pHandle->pVideoStream && pHandle->bfirst == 1){
		return -1;
	}

	if(pHandle->bfirst == 1){		
		ret = avformat_write_header(pHandle->pMp4Format, NULL);
		if(ret != 0){
			av_log(NULL, AV_LOG_ERROR, "avformat_write_header error %s %d\n", av_err2str(ret), ret);
			return -1;
		}

		pHandle->bfirst = 0;
	}
#if 0
    for(i=0;i<16;i++){
        av_log(NULL, AV_LOG_INFO, "%02X ", data[i]);
    }
    av_log(NULL, AV_LOG_INFO, "\n");
#endif

	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = (uint8_t *)data;
	pkt.size = data_len;
	pkt.pts = pHandle->u64AudioPTS;
	pkt.dts = pHandle->u64AudioPTS;
	pkt.stream_index = pHandle->s32AudioStreamIndex;

	pHandle->u32AudioFrameCnt++;

	pHandle->u64AudioPTS+=1024;

	ret = av_interleaved_write_frame(pHandle->pMp4Format, &pkt);

	return ret;
}

static int ffmpeg_mp4_write_pcm(long handle, char *data, int data_len, unsigned long long u64PTS)
{
	int ret = 0;
	int i = 0;

	MP4_HANDLE *pHandle =(MP4_HANDLE *)handle;
	if(pHandle == NULL){
		return -1;
	}

	if(!pHandle->pAudioStream){
		return -1;
	}

	//如果视频也要写，则让视频先把I帧写下来
	if(pHandle->pVideoStream && pHandle->bfirst == 1){
		return -1;
	}

	if(pHandle->bfirst == 1){		
		ret = avformat_write_header(pHandle->pMp4Format, NULL);
		if(ret != 0){
			av_log(NULL, AV_LOG_ERROR, "avformat_write_header error %s %d\n", av_err2str(ret), ret);
			return -1;
		}

		pHandle->bfirst = 0;
	}

	if(pHandle->s64AACEncoderHandle == 0 || pHandle->s64PCMSkinHandle == 0){
		av_log(NULL, AV_LOG_ERROR, "%s %d error\n", __FUNCTION__, __LINE__);
		return -1;
	}


	//把数据放入缓冲池
	ret = ffmpeg_pcmskin_put(pHandle->s64PCMSkinHandle, data, data_len);
	if(ret <= 0){
		av_log(NULL, AV_LOG_ERROR, "%s %d error\n", __FUNCTION__, __LINE__);
		return -1;
	}

	while(1){
		//看是否能从缓冲池获取到pcm数据，如果缓冲池达到了AAC的能编码的数据是会返回的
		ret = ffmpeg_pcmskin_get(pHandle->s64PCMSkinHandle, pHandle->pcmskin_data, pHandle->pcmskin_data_len);
		if(ret <= 0){
			break;
		}

		ret = ffmpeg_aac_endoder_encode(pHandle->s64AACEncoderHandle, pHandle->pcmskin_data, pHandle->pcmskin_data_len, pHandle->aac_data, AAC_DATA_LEN);
		if(ret > 0){
			pHandle->aac_data_len = ret;
			ret = ffmpeg_mp4_write_aac(handle, pHandle->aac_data, pHandle->aac_data_len, u64PTS);
		}
	}

	return 0;
}

static int ffmpeg_mp4_write_alaw(long handle, char *data, int data_len, unsigned long long u64PTS)
{
	int ret = 0;
	int i = 0;

	MP4_HANDLE *pHandle =(MP4_HANDLE *)handle;
	if(pHandle == NULL){
		return -1;
	}

	if(!pHandle->pAudioStream){
		return -1;
	}

	//如果视频也要写，则让视频先把I帧写下来
	if(pHandle->pVideoStream && pHandle->bfirst == 1){
		return -1;
	}

	if(pHandle->bfirst == 1){		
		ret = avformat_write_header(pHandle->pMp4Format, NULL);
		if(ret != 0){
			av_log(NULL, AV_LOG_ERROR, "avformat_write_header error %s %d\n", av_err2str(ret), ret);
			return -1;
		}

		pHandle->bfirst = 0;
	}

	if(pHandle->s64AudioDecoderHandle == 0 || pHandle->s64AACEncoderHandle == 0 || pHandle->s64PCMSkinHandle == 0){
		av_log(NULL, AV_LOG_ERROR, "%s %d error\n", __FUNCTION__, __LINE__);
		return -1;
	}

	//把G7XX数据送去解码，获取到PCM数据
	ret = ffmpeg_audio_decoder_put(pHandle->s64AudioDecoderHandle, data, data_len);
	if(ret <= 0){
		av_log(NULL, AV_LOG_ERROR, "ffmpeg_audio_decoder_put error\n");
		return -1;
	}

	if(pHandle->pcm_data == NULL){
		pHandle->pcm_data_len = ret;
		pHandle->pcm_data = (char *)malloc(pHandle->pcm_data_len);
	}
	
	//每次解码出来的数据应该都是一样的，如果不一样，返回错误 
	if(pHandle->pcm_data && pHandle->pcm_data_len != ret){
		av_log(NULL, AV_LOG_ERROR, "%s %d error\n", __FUNCTION__, __LINE__);
		return -1;
	}

	//获取解码出来的数据
	ret = ffmpeg_audio_decoder_get(pHandle->s64AudioDecoderHandle, pHandle->pcm_data, pHandle->pcm_data_len);
	if(ret <= 0){
		av_log(NULL, AV_LOG_ERROR, "%s %d error\n", __FUNCTION__, __LINE__);
		return -1;
	}

	return ffmpeg_mp4_write_pcm(handle, pHandle->pcm_data, pHandle->pcm_data_len, u64PTS);
}

static int ffmpeg_mp4_write_audio_nsync(long handle, char *data, int data_len, unsigned long long u64PTS)
{
    int ret = 0;
    int i = 0;

    MP4_HANDLE *pHandle =(MP4_HANDLE *)handle;
    if(pHandle == NULL){
        return -1;
    }

    if(pHandle->stAudioAttr.codeid == AV_CODEC_ID_AAC){
        ret = ffmpeg_mp4_write_aac(handle, data, data_len, u64PTS);
    }else if(pHandle->stAudioAttr.codeid == AV_CODEC_ID_PCM_S16LE){
        ret = ffmpeg_mp4_write_pcm(handle, data, data_len, u64PTS);
    }else if(pHandle->stAudioAttr.codeid == AV_CODEC_ID_PCM_ALAW){
        ret = ffmpeg_mp4_write_alaw(handle, data, data_len, u64PTS);
    }

    return ret;
}

int ffmpeg_mp4_write_open(long *handle, MP4_VIDEO_ATTR *pVideoAttr, MP4_AUDIO_ATTR *pAudioAttr, char *mp4Path)
{
    int ret = 0;

    if(!mp4Path || (pVideoAttr == NULL && pAudioAttr == NULL)){
        return -1;
    }

    AVStream *pVideoStream = NULL;
    AVCodecParameters videoCodecpar;

    AVStream *pAudioStream = NULL;
    AVCodecParameters audioCodecpar;

    AVFormatContext* pMp4Format = NULL;

    int s32VideoStreamIndex = -1;

    int s32AudioStreamIndex = -1;

    int s32StreamIndex = 0;

    long s64AudioDecoderHandle = 0;

    long s64AACEncoderHandle = 0L;
    char *aac_data = NULL;//编码出来的AAC数据
    int aac_data_len = 0;

    long s64PCMSkinHandle = 0L;
    char *pcmskin_data = NULL;//从缓冲池获取到的数据
    int pcmskin_data_len = 0;

    if (avformat_alloc_output_context2(&pMp4Format, NULL, "mp4", mp4Path) < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "avformat_alloc_output_context2 error\n");
        ret = 0;
        goto err;
    }

    if (avio_open(&(pMp4Format->pb), mp4Path, AVIO_FLAG_READ_WRITE) < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "avformat_alloc_output_context2 error\n");
        ret = 0;
        goto err;
    }

    if(pVideoAttr){

        pVideoStream = avformat_new_stream(pMp4Format, NULL);
        if (!pVideoStream) {
            av_log(NULL, AV_LOG_ERROR, "avformat_new_stream video error\n");
            ret = 0;
            goto err;
        }

        //写入MP4视频属性
        pVideoStream->codecpar->codec_id = pVideoAttr->codeid;
        pVideoStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
        pVideoStream->codecpar->bit_rate = 400000;//这个随便写，不影响
        pVideoStream->codecpar->width = pVideoAttr->width;
        pVideoStream->codecpar->height	= pVideoAttr->height;
        pVideoStream->codecpar->extradata = NULL;//这个等会在写I帧的时候写入
        pVideoStream->codecpar->extradata_size = 0;

        s32VideoStreamIndex = s32StreamIndex;

        s32StreamIndex++;
    }

    if(pAudioAttr){

        pAudioStream = avformat_new_stream(pMp4Format, NULL);
        if (!pAudioStream) {
            av_log(NULL, AV_LOG_ERROR, "avformat_new_stream audio error\n");
            ret = 0;
            goto err;
        }

        //写入MP4音频属性
        pAudioStream->codecpar->codec_id = pAudioAttr->codeid;
        pAudioStream->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
        pAudioStream->codecpar->sample_rate= pAudioAttr->sampleRate;
        pAudioStream->codecpar->channels = pAudioAttr->channels;
        pAudioStream->codecpar->channel_layout = av_get_default_channel_layout(pAudioAttr->channels);
        pAudioStream->codecpar->frame_size = AAC_FRAME_SIZE;//AAC固定为1024个采样点

        s32AudioStreamIndex = s32StreamIndex;

        s32StreamIndex++;

        //所有不是AAC的音频，都需要转码为AAC
        if(pAudioStream->codecpar->codec_id != AV_CODEC_ID_AAC){

            //如果进入的是非PCM数据，则创建解码器，把数据解码到PCM
            if(pAudioStream->codecpar->codec_id != AV_CODEC_ID_PCM_S16LE){
                ret = ffmpeg_audio_decoder_open(&s64AudioDecoderHandle, pAudioAttr->sampleRate, pAudioAttr->channels, pAudioAttr->codeid);
            }

            //IPC过来的数据总是16位宽，所以这里写死AV_SAMPLE_FMT_S16
            //1024是AAC固定为1024个采样点
            ret = ffmpeg_pcmskin_open(&s64PCMSkinHandle, pAudioAttr->sampleRate, pAudioAttr->channels, AV_SAMPLE_FMT_S16, AAC_FRAME_SIZE);
            pcmskin_data = (char *)malloc(AAC_FRAME_SIZE*av_get_bytes_per_sample(AV_SAMPLE_FMT_S16));
            pcmskin_data_len = AAC_FRAME_SIZE*av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

            //创建AAC编码器
            ret = ffmpeg_aac_endoder_open(&s64AACEncoderHandle, pAudioAttr->sampleRate, pAudioAttr->channels);
            aac_data = (char *)malloc(AAC_DATA_LEN);
            aac_data_len = AAC_DATA_LEN;

            pAudioStream->codecpar->codec_id = AV_CODEC_ID_AAC;
        }
    }

    MP4_HANDLE *pHandle = (MP4_HANDLE *)malloc(sizeof(MP4_HANDLE));
    memset(pHandle, 0, sizeof(MP4_HANDLE));

    pHandle->pMp4Format = pMp4Format;


    //视频
    if(pVideoAttr){
        memcpy(&pHandle->stVideoAttr, pVideoAttr, sizeof(pHandle->stVideoAttr));
        pHandle->pVideoStream = pVideoStream;
    }

    //音频
    if(pAudioAttr){
        memcpy(&pHandle->stAudioAttr, pAudioAttr, sizeof(pHandle->stAudioAttr));
        pHandle->pAudioStream = pAudioStream;
    }

    pHandle->bfirst= 1;

    if(mp4Path)strncpy(pHandle->mp4Path, mp4Path, sizeof(pHandle->mp4Path)-1);

    pHandle->u64VideoStartPTS = AV_NOPTS_VALUE;

    pHandle->s32VideoStreamIndex = s32VideoStreamIndex;

    pHandle->s32AudioStreamIndex = s32AudioStreamIndex;

    pHandle->s64AudioDecoderHandle = s64AudioDecoderHandle;

    pHandle->s64AACEncoderHandle = s64AACEncoderHandle;

    pHandle->aac_data = aac_data;

    pHandle->aac_data_len = aac_data_len;

    pHandle->s64PCMSkinHandle= s64PCMSkinHandle;

    pHandle->pcmskin_data = pcmskin_data;

    pHandle->pcmskin_data_len = pcmskin_data_len;

    pthread_mutex_init(&(pHandle->writeMutex),NULL);

    *handle = (long)pHandle;

    return 0;

    err:
    return -1;
}

int ffmpeg_mp4_write_video(long handle, char *data, int data_len, unsigned long long u64PTS)
{
    MP4_HANDLE *pHandle =(MP4_HANDLE *)handle;
    if(pHandle == NULL){
        return -1;
    }
    int ret = 0;

    pthread_mutex_lock(&(pHandle->writeMutex));
    ret = ffmpeg_mp4_write_video_nsync(handle, data, data_len, u64PTS);
    pthread_mutex_unlock(&(pHandle->writeMutex));

    return ret;
}

int ffmpeg_mp4_write_audio(long handle, char *data, int data_len, unsigned long long u64PTS)
{
    MP4_HANDLE *pHandle =(MP4_HANDLE *)handle;
    if(pHandle == NULL){
        return -1;
    }
    int ret = 0;

    pthread_mutex_lock(&(pHandle->writeMutex));
    ret = ffmpeg_mp4_write_audio_nsync(handle, data, data_len, u64PTS);
    pthread_mutex_unlock(&(pHandle->writeMutex));

    return ret;
}

int ffmpeg_mp4_write_close(long handle)
{
	MP4_HANDLE *pHandle =(MP4_HANDLE *)handle;
	if(pHandle == NULL){
		return -1;
	}

	if(pHandle->bfirst != 1)
		av_write_trailer(pHandle->pMp4Format);
	
	avio_close(pHandle->pMp4Format->pb);

	avformat_free_context(pHandle->pMp4Format);

	if(pHandle->pcmskin_data)
		free(pHandle->pcmskin_data);
	
	if(pHandle->s64PCMSkinHandle != 0)
		ffmpeg_pcmskin_close(pHandle->s64PCMSkinHandle);
	
	if(pHandle->aac_data)
		free(pHandle->aac_data);
	
	if(pHandle->s64AACEncoderHandle != 0)
		ffmpeg_aac_endoder_close(pHandle->s64AACEncoderHandle);

	if(pHandle->pcm_data)
			free(pHandle->pcm_data);

	if(pHandle->s64AudioDecoderHandle!= 0)
		ffmpeg_audio_decoder_close(pHandle->s64AudioDecoderHandle);

    pthread_mutex_destroy(&(pHandle->writeMutex));

	free(pHandle);
	pHandle = NULL;

	return 0;
}


int ffmpeg_mp4_write_fileinfo(long handle, MP4_FILE_INFO *pFileInfo)
{
	MP4_HANDLE *pHandle =(MP4_HANDLE *)handle;
	if(pHandle == NULL || pFileInfo == NULL){
		return -1;
	}

	if(pHandle->pVideoStream){
		
		if(pHandle->stVideoAttr.fps > 0){
			//帧数*每帧时长
			pFileInfo->u32VideoDuration = (1000/pHandle->stVideoAttr.fps) * pHandle->u32VideoFrameCnt;
		}else{
			unsigned int period= (unsigned int)(pHandle->u64VideoEndPTS-pHandle->u64VideoStartPTS);
			
			unsigned int sec = (period/VIDEO_TIME_BASE);
			
			float f_msec = (float)(period%VIDEO_TIME_BASE)/(VIDEO_TIME_BASE/1000.0);
			
			unsigned int msec = (unsigned int)f_msec+sec*1000;
		
			//LOGI("%llu %llu %d %d %f %d\n", pHandle->u64VideoEndPTS, pHandle->u64VideoStartPTS, period, sec, f_msec, msec);
			
			pFileInfo->u32VideoDuration = msec;
		}

	}

	if(pHandle->pAudioStream){
		float fps = pHandle->stAudioAttr.sampleRate/1024;//AAC的帧率
		float period = 1000.0/fps;//每帧的时长
		pFileInfo->u32AudioDuration = (unsigned int)(period*pHandle->u32AudioFrameCnt);
	}

	return 0;
}


