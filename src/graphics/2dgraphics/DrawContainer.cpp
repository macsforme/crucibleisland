// DrawContainer.cpp
// Crucible Island

#include "graphics/2dgraphics/DrawContainer.h"

#include <cmath>
#include <vector>

#include "math/ScalarMath.h"
#include "graphics/GameGraphics.h"
#include "graphics/UILayoutAuthority.h"
#include "platform/OpenGLHeaders.h"

extern GameGraphics* gameGraphics;

void DrawContainer::drawCurve(
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

void DrawContainer::drawBorder(
		std::vector<VertexEntry>* quadVertices,
		Vector2 position,
		Vector2 size,
		float rotation,
		bool highlight
	) {
	VertexEntry vertices[4];

	// set attributes that stay the same
	for(size_t i = 0; i < 4; ++i) {
		vertices[i].curveOriginCoord = Vector2(
				-cos(radians(rotation)),
				-sin(radians(rotation))
			);
		vertices[i].highlight = highlight;
		vertices[i].concave = false;
	}

	// set distinct attributes (position, primCoord)
	vertices[0].position = Vector2(
			position.x - size.x / 2.0f,
			position.y - size.y / 2.0f
		);
	vertices[0].primCoord = Vector2(
			-absolute(cos(radians(rotation))),
			-absolute(sin(radians(rotation)))
		);

	vertices[1].position = Vector2(
			position.x - size.x / 2.0f,
			position.y + size.y / 2.0f
		);
	vertices[1].primCoord = Vector2(
			-absolute(cos(radians(rotation))),
			absolute(sin(radians(rotation)))
		);

	vertices[2].position = Vector2(
			position.x + size.x / 2.0f,
			position.y + size.y / 2.0f
		);
	vertices[2].primCoord = Vector2(
			absolute(cos(radians(rotation))),
			absolute(sin(radians(rotation)))
		);

	vertices[3].position = Vector2(
			position.x + size.x / 2.0f,
			position.y - size.y / 2.0f
		);
	vertices[3].primCoord = Vector2(
			absolute(cos(radians(rotation))),
			-absolute(sin(radians(rotation)))
		);

	// push back the quad vertices
	for(size_t i = 0; i < 4; ++i)
		quadVertices->push_back(vertices[i]);
}

void DrawContainer::drawFiller(
		std::vector<VertexEntry>* quadVertices,
		Vector2 position,
		Vector2 size,
		bool highlight
	) {
	VertexEntry vertices[4];

	// set attributes that stay the same
	for(size_t i = 0; i < 4; ++i) {
		vertices[i].primCoord = Vector2(0.0f, 0.0f);
		vertices[i].curveOriginCoord = Vector2(0.0f, 0.0f);
		vertices[i].highlight = highlight;
		vertices[i].concave = false;
	}

	// set distinct attribute (position)
	vertices[0].position = Vector2(
			position.x - size.x / 2.0f,
			position.y - size.y / 2.0f
		);

	vertices[1].position = Vector2(
			position.x - size.x / 2.0f,
			position.y + size.y / 2.0f
		);

	vertices[2].position = Vector2(
			position.x + size.x / 2.0f,
			position.y + size.y / 2.0f
		);

	vertices[3].position = Vector2(
			position.x + size.x / 2.0f,
			position.y - size.y / 2.0f
		);

	// push back the quad vertices
	for(size_t i = 0; i < 4; ++i)
		quadVertices->push_back(vertices[i]);
}

DrawContainer::DrawContainer() {
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

DrawContainer::~DrawContainer() {
	// undo shader setup
	glDeleteBuffers(1, &(vertexBuffers["vertices"]));
	glDeleteBuffers(1, &(vertexBuffers["elements"]));

	if(! glIsShader(shaderProgram)) // sometimes duplicate shaders get optimized out so check for validity
		return;

	GLsizei shaderCount;
	GLuint* shaders = new GLuint[2];
	glGetAttachedShaders(shaderProgram, 2, &shaderCount, shaders);

	for(size_t i = 0; i < shaderCount; ++i) {
		glDetachShader(shaderProgram, shaders[i]);
		glDeleteShader(shaders[i]);
	}

	delete[] shaders;

	glDeleteProgram(shaderProgram);
}

DrawStackArgList DrawContainer::instantiateArgList() {
	DrawStackArgList argList;

	argList["border"] = (void*) new float;			// thickness of border in pixels
	argList["borderColor"] = (void*) new Vector4;	// color of border of container
	argList["insideColor"] = (void*) new Vector4;	// color of inside of container
	argList["metrics"] = (void*) new UIMetrics;		// UI element metrics
	argList["outsideColor"] = (void*) new Vector4;	// color of outside of container
	argList["padding"] = (void*) new float;			// width/height of curved edge in pixels
	argList["size"] = (void*) new Vector2;			// width/height of container in screen dimensions
	argList["softEdge"] = (void*) new float;		// thickness of antialiasing of border in pixels

	return argList;
}

void DrawContainer::deleteArgList(DrawStackArgList argList) {
	if(argList.find("border") != argList.end()) delete (float*) argList["border"];
	if(argList.find("borderColor") != argList.end()) delete (Vector4*) argList["borderColor"];
	if(argList.find("insideColor") != argList.end()) delete (Vector4*) argList["insideColor"];
	if(argList.find("metrics") != argList.end()) delete (UIMetrics*) argList["metrics"];
	if(argList.find("outsideColor") != argList.end()) delete (Vector4*) argList["outsideColor"];
	if(argList.find("padding") != argList.end()) delete (float*) argList["padding"];
	if(argList.find("size") != argList.end()) delete (Vector2*) argList["size"];
	if(argList.find("softEdge") != argList.end()) delete (float*) argList["softEdge"];
}

void DrawContainer::execute(DrawStackArgList argList) {
	// set up geometry
	Vector2 position = ((UIMetrics*) argList["metrics"])->position;
	Vector2 size = ((UIMetrics*) argList["metrics"])->size;//*((Vector2*) argList["size"]);
	Vector2 padding = Vector2(
			*((float*) argList["padding"]) * 2.0f / (float) gameGraphics->resolutionX,
			*((float*) argList["padding"]) * 2.0f / (float) gameGraphics->resolutionY
		);
	float border = *((float*) argList["border"]) * 2.0f / *((float*) argList["padding"]);
	std::vector<VertexEntry> quadVertices;

	drawCurve(
			&quadVertices,
			Vector2(-size.x / 2.0f + padding.x / 2.0f, -size.y / 2.0f + padding.y / 2.0f),
			padding,
			180.0f
		);
	drawCurve(
			&quadVertices,
			Vector2(-size.x / 2.0f + padding.x / 2.0f, size.y / 2.0f - padding.y / 2.0f),
			padding,
			90.0f
		);
	drawCurve(
			&quadVertices,
			Vector2(size.x / 2.0f - padding.x / 2.0f, size.y / 2.0f - padding.y / 2.0f),
			padding,
			0.0f
		);
	drawCurve(
			&quadVertices,
			Vector2(size.x / 2.0f - padding.x / 2.0f, -size.y / 2.0f + padding.y / 2.0f),
			padding,
			270.0f
		);

	drawBorder(
			&quadVertices,
			Vector2(-size.x / 2.0f + padding.x / 2.0f, 0.0f),
			Vector2(padding.x, size.y - padding.y * 2.0f),
			180.0f
		);
	drawBorder(
			&quadVertices,
			Vector2(0.0f, size.y / 2.0f - padding.y / 2.0f),
			Vector2(size.x - padding.x * 2.0f, padding.y),
			90.0f
		);

	drawBorder(
			&quadVertices,
			Vector2(size.x / 2.0f - padding.x / 2.0f, 0.0f),
			Vector2(padding.x, size.y - padding.y * 2.0f),
			0.0f
		);

	drawBorder(
			&quadVertices,
			Vector2(0.0f, -size.y / 2.0f + padding.y / 2.0f),
			Vector2(size.x - padding.x * 2.0f, padding.y),
			270.0f
		);
	drawFiller(
			&quadVertices,
			Vector2(0.0f, 0.0f),
			size - padding * 2.0f,
			false
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

	size_t vertexBufferArraySize = quadVertices.size() * 8;
	GLfloat* vertexBufferArray = new GLfloat[vertexBufferArraySize];

	for(size_t i = 0; i < quadVertices.size(); ++i) {
		vertexBufferArray[i * 8 + 0] = quadVertices[i].position.x + position.x;
		vertexBufferArray[i * 8 + 1] = quadVertices[i].position.y + position.y;
		vertexBufferArray[i * 8 + 2] = quadVertices[i].primCoord.x;
		vertexBufferArray[i * 8 + 3] = quadVertices[i].primCoord.y;
		vertexBufferArray[i * 8 + 4] = quadVertices[i].curveOriginCoord.x;
		vertexBufferArray[i * 8 + 5] = quadVertices[i].curveOriginCoord.y;
		vertexBufferArray[i * 8 + 6] = 2.0f - border;
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
	Vector4 insideColor = *((Vector4*) argList["insideColor"]);
	Vector4 borderColor = *((Vector4*) argList["borderColor"]);
	Vector4 outsideColor = *((Vector4*) argList["outsideColor"]);
	glUniform4f(uniforms["insideColor"], insideColor.x, insideColor.y, insideColor.z, insideColor.w);
	glUniform4f(uniforms["borderColor"], borderColor.x, borderColor.y, borderColor.z, borderColor.w);
	glUniform4f(uniforms["outsideColor"], outsideColor.x, outsideColor.y, outsideColor.z, outsideColor.w);
	glUniform1f(uniforms["softEdge"], *((float*) argList["softEdge"]) * 2.0f / *((float*) argList["padding"]));

	// draw the data stored in GPU memory
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers["vertices"]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffers["elements"]);

	glVertexAttribPointer(attributes["position"], 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
			(GLvoid*) 0);
	glVertexAttribPointer(attributes["primCoord"], 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
			(GLvoid*) (2 * sizeof(GLfloat)));
	glVertexAttribPointer(attributes["curveOriginCoord"], 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
			(GLvoid*) (4 * sizeof(GLfloat)));
	glVertexAttribPointer(attributes["border1Dist"], 1, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
			(GLvoid*) (6 * sizeof(GLfloat)));
	glVertexAttribPointer(attributes["border2Dist"], 1, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
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
