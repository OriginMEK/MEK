#include "MEKDecoder.h"
#include "RE_Texture2D.h"
#include "RE_RenderDevice11.h"
#include <queue>
 using namespace M1000Group;
extern Texture2D* gDynamicTexuture[10];
extern RenderDevice	*gDevice; 
extern RenderQueue		gRenderQueue;


int Snapt(AVCodecContext* m_pCodecCtx, AVFrame* pFrame, int width, int height, int bpp, char* fileName)
{
	FILE *fp;
	
	fp = fopen(fileName, "wb");
	if (!fp)
	{
		DWORD err = ::GetLastError();
		return -1;
	}

	AVFrame* pFrameRGB = av_frame_alloc();
	//int numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, width, height);
	//uint8_t* data = (uint8_t *)av_malloc(numBytes);
	//avpicture_fill((AVPicture*)pFrameRGB, data, AV_PIX_FMT_RGB24, width, height);

	int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, width, height, 1);
	uint8_t* out_buffer_rgb = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
	av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, out_buffer_rgb, AV_PIX_FMT_RGB24, width, height, 1);

	SwsContext* pSWSCtx = sws_getContext(m_pCodecCtx->width, m_pCodecCtx->height, m_pCodecCtx->pix_fmt, width, height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
	sws_scale(pSWSCtx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, m_pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

	BITMAPFILEHEADER bmpHeader;
	BITMAPINFO	bmpInfo;

	bmpHeader.bfType = ('M' << 8) | 'B';
	bmpHeader.bfType = MAKEWORD(66, 77);
	bmpHeader.bfReserved1 = 0;
	bmpHeader.bfReserved2 = 0;
	bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmpHeader.bfSize = bmpHeader.bfOffBits + width * height * bpp / 8;

	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = width;
	bmpInfo.bmiHeader.biHeight = -height;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = bpp;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biSizeImage = 0;
	bmpInfo.bmiHeader.biXPelsPerMeter = 100;
	bmpInfo.bmiHeader.biYPelsPerMeter = 100;
	bmpInfo.bmiHeader.biClrUsed = 0;
	bmpInfo.bmiHeader.biClrImportant = 0;

	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(pFrameRGB->data[0], width*height*bpp / 8, 1, fp);
	fclose(fp);
	return 0;
}

MEKDecoder::MEKDecoder()
{
	mData = NULL;
}

MEKDecoder::~MEKDecoder()
{

}

bool MEKDecoder::BeginDecode(MEKParam* mData)
{
	this->mData = mData;
	::_beginthread(StartDecoderVideo, 0, this);
	::_beginthread(StartDecoderAudio, 0, this);
	return true;
}

void MEKDecoder::DecoderVideoThread()
{
	while (true)
	{
		PARSERDISPINFO oDisplayInfo;

		if (mData->videoParam->pVideoQueue->dequeue(&oDisplayInfo))
		{
			int got_frame;
			DecodePacket(oDisplayInfo.data, &got_frame, 0);
		}
	}
}

void MEKDecoder::DecoderAudioThread()
{
	while (true)
	{
		PARSERDISPINFO oDisplayInfo;

		if (mData->audioParam->pAudioQueue->dequeue(&oDisplayInfo))
		{
			int got_frame;
			DecodePacket(oDisplayInfo.data, &got_frame, 0);
		}
	}
}

int MEKDecoder::DecodePacket(AVPacket pkt, int *gotFrame, int cached)
{
	int ret = 0;
	int decoded = pkt.size;
	*gotFrame = 0;
	if (pkt.stream_index == mData->videoParam->nVideoIndex)
	{
		if (!mData->videoParam->pYUVFrame)
		{
			mData->videoParam->pYUVFrame = av_frame_alloc();
		}

		if (!mData->videoParam->pRGBFrame)
		{
			mData->videoParam->pRGBFrame = av_frame_alloc();
		}

		ret = avcodec_send_packet(mData->videoParam->pVideoContex, &pkt);
		if (ret != 0) 
			return ret;
		ret = avcodec_receive_frame(mData->videoParam->pVideoContex, mData->videoParam->pYUVFrame);
		if (ret != 0) return ret;
		/*ret = avcodec_decode_video2(mData->videoParam->pVideoContex, frame, gotFrame, &pkt);
		if (ret < 0)
		{
			return ret;
		}*/

		//if (*gotFrame)
		{
			if (mData->videoParam->pYUVFrame->width != mData->width || mData->videoParam->pYUVFrame->height != mData->height || mData->videoParam->pYUVFrame->format != mData->pix_fmt)
			{
				return -1;
			}
			//copy to texture;
			if (!mData->videoParam->pImgConvertCtx)
			{
				mData->videoParam->pImgConvertCtx = sws_getContext(mData->width, mData->height, mData->pix_fmt, mData->width,
					mData->height, AV_PIX_FMT_BGR32, SWS_BICUBIC, NULL, NULL, NULL);

				//int numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, mData->width, mData->height);
				int numBytes = av_image_get_buffer_size(AV_PIX_FMT_BGR32, mData->width, mData->height, 1);
				uint8_t* out_buffer_rgb = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
				
				//avpicture_fill((AVPicture*)(mData->videoParam->pRGBFrame), out_buffer_rgb, AV_PIX_FMT_RGB32, mData->width, mData->height);
				av_image_fill_arrays(mData->videoParam->pRGBFrame->data, mData->videoParam->pRGBFrame->linesize, out_buffer_rgb, AV_PIX_FMT_BGR32, mData->width, mData->height, 1);
			}

			sws_scale(mData->videoParam->pImgConvertCtx, (uint8_t const * const *)mData->videoParam->pYUVFrame->data, mData->videoParam->pYUVFrame->linesize, 0, mData->height, mData->videoParam->pRGBFrame->data, mData->videoParam->pRGBFrame->linesize);
			
			int index = gRenderQueue.GetWriteIndex();
			if (index != -1)
			{
				if (gDevice && gDynamicTexuture[index])
				{
					int pitch;
					int stride = mData->videoParam->pRGBFrame->linesize[0];

					char buff[256] = { 0 };
					sprintf(buff, "------MapIndex:%d-----\n",index);
					OutputDebugStringA(buff);

					void* pData = gDevice->Map(gDynamicTexuture[index], pitch);
					for (int h = 0; h < mData->height; h++)
					{
						unsigned char* srcLine = (unsigned char*)(mData->videoParam->pRGBFrame->data[0]) + h * mData->videoParam->pRGBFrame->linesize[0];
						unsigned char* dstLine = (unsigned char*)pData + h * pitch;
						memcpy(dstLine, srcLine, pitch);
					}
					gDevice->Unmap(gDynamicTexuture[index]);
				}
			}
			else
			{
				Sleep(1);
			}
			
			Sleep(40);
			char file[256] = { 0 };
			SYSTEMTIME st = { 0 };
			GetLocalTime(&st);
			sprintf_s(file, "pictures.bmp", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

			//Snapt(mData->videoParam->pVideoContex, mData->videoParam->pYUVFrame, mData->width, mData->height, 24, file);
		}
	}
	else if (pkt.stream_index == mData->audioParam->nAudioIndex)
	{
#if 0
		ret = avcodec_send_packet(mData->videoParam->pVideoContex, &pkt);
		if (ret != 0) return ret;

		ret = avcodec_receive_frame(mData->videoParam->pVideoContex, frame);
		if (ret != 0) return ret;
		//ret = avcodec_decode_audio4(mData->audioParam->pAudioContex, frame, gotFrame, &pkt);
		if (ret < 0)
		{
			return ret;
		}
		decoded = FFMIN(ret, pkt.size);
		
		if (*gotFrame)
		{
			size_t unpadded_linesize = frame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)frame->format);
		}


#endif

	}
	return decoded;
	
}
