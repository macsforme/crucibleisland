/*
 *  FontLoader.cpp
 *  ModelViewer
 *
 *  Created by Joshua Bodine on 5/28/10.
 *  Copyright 2010 ModelViewer. All rights reserved.
 *
 */

#include "FontManager.h"

FontManager::FontManager() {
	// initialize the freetype library
	if(FT_Init_FreeType(&library))
		ProgramLog::report(LOG_FATAL, "The FreeType2 library could not be initialized.");

	// load the font face
	if(
			FT_New_Face(
					library,
					std::string(platform.dataPath +
							"/data/fonts/" +
							gamePrefs.getString("fontStandardSize")).c_str(),
					0,
					&fontFace
				)
		)
		ProgramLog::report(LOG_FATAL,
				std::string("The font face could not be loaded from " +
						platform.dataPath +
						"/data/fonts/" +
						gamePrefs.getString("fontStandardSize")).c_str()
			);

	// set a default size
	if(
			FT_Set_Char_Size(
					fontFace,
					0,
					gamePrefs.getInt("fontStandardSize") * 64,
					0,
					0
				)
		)
		ProgramLog::report(LOG_FATAL, "The default font size could not be set.");
}

FontManager::~FontManager() {
	// free all the texture memory for each cached character
	for(
			std::map< char, std::map<unsigned int, FontData> >::iterator charItr =
					fontData.begin();
			charItr != fontData.end();
			++charItr
		)
		// multiple sizes
		for(
				std::map<unsigned int, FontData>::iterator sizeItr = charItr->second.begin();
				sizeItr != charItr->second.end();
				++sizeItr
			)
			delete(sizeItr->second.bitmap);

	// delete any textures we've used
	for(
			std::map<unsigned int, GLuint>::iterator texItr = textureIDs.begin();
			texItr != textureIDs.end();
			++texItr
		)
		if(glIsTexture(texItr->second))
			glDeleteTextures(1, &texItr->second);
}

float FontManager::positiveNormalize(unsigned int coordinate, unsigned int scale) {
	return (float) coordinate / (float) scale;
}

void FontManager::rebuildFontTextureCache(unsigned int size) {
	// don't rebuild a cache if there are no characters to cache
	if(getCachedCharsList(size).size() == 0) {
		return;
	}

	// determine the maximum width and height for all characters to create a square-ish texture
	unsigned int maxX = 0, maxY = 0;

	std::vector<char> charList = getCachedCharsList(size);
	for(unsigned int i = 0; i < charList.size(); ++i) {
		unsigned int thisX = fontData[charList[i]][size].width;
		unsigned int thisY = fontData[charList[i]][size].height;

		maxX = (maxX < thisX ? thisX : maxX);
		maxY = (maxY < thisY ? thisY : maxY);
	}

	// calculate the size of the texture needed
	uint8_t charsSquare = sqrt(charList.size()) + 1;	// add one due to integer rounding

	// create a local texture for the font cache
	Texture* thisFontCache = new Texture(
			maxX * charsSquare,
			maxY * charsSquare,
			Texture::FORMAT_RGBA
		);

	// copy the bitmaps into the texture and assign the element info
	unsigned int xIndex = 0, yIndex = 0;
	for(unsigned int i = 0; i < charList.size(); ++i) {
		// copy each pixel row by row, column by column
		Texture* thisGlyph = fontData[charList[i]][size].bitmap;

		for(unsigned int x = 0; x < thisGlyph->getWidth(); ++x)
			for(unsigned int y = 0; y < thisGlyph->getHeight(); ++y)
				thisFontCache->setColorAt(
						xIndex * maxX + x,
						yIndex * maxY + y,
						thisGlyph->getRedValueAt(x, y),
						thisGlyph->getGreenValueAt(x, y),
						thisGlyph->getBlueValueAt(x, y),
						thisGlyph->getAlphaValueAt(x, y)
					);

		// assign the texture coordinate info
		fontData[charList[i]][size].sXCoord = positiveNormalize(xIndex * maxX, thisFontCache->getWidth());
		fontData[charList[i]][size].sYCoord = positiveNormalize(yIndex * maxY, thisFontCache->getHeight());
		fontData[charList[i]][size].eXCoord = positiveNormalize(
				xIndex * maxX + thisGlyph->getWidth(),
				thisFontCache->getWidth()
			);
		fontData[charList[i]][size].eYCoord = positiveNormalize(
				yIndex * maxY + thisGlyph->getHeight(),
				thisFontCache->getHeight()
			);

		// advance the indices
		if(++xIndex * maxX + maxX > thisFontCache->getWidth()) {
			++yIndex;
			xIndex = 0;
		}
	}

	// send the texture to the GPU
	if(textureIDs.find(size) == textureIDs.end() || ! glIsTexture(textureIDs[size]))
		glGenTextures(1, &textureIDs[size]);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureIDs[size]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(
			GL_TEXTURE_2D,
			0,
			(thisFontCache->getFormat() == Texture::FORMAT_RGBA ? GL_RGBA : GL_RGB),
			thisFontCache->getWidth(),
			thisFontCache->getHeight(),
			0,
			(thisFontCache->getFormat() == Texture::FORMAT_RGBA ? GL_RGBA : GL_RGB),
			GL_UNSIGNED_BYTE,
			thisFontCache->getDataPointer()
		);

	// free memory
	delete(thisFontCache);
}

