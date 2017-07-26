#pragma once
#include "thumbnail.h"

class thumbnailBmp : public thumbnail
{
public:
	thumbnailBmp(void);
	~thumbnailBmp(void);

	// 转换为bmp格式图像
	virtual uint8_t* EncodeDataToImage(std::string OutputFilename, uint8_t* rgb_data, int width, int height);
};

