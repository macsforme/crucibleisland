/*
 * DrawTexture.h
 * Dominicus
 *
 * Copyright 2010-2011, Joshua Bodine
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 	* Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer.
 *
 *	* Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in the
 *	documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef DRAWTEXTURE_H
#define DRAWTEXTURE_H

// program headers
#include "graphics/GameGraphics.h"
#include "graphics/texture/Texture.h"
#include "graphics/2dgraphics/UI/BaseUIElement.h"
#include "logic/UILayoutAuthority.h"
#include "math/VectorMath.h"
#include "platform/OpenGLHeaders.h"

// library headers
#include <map>
#include <string>

// global variables
extern GameGraphics* gameGraphics;

class DrawTexture : public BaseUIElement {
public:
	DrawTexture();
	~DrawTexture();

	/*
		Arguments Layout
		----------------
		metrics			UI element metrics
		texture			texture filename
	*/

	Vector2 getSize(std::map<std::string, void*> arguments);
	void execute(std::map<std::string, void*> arguments);
};

#endif // DRAWTEXTURE_H
