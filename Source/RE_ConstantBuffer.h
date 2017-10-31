#pragma once

#ifndef _RE_ConstantBuffer_H__
#define _RE_ConstantBuffer_H__

#include "RE_Buffer.h"
#include <d3d11.h>

namespace M1000Group
{
	class ConstantBuffer :public Buffer
	{
		friend class RenderDevice11;
	public:
		virtual int GetSize()
		{
			return PerSize;
		}
		virtual int GetCount()
		{
			return Count;
		}
		virtual BufferType GetType()
		{
			return BT_Constant;
		}
		virtual void* Map()
		{
			return NULL;
		}
		virtual void UnMap()
		{

		}
	public:
		int Count;
		int PerSize;
		ID3D11Buffer* buffer;
	};
} //namespace M1000Group

#endif //_RE_ConstantBuffer_H__