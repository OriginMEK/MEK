#pragma once
//RE_Vector2.h

#ifndef _RE_Vector2_H__
#define _RE_Vector2_H__

namespace M1000Group
{
	class Vector2
	{
	public:
		Vector2()
		{
			x = 0;
			y = 0;
		}

		Vector2(float x_, float y_)
		{
			x = x_;
			y = y_;
		}

	public:
		float x;
		float y;
	};
} //namespace M1000Group

#endif //_RE_Vector2_H__