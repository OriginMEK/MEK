/***************************************************************************************
	Time : 2017-10-27 17:22:53
	author : zhaohu
	����һ�����ڽ���dll�����ļ�
***************************************************************************************/
#ifdef MEKCODEC_EXPORTS
#define MEKCODEC_API __declspec(dllexport)
#else
#define MEKCODEC_API __declspec(dllimport)
#endif

//#include "MEKIncludeDef.h"
#include "Singleton.h"
#include "MEKPlayer.h"
// This class is exported from the MEKCodec.dll
class MEKCODEC_API CMEKCodec {
public:
	CMEKCodec(void);
	// TODO: add your methods here.
	bool Init(char* pathName);
	bool SetCallBackFn(pCallBackFn fn);
	bool Start();
	bool Stop();
};

extern MEKCODEC_API int nMEKCodec;

MEKCODEC_API int fnMEKCodec(void);
