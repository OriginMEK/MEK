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
	::_beginthread(Start, 0, this);
	return true;
}

void MEKDecoder::DecoderVideoThread()
{
	while (true)
	{
		PARSERDISPINFO oDisplayInfo;

		if (mData->videoParam->videoQueue.dequeue(&oDisplayInfo))
		{

		}
	}
}

void MEKDecoder::DecoderAudioThread()
{
	while (true)
	{
	}
}

int MEKDecoder::DecodePacket(AVPacket pkt, int *gotFrame, int cached)
{
	int ret = 0;
	int decoded = pkt.size;
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
