#pragma once
//RE_RenderDevice11.h

#ifndef _RE_RenderDevice11_H__
#define _RE_RenderDevice11_H__

#include <d3d11.h>
#include "RE_RenderDevice.h"
#include "RE_VertexBuffer.h"
#include "RE_IndexBuffer.h"
#include "RE_ConstantBuffer.h"
#include "RE_Texture2D.h"
#include "RE_VertexShader.h"
#include "RE_PixelShader.h"
#include "RE_Log.h"
#include "RE_Declare.h"

namespace M1000Group
{
	class RenderDevice11 :public RenderDevice
	{
	public:
		RenderDevice11();
		virtual ~RenderDevice11();

		virtual DeviceType GetType()
		{
			return DT_Direct3D11;
		}

		ErrorCode Initialize(void* hwnd, int w, int h);
		virtual Buffer* CreateBuffer(enum BufferType type, void* data, int perSize, int Count);
		virtual Texture2D* CreateTexture(enum TextureType type, class String fileName);
		virtual class Shader* CreateShader(enum ShaderStage stage, class String fileName);
		virtual void Render(Buffer* vertex, Buffer* index, class Texture* tex, Shader* vs, Shader* ps);

	private:
		ID3D11Device*		mDevice;
		ID3D11DeviceContext*	mContext;
		IDXGISwapChain*		mSwapChain;
		IDXGIFactory*				mDxGIFactory;
		IDXGIAdapter*				mAdapter;
		IDXGIOutput*					mOutput;
		ID3D11RenderTargetView*	mRenderTargetView;
		ID3D11Texture2D*					mDepthStencilBuffer;
		ID3D11DepthStencilState*	mDepthStencilState;
		ID3D11DepthStencilView*	mDepthStencilView;
		ID3D11RasterizerState*			mRasterState;
		D3D11_VIEWPORT					mD3DViewPort;

		HWND			mWindow;
		int					mScreenWidth;
		int					mScreenHeight;
		bool				bInitialized;

	};
}//namespace M1000Group

#endif //_RE_RenderDevice11_H__