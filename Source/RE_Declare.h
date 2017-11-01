#pragma once
//RE_Declare.h

#ifndef _RE_DECLARE_H__
#define _RE_DECLARE_H__

#include "RE_Vector2.h"
#include "RE_Vector3.h"

namespace M1000Group
{
	enum ErrorCode
	{
		EC_None = 0,
		EC_CreateDeviceFailed = 1,
		EC_MallocFailed = 2,
	};

	typedef struct VertexType
	{
		VertexType()
		{
			position = Vector3();
			texture = Vector2();
		}

		VertexType(Vector3 p, Vector2 t)
		{
			position = p;
			texture = t;
		}
		Vector3 position;
		Vector2 texture;
	};
} //namespace M1000Group

#endif //_RE_DECLARE_H__