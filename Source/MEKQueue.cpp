#include "MEKQueue.h"

//template<typename T>
FrameQueue::FrameQueue() : hEvent_(0)
	,nReadPosition_(0)
	,nFramesInQueue_(0)
	,bEndOfDecode_(0)
	,serial(1)
{
	hEvent_ = CreateEvent(NULL, false, false, NULL);
	InitializeCriticalSection(&oCriticalSection_);
	memset(aDisplayQueue_, 0, cnMaximumSize * sizeof(PARSERDISPINFO));
	memset((void *)aIsFrameInUse_, 0, cnMaximumSize * sizeof(int));
}


//template<typename T>
FrameQueue::~FrameQueue()
{
	DeleteCriticalSection(&oCriticalSection_);
	CloseHandle(hEvent_);
}


//template<typename T>
void FrameQueue::waitForQueueUpdate()
{
	WaitForSingleObject(hEvent_, 10);
}

//template<typename T>
void FrameQueue::endDecode()
{
	bEndOfDecode_ = true;
	signalStatusChange();  // Signal for the display thread
}

//template<typename T>
bool FrameQueue::isEndOfDecode() const
{
	return (0 != bEndOfDecode_);
}

//template<typename T>
bool FrameQueue::isInUse(int nPictureIndex) const
{
	//assert(nPictureIndex >= 0);
	//assert(nPictureIndex < (int)cnMaximumSize);

	return (0 != aIsFrameInUse_[nPictureIndex]);
}

//template<typename T>
void FrameQueue::releaseFrame(const PARSERDISPINFO *pPicParams)
{
}

//template<typename T>
bool FrameQueue::dequeue(PARSERDISPINFO *pDisplayInfo)
{
	pDisplayInfo->picture_index = -1;
	bool bHaveNewFrame = false;

	enter_CS(&oCriticalSection_);

	if (nFramesInQueue_ > 0)
	{
		int iEntry = nReadPosition_;
		*pDisplayInfo = aDisplayQueue_[iEntry];
		nReadPosition_ = (iEntry + 1) % cnMaximumSize;
		nFramesInQueue_--;
		bHaveNewFrame = true;
	}

	leave_CS(&oCriticalSection_);

	return bHaveNewFrame;
}

//template<typename T>
void FrameQueue::enqueue(const PARSERDISPINFO *pPicParams)
{
	// Mark the frame as 'in-use' so we don't re-use it for decoding until it is no longer needed
	// for display
	
	aIsFrameInUse_[pPicParams->picture_index] = true;

	// Wait until we have a free entry in the display queue (should never block if we have enough entries)
	do
	{
		bool bPlacedFrame = false;
		enter_CS(&oCriticalSection_);

		if (nFramesInQueue_ < (int)FrameQueue::cnMaximumSize)
		{
			int iWritePosition = (nReadPosition_ + nFramesInQueue_) % cnMaximumSize;
			aDisplayQueue_[iWritePosition] = *pPicParams;
			nFramesInQueue_++;
			bPlacedFrame = true;
		}

		leave_CS(&oCriticalSection_);

		if (bPlacedFrame) // Done
			break;

		sleep(1);   // Wait a bit
	} while (!bEndOfDecode_);

	signalStatusChange();  // Signal for the display thread
}

//template<typename T>
void FrameQueue::reset_event(HANDLE event)
{
	ResetEvent(event);
}

//template<typename T>
void FrameQueue::set_event(HANDLE event)
{
	SetEvent(event);
}

//template<typename T>
void FrameQueue::leave_CS(CRITICAL_SECTION *pCS)
{
	LeaveCriticalSection(pCS);
}

//template<typename T>
void FrameQueue::enter_CS(CRITICAL_SECTION *pCS)
{
	EnterCriticalSection(pCS);
}

//template<typename T>
void FrameQueue::signalStatusChange()
{
	set_event(hEvent_);
}

//template<typename T>
bool FrameQueue::waitUntilFrameAvailable(int nPictureIndex)
{
	while (isInUse(nPictureIndex))
	{
		sleep(1);   // Decoder is getting too far ahead from display

		if (isEndOfDecode())
			return false;
	}

	return true;
}