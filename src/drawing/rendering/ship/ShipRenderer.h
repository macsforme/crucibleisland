/*
 *  ShipRenderer.h
 *  dominicus
 *
 *  Created by Joshua Bodine on 7/26/10.
 *  Copyright 2010 Joshua Bodine. All rights reserved.
 *
 */

#ifndef SHIPRENDERER_H
#define SHIPRENDERER_H

// program headers
#include "core/GamePrefs.h"
#include "core/ProgramLog.h"
#include "drawing/texture/BMPImage.h"
#include "drawing/ShaderTools.h"
#include "geometry/Mesh.h"
#include "math/MatrixMath.h"
#include "math/ScalarMath.h"
#include "math/VectorMath.h"
#include "platform/OpenGLHeaders.h"
#include "platform/Platform.h"

// library headers
#include <stdint.h>
#include <string>
#include <vector>

// global variables
extern Platform platform;
extern ProgramLog programLog;

class ShipRenderer {
private:
	// program/shader names
	GLuint modelProgram, modelVertexShader, modelFragmentShader;
	GLuint wireframeProgram, wireframeVertexShader, wireframeFragmentShader;

	// vertex/element buffers
//	GLuint vertexBuffer;
//	std::map<std::string,GLuint> elementBuffers;

	// attributes
	GLint positionAttrib, normalAttrib, texCoordAttrib;

	// uniforms
	GLint mvpMatrixUniform, textureUniform, clockUniform;

	// texture IDs
	std::map<std::string,GLuint> textureIDs;

	// view switches
	bool viewModel;
	bool viewWireFrame;

public:
	Mesh ship;

	ShipRenderer();
	~ShipRenderer();

	void render(Matrix4 mvpMatrix);
};

#endif
