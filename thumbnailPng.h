#pragma once
#include "thumbnail.h"

class thumbnailPng : public thumbnail
{
public:
	thumbnailPng(void);
	~thumbnailPng(void);

	// ת��ΪPng��ʽͼ��
	virtual uint8_t* EncodeDataToImage(std::string OutputFilename, uint8_t* rgb_data, int width, int height);
};

