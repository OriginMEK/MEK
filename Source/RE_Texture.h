#pragma once
//RE_Texture.h

#ifndef _RE_Texture_H__
#define  _RE_Texture_H__

namespace M1000Group
{
	enum TextureType
	{
		TT_Texture1D = 0,
		TT_Texture2D = 1,
		TT_TextureRT = 2,
	};

	enum TextureFormat
	{
		TF_RGBA32 = 0,
		TF_Unknow = 1,
	};

	class Texture
	{
	public:
		virtual ~Texture() {}

		virtual TextureType GetType() = 0;
		virtual TextureFormat GetFormat() = 0;
		virtual int GetWidth() = 0;
		virtual int GetHeight() = 0;
		virtual int GetDepth() = 0;
		virtual void* Map() = 0;
		virtual void UnMap() = 0;
	};
} //namespace M1000Group

#endif	//_RE_Texture_H__