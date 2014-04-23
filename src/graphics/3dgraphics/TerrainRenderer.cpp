// TerrainRenderer.cpp
//
// Dominicus
// Copyright 2010-2011, Joshua Bodine
//
// Released under the terms of the "Simplified BSD License." See the file
// licenses/DOMINICUS.txt for the license text.

#include "graphics/3dgraphics/TerrainRenderer.h"

TerrainRenderer::TerrainRenderer() {
	// set up shader
	GLuint shaderID = 0;
	std::vector<GLuint> shaderIDs;

	shaderID = gameGraphics->getShaderID(GL_VERTEX_SHADER, "terrain"); shaderIDs.push_back(shaderID);
	shaderID = gameGraphics->getShaderID(GL_FRAGMENT_SHADER, "terrain"); shaderIDs.push_back(shaderID);
	shaderProgram = gameGraphics->makeProgram(shaderIDs);

	// set up uniforms and attributes
	uniforms["mvpMatrix"] = glGetUniformLocation(shaderProgram, "mvpMatrix");
	uniforms["textures"] = glGetUniformLocation(shaderProgram, "textures");
	uniforms["lightPosition"] = glGetUniformLocation(shaderProgram, "lightPosition");
	uniforms["lightColor"] = glGetUniformLocation(shaderProgram, "lightColor");
	uniforms["depth"] = glGetUniformLocation(shaderProgram, "depth");
	uniforms["height"] = glGetUniformLocation(shaderProgram, "height");

	attributes["position"] = glGetAttribLocation(shaderProgram, "position");
	attributes["normal"] = glGetAttribLocation(shaderProgram, "normal");
	attributes["texCoord"] = glGetAttribLocation(shaderProgram, "texCoord");

	// set up vertex buffers
	glGenBuffers(1, &(vertexBuffers["vertices"]));
	glGenBuffers(1, &(vertexBuffers["elements"]));
}

TerrainRenderer::~TerrainRenderer() {
	;
}

void TerrainRenderer::reloadGraphics() {
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers["vertices"]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffers["elements"]);

	Mesh* terrainMesh = &(gameState->island);
	std::vector<Mesh::Face>* faceGroup = &(terrainMesh->faceGroups.begin()->second);
	size_t vertDataBufferArraySize =
			24 *
			faceGroup->size() *
			sizeof(GLfloat); // 3 vertices + 3 normals + 2 texcoords * 3 per face

	GLfloat* vertDataBufferArray = (GLfloat*) malloc(vertDataBufferArraySize);
	for(size_t i = 0; i < faceGroup->size(); ++i) {
		vertDataBufferArray[i * 24 + 0] = terrainMesh->vertices[(*faceGroup)[i].vertices[0]].x;
		vertDataBufferArray[i * 24 + 1] = terrainMesh->vertices[(*faceGroup)[i].vertices[0]].y;
		vertDataBufferArray[i * 24 + 2] = terrainMesh->vertices[(*faceGroup)[i].vertices[0]].z;

		vertDataBufferArray[i * 24 + 3] = terrainMesh->normals[(*faceGroup)[i].normals[0]].x;
		vertDataBufferArray[i * 24 + 4] = terrainMesh->normals[(*faceGroup)[i].normals[0]].y;
		vertDataBufferArray[i * 24 + 5] = terrainMesh->normals[(*faceGroup)[i].normals[0]].z;

		vertDataBufferArray[i * 24 + 6] = terrainMesh->texCoords[(*faceGroup)[i].texCoords[0]].x;
		vertDataBufferArray[i * 24 + 7] = terrainMesh->texCoords[(*faceGroup)[i].texCoords[0]].y;

		vertDataBufferArray[i * 24 + 8] = terrainMesh->vertices[(*faceGroup)[i].vertices[1]].x;
		vertDataBufferArray[i * 24 + 9] = terrainMesh->vertices[(*faceGroup)[i].vertices[1]].y;
		vertDataBufferArray[i * 24 + 10] = terrainMesh->vertices[(*faceGroup)[i].vertices[1]].z;

		vertDataBufferArray[i * 24 + 11] = terrainMesh->normals[(*faceGroup)[i].normals[1]].x;
		vertDataBufferArray[i * 24 + 12] = terrainMesh->normals[(*faceGroup)[i].normals[1]].y;
		vertDataBufferArray[i * 24 + 13] = terrainMesh->normals[(*faceGroup)[i].normals[1]].z;

		vertDataBufferArray[i * 24 + 14] = terrainMesh->texCoords[(*faceGroup)[i].texCoords[1]].x;
		vertDataBufferArray[i * 24 + 15] = terrainMesh->texCoords[(*faceGroup)[i].texCoords[1]].y;

		vertDataBufferArray[i * 24 + 16] = terrainMesh->vertices[(*faceGroup)[i].vertices[2]].x;
		vertDataBufferArray[i * 24 + 17] = terrainMesh->vertices[(*faceGroup)[i].vertices[2]].y;
		vertDataBufferArray[i * 24 + 18] = terrainMesh->vertices[(*faceGroup)[i].vertices[2]].z;

		vertDataBufferArray[i * 24 + 19] = terrainMesh->normals[(*faceGroup)[i].normals[2]].x;
		vertDataBufferArray[i * 24 + 20] = terrainMesh->normals[(*faceGroup)[i].normals[2]].y;
		vertDataBufferArray[i * 24 + 21] = terrainMesh->normals[(*faceGroup)[i].normals[2]].z;

		vertDataBufferArray[i * 24 + 22] = terrainMesh->texCoords[(*faceGroup)[i].texCoords[2]].x;
		vertDataBufferArray[i * 24 + 23] = terrainMesh->texCoords[(*faceGroup)[i].texCoords[2]].y;
	}

	size_t vertElementBufferArraySize = 3 * faceGroup->size() * sizeof(GLuint);
	GLuint* vertElementBufferArray = (GLuint*) malloc(vertElementBufferArraySize);
	for(size_t i = 0; i < faceGroup->size() * 3; ++i) {
		vertElementBufferArray[i] = i;
	}

	// send the buffer data
	glBufferData(GL_ARRAY_BUFFER, vertDataBufferArraySize, vertDataBufferArray, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertElementBufferArraySize, vertElementBufferArray,
			GL_STATIC_DRAW);

	free(vertDataBufferArray);
	free(vertElementBufferArray);

	// send the noise texture
	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &noiseTextureID);
	glBindTexture(GL_TEXTURE_2D, noiseTextureID);

	glTexImage2D(
			GL_TEXTURE_2D,
			0,
			(gameGraphics->fourDepthNoiseTexture->format == Texture::FORMAT_RGBA ? GL_RGBA : GL_RGB),
			gameGraphics->fourDepthNoiseTexture->width,
			gameGraphics->fourDepthNoiseTexture->height,
			0,
			(gameGraphics->fourDepthNoiseTexture->format == Texture::FORMAT_RGBA ? GL_RGBA : GL_RGB),
			GL_UNSIGNED_BYTE,
			gameGraphics->fourDepthNoiseTexture->getDataPointer()
	);

	glGenerateMipmap(GL_TEXTURE_2D);

	glDisable(GL_TEXTURE_2D);
}

