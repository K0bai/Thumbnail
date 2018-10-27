
#if !defined(THUMBNAIL_H)
#define THUMBNAIL_H

#include <cstdint>
#include <string>

class thumbnail
{
public:
	thumbnail(void);

	int GetImageData(std::string InputFilename, std::string OutputFilename, int DstWidth, int DstHeight);
	virtual uint8_t* EncodeDataToImage(std::string OutputFilename, uint8_t* rgb_data, int width, int height) = 0;

	~thumbnail(void);


};


#endif // THUMBNAIL_H



