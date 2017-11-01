#pragma once
#ifndef _RE_BUFFER_H__
#define  _RE_BUFFER_H__

namespace M1000Group
{
	enum BufferType
	{
		BT_Vertex = 0,
		BT_Index = 1,
		BT_Constant = 2,
	};

	class  Buffer
	{
	public:
		virtual int GetSize() = 0;
		virtual int GetCount() = 0;
		virtual BufferType GetType() = 0;
		virtual void* Map() = 0;
		virtual void UnMap() = 0;
	};
} //namespace M1000Group

#endif	//_RE_DECLARE_H__