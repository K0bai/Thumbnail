#pragma once
#include "thumbnail.h"

class thumbnailBmp : public thumbnail
{
public:
	thumbnailBmp(void);
	~thumbnailBmp(void);

	// ת��Ϊbmp��ʽͼ��
	virtual uint8_t* EncodeDataToImage(std::string OutputFilename, uint8_t* rgb_data, int width, int height);
};

