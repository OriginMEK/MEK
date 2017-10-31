#include "RE_RenderDevice11.h"
#include <D3DX10math.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "dxgi.lib")

namespace M1000Group
{

	RenderDevice11::RenderDevice11()
	{
		mWindow = NULL;
		mScreenWidth = 0;
		mScreenHeight = 0;
		bInitialized = false;
	}

	RenderDevice11::~RenderDevice11()
	{
		mWindow = NULL;
		mScreenWidth = 0;
		mScreenHeight = 0;
		bInitialized = false;
	}

	ErrorCode RenderDevice11::Initialize(void* hwnd, int w, int h)
	{
		mWindow = (HWND)hwnd;
		mScreenWidth = w;
		mScreenHeight = h;

		HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&mDxGIFactory);
		if (FAILED(hr))
		{
			//Log_e(String(_T("CreateDXGIFactory Failed!")));
			return EC_CreateDeviceFailed;
		}

		hr = mDxGIFactory->EnumAdapters(0, &mAdapter);
		if (FAILED(hr))
		{
			//Log_e(String(_T("EnumAdapters Failed")));
			return EC_CreateDeviceFailed;
		}

		hr = mAdapter->EnumOutputs(0, &mOutput);
		if (FAILED(hr))
		{
			//Log_e(String(_T("EnumOutputs Failed!")));
			return EC_CreateDeviceFailed;
		}

		unsigned numModes = 0;
		hr = mOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
		if (FAILED(hr))
		{
			//Log_e(String(_T("GetDisplayModeList Failed!")));
			return EC_CreateDeviceFailed;
		}

		DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
		if (!displayModeList)
		{
			//Log_e(String(_T("GetDisplayModeList Failed!")));
			return EC_MallocFailed;
		}

		hr = mOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
		if (FAILED(hr))
		{
			//Log_e(String(_T("GetDisplayModeList Failed!")));
			return EC_CreateDeviceFailed;
		}

		unsigned int numerator = 0;
		unsigned int denominator = 1;
		for (unsigned int i = 0; i < numModes;i++)
		{
			if (displayModeList[i].Width == mScreenWidth)
			{
				if (displayModeList[i].Height == mScreenHeight)
				{
					numerator = displayModeList[i].RefreshRate.Numerator;
					denominator = displayModeList[i].RefreshRate.Denominator;
				}
			}
		}

