/*
 *  DrawCursor.h
 *  dominicus
 *
 *  Created by Joshua Bodine on 11/5/10.
 *  Copyright 2010 Joshua Bodine. All rights reserved.
 *
 */

#ifndef DRAWCURSOR_H
#define DRAWCURSOR_H

// program headers
#include "GameWindow.h"
#include "Mouse.h"
#include "OpenGLHeaders.h"
#include "Platform.h"
#include "ShaderTools.h"
#include "VectorMath.h"

// global variables
extern Mouse mouse;
extern Platform platform;

class DrawCursor {
private:
	// screen dimensions
	GameWindow* gameWindow;

	// program/shader names
	GLuint program, vertexShader, fragmentShader;

	// attributes
	GLint positionAttrib, boxCoordAttrib;

	// uniforms
	GLint thicknessUniform;

public:
	DrawCursor(GameWindow* gameWindow);

	void draw();
};

#endif
