#include "MEKDecoder.h"
#include "RE_Texture2D.h"
#include "RE_RenderDevice11.h"

using namespace M1000Group;
extern Texture2D	*gDynamicTexuture[10];
extern RenderDevice	*gDevice; 
extern RenderQueue<void*>		gCanRenderTex;
extern RenderQueue<void*>		gUsingTex;
extern RenderQueue<void*>		gNeedMapTex;
static int64_t audio_callback_time = 0;
#define  SDL_AUDIO_BUFFER_SIZE 1024

#define FFMIN(a,b) ((a) > (b) ? (b) : (a))
#define FFMAX(a,b) ((a) > (b) ? (a) : (b))

#define AV_SYNC_THRESHOLD_MIN 0.04
#define AV_SYNC_THRESHOLD_MAX 0.1
#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1

void SetColckAt(Clock* c, double pts, double time, int serial)
{
	c->serial = serial;
	c->pts = pts;
	c->last_updated = time;
	c->pts_drift = c->pts - time;
}

double GetClock(Clock* c)
{
	if (*c->queue_serial != c->serial)
		return NAN;
	double time = av_gettime_relative() / 1000000.0;
	return c->pts_drift + time;
}

double compute_target_delay(double delay, Clock* audio, Clock* video)
{
	double sync_threshold, diff = 0;
	double v = GetClock(video);

	double a = GetClock(audio);		diff = v - a;	sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
	if (!isnan(diff) && fabs(diff) < 3600.0)
	{
		if (diff <= -sync_threshold)
		{
			delay = FFMAX(0, delay + diff);
		}
		else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD)
		{
			delay = delay + diff;
		}
		else if (diff >= sync_threshold)
		{
			delay = 2 * delay;
		}
	}
	return delay;
}

void AudioCallBackFn(void *udata, Uint8 *stream, int len)
{
	MEKDecoder* decoder = (MEKDecoder*)udata;

	PARSERDISPINFO oDisplayInfo;

	/*MEKDecoder* Decoder = (MEKDecoder*)udata;
	if (Decoder->GetData()->audioParam->pAudioQueue->dequeue(&oDisplayInfo))
	{
		int got_frame;
		Decoder->DecodePacket(oDisplayInfo.data, &got_frame, 0);
	}*/

	/*
	将解码后的数据全部拷贝到SDL缓冲区
	*/
	MEKAudio* audioParam = decoder->GetData()->audioParam;
	int len1, audio_size;
	double pts;
	audio_callback_time = av_gettime_relative();
	while (len > 0)
	{
		if (audioParam->audio_buf_index >= audioParam->audio_buf_size)
		{
			int got_frame;	
			//audio_size = decoder->DecodePacket(oDisplayInfo.data, &got_frame, 0);
			audio_size = decoder->DecodeAudio(decoder->GetData(), audioParam->audio_buf, sizeof(audioParam->audio_buf), &pts);
			if (audio_size < 0)
			{
				audioParam->audio_buf_size = 1024;
				memset(audioParam->audio_buf, 0, audioParam->audio_buf_size);
			}
			else
			{
				audioParam->audio_buf_size = audio_size;
			}
			audioParam->audio_buf_index = 0;
		}
		len1 = audioParam->audio_buf_size - audioParam->audio_buf_index;
		if (len1 > len)
			len1 = len;
		memcpy(stream, (uint8_t*)audioParam->audio_buf + audioParam->audio_buf_index, len1);
		len -= len1;
		stream += len1;
		audioParam->audio_buf_index += len1;
	}
    audioParam->audio_write_buf_size = audioParam->audio_buf_size - audioParam->audio_buf_index;
    /*update clock */
    SetColckAt(&audioParam->aClock, audioParam->audio_clock - (double)(2 * audioParam->audio_hw_buf_size + audioParam->audio_write_buf_size)/ audioParam->byte_per_sec, audio_callback_time / 1000000.0, 1);
}

