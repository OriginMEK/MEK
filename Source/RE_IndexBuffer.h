#pragma once

#include <d3d11.h>
#include "RE_Buffer.h"

namespace M1000Group
{
	class IndexBuffer :public Buffer
	{
		friend class RenderDevice11;
	public:
		virtual int GetSize()
		{
			return PerSize;
		}
		virtual int GetCount()
		{
			return BufferCount;
		}
		virtual BufferType GetType()
		{
			return BT_Index;
		}
		virtual void* Map() { return NULL; }
		virtual void UnMap(){}
	private:
		int		BufferCount;
		int		PerSize;
		ID3D11Buffer*	BufferData;
	};
}