#pragma once
#include "thumbnail.h"

class thumbnailJpeg : public thumbnail
{
public:
	thumbnailJpeg(void);
	~thumbnailJpeg(void);

	// ת��ΪJpeg��ʽͼ��
	virtual uint8_t* EncodeDataToImage(std::string OutputFilename, uint8_t* rgb_data, int width, int height);
};

