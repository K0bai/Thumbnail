#include "thumbnail.h"
#include <cassert>
#include <sstream>

extern "C"
{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
};


thumbnail::thumbnail(void)
{

}


thumbnail::~thumbnail(void)
{
}

int thumbnail::GetImageData(std::string InputFilename, std::string OutputFilename, int DstWidth, int DstHeight)
{
	int iResult = 0;

	// 注册
	av_register_all();
	AVFormatContext* pavFmtCxt = NULL;

	// 打开
	iResult = avformat_open_input(&pavFmtCxt, InputFilename.c_str(), NULL, NULL);
	assert(iResult == 0);

	// 读取一部分视频，音频信息
	iResult = avformat_find_stream_info(pavFmtCxt, NULL);
	assert(iResult >= 0);

	int iVidStrmID = -1;
	for (int i = 0; i < pavFmtCxt->nb_streams; ++i)
	{
		if (AVMEDIA_TYPE_VIDEO == pavFmtCxt->streams[i]->codec->codec_type)
		{
			iVidStrmID = i;
		}
	}
	assert(iVidStrmID != -1);

	// 查找并打开解码器.
	AVCodec* pDecodec = avcodec_find_decoder(pavFmtCxt->streams[iVidStrmID]->codec->codec_id);
	AVCodecContext* pavCCxt = NULL;
	pavCCxt = pavFmtCxt->streams[iVidStrmID]->codec;
	iResult = avcodec_open2(pavCCxt, pDecodec, NULL);
//	iResult = avcodec_open2(pavFmtCxt->streams[iVidStrmID]->codec, pDecodec, NULL);
	assert(iResult >= 0);

//	AVFrame* pFrame = avcodec_alloc_frame();
	AVFrame* pFrame = nullptr;
	// 读取文件,解码.
	AVPacket pkt;
//	av_init_packet(&pkt);
	uint8_t *rgb_data = nullptr;
	
	while (av_read_frame(pavFmtCxt, &pkt)>= 0)
	{
		if (pkt.stream_index == iVidStrmID)
		{
			int iFinished = 0; 
			
			av_frame_free(&pFrame);
			pFrame = avcodec_alloc_frame();
			int iDecoded = avcodec_decode_video2(pavCCxt, pFrame, &iFinished, &pkt);
			 
			av_free_packet(&pkt);
			if (iDecoded > 0 && iFinished)
			{
				int width, height;
				width = pavFmtCxt->streams[iVidStrmID]->codec->width;
				height = pavFmtCxt->streams[iVidStrmID]->codec->height;

				SwsContext* pSwsCxt = sws_getContext(width, height, PIX_FMT_YUV420P,
					DstWidth, DstHeight, PIX_FMT_RGB32, SWS_BILINEAR, NULL, NULL, NULL);
				rgb_data = static_cast<uint8_t*>(av_malloc(DstWidth*DstHeight*4));
				uint8_t *rgb_src[3]= {rgb_data, NULL, NULL};
				int rgb_stride[3]={4*DstWidth, 0, 0};
				assert(pSwsCxt);

				// 直接用sws_scale 进行缩放
				iResult = sws_scale(pSwsCxt, pFrame->data, pFrame->linesize,
					0, height, rgb_src, rgb_stride);
				assert(iResult == DstHeight);

				for (int k = 0; k < DstHeight; ++k)
				{
					for (int j = 0; j < (DstWidth-1) * 4; j += 4)
					{
						uint8_t tempc = *(rgb_data + k * 4 * DstWidth + j);
						*(rgb_data + k * 4 * DstWidth + j) = *(rgb_data + k * 4 * DstWidth + j + 2);
						*(rgb_data + k * 4 * DstWidth + j + 2) = tempc;

					}
				}
				sws_freeContext(pSwsCxt);

				// 解码成功,退出
				break;
			}
		}
	}
	avcodec_close(pavCCxt);
	av_close_input_file(pavFmtCxt);

	// 调用图像编码函数
	EncodeDataToImage(OutputFilename, rgb_data, DstWidth, DstHeight);
	av_free(rgb_data);
	return 0;
}