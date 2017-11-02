#pragma once
#ifndef MEKINCLUDEDEF_H_
#define MEKINCLUDEDEF_H_

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#include "libavformat/avformat.h"
#include "sdl/SDL.h"
//#include "sdl/SDL_thread.h"
}
#include "MEKQueue.h"
#include <process.h>
#define SAFEDELETE(x) if(x){ delete (x); x = NULL;}

typedef struct MEKVideo
{
	AVCodecContext	*pVideoContex = NULL;
	AVCodec			*pVideoCodec = NULL;
	AVStream		*pVideoStream = NULL;
	struct SwsContext *pImgConvertCtx = NULL;
	AVFrame			*pYUVFrame = NULL;
	AVFrame			*pRGBFrame = NULL;
	int				nVideoIndex = -1;
	FrameQueue		*pVideoQueue;
	MEKVideo()
	{
		pVideoQueue = new FrameQueue();
		pYUVFrame = av_frame_alloc();
		pRGBFrame = av_frame_alloc();
	}
}*pMEKVideo;

typedef struct MEKAudio
{
	AVCodecContext	*pAudioContex = NULL;
	AVCodec			*pAudioCodec = NULL;
	AVStream		*pAudioStream = NULL;
	int				nAudioIndex = -1;
	FrameQueue		*pAudioQueue;

	MEKAudio()
	{
		pAudioQueue = new FrameQueue();
	}
}*pMEKAudio;

typedef struct MEKParam 
{
	int width = 0;
	int height = 0;
	AVPixelFormat pix_fmt;
	AVFormatContext *pFormatContex = NULL;
	MEKVideo* videoParam = NULL;
	MEKAudio* audioParam = NULL;
	MEKParam()
	{
		videoParam = new MEKVideo();
		audioParam = new MEKAudio();
	}
}*pMEKParam;

class RenderQueue
{
private:
	int mData[10] = {0,1,2,3,4,5,6,7,8,9 };
	volatile int nReadPosotion_;
	volatile int mTotalInQueue_;
	volatile int bEndofDecode_;
	CRITICAL_SECTION cs;
public:
	RenderQueue():
		nReadPosotion_(0),
		mTotalInQueue_(0), 
		bEndofDecode_(0)
	{
		::InitializeCriticalSection(&cs);
	}

	~RenderQueue()
	{
		::DeleteCriticalSection(&cs);
	}

	int GetWriteIndex()
	{
		int ret = -1;
		do 
		{
			bool bPlacedFrame = false;
			::EnterCriticalSection(&cs);

			if (mTotalInQueue_ < 10)
			{
				int iWritePosition = (nReadPosotion_ + mTotalInQueue_) % 10;
				ret = iWritePosition;
				mTotalInQueue_++;
				bPlacedFrame = true;
			}
			::LeaveCriticalSection(&cs);

			if (bPlacedFrame)
				break;

			sleep(1);   // Wait a bit
		} while (!bEndofDecode_);
		return ret;
	}

	int GetReadIndex()
	{
		int ret = -1;
		bool bHaveNewFrame = false;
		::EnterCriticalSection(&cs);

		if (mTotalInQueue_ > 0)
		{
			int iEntry = nReadPosotion_;
			ret = iEntry;
			nReadPosotion_ = (iEntry + 1) % 10;
			mTotalInQueue_--;
			bHaveNewFrame = true;
		}
		else
		{
			ret = nReadPosotion_ - 1;
		}
		::LeaveCriticalSection(&cs);
		return ret;
	}

};
/*
	时间 : 2017-10-27 17:30:02
	作者	: zhaohu
	作用 : 定义共用的结构体定义等等
*/

/*
Call Back Function
*/
typedef void (*pCallBackFn)(void* data, int width, int heigth, int pitch);

#endif //MEKINCLUDEDEF_H_