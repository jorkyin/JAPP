#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libavutil/channel_layout.h>

#include "libavutil/opt.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersrc.h"
#include "libavfilter/buffersink.h"

#include "ffmpeg_pcmskin.h"

typedef struct _PCM_BUFFERSKIN_HANDLE_{
	AVFilterGraph *pAVfilterGraph;
	
	AVFilter *pAVBufferSrc;
	AVFilterContext *pAVBufferSrcCtx;
	AVFilterInOut *pAVBufferIn;
	AVFrame *pAVAudioFrameIn;
	
	AVFilter *pAVBufferSink;
	AVFilterContext *pAVBufferSinkCtx;
	AVFilterInOut *pAVBufferOut;
	AVFrame *pAVAudioFrameOut;

	int sample_rate;
	int channels;
	int sample_fmt;
	int samples;

}PCM_BUFFERSKIN_HANDLE;


int ffmpeg_pcmskin_open(long *handle, int sample_rate, int channels, int sample_fmt, int samples)
{
	char args[512];
	int ret = 0;
	
	AVFilterGraph *pAVfilterGraph = NULL;

	AVFilter *pAVBufferSrc = NULL;
	AVFilterContext *pAVBufferSrcCtx = NULL;
	AVFilterInOut *pAVBufferIn  = NULL;
	AVFrame *pAVAudioFrameIn = NULL;
	
	AVFilter *pAVBufferSink = NULL;
	AVFilterContext *pAVBufferSinkCtx = NULL;
	AVFilterInOut *pAVBufferOut	= NULL;
	AVFrame *pAVAudioFrameOut = NULL;

	//注册所有滤镜
	avfilter_register_all();

	//查找abuffer
	pAVBufferSrc  = avfilter_get_by_name("abuffer");
	if(pAVBufferSrc == NULL){
		av_log(NULL, AV_LOG_ERROR, "avfilter_get_by_name abuffer error\n");
		return -1;
	}

	//查找abuffersink
	pAVBufferSink = avfilter_get_by_name("abuffersink");
	if(pAVBufferSink == NULL){
		av_log(NULL, AV_LOG_ERROR, "avfilter_get_by_name abuffersink error\n");
		return -1;
	}

	pAVfilterGraph = avfilter_graph_alloc();
	if(pAVfilterGraph == NULL){
		av_log(NULL, AV_LOG_ERROR, "avfilter_graph_alloc error\n");
		return -1;
	}
	pAVfilterGraph->nb_threads = 1;	

	memset(args, 0, sizeof(args));
	snprintf(args, sizeof(args)-1,"sample_rate=%d:sample_fmt=%s:channel_layout=0x%x",\
		sample_rate,av_get_sample_fmt_name(sample_fmt),av_get_default_channel_layout(channels));

	av_log(NULL, AV_LOG_INFO, "%s\n", args);

	//按指定音频属性，创建源滤镜
	ret = avfilter_graph_create_filter(&pAVBufferSrcCtx, pAVBufferSrc, "in", args, NULL, pAVfilterGraph);
	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot create audio out_buffer source\n");
		goto err;
	}

	//按指定音频属性，创建缓冲池滤镜
	ret = avfilter_graph_create_filter(&pAVBufferSinkCtx, pAVBufferSink, "out", NULL, NULL, pAVfilterGraph);		
	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot create audio out_buffer sink\n");
		goto err;
	}
#if 0
	static const enum AVSampleFormat out_sample_fmts[] = { sample_fmt, AV_SAMPLE_FMT_NONE };
	static const int64_t out_channel_layouts[] = { 0x4, -1 };
	static const int out_sample_rates[] = {16000 , -1 };


	ret = av_opt_set_int_list(pAVBufferSinkCtx, "sample_fmts", out_sample_fmts, -1, AV_OPT_SEARCH_CHILDREN);
	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot set output sample format\n");
		return ret;
	}

	ret = av_opt_set_int_list(pAVBufferSinkCtx, "channel_layouts", out_channel_layouts, -1, AV_OPT_SEARCH_CHILDREN);
	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot set output channel layout\n");
		return ret;
	}

	ret = av_opt_set_int_list(pAVBufferSinkCtx, "sample_rates", out_sample_rates, -1, AV_OPT_SEARCH_CHILDREN);
	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot set output sample rate\n");
		return ret;
	}
