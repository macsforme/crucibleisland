/*
 * DrawCursor.cpp
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

#include "graphics/2dgraphics/cursor/DrawCursor.h"

DrawCursor::DrawCursor() {
	// set up shader
	GLuint shaderID = 0;
	std::vector<GLuint> shaderIDs;

	shaderID = gameGraphics->getShaderID(GL_VERTEX_SHADER, "cursor"); shaderIDs.push_back(shaderID);
	shaderID = gameGraphics->getShaderID(GL_FRAGMENT_SHADER, "cursor"); shaderIDs.push_back(shaderID);
	shaderProgram = gameGraphics->makeProgram(shaderIDs);

	// set up uniforms and attributes
	uniforms["thickness"] = glGetUniformLocation(shaderProgram, "thickness");
	uniforms["color"] = glGetUniformLocation(shaderProgram, "color");

	attributes["position"] = glGetAttribLocation(shaderProgram, "position");
	attributes["boxCoord"] = glGetAttribLocation(shaderProgram, "boxCoord");

	// set up vertex buffers
	glGenBuffers(1, &(vertexBuffers["vertices"]));
	glGenBuffers(1, &(vertexBuffers["elements"]));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffers["elements"]);

	GLushort elementBufferArray[] = { 0, 1, 2, 3 };

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elementBufferArray), elementBufferArray,
			GL_STATIC_DRAW);
}

DrawCursor::~DrawCursor() {
	// delete buffers
	if(glIsBuffer(vertexBuffers["vertices"]))
		glDeleteBuffers(1, &(vertexBuffers["vertices"]));
	if(glIsBuffer(vertexBuffers["elements"]))
		glDeleteBuffers(1, &(vertexBuffers["elements"]));

	// delete shader program
	if(glIsProgram(shaderProgram))
		glDeleteProgram(shaderProgram);
}

void DrawCursor::execute(std::map<std::string, void*> arguments) {
	// update position
	Vector2* position = *((Vector2**) arguments["position"]);
	float cursorSize = *((float*) arguments["size"]);
	float distY = cursorSize / (float) gameGraphics->resolutionY;
	float distX = distY / gameGraphics->aspectRatio;

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers["vertices"]);

	GLfloat vertexBufferArray[] = {
			position->x - distX, position->y - distY, -1.0f, -1.0f,
			position->x - distX, position->y + distY, -1.0f, 1.0f,
			position->x + distX, position->y + distY, 1.0f, 1.0f,
			position->x + distX, position->y - distY, 1.0f, -1.0f
		};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferArray), vertexBufferArray, GL_STREAM_DRAW);
//	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexBufferArray), vertexBufferArray);

	// state
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// enable shader
	glUseProgram(shaderProgram);

	// set uniforms
	glUniform1f(uniforms["thickness"],
			*((float*) arguments["thickness"]) /
			gameGraphics->resolutionY / distY
		);	// effectively half
	Vector4 color = *((Vector4*) arguments["color"]);
	glUniform4f(uniforms["color"], color.x, color.y, color.z, color.w);

	// draw the data stored in GPU memory
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers["vertices"]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffers["elements"]);

	glVertexAttribPointer(attributes["position"], 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*) 0);
	glVertexAttribPointer(attributes["boxCoord"], 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT),
			(GLvoid*) (2 * sizeof(GLfloat)));

	glEnableVertexAttribArray(attributes["position"]);
	glEnableVertexAttribArray(attributes["boxCoord"]);

	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, NULL);

	glDisableVertexAttribArray(attributes["position"]);
	glDisableVertexAttribArray(attributes["boxCoord"]);

	// undo state
	glDisable(GL_BLEND);
}
