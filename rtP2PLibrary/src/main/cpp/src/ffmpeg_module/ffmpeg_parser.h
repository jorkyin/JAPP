#ifndef _FFMPEG_PARSER_H_
#define _FFMPEG_PARSER_H_

int ffmpeg_parser_open(long *handle, int codec_id);
int ffmpeg_parser_parse2(long handle, uint8_t **poutbuf, int *poutbuf_size, const uint8_t *data, int data_len);
int ffmpeg_parser_close(long handle);

#endif
