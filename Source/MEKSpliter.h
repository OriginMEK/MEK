#pragma once
/*
·Ö°üÆ÷
*/
#ifndef MEKSPLITER_H_
#define MEKSPLITER_H_
#include "MEKIncludeDef.h"
class MEKSpliter
{
public:
	MEKSpliter();
	~MEKSpliter();

	bool AnalyzeFile(char* fileName,MEKParam* data);
	static void Start(void *param)
	{
		MEKSpliter* spliter = (MEKSpliter*)param;
		spliter->SpliterThread();
	}

private:
	MEKParam* mData;
	int OpenCodecContext(int *streamIndex, AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type);
	void SpliterThread();
};
#endif //MEKSPLITER_H_