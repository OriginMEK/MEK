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
<<<<<<< HEAD
	static void StartDecoderVideo(void* param)
	{
		MEKDecoder* decoder = (MEKDecoder*)param;
		decoder->DecoderVideoThread();
	}

	static void StartDecoderAudio(void* param)
	{
		MEKDecoder* decoder = (MEKDecoder*)param;
=======
	static void Start(void* param)
	{
		MEKDecoder* decoder = (MEKDecoder*)param;
		decoder->DecoderVideoThread();
>>>>>>> b22f8a189a2ab8ef855b339c2f2e8419fcfb1eaf
		decoder->DecoderAudioThread();
	}
private:
	MEKParam* mData;
	void DecoderVideoThread();
	void DecoderAudioThread();
	int DecodePacket(AVPacket pkt, int *gotFrame, int cached);
};
#endif //MEKDECODER_H_