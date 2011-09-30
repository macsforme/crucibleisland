// Texture.h
//
// Dominicus
// Copyright 2010-2011, Joshua Bodine
//
// Released under the terms of the "Simplified BSD License." See the file
// licenses/DOMINICUS.txt for the license text.

#ifndef TEXTURE_H
#define TEXTURE_H

// program headers
#include "core/GameSystem.h"

// library headers
#include <cstring>
#include <fstream>
#include <sstream>
#include <stdint.h>
#include <string>

// global variables
extern GameSystem* gameSystem;

class Texture {
public:
	enum PixelFormat {
			FORMAT_RGB,
			FORMAT_RGBA
		};

	uint32_t width, height;
	PixelFormat format;

	void* pixelData;

	void* getDataPointer() { return pixelData; }

	uint8_t getRedValueAt(uint32_t column, uint32_t row);
	uint8_t getGreenValueAt(uint32_t column, uint32_t row);
	uint8_t getBlueValueAt(uint32_t column, uint32_t row);
	uint8_t getAlphaValueAt(uint32_t column, uint32_t row);
	void setColorAt(uint32_t column, uint32_t row, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);

	void setDepth(unsigned int depth);

	Texture(uint32_t newWidth, uint32_t newHeight, PixelFormat newFormat);
	Texture(std::string filename);
	~Texture();
};

#endif