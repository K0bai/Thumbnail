#include "thumbnailBmp.h"
#include "thumbnailJpeg.h"
#include "thumbnailPng.h"

int main()
{
	thumbnailBmp tBmp;
	thumbnailJpeg tJpeg;
	thumbnailPng tPng;
	tBmp.GetImageData("video/Titanic.mkv", "output/testBmp.bmp", 1000, 1200);
	tJpeg.GetImageData("video/Titanic.mkv", "output/testJpeg.jpg", 1000, 1200);
	tPng.GetImageData("video/Titanic.mkv", "output/testPng.png", 1000, 1200);

	return 0;
}







// 完整的一个CPP，可以运行
/*
#include <string>
#include <cassert>
#include <sstream>

extern "C"
{
#ifndef INT64_C
#define INT64_C(c) (c ## LL)b
#define UINT64_C(c) (c ## ULL)
#endif
#include <stdlib.h>
//#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
//#include <libavcodec/avcodec.h>
#include <png.h> 
#include "jpeglib.h"
#include "jconfig.h"
};

#pragma pack(1)

typedef struct tagBITMAPFILEHEADER
{
	unsigned short bfType; 
	unsigned long bfSize; 
	unsigned short bfReserved1; 
	unsigned short bfReserved2;  
	unsigned long bfOffBits; 
} BITMAPFILEHEADER; 


typedef struct tagBITMAPINFOHEADER{
	unsigned long biSize;  
	long biWidth;  
	long biHeight;  
	unsigned short biPlanes;  
	unsigned short biBitCount; 
	unsigned long biCompression;  
	unsigned long biSizeImage; 
	long biXPelsPerMeter; 
	long biYPelsPerMeter; 
	unsigned long biClrUsed; 
	unsigned long biClrImportant; 
} BITMAPINFOHEADER; 

bool CreateBmp(std::string filename, uint8_t *pRGBBuffer, int width, int height, int bpp);
bool CreateJpeg(std::string filename, uint8_t *pRGBBuffer, int width, int height);
bool CreatePng(std::string filename, uint8_t *pRGBBuffer, int width, int height);

int main(int argc, char* argv[])
{

	int iResult = 0;
	// 注册
	av_register_all();
	std::string strfile = "video/Titanic.mkv";
	AVFormatContext* pavFmtCxt = NULL;
	// 打开
	iResult = avformat_open_input(&pavFmtCxt, strfile.c_str(), NULL, NULL);
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
	iResult = avcodec_open2(pavFmtCxt->streams[iVidStrmID]->codec, pDecodec, NULL);
	assert(iResult >= 0);

	// 显示信息
	av_dump_format(pavFmtCxt, iVidStrmID, strfile.c_str(), 0);

	// 读取文件,解码.
	AVFrame* pFrame = avcodec_alloc_frame();
	AVPacket pkt;
	av_init_packet(&pkt);
	while (av_read_frame(pavFmtCxt, &pkt)>= 0)
	{
		if (pkt.stream_index == iVidStrmID)
		{
			int iFinished = 0;
			AVCodecContext* pavCCxt = NULL;
			pavCCxt = pavFmtCxt->streams[iVidStrmID]->codec;
			int iDecoded = avcodec_decode_video2(pavCCxt, pFrame,
				&iFinished, &pkt);
			
			if (iDecoded > 0 && iFinished)
			{
				int width, height;
				width = pavFmtCxt->streams[iVidStrmID]->codec->width;
				height = pavFmtCxt->streams[iVidStrmID]->codec->height;
				// 目标大小
				int dstwidth = width / 2;
				int dstheight = height / 2;
				SwsContext* pSwsCxt = sws_getContext(width, height, PIX_FMT_YUV420P,
					dstwidth, dstheight, PIX_FMT_RGB32, SWS_BILINEAR, NULL, NULL, NULL);
				uint8_t *rgb_data = static_cast<uint8_t*>(av_malloc(dstwidth*dstheight*4));
				uint8_t *rgb_src[3]= {rgb_data, NULL, NULL};
				int rgb_stride[3]={4*dstwidth, 0, 0};
				assert(pSwsCxt);
				// 直接用sws_scale 进行缩放
				iResult = sws_scale(pSwsCxt, pFrame->data, pFrame->linesize,
					0, height, rgb_src, rgb_stride);
				assert(iResult == dstheight);

				std::string sname;
				std::stringstream ss;
				ss << pkt.pts;
				ss >> sname;
				bool ret = false;

				// 现在rgb_data 中是BGRA排序
				// 将rgb_data 转换为RGBA排序
				for (int k = 0; k < dstheight; ++k)
				{
					for (int j = 0; j < (dstwidth-1) * 4; j += 4)
					{
						uint8_t tempc = *(rgb_data + k * 4 * dstwidth + j);
						*(rgb_data + k * 4 * dstwidth + j) = *(rgb_data + k * 4 * dstwidth + j + 2);
						*(rgb_data + k * 4 * dstwidth + j + 2) = tempc;

					}
				}

				ret = CreateJpeg("output/"+sname, rgb_data, dstwidth, dstheight);
				ret = CreatePng("output/"+sname, rgb_data, dstwidth, dstheight);
				ret = CreateBmp("output/"+sname, rgb_data, dstwidth, dstheight, 32);
			//	assert(ret);

				::memset(rgb_data, 0, dstwidth*dstheight*4);
				av_freep(&rgb_data);
				sws_freeContext(pSwsCxt);

				// 解码成功,退出
				break;
			}
			else
			{
				printf("解码失败");
			}
		}
	}
	
	return 0;
}

bool CreateBmp(std::string filename, uint8_t *pRGBBuffer, int width, int height, int bpp)
{
	for (int k = 0; k < height; ++k)
	{
		for (int j = 0; j < (width-1) * 4; j += 4)
		{
			uint8_t tempc = *(pRGBBuffer + k * 4 * width + j);
			*(pRGBBuffer + k * 4 * width + j) = *(pRGBBuffer + k * 4 * width + j + 2);
			*(pRGBBuffer + k * 4 * width + j + 2) = tempc;

		}
	}

	BITMAPFILEHEADER bmpheader;
	BITMAPINFOHEADER bmpinfo;
	FILE *fp = NULL;

	filename = filename + ".bmp";
	fp = fopen(filename.c_str(),"wb");
	if( fp == NULL )
	{
		return false;
	}

	bmpheader.bfType = ('M' <<8)|'B';
	bmpheader.bfReserved1 = 0;
	bmpheader.bfReserved2 = 0;
	bmpheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmpheader.bfSize = bmpheader.bfOffBits + width*height*bpp/8;

	bmpinfo.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.biWidth = width;
	bmpinfo.biHeight = 0 - height;
	bmpinfo.biPlanes = 1;
	bmpinfo.biBitCount = bpp;
	bmpinfo.biCompression = 0x0;
	bmpinfo.biSizeImage = 0;
	bmpinfo.biXPelsPerMeter = 100;
	bmpinfo.biYPelsPerMeter = 100;
	bmpinfo.biClrUsed = 0;
	bmpinfo.biClrImportant = 0;

	fwrite(&bmpheader,sizeof(BITMAPFILEHEADER),1,fp);
	fwrite(&bmpinfo,sizeof(BITMAPINFOHEADER),1,fp);
	fwrite(pRGBBuffer,width*height*bpp/8,1,fp);
	fclose(fp);
	fp = NULL;

	return true;
}

bool CreateJpeg(std::string filename, uint8_t *pRGBBuffer, int width, int height)
{
	filename = filename + ".jpg";

	struct jpeg_compress_struct cinfo;  
	struct jpeg_error_mgr jerr;  
	int row_stride = 0;  
	FILE* fp = NULL;  
	JSAMPROW row_pointer[1];  

	cinfo.err = jpeg_std_error(&jerr);  
	jpeg_create_compress(&cinfo);  
	fp = fopen(filename.c_str(), "wb");  
	if (fp == NULL)  
	{  
		printf("open file %s failed.\n", filename.c_str());  
		return -1;  
	}  
	jpeg_stdio_dest(&cinfo, fp);  
	cinfo.image_width = width;  
	cinfo.image_height = height;  
	cinfo.input_components = 4;  
	cinfo.in_color_space = JCS_EXT_RGBA;  
	jpeg_set_defaults(&cinfo);  
	jpeg_set_quality(&cinfo, 75, 1); 
	jpeg_start_compress(&cinfo, TRUE);  
	row_stride = width * cinfo.input_components;  

	while (cinfo.next_scanline < cinfo.image_height)  
	{  
		row_pointer[0] = &pRGBBuffer[cinfo.next_scanline * row_stride];  
		jpeg_write_scanlines(&cinfo, row_pointer, 1);  
	}  

	jpeg_finish_compress(&cinfo);  
	jpeg_destroy_compress(&cinfo);  
	fclose(fp);  
	 
	return true;
}

bool CreatePng(std::string filename, uint8_t *pRGBBuffer, int width, int height)
{
	filename = filename + ".png";

	png_structp png_ptr;
	png_infop info_ptr;
	FILE *png_file = fopen(filename.c_str(), "wb");
	assert(png_file);
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(png_ptr == NULL)
	{
		printf("ERROR:png_create_write_struct/n"); 
		return false;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if(info_ptr == NULL)
	{
		printf("ERROR:png_create_info_struct/n"); 
		png_destroy_write_struct(&png_ptr, NULL);
		return false;
	}
	png_init_io(png_ptr, png_file);
	png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGBA,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
//	png_set_PLTE(png_ptr, info_ptr, palette, palette_len);
	png_colorp palette = (png_colorp)png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH
		* (sizeof (png_color)));
	png_set_PLTE(png_ptr, info_ptr, palette, PNG_MAX_PALETTE_LENGTH);
	png_write_info(png_ptr, info_ptr);

	png_bytepp row_pointers = new png_bytep[height];
	for (int i=0; i<height; ++i)
	{
		row_pointers[i] = pRGBBuffer + i * 4 * width;
	}
	png_write_image(png_ptr, row_pointers);
	delete row_pointers;
	png_write_end(png_ptr, info_ptr);
	png_free(png_ptr, palette);
	palette=NULL;
	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(png_file);

	return true;
}
*/