#endif
	pAVBufferOut  = avfilter_inout_alloc();
	if(pAVBufferOut == NULL){
		av_log(NULL, AV_LOG_ERROR, "avfilter_inout_alloc error\n");
		goto err;
	}

	pAVBufferIn = avfilter_inout_alloc();
	if(pAVBufferIn == NULL){
		av_log(NULL, AV_LOG_ERROR, "avfilter_inout_alloc error\n");
		goto err;
	}

	//绑定Graph的中的滤镜输入输出关系
	pAVBufferOut->name       = av_strdup("in");
	pAVBufferOut->filter_ctx =  pAVBufferSrcCtx;;
	pAVBufferOut->pad_idx    = 0;
	pAVBufferOut->next       = NULL;

	pAVBufferIn->name       = av_strdup("out");
	pAVBufferIn->filter_ctx = pAVBufferSinkCtx;
	pAVBufferIn->pad_idx    = 0;
	pAVBufferIn->next       = NULL;

	char *filters = "anull";//anull的作用是不改变输入输出数据
	
	ret = avfilter_graph_parse_ptr(pAVfilterGraph, filters, &pAVBufferIn, &pAVBufferOut, NULL);
	if (ret < 0){
		av_log(NULL, AV_LOG_ERROR, "avfilter_graph_parse_ptr error\n");
		goto err;
	}

	//检查整个链接是否有问题
	ret = avfilter_graph_config(pAVfilterGraph, NULL);
	if (ret < 0){
		av_log(NULL, AV_LOG_ERROR, "avfilter_graph_config error\n");
		goto err;
	}


	//设置缓冲池的大小，这里就是我们每次取数据的大小
	av_buffersink_set_frame_size(pAVBufferSinkCtx, samples);

	//pAVAudioFrameIn用来做输入源的AVFrame
	pAVAudioFrameIn = av_frame_alloc();
	if(pAVAudioFrameIn == NULL){
		av_log(NULL, AV_LOG_ERROR, "av_frame_alloc pAVAudioFrameIn error\n");
		goto err;
	}
	pAVAudioFrameIn->sample_rate = sample_rate;
	pAVAudioFrameIn->channels = channels;
	pAVAudioFrameIn->format = sample_fmt;
	pAVAudioFrameIn->channel_layout = av_get_default_channel_layout(channels);	

	//pAVAudioFrameOut用来做输出源的AVFrame
	pAVAudioFrameOut = av_frame_alloc();
	if(pAVAudioFrameOut == NULL){
		av_log(NULL, AV_LOG_ERROR, "av_frame_alloc pAVAudioFrameOut error\n");
		goto err;
	}


	PCM_BUFFERSKIN_HANDLE *pHandle = (PCM_BUFFERSKIN_HANDLE *)malloc(sizeof(PCM_BUFFERSKIN_HANDLE));
	if(pHandle == NULL){
		av_log(NULL, AV_LOG_ERROR, "malloc PCM_BUFFERSKIN_HANDLE error\n");
		goto err;
	}
	
	pHandle->pAVfilterGraph = pAVfilterGraph;

	pHandle->pAVBufferSrc= pAVBufferSrc;
	pHandle->pAVBufferSrcCtx = pAVBufferSrcCtx;
	pHandle->pAVBufferIn = pAVBufferIn;
	pHandle->pAVAudioFrameIn = pAVAudioFrameIn;

	pHandle->pAVBufferSink = pAVBufferSink;
	pHandle->pAVBufferSinkCtx = pAVBufferSinkCtx;
	pHandle->pAVBufferOut = pAVBufferOut;
	pHandle->pAVAudioFrameOut = pAVAudioFrameOut;


	pHandle->sample_rate = sample_rate;
	pHandle->channels = channels;
	pHandle->sample_fmt = sample_fmt;
	pHandle->samples = samples;

	*handle = (long)pHandle;

	return 0;
	
