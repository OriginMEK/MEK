#pragma once
//RE_String.h

#ifndef _RE_String_H__
#define _RE_String_H__

#include<windows.h>
#include <tchar.h>

namespace M1000Group
{
	class String
	{
	public:
		String()
		{
			data = new TCHAR[1];
			memset(data, 0, sizeof(TCHAR));
			data[0] = '\0';
		}

		String(TCHAR* t)
		{
			size_t len = _tcslen(t);
			++len;
			if (sizeof(TCHAR) != sizeof(char))
			{
				len *= 2;
			}
			data = new TCHAR[len];
			memset(data, 0, sizeof(TCHAR)*len);
			_tcscpy_s(data, len, t);
		}

		~String() {}

		TCHAR* GetData()
		{
			return data;
		}
	private:
			TCHAR* data;
	};
} //namespace M1000Group

#endif //_RE_String_H__