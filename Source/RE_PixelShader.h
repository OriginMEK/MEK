#pragma once
//RE_PixelShader.h

#ifndef _RE_PixelShader_H__
#define _RE_PixelShader_H__

#include <d3d11.h>
#include "RE_Shader.h"

namespace M1000Group
{
	class PixelShader : public Shader
	{
		friend class RenderDevice11;
	public:
		PixelShader();
		~PixelShader();

		virtual ShaderStage GetStage()
		{
			return SS_Pixel;
		}

		virtual void SetUniformBuffer(class Buffer* buffer, int i /* = 0 */);

	private:
		ID3D11PixelShader* mD3DShader;
		class Buffer*		mD3DBuffer;
	};
} //namespace M1000Group

#endif //_RE_PixelShader_H__