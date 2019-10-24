#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

typedef struct _PARSER_HANDLE_{
	AVCodecParserContext * parsetCtx;
	AVCodec *codec;
	AVCodecContext *avCtx;
}PARSER_HANDLE;

int ffmpeg_parser_open(long *handle, int codec_id)
{
	int ret = 0;

	AVCodec *codec = avcodec_find_decoder(codec_id);
	if (!codec) {
		av_log(NULL, AV_LOG_ERROR, "Codec not found\n");
		ret = -1;
		goto err;
	}

	AVCodecContext *avCtx = avcodec_alloc_context3(codec);
	if (!avCtx) {
		av_log(NULL, AV_LOG_ERROR, "Could not allocate video codec context\n");
		ret =  -1;
		goto err;
	}

	/* open it */
	if (avcodec_open2(avCtx, codec, NULL) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Could not open codec\n");
		ret =  -1;
		goto err;
	}

	AVCodecParserContext * parsetCtx = av_parser_init(codec->id);
	if (!parsetCtx) {
		av_log(NULL, AV_LOG_ERROR, "parser not found\n");
		ret =  -1;
		goto err;
	}

	PARSER_HANDLE *pHandle = (PARSER_HANDLE *)malloc(sizeof(PARSER_HANDLE));
	pHandle->parsetCtx = parsetCtx;
	pHandle->codec = codec;
	pHandle->avCtx = avCtx;

	*handle = (long)pHandle;

	return 0;
	
err:
	return ret;
}

int ffmpeg_parser_parse2(long handle, uint8_t **poutbuf, int *poutbuf_size, const uint8_t *data, int data_len)
{
	PARSER_HANDLE *pHandle = (PARSER_HANDLE *)handle;
	if(!pHandle){
		return -1;
	}
	
	return av_parser_parse2(pHandle->parsetCtx, pHandle->avCtx, poutbuf, poutbuf_size, data, data_len, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
}

int ffmpeg_parser_close(long handle)
{

	PARSER_HANDLE *pHandle = (PARSER_HANDLE *)handle;
	if(!pHandle){
		return -1;
	}

    av_parser_close(pHandle->parsetCtx);

	avcodec_close(pHandle->avCtx);

	avcodec_free_context(&pHandle->avCtx);

	free(pHandle);
	pHandle = NULL;

	return 0;
}