void FontManager::buildChar(const char character, unsigned int size) {
	// don't re-build characters we have cached already
	if(isCharCached(character, size))
		return;

	// set the current size in freetype
	if(
			FT_Set_Char_Size(
					fontFace,
					0,
					size * 64,
					0,
					0
				)
		) {
		std::stringstream err;
		err << "Could not set the desired font size of " <<
				size <<
				" for the character'" <<
				character <<
				"'.";

		ProgramLog::report(LOG_FATAL, err.str().c_str());
	}

	// load the glyph
	FT_UInt glyphIndex = FT_Get_Char_Index(fontFace, character);
	if(FT_Load_Glyph(fontFace, glyphIndex, FT_LOAD_DEFAULT)) {
		std::stringstream err;
		err << "Unable to load glyph for character '" << character << "'.";
		ProgramLog::report(LOG_FATAL, err.str().c_str());
	}
	if(FT_Render_Glyph(fontFace->glyph, FT_RENDER_MODE_NORMAL)) {
		std::stringstream err;
		err << "Unable to render glyph for character '" << character << "'.";
		ProgramLog::report(LOG_FATAL, err.str().c_str());
	}

	// Here we extract the bitmap info from the rendered glyph into a texture,
	// which we will cache for this specific character and size. The data is stored
	// in a grayscale bitmap, which we will use as the value of all color elements
	// (red, green, blue, alpha) of that pixel.

	// create the font data element
	FontData thisData;

	// populate the data
	thisData.width = fontFace->glyph->bitmap.width;
	thisData.height = fontFace->glyph->bitmap.rows;
	thisData.adjustX = fontFace->glyph->bitmap_left;
	thisData.adjustY = -(fontFace->glyph->bitmap.rows - fontFace->glyph->bitmap_top);	// +Y = up
	thisData.advanceX = fontFace->glyph->advance.x / 64;
	thisData.advanceY = (int) ((float) thisData.height * ADVANCE_Y_FACTOR);

	// create the texture bitmap for this glyph
	thisData.bitmap = new Texture(
			thisData.width,
			thisData.height,
			Texture::FORMAT_RGBA
		);
	for(int y = 0; y < fontFace->glyph->bitmap.rows; ++y) {
		for(int x = 0; x < fontFace->glyph->bitmap.width; ++x) {
			// get the location for the value of this pixel
			uint8_t* value =
					fontFace->glyph->bitmap.buffer +
					(fontFace->glyph->bitmap.rows - y - 1) *
					fontFace->glyph->bitmap.pitch +	// pitch = amount of memory per row
					x;

			// set the color and alpha to the grayscale value
			if(*value)
				thisData.bitmap->setColorAt(x, y, *value, *value, *value, *value);
		}
	}

	// store it
	fontData[character][size] = thisData;

	// rebuild the texture cache
	rebuildFontTextureCache(size);
}

bool FontManager::isCharCached(const char character, unsigned int size) {
	// return whether the font is cached or not
	if(fontData.find(character) != fontData.end())
		if(fontData[character].find(size) != fontData[character].end())
			return true;

	return false;
}

std::vector<char> FontManager::getCachedCharsList(unsigned int size) {
	// return a vector list of character glyphs cached at this size
	std::vector<char> chars;
	for(
			std::map< char, std::map<unsigned int, FontData> >::iterator itr = fontData.begin();
			itr != fontData.end();
			++itr
		)
		if(itr->second.find(size) != itr->second.end())
			chars.push_back(itr->first);

	return chars;
}

void FontManager::populateCommonChars(unsigned int size) {
	// build a cache of common characters including letters, numbers, and most symbols
	for(char i = ' '; i < '~'; ++i)
		buildChar(i, size);
}