int Snapt(AVCodecContext* m_pCodecCtx, AVFrame* pFrame, int width, int height, int bpp, char* fileName)
{
	FILE *fp;
	
	fp = fopen(fileName, "wb");
	if (!fp)
	{
		DWORD err = ::GetLastError();
		return -1;
	}

	AVFrame* pFrameRGB = av_frame_alloc();

	int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, width, height, 1);
	uint8_t* out_buffer_rgb = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
	av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, out_buffer_rgb, AV_PIX_FMT_RGB24, width, height, 1);

	SwsContext* pSWSCtx = sws_getContext(m_pCodecCtx->width, m_pCodecCtx->height, m_pCodecCtx->pix_fmt, width, height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
	sws_scale(pSWSCtx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, m_pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

	BITMAPFILEHEADER bmpHeader;
	BITMAPINFO	bmpInfo;

	bmpHeader.bfType = ('M' << 8) | 'B';
	bmpHeader.bfType = MAKEWORD(66, 77);
	bmpHeader.bfReserved1 = 0;
	bmpHeader.bfReserved2 = 0;
	bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmpHeader.bfSize = bmpHeader.bfOffBits + width * height * bpp / 8;

	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = width;
	bmpInfo.bmiHeader.biHeight = -height;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = bpp;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biSizeImage = 0;
	bmpInfo.bmiHeader.biXPelsPerMeter = 100;
	bmpInfo.bmiHeader.biYPelsPerMeter = 100;
	bmpInfo.bmiHeader.biClrUsed = 0;
	bmpInfo.bmiHeader.biClrImportant = 0;

	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(pFrameRGB->data[0], width*height*bpp / 8, 1, fp);
	fclose(fp);
	av_frame_unref(pFrameRGB);
	av_free(out_buffer_rgb);
	sws_freeContext(pSWSCtx);
	return 0;
}

MEKDecoder::MEKDecoder()
{
	mData = NULL;
}

MEKDecoder::~MEKDecoder()
{

}

bool MEKDecoder::BeginDecode(MEKParam* mData)
{
	this->mData = mData;
	::_beginthread(StartDecoderVideo, 0, this);
	::_beginthread(StartDecoderAudio, 0, this);
	return true;
}

void MEKDecoder::DecoderVideoThread()
{
	while (true)
	{
		PARSERDISPINFO oDisplayInfo;
		PARSERDISPINFO *pDisplayInfo = NULL;

		if (mData->videoParam->pVideoQueue->dequeue(&oDisplayInfo))
		{
			int got_frame;
			pDisplayInfo = &oDisplayInfo;
			DecodePacket(oDisplayInfo.packet, &got_frame, 0);
			av_packet_unref(oDisplayInfo.packet);
			if (pDisplayInfo)
			{
				//delete pDisplayInfo;
				//pDisplayInfo = NULL;
			}
		}
	}
	MessageBoxA(0, "Error", "DecoderVideoThread Exit!", 0);
}

void MEKDecoder::DecoderAudioThread()
{
	/*
		解码音频并且使用SDL播放
	*/
	CoInitialize(0);

	if (mData->audioParam->nAudioIndex < 0 || mData->audioParam->nAudioIndex >= mData->pFormatContex->nb_streams)
	{
		return;
	}
	
	SDL_AudioSpec	wanted_spec, spec;
	int64_t			wanted_channel_layout = 0;
	int				wanted_nb_channels;
	const int		next_nb_channels[] = { 0, 0, 1, 6, 2, 6, 4, 6 };

	wanted_nb_channels = mData->audioParam->pAudioContex->channels;
	if (!wanted_channel_layout || wanted_nb_channels != av_get_channel_layout_nb_channels(wanted_channel_layout))
	{
		wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
		wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
	}

	wanted_spec.channels = av_get_channel_layout_nb_channels(wanted_channel_layout);
	wanted_spec.freq = mData->audioParam->pAudioContex->sample_rate;
	if (wanted_spec.freq < 0 || wanted_spec.channels < 0)
	{
		return;
	}

	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.silence = 0;
	wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;
	wanted_spec.callback = AudioCallBackFn;
	wanted_spec.userdata = this;

	do 
	{
		mData->audioParam->audioDeviceID = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(0, 0), 0, &wanted_spec, &spec, 0);
		const char* LastError = SDL_GetError();
		wanted_spec.channels = next_nb_channels[FFMIN(7, wanted_spec.channels)];
		if (!wanted_spec.channels)
		{
			break;
		}
	} while (mData->audioParam->audioDeviceID == 0);

	if (spec.format != AUDIO_S16SYS)
	{
		return;
	}

	if (spec.channels != wanted_spec.channels)
	{
		wanted_channel_layout = av_get_default_channel_layout(spec.channels);
		if (!wanted_channel_layout)
		{
			return;
		}
	}

	/*Save the param*/
    mData->audioParam->audio_hw_buf_size = spec.size;
	mData->audioParam->audio_src_fmt = mData->audioParam->audio_tgt_fmt = AV_SAMPLE_FMT_S16;
	mData->audioParam->audio_src_freq = mData->audioParam->audio_tgt_freq = spec.freq;
	mData->audioParam->audio_src_channel_layout = mData->audioParam->audio_tgt_channel_layout = wanted_channel_layout;
	mData->audioParam->audio_src_channels = mData->audioParam->audio_tgt_channels = spec.channels;
    
    mData->audioParam->frame_size = av_samples_get_buffer_size(NULL, spec.channels, 1, mData->audioParam->audio_src_fmt, 1);
    mData->audioParam->byte_per_sec = av_samples_get_buffer_size(NULL, spec.channels, spec.freq, mData->audioParam->audio_src_fmt, 1);
    
	mData->audioParam->pAudioStream->discard = AVDISCARD_DEFAULT;
	memset(&mData->audioParam->audio_pkt, 0, sizeof(mData->audioParam->audio_pkt));
	SDL_PauseAudioDevice(mData->audioParam->audioDeviceID, 0);
	/*while (true)
	{
		PARSERDISPINFO oDisplayInfo;

		if (mData->audioParam->pAudioQueue->dequeue(&oDisplayInfo))
		{
			int got_frame;
			DecodePacket(oDisplayInfo.data, &got_frame, 0);
		}
	}*/
}

