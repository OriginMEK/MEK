#pragma once
//RE_VertexShader.h

#ifndef _RE_VertexShader_H__
#define _RE_VertexShader_H__

#include <d3d11.h>
#include "RE_Shader.h"

namespace M1000Group
{
	class VertexShader : public Shader
	{
		friend class RenderDevice11;
	public:
		VertexShader();
		~VertexShader();

		virtual ShaderStage GetStage()
		{
			return SS_Vertex;
		}

		virtual void SetUniformBuffer(class Buffer* buffer, int i /* = 0 */);

	public:
		ID3D11VertexShader*	mD3DShader;
		ID3D11InputLayout*		mVertexIL;
		class Buffer*		mD3DBuffer;
	};
} //namespace M1000Group

#endif //_RE_VertexShader_H__