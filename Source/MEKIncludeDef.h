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
#include <queue>
#define SAFEDELETE(x) if(x){ delete (x); x = NULL;}
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000 
typedef struct Clock
{
    double pts;
    double pts_drift;
    double last_updated;
	int serial;           /* clock is based on a packet with this serial */
	int* queue_serial;    /* pointer to the current packet queue serial, used for obsolete clock detection */
};

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
    Clock           vClock;
	MEKVideo()
	{
		pVideoQueue = new FrameQueue();
		pYUVFrame = av_frame_alloc();
		pRGBFrame = av_frame_alloc();
		video_clock = 0.0f;
		vClock.pts = NAN;
		vClock.serial = -1;
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
    int             frame_size;
    int             byte_per_sec;

	SDL_AudioDeviceID	audioDeviceID;
    int             audio_hw_buf_size;
    Clock           aClock;
    unsigned int    audio_write_buf_size; /* in bytes */
	unsigned int	audio_buf_size;
	unsigned int	audio_buf_index;
	int				audio_pkt_size;
	uint8_t			*audio_pkt_data;
	uint8_t			*audio_buf;
	uint8_t			* audio_buf1;
	unsigned int audio_buf1_size;
	double			audio_clock;
	DECLARE_ALIGNED(16, uint8_t, audio_buf2)[AVCODEC_MAX_AUDIO_FRAME_SIZE * 4];
	struct SwrContext	*pAudioSwrConvert;

	int64_t next_pts;
	AVRational next_pts_tb;
	MEKAudio()
	{
		pAudioQueue = new FrameQueue();
		pFrame = av_frame_alloc();
		audio_buf_size = 0;
		audio_buf_index = 0;
		audio_pkt_size = 0;
		pAudioSwrConvert = NULL;
		audio_clock = 0.0f;
		audio_buf1 = NULL;
		audio_buf1_size = 0;
		next_pts = AV_NOPTS_VALUE;
		aClock.pts = NAN;
		aClock.serial = -1;
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
	double frame_timer;
	MEKParam()
	{
		videoParam = new MEKVideo();
		audioParam = new MEKAudio();
	}
}*pMEKParam;

template <typename T>
class RenderQueue
{
private:
	std::queue<T> _data;
	int				_size;
	CRITICAL_SECTION cs;
public:
	RenderQueue():_size(10)
	{
		::InitializeCriticalSection(&cs);
	}

	~RenderQueue()
	{
		::DeleteCriticalSection(&cs);
	}

	T GetData(bool bWaite = true)
	{
		T ret = NULL;
		do 
		{
			bool bGetTex = false;
			::EnterCriticalSection(&cs);
			if (_data.size() > 0)
			{
				ret = _data.front();
				_data.pop();
				bGetTex = true;
			}
			::LeaveCriticalSection(&cs);

			if (!bWaite)
				break;

			if (bGetTex)
				break;

			sleep(1);
		} while (true);

		return ret;
	}

	void SetData(T data)
	{
		::EnterCriticalSection(&cs);
		_data.push(data);
		::LeaveCriticalSection(&cs);
	}

	int GetSize()
	{
		return _data.size();
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