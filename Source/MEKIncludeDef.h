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
#include "libavutil/time.h"
#include "libswscale/swscale.h"
#include "libavformat/avformat.h"
#include "sdl/SDL.h"
}
#include "MEKQueue.h"
#include <process.h>
#define SAFEDELETE(x) if(x){ delete (x); x = NULL;}
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000 
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
	double			video_clock;
	MEKVideo()
	{
		pVideoQueue = new FrameQueue();
		pYUVFrame = av_frame_alloc();
		pRGBFrame = av_frame_alloc();
		video_clock = 0.0f;
	}
}*pMEKVideo;

typedef struct MEKAudio
{
	AVCodecContext	*pAudioContex = NULL;
	AVCodec			*pAudioCodec = NULL;
	AVStream		*pAudioStream = NULL;
	AVPacket		audio_pkt;
	AVFrame			*pFrame = NULL;
	int				nAudioIndex = -1;
	FrameQueue		*pAudioQueue;

	enum AVSampleFormat audio_src_fmt;
	enum AVSampleFormat audio_tgt_fmt;
	int				audio_src_freq;
	int				audio_tgt_freq;
	int				audio_tgt_channel_layout;
	int				audio_src_channel_layout;
	int				audio_src_channels;
	int				audio_tgt_channels;

	SDL_AudioDeviceID	audioDeviceID;
	unsigned int	audio_buf_size;
	unsigned int	audio_buf_index;
	int				audio_pkt_size;
	uint8_t			*audio_pkt_data;
	uint8_t			*audio_buf;
	double			audio_clock;
	DECLARE_ALIGNED(16, uint8_t, audio_buf2)[AVCODEC_MAX_AUDIO_FRAME_SIZE * 4];
	struct SwrContext	*pAudioSwrConvert;

	MEKAudio()
	{
		pAudioQueue = new FrameQueue();
		pFrame = av_frame_alloc();
		audio_buf_size = 0;
		audio_buf_index = 0;
		audio_pkt_size = 0;
		pAudioSwrConvert = NULL;
		audio_clock = 0.0f;
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

		if (mTotalInQueue_ > 1)
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