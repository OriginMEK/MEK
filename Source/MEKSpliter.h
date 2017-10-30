#pragma once
#ifndef MEKSPLITER_H_
#define MEKSPLITER_H_
#include "MEKIncludeDef.h"
#include <process.h>
class MEKSpliter
{
public:
	MEKSpliter();
	~MEKSpliter();

	bool AnalyzeFile(char* fileName,MEKParam* data);
	int OpenCodecContext(int *streamIndex, AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type);
	static void Start(void *param)
	{
		MEKSpliter* spliter = (MEKSpliter*)param;
		spliter->SpliterThread();
	}

	void SpliterThread();
private:
	MEKParam* mData;
};
#endif //MEKSPLITER_H_