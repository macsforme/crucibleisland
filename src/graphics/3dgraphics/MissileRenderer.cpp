// MissileRenderer.cpp
// Dominicus

#include "graphics/3dgraphics/MissileRenderer.h"

#include <cstdlib>
#include <map>
#include <string>
#include <vector>

#include "core/GameSystem.h"
#include "graphics/GameGraphics.h"
#include "math/MatrixMath.h"
#include "math/ScalarMath.h"
#include "math/VectorMath.h"
#include "platform/OpenGLHeaders.h"
#include "state/GameState.h"

extern GameGraphics* gameGraphics;
extern GameState* gameState;
extern GameSystem* gameSystem;

MissileRenderer::MissileRenderer() : missileMesh(Mesh("missile")) {
	missileMesh = Mesh("missile");

	// set up vertex buffers
	glGenBuffers(1, &(vertexBuffers["vertices"]));
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers["vertices"]);

	unsigned int totalFaces = 0;
	for(
			std::map<std::string, std::vector<Mesh::Face> >::iterator itr =
					missileMesh.faceGroups.begin();
			itr != missileMesh.faceGroups.end();
			++itr
		)
		totalFaces += itr->second.size();

	GLfloat* vertDataBufferArray = new GLfloat[totalFaces * 36];	// 3 vertices + 3 normals + 2 texcoords + 4 colors * 3 faces
	unsigned int bufferIndex = 0;

	for(
			std::map<std::string, std::vector<Mesh::Face> >::iterator itr =
					missileMesh.faceGroups.begin();
			itr != missileMesh.faceGroups.end();
			++itr
		) {
		const unsigned int firstIndex = bufferIndex;

		// insert the vertex attribute data
		for(size_t i = 0; i < itr->second.size(); ++i) {
			for(size_t p = 0; p < 3; ++p) {
				vertDataBufferArray[bufferIndex * 36 + p * 12 + 0] = missileMesh.vertices[itr->second[i].vertices[p]].x;
				vertDataBufferArray[bufferIndex * 36 + p * 12 + 1] = missileMesh.vertices[itr->second[i].vertices[p]].y;
				vertDataBufferArray[bufferIndex * 36 + p * 12 + 2] = missileMesh.vertices[itr->second[i].vertices[p]].z;

				vertDataBufferArray[bufferIndex * 36 + p * 12 + 3] = missileMesh.normals[itr->second[i].normals[p]].x;
				vertDataBufferArray[bufferIndex * 36 + p * 12 + 4] = missileMesh.normals[itr->second[i].normals[p]].y;
				vertDataBufferArray[bufferIndex * 36 + p * 12 + 5] = missileMesh.normals[itr->second[i].normals[p]].z;

				vertDataBufferArray[bufferIndex * 36 + p * 12 + 6] = missileMesh.texCoords[itr->second[i].texCoords[p]].x;
				vertDataBufferArray[bufferIndex * 36 + p * 12 + 7] = missileMesh.texCoords[itr->second[i].texCoords[p]].y;

				vertDataBufferArray[bufferIndex * 36 + p * 12 + 8] = 1.0f;
				vertDataBufferArray[bufferIndex * 36 + p * 12 + 9] = 1.0f;
				vertDataBufferArray[bufferIndex * 36 + p * 12 + 10] = 1.0f;
				vertDataBufferArray[bufferIndex * 36 + p * 12 + 11] = 1.0f;
			}

			++bufferIndex;
		}

		glGenBuffers(1, &(vertexBuffers[std::string("elements_" + itr->first).c_str()]));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffers[std::string("elements_" + itr->first).c_str()]);

		GLuint* vertElementBufferArray = new GLuint[itr->second.size() * 3];

		for(size_t i = 0; i < itr->second.size() * 3; ++i) {
			vertElementBufferArray[i] = firstIndex * 3 + i;
		}

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, itr->second.size() * 3 * sizeof(GLuint), vertElementBufferArray,
				GL_STATIC_DRAW);

		delete[] vertElementBufferArray;
	}

	glBufferData(GL_ARRAY_BUFFER, totalFaces * 36 * sizeof(GLfloat), vertDataBufferArray, GL_STATIC_DRAW);

	delete[] vertDataBufferArray;
}

MissileRenderer::~MissileRenderer() {
	// undo vertex buffer setup
	glDeleteBuffers(1, &(vertexBuffers["vertices"]));
	glDeleteBuffers(1, &(vertexBuffers["elements"]));
}

