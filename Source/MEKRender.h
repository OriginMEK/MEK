#pragma once
/*
	äÖÈ¾Æ÷
*/
#ifndef MEKRENDER_H_
#define MEKRENDER_H_
#include <windows.h>
#include "MEKIncludeDef.h"
class MEKRender
{
public:
	MEKRender(MEKParam* mData);
	~MEKRender();
	
	void SetRenderWnd(HWND hWnd);
	static void StartRender(void *param)
	{
		MEKRender* render = (MEKRender*)param;
		render->RenderThread();
	}
private:
	HWND hWnd;
	MEKParam* mData;
	bool RenderThread();
};
#endif //MEKRENDER_H_