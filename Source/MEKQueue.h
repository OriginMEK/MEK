#pragma once
#ifndef MEKQUEUE_H_
#define MEKQUEUE_H_

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libavutil/avutil.h"
}
#include <windows.h>
#define sleep(x) Sleep(x)

////template<typename T>

typedef struct _PARSERDISPINFO
{
	int picture_index;         /**<                 */
	int progressive_frame;     /**<                 */
	int top_field_first;       /**<                 */
	int repeat_first_field;    /**< Number of additional fields (1=ivtc, 2=frame doubling, 4=frame tripling, -1=unpaired field)  */
	AVPacket data;        /**<     */
} PARSERDISPINFO;

class FrameQueue
{
public:
	static const unsigned int cnMaximumSize = 20;
private:
	void signalStatusChange();

	HANDLE hEvent_;
	CRITICAL_SECTION			oCriticalSection_;
	volatile int				nReadPosition_;
	volatile int				nFramesInQueue_;
	PARSERDISPINFO				aDisplayQueue_[cnMaximumSize];
	volatile int				aIsFrameInUse_[cnMaximumSize];
	volatile int				bEndOfDecode_;
public:
	FrameQueue();

	virtual ~FrameQueue();

	void waitForQueueUpdate();

	void enter_CS(CRITICAL_SECTION *pCS);

	void leave_CS(CRITICAL_SECTION *pCS);

	void set_event(HANDLE event);

	void reset_event(HANDLE event);

	void enqueue(const PARSERDISPINFO *pPicParams);

	bool dequeue(PARSERDISPINFO *pDisplayInfo);
	
	void releaseFrame(const PARSERDISPINFO *pPicParams);

	bool isInUse(int nPictureIndex) const;

	bool isEndOfDecode() const;

	void endDecode();

	bool isEmpty() { return nFramesInQueue_ == 0; }

	bool waitUntilFrameAvailable(int nPictureIndex);
};
#endif //MEKQUEUE_H_
