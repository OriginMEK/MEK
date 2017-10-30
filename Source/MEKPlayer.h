#pragma once
#ifndef MEKPLAYER_H_
#define MEKPLAYER_H_

#include "MEKSpliter.h"
#include "MEKQueue.h"
class MEKPlayer
{
public:
	MEKPlayer();
	~MEKPlayer();

	bool Init(char* pathName);
	bool SetCallBackFn(pCallBackFn fn);
	bool Start();
	bool Stop();

private:
	pCallBackFn mCallBackFn;/*
	FrameQueue<AVPacket> *mSpliterVideoQueue;
	FrameQueue<AVPacket> *mSpliterAudioQueue;*/
	MEKSpliter	*mSpliter;
	MEKParam	*mData;
};
#endif //MEKPLAYER_H_