int MEKDecoder::DecodePacket(AVPacket *pkt, int *gotFrame, int cached)
{
	int ret = 0;
	int decoded = pkt->size;
	*gotFrame = 0;
	if (pkt->stream_index == mData->videoParam->nVideoIndex)
	{
		AVRational tb = mData->videoParam->pVideoStream->time_base;
		AVRational frame_rate = av_guess_frame_rate(mData->pFormatContex, mData->videoParam->pVideoStream, NULL);

		if (!mData->videoParam->pYUVFrame)
		{
			mData->videoParam->pYUVFrame = av_frame_alloc();
		}

		if (!mData->videoParam->pRGBFrame)
		{
			mData->videoParam->pRGBFrame = av_frame_alloc();
		}

		mData->videoParam->video_clock = av_q2d(mData->videoParam->pVideoStream->time_base) * pkt->pts;

		/*int ret = avcodec_decode_video2(mData->videoParam->pVideoContex, mData->videoParam->pYUVFrame, gotFrame, pkt);
		if (ret < 0)
		{
			return ret;
		}*/
		if (avcodec_send_packet(mData->videoParam->pVideoContex, pkt) == AVERROR(EAGAIN))
		{
			av_log(mData->videoParam->pVideoContex, AV_LOG_ERROR, "Receive_frame and send_packet both returned EAGAIN, which is an API violation.\n");
			return -1;
		}

		int ret = avcodec_receive_frame(mData->videoParam->pVideoContex, mData->videoParam->pYUVFrame);
		if (ret >= 0)
		{
			mData->videoParam->pYUVFrame->pts = mData->videoParam->pYUVFrame->best_effort_timestamp;

			if (true)
			{
#if 1
				if (mData->videoParam->pYUVFrame->width != mData->width || mData->videoParam->pYUVFrame->height != mData->height || mData->videoParam->pYUVFrame->format != mData->pix_fmt)
				{
					return -1;
				}
				//copy to texture;
				if (!mData->videoParam->pImgConvertCtx)
				{
					mData->videoParam->pImgConvertCtx = sws_getContext(mData->width, mData->height, mData->pix_fmt, mData->width,
						mData->height, AV_PIX_FMT_BGR32, SWS_BICUBIC, NULL, NULL, NULL);

					int numBytes = av_image_get_buffer_size(AV_PIX_FMT_BGR32, mData->width, mData->height, 1);
					uint8_t* out_buffer_rgb = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));

					av_image_fill_arrays(mData->videoParam->pRGBFrame->data, mData->videoParam->pRGBFrame->linesize, out_buffer_rgb, AV_PIX_FMT_BGR32, mData->width, mData->height, 1);
				}

				sws_scale(mData->videoParam->pImgConvertCtx, (uint8_t const* const*)mData->videoParam->pYUVFrame->data, mData->videoParam->pYUVFrame->linesize, 0, mData->height, mData->videoParam->pRGBFrame->data, mData->videoParam->pRGBFrame->linesize);

				Texture2D* tex2d = (Texture2D*)gNeedMapTex.GetData();
				if (gDevice && tex2d)
				{
					tex2d->pts = pkt->pts;
					int pitch;
					int stride = mData->videoParam->pRGBFrame->linesize[0];

					void* pData = gDevice->Map(tex2d, pitch);
					for (int h = 0; h < mData->height; h++)
					{
						unsigned char* srcLine = (unsigned char*)(mData->videoParam->pRGBFrame->data[0]) + h * mData->videoParam->pRGBFrame->linesize[0];
						unsigned char* dstLine = (unsigned char*)pData + h * pitch;
						memcpy(dstLine, srcLine, pitch);
					}
					gDevice->Unmap(tex2d);

					gCanRenderTex.SetData(tex2d);
				}
#endif
				AVRational temp = { frame_rate.den, frame_rate.num };
				double duration = (frame_rate.num && frame_rate.den ? av_q2d(temp) : 0);
				double pts = (mData->videoParam->pYUVFrame->pts == AV_NOPTS_VALUE) ? NAN : mData->videoParam->pYUVFrame->pts * av_q2d(tb);

				static double last_pts = pts;
				double last_duration = pts - last_pts <= 0 ? duration : pts - last_pts;
				double delay = compute_target_delay(last_duration, &mData->audioParam->aClock, &mData->videoParam->vClock);

				printf("=====delay:%0.3f\n", delay);
				static double arr[1000] = { 0 };
				static int index = 0;
				//arr[index++] = delay;

				double time = av_gettime_relative() / 1000000.0;
				if (time < mData->frame_timer + delay)
				{
					double t = mData->frame_timer + delay - time;
					sleep(t * 1000);
				}

				mData->frame_timer += delay;
				if (delay > 0 && time - mData->frame_timer > AV_SYNC_THRESHOLD_MAX)
				{
					mData->frame_timer = time;
				}
				SetColckAt(&mData->videoParam->vClock, pkt->pts * av_q2d(mData->videoParam->pVideoStream->time_base), time, 1);
				last_pts = pts;
#if 0
				//begin syn
				double audio_pts = GetAudioClock(mData);
				double video_pts = GetVideoClock(pkt->pts);
				SynchronizeVideo(mData->videoParam->pYUVFrame->repeat_pict, video_pts);
				if (mData->videoParam->video_clock - audio_pts > 0.001)
				{
					double n = (mData->videoParam->video_clock - audio_pts);
					//if (n - 100.0f > 0.000001f)
					//	n = 0.0f;
					char buff[256] = { 0 };
					sprintf(buff, "******times:%lf******\n", n);
					sleep(n);
					OutputDebugStringA(buff);
				}
				//end syn
#endif
			//Sleep(40);
				char file[256] = { 0 };
				SYSTEMTIME st = { 0 };
				GetLocalTime(&st);
				sprintf_s(file, "pictures.bmp", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

				//Snapt(mData->videoParam->pVideoContex, mData->videoParam->pYUVFrame, mData->width, mData->height, 24, file);
			}
		}
	}
	else if (pkt->stream_index == mData->audioParam->nAudioIndex)
	{
#if 0
		int resampled_data_size, wanted_nb_samples;
		if (!mData->audioParam->pFrame)
		{
			mData->audioParam->pFrame = av_frame_alloc();
		}

		ret = avcodec_decode_audio4(mData->audioParam->pAudioContex, mData->audioParam->pFrame, gotFrame, pkt);
		if (ret < 0)
		{
			return -1;
			mData->audioParam->audio_pkt_size = 0;
		}
		mData->audioParam->audio_pkt_data += ret;
		mData->audioParam->audio_pkt_size -= ret;

		if (!gotFrame)
		{
			return -1;
		}
		
		size_t unpadded_linesize = mData->audioParam->pFrame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)mData->audioParam->pFrame->format);
		/*计算解码出来的帧需要的缓冲大小*/
		int decoded_data_size = av_samples_get_buffer_size(NULL, mData->audioParam->pFrame->channels, mData->audioParam->pFrame->nb_samples, (AVSampleFormat)mData->audioParam->pFrame->format, 1);

		int64_t dec_channel_layout = (mData->audioParam->pFrame->channel_layout &&
			mData->audioParam->pFrame->channels == av_get_channel_layout_nb_channels(mData->audioParam->pFrame->channel_layout)) ? mData->audioParam->pFrame->channel_layout : av_get_default_channel_layout(mData->audioParam->pFrame->channels);

		wanted_nb_samples = mData->audioParam->pFrame->nb_samples;

		if (mData->audioParam->pFrame->format != mData->audioParam->audio_src_fmt
			|| dec_channel_layout != mData->audioParam->audio_src_channel_layout
			|| mData->audioParam->pFrame->sample_rate != mData->audioParam->audio_src_freq
			|| (wanted_nb_samples != mData->audioParam->pFrame->nb_samples
				&& !mData->audioParam->pAudioSwrConvert))
		{
			if (mData->audioParam->pAudioSwrConvert)
			{
				swr_free(&mData->audioParam->pAudioSwrConvert);				
			}

			mData->audioParam->pAudioSwrConvert = swr_alloc_set_opts(NULL,
				mData->audioParam->audio_tgt_channel_layout, (AVSampleFormat)mData->audioParam->audio_tgt_fmt,
				mData->audioParam->audio_tgt_freq, dec_channel_layout,
				(AVSampleFormat)mData->audioParam->pFrame->format, mData->audioParam->pFrame->sample_rate,
				0, NULL);
			if (!mData->audioParam->pAudioSwrConvert || swr_init(mData->audioParam->pAudioSwrConvert) < 0)
			{
				return -1;
			}
			mData->audioParam->audio_src_channel_layout = dec_channel_layout;
			mData->audioParam->audio_src_channels = mData->audioParam->pAudioContex->channels;
			mData->audioParam->audio_src_freq = mData->audioParam->pAudioContex->sample_rate;
			mData->audioParam->audio_src_fmt = mData->audioParam->pAudioContex->sample_fmt;
			/*
				调整采样
			*/
			if (mData->audioParam->pAudioSwrConvert)
			{
				const uint8_t **in = (const uint8_t **)mData->audioParam->pFrame->extended_data;
				uint8_t *out[] = { mData->audioParam->audio_buf2 };
				if (wanted_nb_samples != mData->audioParam->pFrame->nb_samples)
				{
					if (swr_set_compensation(mData->audioParam->pAudioSwrConvert,
						(wanted_nb_samples - mData->audioParam->pFrame->nb_samples) * mData->audioParam->audio_tgt_freq / mData->audioParam->pFrame->sample_rate,
						wanted_nb_samples * mData->audioParam->audio_tgt_fmt / mData->audioParam->pFrame->sample_rate) < 0)
					{
					}
				}

				int len2 = swr_convert(mData->audioParam->pAudioSwrConvert, out, sizeof(mData->audioParam->audio_buf2) / mData->audioParam->audio_tgt_channels / av_get_bytes_per_sample(mData->audioParam->audio_tgt_fmt),
					in, mData->audioParam->pFrame->nb_samples);
				if (len2 < 0)
				{
					return -1;
				}
				if (len2 == sizeof(mData->audioParam->audio_buf2) / mData->audioParam->audio_tgt_channels / av_get_bytes_per_sample(mData->audioParam->audio_tgt_fmt))
				{
					swr_init(mData->audioParam->pAudioSwrConvert);
				}
				mData->audioParam->audio_buf = mData->audioParam->audio_buf2;
				resampled_data_size = len2 * mData->audioParam->audio_tgt_channels * av_get_bytes_per_sample(mData->audioParam->audio_tgt_fmt);
			}
			else
			{
				resampled_data_size = decoded_data_size;
				mData->audioParam->audio_buf = mData->audioParam->pFrame->data[0];
			}
			double pts = mData->audioParam->audio_clock;
			//int* pts_ptr;
			//*pts_ptr = pts;
			int n = 2 * mData->audioParam->pAudioContex->channels;
			mData->audioParam->audio_clock += (double)resampled_data_size / (double)(n * mData->audioParam->pAudioContex->sample_rate);
			return resampled_data_size;
		}

