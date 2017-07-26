#pragma once
#include <cstdint>
#include <string>

class thumbnail
{
public:
	thumbnail(void);

	// ������Ƶ��һ֡����ͼ
	int GetImageData(std::string InputFilename, std::string OutputFilename, int DstWidth, int DstHeight);

	// ��RGB���ݽ��б���
	virtual uint8_t* EncodeDataToImage(std::string OutputFilename, uint8_t* rgb_data, int width, int height) = 0;

	~thumbnail(void);


};