void TerrainRenderer::execute(std::map<std::string, void*> arguments) {
	// prepare variables
	Matrix4 mvpMatrix = gameGraphics->testCamera->terrainMatrix;
	mvpMatrix *= gameGraphics->ppMatrix;
	float mvpMatrixArray[] = {
			mvpMatrix.m11, mvpMatrix.m12, mvpMatrix.m13, mvpMatrix.m14,
			mvpMatrix.m21, mvpMatrix.m22, mvpMatrix.m23, mvpMatrix.m24,
			mvpMatrix.m31, mvpMatrix.m32, mvpMatrix.m33, mvpMatrix.m34,
			mvpMatrix.m41, mvpMatrix.m42, mvpMatrix.m43, mvpMatrix.m44
		};

	// state
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);

	// enable shader
	glUseProgram(shaderProgram);

	// set uniforms
	glUniformMatrix4fv(uniforms["mvpMatrix"], 1, GL_FALSE, mvpMatrixArray);

	GLint textureUniforms[] = { 0, 1, 2, 3, 4 };
	glUniform1iv(uniforms["textures"], 5, textureUniforms);
	glUniform1f(uniforms["depth"], (GLfloat) 0.5f);
	glUniform1f(uniforms["height"], (GLfloat) gameSystem->getFloat("islandMaximumHeight"));

	// activate the textures
	for(int i = 0; i < 3; ++i) {
		if(i == 0) glActiveTexture(GL_TEXTURE0);
		else if(i == 1) glActiveTexture(GL_TEXTURE1);
		else if(i == 2) glActiveTexture(GL_TEXTURE2);

		std::string terrainFile;
		if(i == 0) terrainFile = "terrain/green";
		else if(i == 1) terrainFile = "terrain/gray";
		else if(i == 2) terrainFile = "terrain/brown";
		glBindTexture(GL_TEXTURE_2D, gameGraphics->getTextureID(terrainFile.c_str()));

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		float maxAniso = 4.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);

	}

	// activate the noise texture
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, noiseTextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// draw the data stored in GPU memory
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers["vertices"]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffers["elements"]);

	glVertexAttribPointer(attributes["position"], 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*) 0);
	glVertexAttribPointer(attributes["normal"], 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT),
			(GLvoid*) (3 * sizeof(GLfloat)));
	glVertexAttribPointer(attributes["texCoord"], 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT),
			(GLvoid*) (6 * sizeof(GLfloat)));

	glEnableVertexAttribArray(attributes["position"]);
	glEnableVertexAttribArray(attributes["normal"]);
	glEnableVertexAttribArray(attributes["texCoord"]);

	glDrawElements(GL_TRIANGLES, gameState->island.faceGroups.begin()->second.size() * 3, GL_UNSIGNED_INT, NULL);

	glDisableVertexAttribArray(attributes["position"]);
	glDisableVertexAttribArray(attributes["normal"]);
	glDisableVertexAttribArray(attributes["texCoord"]);

	// undo state
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_MULTISAMPLE);
}