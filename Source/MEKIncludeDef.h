#pragma once
#ifndef MEKINCLUDEDEF_H_
#define MEKINCLUDEDEF_H_

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libavutil/avutil.h"
}
#include "MEKQueue.h"
#include <process.h>
#define SAFEDELETE(x) if(x){ delete (x); x = NULL;}


typedef struct MEKVideo
{
	AVCodecContext	*pVideoContex = NULL;
	AVCodec			*pVideoCodec = NULL;
	AVStream		*pVideoStream = NULL;
	int				nVideoIndex = -1;
	FrameQueue		videoQueue;
	MEKVideo()
	{
	}
}*pMEKVideo;

typedef struct MEKAudio
{
	AVCodecContext	*pAudioContex = NULL;
	AVCodec			*pAudioCodec = NULL;
	AVStream		*pAudioStream = NULL;
	int				nAudioIndex = -1;
	FrameQueue		audioQueue;

	MEKAudio()
	{
		
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