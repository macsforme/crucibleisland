/*
 * DrawConsole.h
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

#ifndef DRAWCONSOLE_H
#define DRAWCONSOLE_H

// program headers
#include "core/GamePrefs.h"
#include "core/ProgramLog.h"
#include "drawing/HUD/DrawHUDContainerUtility.h"
#include "drawing/HUD/HUDArrangement.h"
#include "drawing/HUD/HUDElement.h"
#include "drawing/ShaderTools.h"
#include "drawing/text/FontManager.h"
#include "drawing/text/TextBlock.h"
#include "platform/OpenGLHeaders.h"
#include "platform/Platform.h"
#include "math/VectorMath.h"
#include "windowing/GameWindow.h"

// library headers
#include <string>
#include <vector>

// global variables
extern Platform* platform;
extern GamePrefs* gamePrefs;
extern GameWindow* gameWindow;
extern ProgramLog* programLog;

class DrawConsole {
private:
	FontManager* fontManager;
	HUDElement* myElement;
	TextBlock* textBlock;
	DrawHUDContainerUtility* containerUtility;

	// program/shader names
	GLuint program, vertexShader, fragmentShader;

	// attributes
	GLint positionAttrib, texCoordAttrib, colorAttrib;

	// uniforms
	GLint mvpMatrixUniform, textureUniform, useTextureUniform, useLightingUniform;

public:
	DrawConsole(HUDArrangement* hudArrangement, FontManager* fontManager);

	void draw();
};

#endif