void MissileRenderer::execute(DrawStackArgList arguments) {
	// state
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	if(gameGraphics->supportsMultisampling) glEnable(GL_MULTISAMPLE);
	glDisable(GL_SCISSOR_TEST);
	glEnable(GL_TEXTURE_2D);

	// enable shader
	glUseProgram(gameGraphics->getProgramID("colorTextureLighting"));

	// set uniforms
	glUniform1i(glGetUniformLocation(gameGraphics->getProgramID("colorTextureLighting"), "texture"), 0);
	glUniform3f(glGetUniformLocation(gameGraphics->getProgramID("colorTextureLighting"), "ambientColor"), 0.15f, 0.15f, 0.15f);
	glUniform3f(glGetUniformLocation(gameGraphics->getProgramID("colorTextureLighting"), "diffuseColor"), 0.5f, 0.5f, 0.5f);
	glUniform3f(glGetUniformLocation(gameGraphics->getProgramID("colorTextureLighting"), "specularColor"), 0.8f, 0.8f, 0.8f);
	Vector4 lightPosition = Vector4(1.0f, 1.0f, -1.0f, 0.0f) * gameGraphics->currentCamera->lightMatrix;
	glUniform3f(glGetUniformLocation(gameGraphics->getProgramID("colorTextureLighting"), "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
	glUniform1f(glGetUniformLocation(gameGraphics->getProgramID("colorTextureLighting"), "shininess"), 10.0f);

	// set the overall drawing state
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers["vertices"]);

	glVertexAttribPointer(glGetAttribLocation(gameGraphics->getProgramID("colorTextureLighting"), "position"), 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*) 0);
	glVertexAttribPointer(glGetAttribLocation(gameGraphics->getProgramID("colorTextureLighting"), "normal"), 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*) (3 * sizeof(GLfloat)));
	glVertexAttribPointer(glGetAttribLocation(gameGraphics->getProgramID("colorTextureLighting"), "texCoord"), 2, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*) (6 * sizeof(GLfloat)));
	glVertexAttribPointer(glGetAttribLocation(gameGraphics->getProgramID("colorTextureLighting"), "color"), 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*) (8 * sizeof(GLfloat)));

	glEnableVertexAttribArray(glGetAttribLocation(gameGraphics->getProgramID("colorTextureLighting"), "position"));
	glEnableVertexAttribArray(glGetAttribLocation(gameGraphics->getProgramID("colorTextureLighting"), "normal"));
	glEnableVertexAttribArray(glGetAttribLocation(gameGraphics->getProgramID("colorTextureLighting"), "texCoord"));
	glEnableVertexAttribArray(glGetAttribLocation(gameGraphics->getProgramID("colorTextureLighting"), "color"));

	for(size_t i = 0; i < gameState->missiles.size(); ++i) {
		if(! gameState->missiles[i].alive)
			continue;

		// calculate the matrix for this missile position
		Matrix4 mvMatrix; mvMatrix.identity();
		rotateMatrix(Vector3(0.0f, 0.0f, 1.0f), radians(gameState->missiles[i].tilt), mvMatrix);
		rotateMatrix(Vector3(0.0f, 1.0f, 0.0f), radians(gameState->missiles[i].rotation), mvMatrix);
		translateMatrix(gameState->missiles[i].position.x, gameState->missiles[i].position.y, gameState->missiles[i].position.z, mvMatrix);

		mvMatrix = mvMatrix * gameGraphics->currentCamera->mvMatrix;
		float mvMatrixArray[] = {
				mvMatrix.m11, mvMatrix.m12, mvMatrix.m13, mvMatrix.m14,
				mvMatrix.m21, mvMatrix.m22, mvMatrix.m23, mvMatrix.m24,
				mvMatrix.m31, mvMatrix.m32, mvMatrix.m33, mvMatrix.m34,
				mvMatrix.m41, mvMatrix.m42, mvMatrix.m43, mvMatrix.m44
			};

		// draw the missile
		for(
				std::map<std::string, std::vector<Mesh::Face> >::iterator itr =
					missileMesh.faceGroups.begin();
				itr != missileMesh.faceGroups.end();
				++itr
			) {
			// set the texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gameGraphics->getTextureID(std::string("structure/" + itr->first).c_str()));

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glUniformMatrix4fv(glGetUniformLocation(gameGraphics->getProgramID("colorTextureLighting"), "mvMatrix"), 1, GL_FALSE, mvMatrixArray);
			glUniformMatrix4fv(glGetUniformLocation(gameGraphics->getProgramID("colorTextureLighting"), "pMatrix"), 1, GL_FALSE, (gameState->binoculars ? gameGraphics->ppBinoMatrixArray : gameGraphics->ppMatrixArray));

			// draw the geometry
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffers[std::string("elements_" + itr->first).c_str()]);

			glDrawElements(GL_TRIANGLES, itr->second.size() * 3, GL_UNSIGNED_INT, NULL);
		}
	}

	glDisableVertexAttribArray(glGetAttribLocation(gameGraphics->getProgramID("colorTextureLighting"), "position"));
	glDisableVertexAttribArray(glGetAttribLocation(gameGraphics->getProgramID("colorTextureLighting"), "normal"));
	glDisableVertexAttribArray(glGetAttribLocation(gameGraphics->getProgramID("colorTextureLighting"), "texCoord"));
	glDisableVertexAttribArray(glGetAttribLocation(gameGraphics->getProgramID("colorTextureLighting"), "color"));
}
