#pragma once
#include "MEKIncludeDef.h"
/*
	½âÂëÆ÷
*/
#ifndef MEKDECODER_H_
#define MEKDECODER_H_
class MEKDecoder
{
public:
	MEKDecoder();
	~MEKDecoder();
	bool BeginDecode(MEKParam* mData);
	static void StartDecoderVideo(void* param)
	{
		MEKDecoder* decoder = (MEKDecoder*)param;
		decoder->DecoderVideoThread();
	}

	static void StartDecoderAudio(void* param)
	{
		MEKDecoder* decoder = (MEKDecoder*)param;
		decoder->DecoderAudioThread();
	}
	MEKParam* GetData() { return mData; }

	int DecodePacket(AVPacket *pkt, int *gotFrame, int cached);
	int DecodeAudio(MEKParam* param, uint8_t *audio_buf, int buf_size, double *pts_ptr);
	double GetAudioClock(MEKParam* param);
	double GetVideoClock(long long value);
	double SynchronizeVideo(int repeat_pict, double pts);
private:
	MEKParam* mData;
	void DecoderVideoThread();
	void DecoderAudioThread();
};
#endif //MEKDECODER_H_