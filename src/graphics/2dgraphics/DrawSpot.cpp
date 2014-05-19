// DrawSpot.cpp
//
// Dominicus
// Copyright 2010-2011, Joshua Bodine
//
// Released under the terms of the "Simplified BSD License." See the file
// licenses/DOMINICUS.txt for the license text.

#include "graphics/2dgraphics/DrawSpot.h"

DrawSpot::DrawSpot() {
	// set up shader
	GLuint shaderID = 0;
	std::vector<GLuint> shaderIDs;

	shaderID = gameGraphics->getShaderID(GL_VERTEX_SHADER, "hudContainer"); shaderIDs.push_back(shaderID);
	shaderID = gameGraphics->getShaderID(GL_FRAGMENT_SHADER, "hudContainer"); shaderIDs.push_back(shaderID);
	shaderProgram = gameGraphics->makeProgram(shaderIDs);

	// set up uniforms and attributes
	uniforms["insideColor"] = glGetUniformLocation(shaderProgram, "insideColor");
	uniforms["borderColor"] = glGetUniformLocation(shaderProgram, "borderColor");
	uniforms["outsideColor"] = glGetUniformLocation(shaderProgram, "outsideColor");
	uniforms["softEdge"] = glGetUniformLocation(shaderProgram, "softEdge");

	attributes["position"] = glGetAttribLocation(shaderProgram, "position");
	attributes["primCoord"] = glGetAttribLocation(shaderProgram, "primCoord");
	attributes["curveOriginCoord"] = glGetAttribLocation(shaderProgram, "curveOriginCoord");
	attributes["border1Dist"] = glGetAttribLocation(shaderProgram, "border1Dist");
	attributes["border2Dist"] = glGetAttribLocation(shaderProgram, "border2Dist");

	// set up vertex buffers
	glGenBuffers(1, &(vertexBuffers["vertices"]));
	glGenBuffers(1, &(vertexBuffers["elements"]));
}