#endif

	}
	return decoded;
	
}

int MEKDecoder::DecodeAudio(MEKParam* param, uint8_t *audio_buf, int buf_size, double *pts_ptr)
{
	int ret = AVERROR(EAGAIN);
	int len1, len2, decoded_data_size;
	//AVPacket *pkt = NULL;
	int got_frame = 0;
	int64_t dec_channle_layout;
	int resampled_data_size, wanted_nb_samples;
	double pts;
	if (!mData->audioParam->pFrame)
	{
		mData->audioParam->pFrame = av_frame_alloc();
	}
	AVRational tb;
	PARSERDISPINFO DisplayInfo;
	for (;;)
	{
		AVPacket pkt;
		while (param->audioParam->audio_pkt_size > 0)
		{
			ret = avcodec_receive_frame(mData->audioParam->pAudioContex, mData->audioParam->pFrame);
			//len1 = avcodec_decode_audio4(mData->audioParam->pAudioContex, mData->audioParam->pFrame, &got_frame, DisplayInfo.packet);
			if (ret < 0)
			{
				mData->audioParam->audio_pkt_size = 0;
				break;
			}

			//tb = (AVRational){ 1, mData->audioParam->pFrame->sample_rate };
			tb.num = 1;
			tb.den = mData->audioParam->pFrame->sample_rate;

			if (mData->audioParam->pFrame->pts != AV_NOPTS_VALUE)
			{
				mData->audioParam->pFrame->pts = av_rescale_q(mData->audioParam->pFrame->pts, mData->audioParam->pAudioContex->pkt_timebase, tb);
			}
			else if (mData->audioParam->next_pts != AV_NOPTS_VALUE)
			{
				mData->audioParam->pFrame->pts = av_rescale_q(mData->audioParam->next_pts, mData->audioParam->next_pts_tb, tb);
			}

			if (mData->audioParam->pFrame->pts != AV_NOPTS_VALUE)
			{
				mData->audioParam->next_pts = mData->audioParam->pFrame->pts + mData->audioParam->pFrame->nb_samples;
				mData->audioParam->next_pts_tb = tb;
			}

			if (ret == AVERROR_EOF)
			{
				avcodec_flush_buffers(mData->audioParam->pAudioContex);
				//return 0;
			}

			if (ret >= 0) //means got frame
			{
				//mData->audioParam->audio_pkt_data += len1;
				//mData->audioParam->audio_pkt_size -= len1;


				//decoded_data_size = mData->audioParam->pFrame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)mData->audioParam->pFrame->format);
				/*计算解码出来的帧需要的缓冲大小*/
				decoded_data_size = av_samples_get_buffer_size(NULL, mData->audioParam->pFrame->channels, mData->audioParam->pFrame->nb_samples, (AVSampleFormat)mData->audioParam->pFrame->format, 1);

				int64_t dec_channel_layout = (mData->audioParam->pFrame->channel_layout &&
					mData->audioParam->pFrame->channels == av_get_channel_layout_nb_channels(mData->audioParam->pFrame->channel_layout)) ? mData->audioParam->pFrame->channel_layout : av_get_default_channel_layout(mData->audioParam->pFrame->channels);

				wanted_nb_samples = mData->audioParam->pFrame->nb_samples;

				if (mData->audioParam->pFrame->format != mData->audioParam->audio_src_fmt
					|| dec_channel_layout != mData->audioParam->audio_src_channel_layout
					|| mData->audioParam->pFrame->sample_rate != mData->audioParam->audio_src_freq
					|| (wanted_nb_samples != mData->audioParam->pFrame->nb_samples
						&& !mData->audioParam->pAudioSwrConvert))
				{

					swr_free(&mData->audioParam->pAudioSwrConvert);
					mData->audioParam->pAudioSwrConvert = swr_alloc_set_opts(NULL,
						mData->audioParam->audio_tgt_channel_layout, (AVSampleFormat)mData->audioParam->audio_tgt_fmt,
						mData->audioParam->audio_tgt_freq, dec_channel_layout,
						(AVSampleFormat)mData->audioParam->pFrame->format, mData->audioParam->pFrame->sample_rate,
						0, NULL);

					if (!mData->audioParam->pAudioSwrConvert || swr_init(mData->audioParam->pAudioSwrConvert) < 0)
					{
						swr_free(&mData->audioParam->pAudioSwrConvert);
						return -1;
					}

					mData->audioParam->audio_src_channel_layout = dec_channel_layout;
					mData->audioParam->audio_src_channels = mData->audioParam->pFrame->channels;
					mData->audioParam->audio_src_freq = mData->audioParam->pFrame->sample_rate;
					mData->audioParam->audio_src_fmt = (AVSampleFormat)mData->audioParam->pFrame->format;
				}
				/*
				调整采样
				*/
				if (mData->audioParam->pAudioSwrConvert)
				{
					const uint8_t** in = (const uint8_t**)mData->audioParam->pFrame->extended_data;
					//uint8_t *out[] = { mData->audioParam->audio_buf2 };
					uint8_t** out = &mData->audioParam->audio_buf1;
					int out_count = (int64_t)wanted_nb_samples * mData->audioParam->audio_tgt_freq / mData->audioParam->pFrame->sample_rate + 256;
					int out_size = av_samples_get_buffer_size(NULL, mData->audioParam->audio_tgt_channels, out_count, mData->audioParam->audio_tgt_fmt, 0);
					int len2;
					if (out_size < 0)
					{
						av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size() failed\n");
						return -1;
					}

					if (wanted_nb_samples != mData->audioParam->pFrame->nb_samples)
					{
						if (swr_set_compensation(mData->audioParam->pAudioSwrConvert,
							(wanted_nb_samples - mData->audioParam->pFrame->nb_samples) * mData->audioParam->audio_tgt_freq / mData->audioParam->pFrame->sample_rate,
							wanted_nb_samples * mData->audioParam->audio_tgt_fmt / mData->audioParam->pFrame->sample_rate) < 0)
						{
							av_log(NULL, AV_LOG_ERROR, "swr_set_compensation failed\n");
							return -1;
						}
					}

					av_fast_malloc(&mData->audioParam->audio_buf1, &mData->audioParam->audio_buf1_size, out_size);
					if (!mData->audioParam->audio_buf1)
					{
						return AVERROR(ENOMEM);
					}

					len2 = swr_convert(mData->audioParam->pAudioSwrConvert, out, out_count, in, mData->audioParam->pFrame->nb_samples);
					if (len2 < 0)
					{
						av_log(NULL, AV_LOG_ERROR, "swr_convert() failed\n");
						return -1;
					}
					if (len2 == out_count)
					{
						av_log(NULL, AV_LOG_WARNING, "audio buffer is probably too small\n");
						if (swr_init(mData->audioParam->pAudioSwrConvert) < 0)
						{
							swr_free(&mData->audioParam->pAudioSwrConvert);
						}
					}
					mData->audioParam->audio_buf = mData->audioParam->audio_buf1;
					resampled_data_size = len2 * mData->audioParam->audio_tgt_channels * av_get_bytes_per_sample(mData->audioParam->audio_tgt_fmt);
				}
				else
				{
					resampled_data_size = decoded_data_size;
					mData->audioParam->audio_buf = mData->audioParam->pFrame->data[0];
				}
				double pts = mData->audioParam->pFrame->pts * av_q2d(tb);
				//double duration = av_q2d((AVRational) { mData->audioParam->pFrame->nb_samples, mData->audioParam->pFrame->sample_rate });
				double pos = mData->audioParam->pFrame->pkt_pos;

				*pts_ptr = pts;
				int n = 2 * mData->audioParam->pAudioContex->channels;
				//mData->audioParam->audio_clock += (double)resampled_data_size / (double)(n * mData->audioParam->pAudioContex->sample_rate);
				mData->audioParam->audio_clock = pts + (double)mData->audioParam->pFrame->nb_samples / (double)(mData->audioParam->pFrame->sample_rate);
				return resampled_data_size;
			}
		} 

		if (DisplayInfo.packet)
		{
			av_packet_unref(DisplayInfo.packet);
		}
		if (param->audioParam->audio_pkt_size == 0)
		{
			if (param->audioParam->pAudioQueue->dequeue(&DisplayInfo))
			{
				pkt = *DisplayInfo.packet;
				param->audioParam->audio_pkt_size = pkt.size;
				param->audioParam->audio_pkt_data = pkt.data;

				if (avcodec_send_packet(param->audioParam->pAudioContex, &pkt) == AVERROR(EAGAIN))
				{
					av_log(param->audioParam->pAudioContex, AV_LOG_ERROR, "send packet return EAGAIN\n");
					//av_packet_move_ref(param->audioParam->pAudioContex->pkt)
				}
			}
			av_packet_unref(&pkt);
		}
	}
}

double MEKDecoder::GetAudioClock(MEKParam* param)
{
	double pts;
	int hw_buf_size, byte_per_sec, n;

	pts = param->audioParam->audio_clock;
	hw_buf_size = param->audioParam->audio_buf_size - param->audioParam->audio_buf_index;
	byte_per_sec = 0;
	n = param->audioParam->pAudioContex->channels * 2;

	byte_per_sec = param->audioParam->pAudioContex->sample_rate * n;
	if (byte_per_sec)
	{
		pts -= (double)hw_buf_size / byte_per_sec;
	}
	return pts;
}

double MEKDecoder::GetVideoClock(long long value)
{
	double video_pts = av_q2d(mData->videoParam->pVideoStream->time_base) * value;
	return video_pts;
}

double MEKDecoder::SynchronizeVideo(int repeat_pict, double pts)
{
	double frame_deley;
	if (pts != 0)
	{
		mData->videoParam->video_clock = pts;
	}
	else
	{
		pts = mData->videoParam->video_clock;
	}
	frame_deley = av_q2d(mData->videoParam->pVideoStream->time_base);
	frame_deley += repeat_pict * (frame_deley * 0.5);
	mData->videoParam->video_clock += frame_deley;
	return pts;
}
