// MEKCodec.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "MEKCodec.h"


// This is an example of an exported variable
MEKCODEC_API int nMEKCodec=0;

// This is an example of an exported function.
MEKCODEC_API int fnMEKCodec(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see MEKCodec.h for the class definition
CMEKCodec::CMEKCodec()
{
    return;
}

bool CMEKCodec::Init(char* pathName)
{
	Singleton<MEKPlayer>::Instance()->Init(pathName);
	return false;
}

bool CMEKCodec::SetCallBackFn(pCallBackFn fn)
{
	Singleton<MEKPlayer>::Instance()->SetCallBackFn(fn);
	return true;
}

bool CMEKCodec::SetWnd(HWND hWnd)
{
	return false;
}

bool CMEKCodec::Start()
{
	Singleton<MEKPlayer>::Instance()->Start();
	return false;
}

bool CMEKCodec::Stop()
{
	Singleton<MEKPlayer>::Instance()->Stop();
	return false;
}
