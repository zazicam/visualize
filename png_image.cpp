#include <stdio.h>
#include <string>
#include <malloc.h>
#include <png.h>

#include <stdlib.h>

#include "png_image.hpp"

inline void setRGB(png_byte *ptr, rgb color)
{
	ptr[0] = color.r;
	ptr[1] = color.g;
	ptr[2] = color.b;
}

Image::Image(int w, int h, rgb color=rgb{0,0,0}) : width(w), height(h), buffer(nullptr) {
	buffer = (rgb *) malloc(width * height * sizeof(rgb));
	if (buffer == NULL) {
		fprintf(stderr, "Error: could not malloc image buffer\n");
		exit(1);
	}
	for(int y=0;y<height;++y)
		for(int x=0;x<width;++x) {
			buffer[y*width+x] = color; 
		}
}

void Image::set_pixel(int x, int y, rgb color) {
	if(x>=0 && x<width && y>=0 && y<height) 
		buffer[y*width+x] = color;
}

void Image::circle(int x, int y, int r, rgb color) {
	for(int i=-r;i<r;++i)
		for(int j=-r;j<r;++j) {
			if(i*i+j*j<=r*r)
				set_pixel(x+j,y+i, color);
		}
}

Image::~Image() {
	if(buffer)
		free(buffer);
	buffer=nullptr;
}

int Image::save(const std::string& filename)
{
	int code = 0;
	FILE *fp = NULL;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep row = NULL;
	
	// Open file for writing (binary mode)
	fp = fopen(filename.c_str(), "wb");
	if (fp == NULL) {
		fprintf(stderr, "Could not open file %s for writing\n", filename.c_str());
		code = 1;
		goto finalise;
	}

	// Initialize write structure
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fprintf(stderr, "Could not allocate write struct\n");
		code = 1;
		goto finalise;
	}

	// Initialize info structure
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fprintf(stderr, "Could not allocate info struct\n");
		code = 1;
		goto finalise;
	}

	// Setup Exception handling
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "Error during png creation\n");
		code = 1;
		goto finalise;
	}

	png_init_io(png_ptr, fp);

	// Write header (8 bit colour depth)
	png_set_IHDR(png_ptr, info_ptr, width, height,
			8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	// Allocate memory for one row (3 bytes per pixel - RGB)
	row = (png_bytep) malloc(3 * width * sizeof(png_byte));

	// Write image data
	int x, y;
	for (y=0;y<height;++y) {
		for (x=0;x<width;++x) {
			rgb color = buffer[y * width + x];
			setRGB(&row[x*3], color);
		}
		png_write_row(png_ptr, row);
	}

	// End write
	png_write_end(png_ptr, NULL);

	finalise:
	if (fp != NULL) fclose(fp);
	if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	if (row != NULL) free(row);

	return code;
}

