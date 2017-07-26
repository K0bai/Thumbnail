#include "thumbnailJpeg.h"

extern "C"
{
#include "jpeglib.h"
#include "jconfig.h"
};

thumbnailJpeg::thumbnailJpeg(void)
{
}


thumbnailJpeg::~thumbnailJpeg(void)
{
}

uint8_t* thumbnailJpeg::EncodeDataToImage(std::string OutputFilename, uint8_t* rgb_data, int width, int height)
{
	uint8_t *jpegdata = nullptr;
	unsigned long jpegsize = 0;
	struct jpeg_compress_struct cinfo;  
	struct jpeg_error_mgr jerr;  
	int row_stride = 0;  
	JSAMPROW row_pointer[1];  

	cinfo.err = jpeg_std_error(&jerr);  
	jpeg_create_compress(&cinfo);  
	jpeg_mem_dest(&cinfo, &jpegdata, &jpegsize);
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
		row_pointer[0] = &rgb_data[cinfo.next_scanline * row_stride];  
		jpeg_write_scanlines(&cinfo, row_pointer, 1);  
	}  

	jpeg_finish_compress(&cinfo);  
	jpeg_destroy_compress(&cinfo);  

	FILE *fp = nullptr;
	fp = fopen(OutputFilename.c_str(), "wb");
	fwrite(jpegdata, jpegsize, 1, fp);
	fclose(fp);

	return nullptr;
}
