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
private:
	MEKParam* mData;
	void DecoderVideoThread();
	void DecoderAudioThread();
	int DecodePacket(AVPacket pkt, int *gotFrame, int cached);
};
#endif //MEKDECODER_H_