err:
	if(pAVfilterGraph)
		avfilter_graph_free(&pAVfilterGraph);

	if(pAVBufferIn)
		avfilter_inout_free(&pAVBufferIn);

	if(pAVBufferOut)
		avfilter_inout_free(&pAVBufferOut);

	if(pAVAudioFrameIn)
		av_frame_free(&pAVAudioFrameIn);

	if(pAVAudioFrameOut)
		av_frame_free(&pAVAudioFrameOut);
	
	return -1;
}

int ffmpeg_pcmskin_put(long handle, char *data, int data_len)
{
	int ret = 0;
	PCM_BUFFERSKIN_HANDLE *pHandle = (PCM_BUFFERSKIN_HANDLE *)handle;
	if(pHandle == NULL){
		return -1;
	}
	
	pHandle->pAVAudioFrameIn->nb_samples = data_len/av_get_bytes_per_sample(pHandle->sample_fmt);
	pHandle->pAVAudioFrameIn->data[0] = data;
	pHandle->pAVAudioFrameIn->linesize[0] = data_len;
	ret = av_buffersrc_add_frame_flags(pHandle->pAVBufferSrcCtx, pHandle->pAVAudioFrameIn, AV_BUFFERSRC_FLAG_PUSH);
	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "buffe src add frame error %s!\n", av_err2str(ret)); 		
		return -1;
	}

	return data_len;
}

int ffmpeg_pcmskin_get(long handle, char *data, int data_len)
{
	int ret = 0;
	PCM_BUFFERSKIN_HANDLE *pHandle = (PCM_BUFFERSKIN_HANDLE *)handle;
	if(pHandle == NULL || data == NULL){
		return -1;
	}

	ret = av_buffersink_get_frame_flags(pHandle->pAVBufferSinkCtx, pHandle->pAVAudioFrameOut, AV_BUFFERSINK_FLAG_NO_REQUEST);
	if(ret < 0){
		if (ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
		    av_log(NULL, AV_LOG_WARNING, "Error in av_buffersink_get_frame_flags(): %s\n", av_err2str(ret));
			return -1;       
		} 
		return 0;
	}

	//空间不够，返回-1
	if(data_len < pHandle->pAVAudioFrameOut->linesize[0]){
		av_log(NULL, AV_LOG_ERROR, "data len is not enough %d %d\n", data_len, pHandle->pAVAudioFrameOut->linesize[0]); 		
		return -1;
	}

	data_len = pHandle->pAVAudioFrameOut->linesize[0];

	memcpy(data, pHandle->pAVAudioFrameOut->data[0], data_len);

	av_frame_unref(pHandle->pAVAudioFrameOut);

	return data_len;
}

int ffmpeg_pcmskin_close(long handle)
{
	int ret = 0;
	PCM_BUFFERSKIN_HANDLE *pHandle = (PCM_BUFFERSKIN_HANDLE *)handle;
	if(pHandle == NULL){
		return -1;
	}

	if(pHandle->pAVfilterGraph)
		avfilter_graph_free(&pHandle->pAVfilterGraph);

	if(pHandle->pAVBufferIn)
		avfilter_inout_free(&pHandle->pAVBufferIn);

	if(pHandle->pAVBufferOut)
		avfilter_inout_free(&pHandle->pAVBufferOut);

	if(pHandle->pAVAudioFrameIn)
		av_frame_free(&pHandle->pAVAudioFrameIn);

	if(pHandle->pAVAudioFrameOut)
		av_frame_free(&pHandle->pAVAudioFrameOut);

	free(pHandle);
	pHandle = NULL;
	
	return 0;
}

