#include "thumbnailBmp.h"
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

thumbnailBmp::thumbnailBmp(void)
{
}


thumbnailBmp::~thumbnailBmp(void)
{
}

uint8_t* thumbnailBmp::EncodeDataToImage(std::string OutputFilename, uint8_t* rgb_data, int width, int height)
{
	for (int k = 0; k < height; ++k)
	{
		for (int j = 0; j < (width-1) * 4; j += 4)
		{
			uint8_t tempc = *(rgb_data + k * 4 * width + j);
			*(rgb_data + k * 4 * width + j) = *(rgb_data + k * 4 * width + j + 2);
			*(rgb_data + k * 4 * width + j + 2) = tempc;

		}
	}

	BITMAPFILEHEADER bmpheader;
	BITMAPINFOHEADER bmpinfo;

	bmpheader.bfType = ('M' <<8)|'B';
	bmpheader.bfReserved1 = 0;
	bmpheader.bfReserved2 = 0;
	bmpheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmpheader.bfSize = bmpheader.bfOffBits + width*height*32/8;

	bmpinfo.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.biWidth = width;
	bmpinfo.biHeight = 0 - height;
	bmpinfo.biPlanes = 1;
	bmpinfo.biBitCount = 32;
	bmpinfo.biCompression = 0x0;
	bmpinfo.biSizeImage = 0;
	bmpinfo.biXPelsPerMeter = 100;
	bmpinfo.biYPelsPerMeter = 100;
	bmpinfo.biClrUsed = 0;
	bmpinfo.biClrImportant = 0;

	int totalsize = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + width*height*4;
	uint8_t *bmpdata = new uint8_t[totalsize];
	memset(bmpdata, 0, totalsize);
	memcpy(bmpdata, &bmpheader, sizeof(BITMAPFILEHEADER));
	memcpy(bmpdata+sizeof(BITMAPFILEHEADER), &bmpinfo, sizeof(BITMAPINFOHEADER));
	memcpy(bmpdata+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER), rgb_data, width*height*4);

	FILE *fp = nullptr;
	fp = fopen(OutputFilename.c_str(), "wb");
	fwrite(bmpdata, totalsize, 1, fp);
	fclose(fp);

	return nullptr;
}
