#include "MEKRender.h"
#include "RE_RenderDevice11.h"
using namespace M1000Group;

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	/*HDC hdc;
	PAINTSTRUCT ps;*/

	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

Texture2D		*gDynamicTexuture[10] = { 0 };

RenderDevice	*gDevice = NULL;
RenderQueue<void*>		gCanRenderTex;
RenderQueue<void*>		gUsingTex;
RenderQueue<void*>		gNeedMapTex;

MEKRender::MEKRender(MEKParam* mData)
{
	this->mData = mData;
}

MEKRender::~MEKRender()
{

}

void MEKRender::SetRenderWnd(HWND hWnd)
{
	this->hWnd = hWnd;
#if 0
	RenderDevice* device = new RenderDevice11();
	if (device->Initialize(hWnd, 800, 600) != EC_None)
	{
		MessageBox(0, _T("初始化设备失败!"), 0, 0);
		return false;
	}
	VertexType* vertexData = new VertexType[24];
	memset(vertexData, 0, sizeof(VertexType) * 24);

	//上 
	vertexData[0] = VertexType(Vector3(-1.0f / 2, -1.0f / 2, -1.0f / 2), Vector2(0, 1));
	vertexData[1] = VertexType(Vector3(-1.0f / 2, 1.0f / 2, 1.0f / 2), Vector2(0, 0));
	vertexData[2] = VertexType(Vector3(1.0f / 2, 1.0f / 2, 1.0f / 2), Vector2(1.0f, 0));
	vertexData[3] = VertexType(Vector3(1.0f / 2, -1.0f / 2, 1.0f / 2), Vector2(1.0f, 1));
	//下
	vertexData[4] = VertexType(Vector3(-1.0f / 2, -1.0f / 2, -1.0f / 2), Vector2(0, 1));
	vertexData[5] = VertexType(Vector3(-1.0f / 2, 1.0f / 2, -1.0f / 2), Vector2(0, 0));
	vertexData[6] = VertexType(Vector3(1.0f / 2, 1.0f / 2, -1.0f / 2), Vector2(1.0f, 0));
	vertexData[7] = VertexType(Vector3(1.0f / 2, -1.0f / 2, -1.0f / 2), Vector2(1.0f, 1));
	//左
	vertexData[8] = VertexType(Vector3(-1.0f / 2, -1.0f / 2, -1.0f / 2), Vector2(1, 0));
	vertexData[9] = VertexType(Vector3(-1.0f / 2, 1.0f / 2, 1.0f / 2), Vector2(0, 0));
	vertexData[10] = VertexType(Vector3(-1.0f / 2, -1.0f / 2, -1.0f / 2), Vector2(1, 1));
	vertexData[11] = VertexType(Vector3(-1.0f / 2, 1.0f / 2, -1.0f / 2), Vector2(0, 1));
	//右
	vertexData[12] = VertexType(Vector3(1.0f / 2, 1.0f / 2, 1.0f / 2), Vector2(1.0f, 0));
	vertexData[13] = VertexType(Vector3(1.0f / 2, -1.0f / 2, 1.0f / 2), Vector2(0, 0));
	vertexData[14] = VertexType(Vector3(1.0f / 2, 1.0f / 2, -1.0f / 2), Vector2(0, 1));
	vertexData[15] = VertexType(Vector3(1.0f / 2, -1.0f / 2, -1.0f / 2), Vector2(1.0f, 1));
	//前
	vertexData[16] = VertexType(Vector3(-1.0f / 2, -1.0f / 2, -1.0f / 2), Vector2(0, 0));
	vertexData[17] = VertexType(Vector3(1.0f / 2, -1.0f / 2, 1.0f / 2), Vector2(1.0f, 0));
	vertexData[18] = VertexType(Vector3(-1.0f / 2, -1.0f / 2, -1.0f / 2), Vector2(0, 1));
	vertexData[19] = VertexType(Vector3(1.0f / 2, -1.0f / 2, -1.0f / 2), Vector2(1.0f, 1));
	//后
	vertexData[20] = VertexType(Vector3(-1.0f / 2, 1.0f / 2, 1.0f / 2), Vector2(0, 0));
	vertexData[21] = VertexType(Vector3(1.0f / 2, 1.0f / 2, 1.0f / 2), Vector2(1.0f, 0));
	vertexData[22] = VertexType(Vector3(-1.0f / 2, 1.0f / 2, -1.0f / 2), Vector2(0, 1));
	vertexData[23] = VertexType(Vector3(1.0f / 2, 1.0f / 2, -1.0f / 2), Vector2(1.0f, 1));
	Buffer* vertex = device->CreateBuffer(BT_Vertex, vertexData, sizeof(VertexType), 24);

	unsigned int* indexData = new unsigned int[36];
	memset(indexData, 0, sizeof(unsigned int) * 36);

	// 上
	indexData[0] = 0;	indexData[1] = 1;	indexData[2] = 2;
	indexData[3] = 0;	indexData[4] = 2;	indexData[5] = 3;
	//下
	indexData[6] = 4;	indexData[7] = 5;	indexData[8] = 6;
	indexData[9] = 4;	indexData[10] = 6;	indexData[11] = 7;
	//Left
	indexData[12] = 8;	indexData[13] = 9;	indexData[14] = 10;
	indexData[15] = 8;	indexData[16] = 10;	indexData[17] = 11;
	//Right
	indexData[18] = 12;	indexData[19] = 13;	indexData[20] = 14;
	indexData[21] = 12;	indexData[22] = 14;	indexData[23] = 15;
	//Front
	indexData[24] = 15;	indexData[25] = 16;	indexData[26] = 17;
	indexData[27] = 15;	indexData[28] = 17;	indexData[29] = 18;
	//Back
	indexData[30] = 19;	indexData[31] = 20;	indexData[32] = 21;
	indexData[33] = 19;	indexData[34] = 21;	indexData[35] = 22;

	Buffer* index = device->CreateBuffer(BT_Index, indexData, sizeof(unsigned int), 36);

	Texture2D* diffuseTexture = device->CreateTexture(TT_Texture2D, String(_T("../Assets/Textures/quad.png")));

	Texture2D* diffuseTexture1 = device->CreateTexture(TT_Texture2D, String(_T("../Assets/Textures/test.png")));

	VertexShader* diffuseVertexShader = (VertexShader*)device->CreateShader(SS_Vertex, String(_T("../Assets/Shaders/Diffuse.vs")));

	PixelShader* diffusePixelShader = (PixelShader*)device->CreateShader(SS_Pixel, String(_T("../Assets/Shaders/Diffuse.ps")));

	device->Render(vertex, index, diffuseTexture, diffuseTexture1, diffuseVertexShader, diffusePixelShader);
#endif
	::_beginthread(StartRender, 0, this);
}

