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
	创建拆包器，解码器，渲染器
	*/
	mData = new MEKParam();
	if (!mSpliter)
	{
		mSpliter = new MEKSpliter();
	}
<<<<<<< HEAD
	//mSpliter->AnalyzeFile(pathName, mData);
=======
	mSpliter->AnalyzeFile(pathName, mData);
>>>>>>> b22f8a189a2ab8ef855b339c2f2e8419fcfb1eaf

	if (!mDecoder)
	{
		mDecoder = new MEKDecoder();
	}	
	mDecoder->BeginDecode(mData);

<<<<<<< HEAD
	if (!mRender)
	{
		mRender = new MEKRender();
	}
=======
>>>>>>> b22f8a189a2ab8ef855b339c2f2e8419fcfb1eaf
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
