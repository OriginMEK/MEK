#pragma once
//RE_Vector3.h

#ifndef _RE_Vector3_H__
#define _RE_Vector3_H__

namespace M1000Group
{
	class Vector3
	{
	public:
		Vector3()
		{
			x = 0;
			y = 0;
			z = 0;
		}

		Vector3(float x_, float y_,float z_)
		{
			x = x_;
			y = y_;
			z = z_;
		}

	public:
		float x;
		float y;
		float z;
	};
} //namespace M1000Group

#endif //_RE_Vector3_H__