bool MEKRender::RenderThread()
{
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);

	WNDCLASSEX wcl;
	wcl.cbSize = sizeof(WNDCLASSEX);
	wcl.hInstance = hInstance;
	wcl.lpszClassName = _T("WndClass");
	wcl.lpfnWndProc = MainWndProc;
	wcl.style = CS_DBLCLKS;
	wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcl.hIconSm = NULL;
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcl.lpszMenuName = NULL;
	wcl.cbClsExtra = 0;
	wcl.cbWndExtra = 0;
	wcl.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	if (!RegisterClassEx(&wcl))
	{
		MessageBox(0, _T("注册窗口类失败!"), 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT r = { 0, 0, mData->width, mData->height };
	AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, false);
	int iWidth = r.right - r.left;
	int iHeight = r.bottom - r.top;
	int iLeft = (GetSystemMetrics(SM_CXSCREEN) - iWidth) / 2;
	int iTop = (GetSystemMetrics(SM_CYSCREEN) - iHeight) / 2;

	/*HWND hwnd = CreateWindowEx(WS_EX_APPWINDOW, _T("WndClass"), _T("MainFrm"),
		WS_OVERLAPPEDWINDOW, iLeft, iTop, iWidth, iHeight,
		NULL, NULL, hInstance, NULL);
	if (!hwnd)
	{
		MessageBox(0, _T("创建窗口失败!"), 0, 0);
		return false;
	}*/

	RenderDevice* device = new RenderDevice11();
	gDevice = device;
	RECT rect;
	GetClientRect(hWnd, &rect);
	if (device->Initialize(hWnd, mData->width, mData->height) != EC_None)
	{
		MessageBox(0, _T("初始化设备失败!"), 0, 0);
		return 0;
	}

	VertexType* vertexData = new VertexType[4];
	memset(vertexData, 0, sizeof(VertexType) * 4);

	vertexData[0] = VertexType(Vector3(-1.0f, -1.0f, 0), Vector2(0, 1));
	vertexData[1] = VertexType(Vector3(-1.0, 1.0f, 0), Vector2(0, 0));
	vertexData[2] = VertexType(Vector3(1.0f, 1.0f, 0), Vector2(1, 0));
	vertexData[3] = VertexType(Vector3(1.0f, -1.0f, 0), Vector2(1, 1));
	Buffer* vertex = device->CreateBuffer(BT_Vertex, vertexData, sizeof(VertexType), 4);

	unsigned int* indexData = new unsigned int[6];
	memset(indexData, 0, sizeof(unsigned int) * 6);

	indexData[0] = 0;	indexData[1] = 1;	indexData[2] = 2;
	indexData[3] = 0;	indexData[4] = 2;	indexData[5] = 3;

	Buffer* index = device->CreateBuffer(BT_Index, indexData, sizeof(unsigned int), 6);

	Texture2D* diffuseTexture = device->CreateTexture(TT_Texture2D, String(_T("../../Bin/Assets/Textures/2.png")));

	Texture2D* dynamicTexure = device->CreateTexture(TT_Texture2D, 512, 512, TF_RGBA32);
	if (!gDynamicTexuture[0])
	{
		for (int i = 0; i < 10 ;i++)
		{
			gDynamicTexuture[i] = device->CreateTexture(TT_Texture2D, mData->width, mData->height, TF_RGBA32);
			gNeedMapTex.SetData(gDynamicTexuture[i]);
		}
	}

	VertexShader* diffuseVertexShader = (VertexShader*)device->CreateShader(SS_Vertex, String(_T("../../Bin/Assets/Shaders/Diffuse.vs")));

	PixelShader* diffusePixelShader = (PixelShader*)device->CreateShader(SS_Pixel, String(_T("../../Bin/Assets/Shaders/Diffuse.ps")));

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	bool done = false;
	MSG msg = { 0 };

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			/*int pitch;
			void* pData = device->Map(dynamicTexure, pitch);
			{
				for (int h = 0 ; h < 512; h++)
				{
					for (int w = 0; w < 512; w++)
					{
						((unsigned char*)pData)[h * pitch + w * 4 + 0] = 166;
						((unsigned char*)pData)[h * pitch + w * 4 + 1] = 166;
						((unsigned char*)pData)[h * pitch + w * 4 + 2] = 0;
						((unsigned char*)pData)[h * pitch + w * 4 + 3] = 255;
					}
				}
			}
			device->Unmap(dynamicTexure);*/

			static Texture2D *sTex = NULL;
			Texture2D *tex = (Texture2D *)gCanRenderTex.GetData(false);
			if (tex)
			{
				sTex = tex;
				device->Render(vertex, index, tex, diffuseVertexShader, diffusePixelShader);
				gUsingTex.SetData(tex);
				if (gUsingTex.GetSize() > 1)
				{
					gNeedMapTex.SetData(gUsingTex.GetData(false));
				}

			}
			else
			{ 
				//if(sTex)
				//	device->Render(vertex, index, sTex, diffuseVertexShader, diffusePixelShader);
			}
		}
	}
	return 0;
}
