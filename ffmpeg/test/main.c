#include <windows.h>
#include <stdio.h>

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/dict.h"


void listAllMuxer(void)
{
	const AVOutputFormat* muxer = NULL;
	void* muxer_opaque = NULL;

	do {
		muxer = av_muxer_iterate(&muxer_opaque);
		if (muxer == NULL) {
			break;
		}

		printf("[%s] [%s]\n", muxer->name, muxer->long_name);
	} while (1);
}

void listAllDemuxer(void)
{
	const AVInputFormat* demuxer = NULL;
	void* demuxer_opaque = NULL;

	do {
		demuxer = av_demuxer_iterate(&demuxer_opaque);
		if (demuxer == NULL) {
			break;
		}

		printf("[%s][%s] [%s]\n", 
			demuxer->name,
			demuxer->long_name,
			demuxer->extensions);
	} while (1);
}

void listAllDecoder(void)
{
	const AVCodec* codec = NULL;
	void* iter = NULL;

	do {
		codec = av_codec_iterate(&iter);
		if (codec == NULL) {
			break;
		}

		switch (codec->type) {
		case AVMEDIA_TYPE_VIDEO:
			//printf("[Video] [%s] [%d] [%d]\n", 
			//	codec->name,
			//	av_codec_is_encoder(codec),
			//	av_codec_is_decoder(codec));
			break;

		case AVMEDIA_TYPE_AUDIO:
			//printf("[Audio] [%s] [%d] [%d]\n",
			//	codec->name,
			//	av_codec_is_encoder(codec),
			//	av_codec_is_decoder(codec));
			break;

		case AVMEDIA_TYPE_SUBTITLE:
			//printf("[Subtitle] [%s] [%d] [%d]\n",
			//	codec->name,
			//	av_codec_is_encoder(codec),
			//	av_codec_is_decoder(codec));
			break;

		default:
			printf("codec->type = %d\n", codec->type);
			printf("[Subtitle] [%s] [%d] [%d]\n",
				codec->name,
				av_codec_is_encoder(codec),
				av_codec_is_decoder(codec));
			break;
		}

	} while (1);
}

void listAllProtocol(int output)
{
	const char* protocol_name = NULL;
	void* opaque = NULL;

	avformat_network_init();

	do {
		protocol_name = avio_enum_protocols(&opaque, output);
		if (protocol_name == NULL) {
			break;
		}

		printf("%s \n", protocol_name);

	} while (1);

	avformat_network_deinit();
}

void probe(void)
{
	int ret = 0;
	AVFormatContext* fmt_ctx = NULL;
	const char* filename = "jinbidao.rmvb";
	int i = 0;

	ret = avformat_open_input(&fmt_ctx, filename, NULL, NULL);
	if (ret < 0) {
		printf("[error] avformat_open_input");
		return;
	}

	ret = avformat_find_stream_info(fmt_ctx, NULL);
	if (ret < 0) {
		printf("[error] avformat_find_stream_info");
		return;
	}

	//// format
	//printf("[Input Format]\n");
	//printf("%s(%s)\n\n", fmt_ctx->iformat->name, fmt_ctx->iformat->long_name);

	//// duration
	//printf("duration(in sec): %lld\n", fmt_ctx->duration / AV_TIME_BASE);

	//// metadata
	//printf("metadata:\n");
	//{
	//	AVDictionaryEntry* tag = NULL;
	//	while(1) {
	//		tag = av_dict_get(fmt_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX);
	//		if (tag == NULL) {
	//			break;
	//		}
	//		printf("[%s] = [%s]\n", tag->key, tag->value);
	//	}
	//}
	//printf("\n");

	// stream
	printf("[Stream]\n");
	printf("nb_streams: %d\n", fmt_ctx->nb_streams);
	for (i = 0; i < fmt_ctx->nb_streams; i++) {
		AVStream* stream = fmt_ctx->streams[i];
		AVCodecParameters* codecpar = stream->codecpar;
		const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id);

		printf("Stream%d:\n", i);
		printf("Type: %s\n", av_get_media_type_string(codecpar->codec_type));
		printf("Codec: %s(%s)\n", codec->name, avcodec_get_name(codecpar->codec_id));


		printf("\n");
	}


	avformat_close_input(&fmt_ctx);
}




int main(void)
{
	//{
	//	unsigned int ver = 0;
	//	ver = avcodec_version();
	//	printf("ver = 0x%08X\n", ver);
	//}

	//{
	//	const char* verinfo = av_version_info();
	//	printf("%s\n", verinfo);
	//}

	//{
	//	const char* cfg = avcodec_configuration();
	//	printf("%s\n", cfg);
	//}

	//listAllMuxer();

	//listAllDemuxer();

	//listAllDecoder();

	//listAllProtocol(0);
	//listAllProtocol(1);

	probe();
	
	return 0;
}