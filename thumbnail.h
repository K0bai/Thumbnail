#pragma once
#include <cstdint>
#include <string>

class thumbnail
{
public:
	thumbnail(void);

	// 返回视频中一帧缩略图
	int GetImageData(std::string InputFilename, std::string OutputFilename, int DstWidth, int DstHeight);

	// 对RGB数据进行编码
	virtual uint8_t* EncodeDataToImage(std::string OutputFilename, uint8_t* rgb_data, int width, int height) = 0;

	~thumbnail(void);


};

