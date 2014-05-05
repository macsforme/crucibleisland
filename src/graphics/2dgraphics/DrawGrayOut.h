// DrawGrayOut.h
//
// Dominicus
// Copyright 2010-2011, Joshua Bodine
//
// Released under the terms of the "Simplified BSD License." See the file
// licenses/DOMINICUS.txt for the license text.

#ifndef DRAWGRAYOUT_H
#define DRAWGRAYOUT_H

// program headers
#include "core/GameSystem.h"
#include "graphics/GameGraphics.h"
#include "math/VectorMath.h"
#include "logic/UILayoutAuthority.h"
#include "platform/OpenGLHeaders.h"

// library headers
#include <map>
#include <string>

// global variables
extern GameGraphics* gameGraphics;
extern GameSystem* gameSystem;

class DrawGrayOut : public BaseUIElement {
public:
	DrawGrayOut();
	~DrawGrayOut();

	Vector2 getSize(std::map<std::string, void*> arguments);
	void execute(std::map<std::string, void*> arguments);
};

#endif // DRAWGRAYOUT_H