#include "MEKDecoder.h"

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
<<<<<<< HEAD
	::_beginthread(StartDecoderVideo, 0, this);
	::_beginthread(StartDecoderAudio, 0, this);
=======
	::_beginthread(Start, 0, this);
>>>>>>> b22f8a189a2ab8ef855b339c2f2e8419fcfb1eaf
	return true;
}

void MEKDecoder::DecoderVideoThread()
{
	while (true)
	{
		PARSERDISPINFO oDisplayInfo;

<<<<<<< HEAD
		if (mData->videoParam->pVideoQueue->dequeue(&oDisplayInfo))
		{
			int got_frame;
			DecodePacket(oDisplayInfo.data, &got_frame, 0);
=======
		if (mData->videoParam->videoQueue.dequeue(&oDisplayInfo))
		{

>>>>>>> b22f8a189a2ab8ef855b339c2f2e8419fcfb1eaf
		}
	}
}

void MEKDecoder::DecoderAudioThread()
{
	while (true)
	{
<<<<<<< HEAD
		PARSERDISPINFO oDisplayInfo;

		if (mData->audioParam->pAudioQueue->dequeue(&oDisplayInfo))
		{
			int got_frame;
			DecodePacket(oDisplayInfo.data, &got_frame, 0);
		}
=======
>>>>>>> b22f8a189a2ab8ef855b339c2f2e8419fcfb1eaf
	}
}

int MEKDecoder::DecodePacket(AVPacket pkt, int *gotFrame, int cached)
{
	int ret = 0;
	int decoded = pkt.size;
<<<<<<< HEAD
	AVFrame *frame = av_frame_alloc();
	*gotFrame = 0;
	if (pkt.stream_index == mData->videoParam->nVideoIndex)
	{
		ret = avcodec_send_packet(mData->videoParam->pVideoContex, &pkt);
		if (ret != 0) return ret;
		ret = avcodec_receive_frame(mData->videoParam->pVideoContex, frame);
		if (ret != 0) return ret;
		/*ret = avcodec_decode_video2(mData->videoParam->pVideoContex, frame, gotFrame, &pkt);
		if (ret < 0)
		{
			return ret;
		}*/

		//if (*gotFrame)
=======
	AVFrame *frame = new AVFrame;
	*gotFrame = 0;
	if (pkt.stream_index == mData->videoParam->nVideoIndex)
	{
		//ret = avcodec_decode_video2(mData->videoParam->pVideoContex, frame, gotFrame, &pkt);
		if (ret < 0)
		{
			return ret;
		}

		if (*gotFrame)
>>>>>>> b22f8a189a2ab8ef855b339c2f2e8419fcfb1eaf
		{
			if (frame->width != mData->width || frame->height != mData->height || frame->format != mData->pix_fmt)
			{
				return -1;
			}
			//copy to texture;
		}
	}
	else if (pkt.stream_index == mData->audioParam->nAudioIndex)
	{
<<<<<<< HEAD
		ret = avcodec_send_packet(mData->videoParam->pVideoContex, &pkt);
		if (ret != 0) return ret;

		ret = avcodec_receive_frame(mData->videoParam->pVideoContex, frame);
		if (ret != 0) return ret;
=======
>>>>>>> b22f8a189a2ab8ef855b339c2f2e8419fcfb1eaf
		//ret = avcodec_decode_audio4(mData->audioParam->pAudioContex, frame, gotFrame, &pkt);
		if (ret < 0)
		{
			return ret;
		}
		decoded = FFMIN(ret, pkt.size);
		
		if (*gotFrame)
		{
			size_t unpadded_linesize = frame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)frame->format);
		}

	}
	return decoded;
	
}
