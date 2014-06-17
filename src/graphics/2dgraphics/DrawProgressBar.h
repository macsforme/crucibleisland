// DrawProgressBar.h
// Crucible Island

#ifndef DRAWPROGRESSBAR_H
#define DRAWPROGRESSBAR_H

// program headers
#include "graphics/GameGraphics.h"
#include "logic/UILayoutAuthority.h"
#include "math/VectorMath.h"
#include "platform/OpenGLHeaders.h"

// library headers
#include <map>
#include <string>

// global variables
extern GameGraphics* gameGraphics;

class DrawProgressBar : public BaseUIElement {
public:
	DrawProgressBar();
	~DrawProgressBar();

	/*
		Arguments Layout
		----------------
		metrics			UI element metrics
		color1Top		color of first section at top
		color1Bottom	color of first section at bottom
		color2Top		color of second section at top
		color2Bottom	color of second section at bottom
		progression		completion ratio
		size			width and height in pixels
	*/

	Vector2 getSize(std::map<std::string, void*> arguments);
	void execute(std::map<std::string, void*> arguments);
};

#endif // DRAWPROGRESSBAR_H
