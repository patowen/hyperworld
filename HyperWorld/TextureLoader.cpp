#include <glad/glad.h>
#include <png.h>
#include <cstdio>
#include <vector>
#include <stdexcept>

#include "TextureLoader.h"

namespace TextureLoader {
	TextureData loadTexture(std::string name) {
		std::string texturePath = "textures/" + name;

		FILE *fp = fopen(texturePath.c_str(), "rb");
		if (!fp) {
			throw std::runtime_error("Texture file not found");
		}

		png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
		if (!png) {
			throw std::runtime_error("Failed to load png");
		}

		png_infop info = png_create_info_struct(png);
		if (!info) {
			throw std::runtime_error("Failed to load png info");
		}

		if (setjmp(png_jmpbuf(png))) {
			throw std::runtime_error("Failed to setjmp");
		}

		png_init_io(png, fp);

		png_read_info(png, info);

		int width = png_get_image_width(png, info);
		int height = png_get_image_height(png, info);
		png_byte color_type = png_get_color_type(png, info);
		png_byte bit_depth = png_get_bit_depth(png, info);

		if (bit_depth == 16) {
			png_set_strip_16(png);
		}

		if (color_type == PNG_COLOR_TYPE_PALETTE) {
			png_set_palette_to_rgb(png);
		}

		if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
			png_set_expand_gray_1_2_4_to_8(png);
		}

		if (png_get_valid(png, info, PNG_INFO_tRNS)) {
			png_set_tRNS_to_alpha(png);
		}

		if (color_type == PNG_COLOR_TYPE_RGB ||
			color_type == PNG_COLOR_TYPE_GRAY ||
			color_type == PNG_COLOR_TYPE_PALETTE) {
			png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
		}

		if (color_type == PNG_COLOR_TYPE_GRAY ||
			color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
			png_set_gray_to_rgb(png);
		}

		png_read_update_info(png, info);

		png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
		for(int y = 0; y < height; y++) {
			row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
		}

		png_read_image(png, row_pointers);

		fclose(fp);

		png_destroy_read_struct(&png, &info, NULL);

		TextureData result(width, height);

		for (int y=0; y<height; ++y) {
			for (int x=0; x<width; ++x) {
				result.data.push_back(row_pointers[y][4*x]);
				result.data.push_back(row_pointers[y][4*x+1]);
				result.data.push_back(row_pointers[y][4*x+2]);
			}
		}

		return result;
	}
}