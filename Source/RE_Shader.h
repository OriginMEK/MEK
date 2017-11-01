#pragma once
//RE_Shader.h

#ifndef _RE_SHADER_H__
#define _RG_SHADER_H__

namespace M1000Group
{
	enum ShaderStage
	{
		SS_Vertex = 0,
		SS_Geometry = 1,
		SS_Fragment = 2,
		SS_Pixel = 3,
	};

	class Shader
	{
	public:
		virtual ~Shader() {}

		virtual ShaderStage GetStage() = 0;
		virtual void SetUniformBuffer(class Buffer* buffer, int i = 0) = 0;
	};
} //namespace M1000Group

#endif //_RG_SHADER_H__