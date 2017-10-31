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
	static void Start(void* param)
	{
		MEKDecoder* decoder = (MEKDecoder*)param;
		decoder->DecoderVideoThread();
		decoder->DecoderAudioThread();
	}
private:
	MEKParam* mData;
	void DecoderVideoThread();
	void DecoderAudioThread();
	int DecodePacket(AVPacket pkt, int *gotFrame, int cached);
};
#endif //MEKDECODER_H_