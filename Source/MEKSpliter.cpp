#include "MEKSpliter.h"

MEKSpliter::MEKSpliter()
{

}

MEKSpliter::~MEKSpliter()
{

}

static void init_clock(Clock* c, int* queue_serial)
{
	double time = av_gettime_relative() / 1000000.0;
	c->pts = NAN;
	c->queue_serial = queue_serial;
	c->last_updated = time;
	c->pts_drift = c->pts - time;
	c->serial = -1;
}
bool MEKSpliter::AnalyzeFile(char* fileName, MEKParam* data)
{
	mData = data;

	init_clock(&mData->videoParam->vClock, &mData->videoParam->pVideoQueue->serial);
	init_clock(&mData->audioParam->aClock, &mData->audioParam->pAudioQueue->serial);

	AVFormatContext* formatContex = NULL;
	av_register_all();
	avformat_network_init();
	data->pFormatContex = avformat_alloc_context();

	if (avformat_open_input(&data->pFormatContex, fileName, NULL, NULL) != 0)
	{
		return false;
	}

	if (avformat_find_stream_info(data->pFormatContex, NULL) < 0)
	{
		return false;
	}

	av_dump_format(data->pFormatContex, 0, fileName, false);
	
	if (OpenCodecContext(&(data->videoParam->nVideoIndex), &(data->videoParam->pVideoContex), data->pFormatContex, AVMEDIA_TYPE_VIDEO) >= 0)
	{
		data->videoParam->pVideoStream = data->pFormatContex->streams[data->videoParam->nVideoIndex];
		data->width = data->videoParam->pVideoContex->width;
		data->height = data->videoParam->pVideoContex->height;
		data->pix_fmt = data->videoParam->pVideoContex->pix_fmt;
	}

	if (OpenCodecContext(&(data->audioParam->nAudioIndex), &(data->audioParam->pAudioContex), data->pFormatContex, AVMEDIA_TYPE_AUDIO) >= 0)
	{
		data->audioParam->pAudioStream = data->pFormatContex->streams[data->audioParam->nAudioIndex];
	}
	//没有视屏轨
	if (data->videoParam->nVideoIndex == -1)
	{
		return false;
	}

	if (data->audioParam->nAudioIndex != -1)
	{
		if (SDL_Init(SDL_INIT_AUDIO))
			return false;
	}

	//开启拆包线程
	::_beginthread(Start, 0, this);
}

int MEKSpliter::OpenCodecContext(int *streamIndex, AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type)
{
	int ret, stream_index;
	AVStream *st;
	AVCodec *dec = NULL;
	AVDictionary *opts = NULL;

	ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
	if (ret < 0)
	{
		return ret;
	}
	else
	{
		stream_index = ret;
		st = fmt_ctx->streams[stream_index];

		

		*dec_ctx = avcodec_alloc_context3(dec);
		if (!*dec_ctx)
		{
			return AVERROR(ENOMEM);
		}

		if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0)
		{
			return ret;
		}
		(*dec_ctx)->pkt_timebase = st->time_base;

		dec = avcodec_find_decoder((*dec_ctx)->codec_id);
		//av_dict_set(&opts, "refcounted_frames", refcount ? "1" ： "0",0);
		if ((ret = avcodec_open2(*dec_ctx, dec, NULL)) < 0)
		{
			return ret;
		}
		*streamIndex = stream_index;
	}
	return 0;
}

void MEKSpliter::SpliterThread()
{
	AVFrame *frame = av_frame_alloc();
	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = NULL;
	pkt.size = 0;

	while (true)
	{
		PARSERDISPINFO*  param = new PARSERDISPINFO;		

		int ret = av_read_frame(mData->pFormatContex, param->packet);
		if (ret < 0 )
		{
			char errbuf[1024] = { 0 };
			av_make_error_string(errbuf, 1024, ret);
			break;
		}

		if (param->packet->stream_index == mData->videoParam->nVideoIndex)
		{
			static int indexVideo = 0;
			param->picture_index = (indexVideo++) % 200;
			mData->videoParam->pVideoQueue->enqueue(param);
		}
		else if (param->packet->stream_index == mData->audioParam->nAudioIndex)
		{
			static int indexAudio = 0;
			param->picture_index = (indexAudio++) % 200;
			mData->audioParam->pAudioQueue->enqueue(param);
		}
		else
		{
			delete param;
			param = NULL;
		}
	}
}
