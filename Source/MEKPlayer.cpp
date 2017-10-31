#include "MEKPlayer.h"

MEKPlayer::MEKPlayer()
{
	mCallBackFn = NULL;
	mSpliter = NULL;
}

MEKPlayer::~MEKPlayer()
{
	SAFEDELETE(mSpliter);
		
}

bool MEKPlayer::Init(char* pathName)
{
	/*
	´´½¨²ð°üÆ÷£¬½âÂëÆ÷£¬äÖÈ¾Æ÷
	*/
	mData = new MEKParam();
	if (!mSpliter)
	{
		mSpliter = new MEKSpliter();
	}
	mSpliter->AnalyzeFile(pathName, mData);

	if (!mDecoder)
	{
		mDecoder = new MEKDecoder();
	}	
	mDecoder->BeginDecode(mData);

	if (!mRender)
	{
		mRender = new MEKRender();
	}
	return false;
}

bool MEKPlayer::SetCallBackFn(pCallBackFn fn)
{
	mCallBackFn = fn;
	return false;
}

bool MEKPlayer::SetWnd(HWND hWnd)
{
	if (mRender)
	{
		mRender->SetRenderWnd(hWnd);
	}
	return false;
}

bool MEKPlayer::Start()
{
	return false;
}

bool MEKPlayer::Stop()
{
	return false;
}
