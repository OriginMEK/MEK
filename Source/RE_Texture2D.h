#pragma once
#ifndef _RG_Texture2D_H__
#define _RG_Texture2D_H__

#include "RE_Texture.h"
#include <D3DX11.h>

namespace M1000Group
{
	class Texture2D :public Texture
	{
		friend class RenderDevice11;
	public:
		Texture2D();
		virtual ~Texture2D();
		
		double pts;
		virtual TextureType GetType()
		{
			return Type;
		}

		virtual TextureFormat GetFormat()
		{
			return Format;
		}

		virtual int GetWidth()
		{
			return Width;
		}

		virtual int GetHeight()
		{
			return Height;
		}

		virtual int GetDepth()
		{
			return -1;
		}

		virtual void* Map();
		virtual void UnMap();
	private:
		int				Width;
		int				Height;
		int				Samples;
		TextureFormat		Format;
		TextureType			Type;

		ID3D11Texture2D*			Tex;
		ID3D11ShaderResourceView*	TexSv;
		ID3D11RenderTargetView*			TexRtv;
		ID3D11DepthStencilView*			TexDsv;
	};
}//namespace M1000Group

#endif //_RG_Texture2D_H__