void DrawSpot::execute(std::map<std::string, void*> arguments) {
	// set up geometry
	Vector2 position = *((Vector2*) arguments["position"]);
	Vector2 size = *((Vector2*) arguments["size"]);
	std::vector<VertexEntry> quadVertices;

	// center geometry
	drawCurve(
			&quadVertices,
			Vector2(position.x - size.x / 2.0f, position.y - size.y / 2.0f),
			size,
			180.0f
		);
	drawCurve(
			&quadVertices,
			Vector2(position.x - size.x / 2.0f, position.y + size.y / 2.0f),
			size,
			90.0f
		);
	drawCurve(
			&quadVertices,
			Vector2(position.x + size.x / 2.0f, position.y + size.y / 2.0f),
			size,
			0.0f
		);
	drawCurve(
			&quadVertices,
			Vector2(position.x + size.x / 2.0f, position.y - size.y / 2.0f),
			size,
			270.0f
		);

	// update vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers["vertices"]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffers["elements"]);

	GLuint* elementBufferArray = new GLuint[quadVertices.size()];
	for(size_t i = 0; i < quadVertices.size(); ++i)
		elementBufferArray[i] = i;

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadVertices.size() * sizeof(GLuint), elementBufferArray,
			GL_STREAM_DRAW);

	delete[] elementBufferArray;

	size_t vertexBufferArraySize = quadVertices.size() * 10;
	GLfloat* vertexBufferArray = new GLfloat[vertexBufferArraySize];

	for(size_t i = 0; i < quadVertices.size(); ++i) {
		vertexBufferArray[i * 8 + 0] = quadVertices[i].position.x;
		vertexBufferArray[i * 8 + 1] = quadVertices[i].position.y;
		vertexBufferArray[i * 8 + 2] = quadVertices[i].primCoord.x;
		vertexBufferArray[i * 8 + 3] = quadVertices[i].primCoord.y;
		vertexBufferArray[i * 8 + 4] = quadVertices[i].curveOriginCoord.x;
		vertexBufferArray[i * 8 + 5] = quadVertices[i].curveOriginCoord.y;
		vertexBufferArray[i * 8 + 6] = 2.0f;
		vertexBufferArray[i * 8 + 7] = 2.0f;
	}

	glBufferData(GL_ARRAY_BUFFER, vertexBufferArraySize * sizeof(GLfloat), vertexBufferArray,
			GL_STREAM_DRAW);

	delete[] vertexBufferArray;

	// state
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// enable shader
	glUseProgram(shaderProgram);

	// set uniforms
	Vector4 insideColor = *((Vector4*) arguments["insideColor"]);
	Vector4 borderColor = *((Vector4*) arguments["outsideColor"]);
	Vector4 outsideColor = *((Vector4*) arguments["outsideColor"]);
	glUniform4f(uniforms["insideColor"], insideColor.x, insideColor.y, insideColor.z, insideColor.w);
	glUniform4f(uniforms["borderColor"], borderColor.x, borderColor.y, borderColor.z, borderColor.w);
	glUniform4f(uniforms["outsideColor"], outsideColor.x, outsideColor.y, outsideColor.z, outsideColor.w);
	glUniform1f(uniforms["softEdge"], *((float*) arguments["softEdge"]) * 2.0f / (size.y * gameGraphics->resolutionY / 2.0f));

	// draw the data stored in GPU memory
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers["vertices"]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffers["elements"]);

	glVertexAttribPointer(attributes["position"], 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT),
			(GLvoid*) 0);
	glVertexAttribPointer(attributes["primCoord"], 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT),
			(GLvoid*) (2 * sizeof(GLfloat)));
	glVertexAttribPointer(attributes["curveOriginCoord"], 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT),
			(GLvoid*) (4 * sizeof(GLfloat)));
	glVertexAttribPointer(attributes["border1Dist"], 1, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT),
			(GLvoid*) (6 * sizeof(GLfloat)));
	glVertexAttribPointer(attributes["border2Dist"], 1, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT),
			(GLvoid*) (7 * sizeof(GLfloat)));

	glEnableVertexAttribArray(attributes["position"]);
	glEnableVertexAttribArray(attributes["primCoord"]);
	glEnableVertexAttribArray(attributes["curveOriginCoord"]);
	glEnableVertexAttribArray(attributes["border1Dist"]);
	glEnableVertexAttribArray(attributes["border2Dist"]);

	glDrawElements(GL_QUADS, quadVertices.size(), GL_UNSIGNED_INT, NULL);

	glDisableVertexAttribArray(attributes["position"]);
	glDisableVertexAttribArray(attributes["primiCoord"]);
	glDisableVertexAttribArray(attributes["curveOriginCoord"]);
	glDisableVertexAttribArray(attributes["border1Dist"]);
	glDisableVertexAttribArray(attributes["border2Dist"]);

	// undo state
	glDisable(GL_BLEND);
}

void DrawSpot::drawCurve(
		std::vector<VertexEntry>* quadVertices,
		Vector2 position,
		Vector2 size,
		float rotation,
		bool highlight,
		bool concave
	) {
	VertexEntry vertices[4];

	// set attributes that stay the same
	for(size_t i = 0; i < 4; ++i) {
		vertices[i].curveOriginCoord = Vector2(
				-(sqrt(2.0f) * cos(radians(rotation + 45.0f))),
				-(sqrt(2.0f) * sin(radians(rotation + 45.0f)))
			);
		vertices[i].highlight = highlight;
		vertices[i].concave = concave;
	}

	// set distinct attributes (position, primCoord)
	vertices[0].position = Vector2(
			position.x - size.x / 2.0f,
			position.y - size.y / 2.0f
		);
	vertices[0].primCoord = Vector2(-1.0f, -1.0f);

	vertices[1].position = Vector2(
			position.x - size.x / 2.0f,
			position.y + size.y / 2.0f
		);
	vertices[1].primCoord = Vector2(-1.0f, 1.0f);

	vertices[2].position = Vector2(
			position.x + size.x / 2.0f,
			position.y + size.y / 2.0f
		);
	vertices[2].primCoord = Vector2(1.0f, 1.0f);

	vertices[3].position = Vector2(
			position.x + size.x / 2.0f,
			position.y - size.y / 2.0f
		);
	vertices[3].primCoord = Vector2(1.0f, -1.0f);

	// push back the quad vertices
	for(size_t i = 0; i < 4; ++i)
		quadVertices->push_back(vertices[i]);
}
