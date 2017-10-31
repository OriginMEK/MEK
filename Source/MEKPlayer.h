#pragma once
#ifndef MEKPLAYER_H_
#define MEKPLAYER_H_

#include "MEKSpliter.h"
#include "MEKDecoder.h"
#include "MEKRender.h"
#include "MEKQueue.h"
class MEKPlayer
{
public:
	MEKPlayer();
	~MEKPlayer();

	bool Init(char* pathName);
	bool SetCallBackFn(pCallBackFn fn);
	bool SetWnd(HWND hWnd);
	bool Start();
	bool Stop();

private:
	pCallBackFn mCallBackFn;
	MEKSpliter	*mSpliter;
	MEKDecoder	*mDecoder;
	MEKRender	*mRender;
	MEKParam	*mData;
};
#endif //MEKPLAYER_H_