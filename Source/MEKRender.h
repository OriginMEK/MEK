#pragma once
/*
	äÖÈ¾Æ÷
*/
#ifndef MEKRENDER_H_
#define MEKRENDER_H_
<<<<<<< HEAD
#include <windows.h>
#include "MEKIncludeDef.h"
class MEKRender
{
public:
	MEKRender();
	~MEKRender();
	
	void SetRenderWnd(HWND hWnd);
	static void StartRender(void *param)
	{
		MEKRender* render = (MEKRender*)param;
		render->RenderThread();
	}
private:
	HWND hWnd;
	bool RenderThread();
=======
class MEKRender
{
	MEKRender();
	~MEKRender();
>>>>>>> b22f8a189a2ab8ef855b339c2f2e8419fcfb1eaf
};
#endif //MEKRENDER_H_