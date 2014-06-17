// DrawCircle.h
// Crucible Island

#ifndef DRAWCIRCLE_H
#define DRAWCIRCLE_H

// program headers
#include "graphics/GameGraphics.h"
#include "math/MatrixMath.h"
#include "math/ScalarMath.h"
#include "math/VectorMath.h"
#include "platform/OpenGLHeaders.h"
#include "platform/Platform.h"

// library headers
#include <map>
#include <string.h>
#include <vector>

// global variables
extern GameGraphics* gameGraphics;
extern Platform* platform;

class DrawCircle : public BaseDrawNode {
protected:
	struct VertexEntry {
		Vector2 position;
		Vector2 primCoord;
		Vector2 curveOriginCoord;
		bool highlight;
		bool concave;
	};

	// utility drawing functions
	void drawCurve(std::vector<VertexEntry>* quadVertices, Vector2 position, Vector2 size, float rotation,
			bool highlight = false, bool concave = false);

public:
	DrawCircle();

	/*
		Arguments Layout
		----------------
		size			width/height in pixels
		position		position
		border			thickness of border in pixels
		softEdge		thickness of antialiasing of border in pixels
		insideColor		color of inside of container
		borderColor		color of border of container
		outsideColor	color of outside of container
	*/

	void execute(std::map<std::string, void*> newDrawData);
};

#endif // DRAWCIRCLE_H