		DXGI_ADAPTER_DESC adapterDesc = { 0 };
		hr = mAdapter->GetDesc(&adapterDesc);
		if (FAILED(hr))
		{
			//Log_e(String(_T("Adapter GetDesc Failed!")));
			return EC_CreateDeviceFailed;
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
		D3D_FEATURE_LEVEL featureLevel;

		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = mScreenWidth;
		swapChainDesc.BufferDesc.Height = mScreenHeight;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		if (false)
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
		}
		else
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		}

		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = mWindow;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		if (false)
		{
			swapChainDesc.Windowed = false;
		}
		else
		{
			swapChainDesc.Windowed = true;
		}

		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		featureLevel = D3D_FEATURE_LEVEL_11_0;

		hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &mSwapChain, &mDevice, NULL, &mContext);
		if (FAILED(hr))
		{
			//Log_e(String(_T("D3D11CreateDeviceAndSwapChain Failed!")));
			return EC_CreateDeviceFailed;
		}

		ID3D11Texture2D* backBuffer = 0;
		hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
		if (FAILED(hr))
		{
			//Log_e(String(_T("GetBuffer Failed!")));
			return EC_CreateDeviceFailed;
		}

		hr = mDevice->CreateRenderTargetView(backBuffer, NULL, &mRenderTargetView);
		if (FAILED(hr))
		{
			//Log_e(String(_T("CreateRenderTargetView Failed!")));
			return EC_CreateDeviceFailed;
		}

		backBuffer->Release();
		backBuffer = 0;

		D3D11_TEXTURE2D_DESC depthBufferDesc = { 0 };
		depthBufferDesc.Width = mScreenWidth;
		depthBufferDesc.Height = mScreenHeight;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		hr = mDevice->CreateTexture2D(&depthBufferDesc, NULL, &mDepthStencilBuffer);
		if (FAILED(hr))
		{
			return EC_CreateDeviceFailed;
		}
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		memset(&depthStencilViewDesc, 0, sizeof(depthStencilViewDesc));

		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		//hr = mDevice->CreateDepthStencilView(mDepthStencilBuffer, &depthStencilViewDesc, &mDepthStencilView);
		hr = mDevice->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView);
		if (FAILED(hr))
		{
			return EC_CreateDeviceFailed;
		}

		mContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = { 0 };
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		hr = mDevice->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState);
		if (FAILED(hr))
		{
			return EC_CreateDeviceFailed;
		}

		mContext->OMSetDepthStencilState(mDepthStencilState, 1);


		D3D11_RASTERIZER_DESC rasterDesc;
		memset(&rasterDesc, 0, sizeof(rasterDesc));

		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_NONE;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		//rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		hr = mDevice->CreateRasterizerState(&rasterDesc, &mRasterState);
		if (FAILED(hr))
		{
			return EC_CreateDeviceFailed;
		}

		mContext->RSSetState(mRasterState);

		mD3DViewPort.Width = ceil(mScreenWidth * 1.39f);
		mD3DViewPort.Height = ceil(mScreenHeight * 1.39f);
		//mD3DViewPort.Width = (float)mScreenWidth;
		//mD3DViewPort.Height = (float)mScreenHeight;
		mD3DViewPort.MinDepth = 0.0f;
		mD3DViewPort.MaxDepth = 1.0f;
		mD3DViewPort.TopLeftX = 0.0f;
		mD3DViewPort.TopLeftY = 0.0f;

		mContext->RSSetViewports(1, &mD3DViewPort);

		bInitialized = true;
		return EC_None;
	}

	Buffer* RenderDevice11::CreateBuffer(enum BufferType type, void* data, int perSize, int Count)
	{
		switch (type)
		{
		case BT_Vertex:
		{
			VertexBuffer* buffer = new VertexBuffer();
			buffer->BufferCount = Count;
			buffer->PerSize = perSize;

			D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
			memset(&vertexBufferDesc, 0, sizeof(vertexBufferDesc));

			vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			vertexBufferDesc.ByteWidth = perSize * Count;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = 0;
			vertexBufferDesc.MiscFlags = 0;
			vertexBufferDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA vectexData;
			memset(&vectexData, 0, sizeof(vectexData));

			vectexData.pSysMem = data;
			vectexData.SysMemPitch = 0;
			vectexData.SysMemSlicePitch = 0;

			HRESULT hr = mDevice->CreateBuffer(&vertexBufferDesc, &vectexData, &(buffer->BufferData));
			if (FAILED(hr))
			{
				//Log_e(String(_T("Create Vectex Buffer Failed!")));
				return 0;
			}
			return buffer;
		}
		break;
		case BT_Index:
		{
			IndexBuffer* buffer = new IndexBuffer();
			buffer->BufferCount = Count;
			buffer->PerSize = perSize;

			D3D11_BUFFER_DESC indexBufferDesc = { 0 };
			memset(&indexBufferDesc, 0, sizeof(indexBufferDesc));

			indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			indexBufferDesc.ByteWidth = perSize * Count;
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexBufferDesc.CPUAccessFlags = 0;
			indexBufferDesc.MiscFlags = 0;
			indexBufferDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA indexData = { 0 };
			memset(&indexData, 0, sizeof(indexData));

			indexData.pSysMem = data;
			indexData.SysMemPitch = 0;
			indexData.SysMemSlicePitch = 0;

			HRESULT hr = mDevice->CreateBuffer(&indexBufferDesc, &indexData, &(buffer->BufferData));
			if (FAILED(hr))
			{
				//Log_e(String(_T("Create index Buffer Failed!")));
				return 0;
			}
			return buffer;
		}
		break;
		case BT_Constant:
		{
			ConstantBuffer* buffer = new ConstantBuffer();
			buffer->Count = Count;
			buffer->PerSize = perSize;

			D3D11_BUFFER_DESC bufferDesc = { 0 };
			memset(&bufferDesc, 0, sizeof(bufferDesc));
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth = perSize * Count;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA constantData = { 0 };
			memset(&constantData, 0, sizeof(constantData));

			constantData.pSysMem = 0;
			constantData.SysMemPitch = 0;
			constantData.SysMemSlicePitch = 0;

			HRESULT hr = mDevice->CreateBuffer(&bufferDesc, 0, &(buffer->buffer));
			if (FAILED(hr))
			{
				//Log_e(String(_T("Create ConstantBuffer Failed.")));
				return 0;
			}
			return buffer;
		}
		break;
		default:
			break;
		}
	}

	Texture2D* RenderDevice11::CreateTexture(enum TextureType type, class String fileName)
	{
		switch (type)
		{
		case TT_Texture1D:
		{
			
		}
		break; 
		case TT_Texture2D:
		{
			Texture2D* texture = new Texture2D();
			HRESULT hr;
			D3DX11CreateTextureFromFile(mDevice, fileName.GetData(), NULL, NULL, (ID3D11Resource**)&(texture->Tex), &hr);
			if (SUCCEEDED(hr))
			{
				mDevice->CreateShaderResourceView((ID3D11Resource*)(texture->Tex), 0, &(texture->TexSv));
			}
#if 0
			HRESULT hr = D3DX11CreateShaderResourceViewFromFile(mDevice, fileName.GetData(), NULL, NULL, &(texture->TexSv), NULL);
			if (FAILED(hr))
			{
				//Log_e(String(_T("D3DX11CreateShaderResourceViewFromFile Failed")));
				return 0;
			}

			texture->TexSv->GetResource((ID3D11Resource**)&(texture->Tex));

#endif
			D3D11_TEXTURE2D_DESC desc = { 0 };
			texture->Tex->GetDesc(&desc);

			texture->Width = desc.Width;
			texture->Height = desc.Height;
			texture->Samples = desc.SampleDesc.Count;

			switch (desc.Format)
			{
			case DXGI_FORMAT_R8G8B8A8_UNORM:
				texture->Format = TF_RGBA32;
				break;
			default:
				texture->Format = TF_Unknow;
				break;
			}
			return texture;
		}
		break;
		case TT_TextureRT:
		{

		}
		break;
		default:
			break;
		}
		return NULL;
	}

	class Shader* RenderDevice11::CreateShader(enum ShaderStage stage, class String fileName)
	{
		switch (stage)
		{
		case SS_Vertex:
		{
			VertexShader* shader = new VertexShader();

			ID3D10Blob* vertexShaderBuffer = 0;
			ID3D10Blob* errorMessage = 0;
			HRESULT hr = D3DX11CompileFromFile(fileName.GetData(), NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
			if (FAILED(hr))
			{
				if (errorMessage)
				{
					//Log_e(fileName);
				}
				else
				{
					//Log_e(String(_T("Missing Shader File.")));
				}
				return 0;
			}
			
			hr = mDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), \
				NULL, &(shader->mD3DShader));
			if (FAILED(hr))
			{
				//Log_e(String(_T("CreateVertexShader Failed.")));
				return 0;
			}

			D3D11_INPUT_ELEMENT_DESC polygonLayout[2] = { { 0 } };

			polygonLayout[0].SemanticName = "POSITION";
			polygonLayout[0].SemanticIndex = 0;
			polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			polygonLayout[0].InputSlot = 0;
			polygonLayout[0].AlignedByteOffset = 0;
			polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			polygonLayout[0].InstanceDataStepRate = 0;

			polygonLayout[1].SemanticName = "TEXCOORD";
			polygonLayout[1].SemanticIndex = 0;
			polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
			polygonLayout[1].InputSlot = 0;
			polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			polygonLayout[1].InstanceDataStepRate = 0;

			unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

			hr = mDevice->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &(shader->mVertexIL));
			if (FAILED(hr))
			{
				//Log_e(String(_T("CreateInputLayout Failed!")));
				return false;
			}

			vertexShaderBuffer->Release();
			vertexShaderBuffer = 0;

			//shader->mD3DBuffer = CreateBuffer(BT_Constant, 0 ,sizeof(float) * 48, 1);

			return shader;
		}
		break; 
		case SS_Pixel:
		{
			PixelShader* shader = new PixelShader();

			ID3D10Blob* errorMessage = 0;
			ID3D10Blob* pixelShaderBuffer = 0;

			HRESULT hr = D3DX11CompileFromFile(fileName.GetData(), NULL, NULL, \
				"main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
				&pixelShaderBuffer, &errorMessage, NULL);
			if (FAILED(hr))
			{
				if (errorMessage)
				{
					//Log_e(fileName);
				}
				else
				{
					//Log_e(String(_T("CreateVertexShader failed.")));
				}

				return 0;
			}

			hr = mDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), \
				pixelShaderBuffer->GetBufferSize(), NULL, &(shader->mD3DShader));
			if (FAILED(hr))
			{
				//Log_e(String(_T("CreatePixelShader failed.")));
				return 0;
			}

			pixelShaderBuffer->Release();
			pixelShaderBuffer = 0;

			return shader;

		}
		break;
		default:
			break;
		}
		return NULL;
	}

	void RenderDevice11::Render(Buffer* vertex, Buffer* index, class Texture* tex, class Texture* tex1,Shader* vs, Shader* ps)
	{
		float color[4];
		color[0] = 0.16f;
		color[1] = 0.16f;
		color[2] = 0.16f;
		color[3] = 0.16f;
		
		mContext->ClearRenderTargetView(mRenderTargetView, color);
		mContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		unsigned int stride;
		unsigned int offset;

		VertexBuffer* vb = (VertexBuffer*)vertex;
		stride = vb->PerSize;
		offset = 0;

		mContext->IASetVertexBuffers(0, 1, &((VertexBuffer*)vertex)->BufferData, &stride, &offset);
		mContext->IASetIndexBuffer(((IndexBuffer*)index)->BufferData, DXGI_FORMAT_R32_UINT, 0);

		mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mContext->IASetInputLayout(((VertexShader*)vs)->mVertexIL);

		mContext->PSSetShaderResources(0, 1, &(((Texture2D*)tex)->TexSv));
		mContext->VSSetShader(((VertexShader*)vs)->mD3DShader, NULL, 0);
		mContext->PSSetShader(((PixelShader*)ps)->mD3DShader, NULL, 0);

#if 0

		//map the const buffer
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		memset(&mappedResource, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));
		VertexShader* shader = (VertexShader*)vs;
		ConstantBuffer* Cbuffer = (ConstantBuffer*)(shader->mD3DBuffer);
		ID3D11Buffer* source = Cbuffer->buffer;
		HRESULT hr = mContext->Map((ID3D11Resource*)source, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		struct MatrixBufferType
		{
			D3DXMATRIX world;
			D3DXMATRIX view;
			D3DXMATRIX	projection;
		}*dataPtr;
		dataPtr = (MatrixBufferType*)mappedResource.pData;

		D3DXMATRIX world;
		D3DXMatrixIdentity(&world);
		D3DXMatrixTranspose(&world, &world);

		D3DXMATRIX view;
		D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(0, 0, -2.0f), &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0));
		D3DXMatrixTranspose(&view, &view);

		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5, 600.0f / 800.0f, 1.0f, 1000);
		D3DXMatrixTranspose(&proj, &proj);

		/*D3DXMATRIX pry;
		D3DXMatrixRotationAxis(&pry, &D3DXVECTOR3(0, 0, 1), D3DX_PI * 0.5f);
		world = pry * world;
		D3DXMatrixTranspose(&world, &world);*/

		dataPtr->world = world;
		dataPtr->view = view;
		dataPtr->projection = proj;

		mContext->Unmap((ID3D11Resource*)source, 0);

		mContext->VSSetConstantBuffers(0, 1, &source);

#endif
		mContext->DrawIndexed(((IndexBuffer*)index)->BufferCount, 0, 0);

		mSwapChain->Present(0, 0);
	}

}