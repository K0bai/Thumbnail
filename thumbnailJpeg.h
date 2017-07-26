#pragma once
#include "thumbnail.h"

class thumbnailJpeg : public thumbnail
{
public:
	thumbnailJpeg(void);
	~thumbnailJpeg(void);

	// 转换为Jpeg格式图像
	virtual uint8_t* EncodeDataToImage(std::string OutputFilename, uint8_t* rgb_data, int width, int height);
};

