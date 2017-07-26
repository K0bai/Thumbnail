#include "thumbnailPng.h"
#include <png.h> 

thumbnailPng::thumbnailPng(void)
{
}


thumbnailPng::~thumbnailPng(void)
{
}

struct mem_encode
{
	char *pngdata;
	size_t pngsize;
};

void my_png_flush(png_structp png_ptr)
{
}

void my_png_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	struct mem_encode* p=(struct mem_encode*)png_get_io_ptr(png_ptr);
	size_t nsize = p->pngsize + length;

	if(p->pngdata)
		p->pngdata = (char*)realloc(p->pngdata, nsize);
	else
		p->pngdata = (char*)malloc(nsize);

	if(!p->pngdata)
		png_error(png_ptr, "Write Error");

	memcpy((p->pngdata + p->pngsize), data, length);
	p->pngsize += length;
}

uint8_t* thumbnailPng::EncodeDataToImage(std::string OutputFilename, uint8_t* rgb_data, int width, int height)
{
	png_structp png_ptr;
	png_infop info_ptr;

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if(png_ptr == nullptr)
	{
		return false;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if(info_ptr == nullptr)
	{
		png_destroy_write_struct(&png_ptr, nullptr);
		return false;
	}

	struct mem_encode state;
	state.pngdata = nullptr;
	state.pngsize = 0;
	png_set_write_fn(png_ptr, &state, my_png_write_data, my_png_flush);

	png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGBA,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_colorp palette = (png_colorp)png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH
		* (sizeof (png_color)));
	png_set_PLTE(png_ptr, info_ptr, palette, PNG_MAX_PALETTE_LENGTH);
	png_write_info(png_ptr, info_ptr);

	png_bytepp row_pointers = new png_bytep[height];
	for (int i=0; i<height; ++i)
	{
		row_pointers[i] = rgb_data + i * 4 * width;
	}
	png_write_image(png_ptr, row_pointers);
	delete row_pointers;
	png_write_end(png_ptr, info_ptr);
	png_free(png_ptr, palette);
	palette = nullptr;
	png_destroy_write_struct(&png_ptr, &info_ptr);

	FILE *fp = nullptr;
	fp = fopen(OutputFilename.c_str(), "wb");
	fwrite(state.pngdata, state.pngsize, 1, fp);
	fclose(fp);

	return nullptr;
}