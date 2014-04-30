// ShipRenderer.cpp
//
// Dominicus
// Copyright 2010-2011, Joshua Bodine
//
// Released under the terms of the "Simplified BSD License." See the file
// licenses/DOMINICUS.txt for the license text.

#include "graphics/3dgraphics/TowerRenderer.h"

TowerRenderer::TowerRenderer() {
	towerMesh = Mesh("tower");

	// determine camera origin
	cameraOrigin = (
			towerMesh.vertices[towerMesh.faceGroups["cameraorigin"][0].vertices[0]] +
			towerMesh.vertices[towerMesh.faceGroups["cameraorigin"][0].vertices[1]] +
			towerMesh.vertices[towerMesh.faceGroups["cameraorigin"][0].vertices[2]]
		) / 3.0f;

	// center turret and determine origin
	turretOrigin = (
			towerMesh.vertices[towerMesh.faceGroups["turretorigin"][0].vertices[0]] +
			towerMesh.vertices[towerMesh.faceGroups["turretorigin"][0].vertices[1]] +
			towerMesh.vertices[towerMesh.faceGroups["turretorigin"][0].vertices[2]]
		) / 3.0f;

	bool* vertexMoved = new bool[towerMesh.vertices.size()];
	for(size_t i = 0; i < towerMesh.vertices.size(); ++i)
		vertexMoved[i] = false;

	for(
			std::vector<Mesh::Face>::iterator itr = towerMesh.faceGroups["turret"].begin();
			itr != towerMesh.faceGroups["turret"].end();
			++itr
		) {
		for(size_t i = 0; i < 3; ++i) {
			if(! vertexMoved[itr->vertices[i]]) {
				towerMesh.vertices[itr->vertices[i]] -= turretOrigin;

				vertexMoved[itr->vertices[i]] = true;
			}
		}
	}

	// set up shader
	GLuint shaderID = 0;
	std::vector<GLuint> shaderIDs;

	shaderID = gameGraphics->getShaderID(GL_VERTEX_SHADER, "colorTextureLighting"); shaderIDs.push_back(shaderID);
	shaderID = gameGraphics->getShaderID(GL_FRAGMENT_SHADER, "colorTextureLighting"); shaderIDs.push_back(shaderID);
	shaderProgram = gameGraphics->makeProgram(shaderIDs);

	// set up uniforms and attributes
	uniforms["mvMatrix"] = glGetUniformLocation(shaderProgram, "mvMatrix");
	uniforms["pMatrix"] = glGetUniformLocation(shaderProgram, "pMatrix");
	uniforms["texture"] = glGetUniformLocation(shaderProgram, "texture");
	uniforms["ambientColor"] = glGetUniformLocation(shaderProgram, "ambientColor");
	uniforms["diffuseColor"] = glGetUniformLocation(shaderProgram, "diffuseColor");
	uniforms["specularColor"] = glGetUniformLocation(shaderProgram, "specularColor");
	uniforms["lightPosition"] = glGetUniformLocation(shaderProgram, "lightPosition");
	uniforms["shininess"] = glGetUniformLocation(shaderProgram, "shininess");

	attributes["position"] = glGetAttribLocation(shaderProgram, "position");
	attributes["normal"] = glGetAttribLocation(shaderProgram, "normal");
	attributes["texCoord"] = glGetAttribLocation(shaderProgram, "texCoord");
	attributes["color"] = glGetAttribLocation(shaderProgram, "color");

	// set up vertex buffers
	glGenBuffers(1, &(vertexBuffers["vertices"]));
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers["vertices"]);

	unsigned int totalFaces = 0;
	for(
			std::map<std::string, std::vector<Mesh::Face> >::iterator itr =
					towerMesh.faceGroups.begin();
			itr != towerMesh.faceGroups.end();
			++itr
		)
		totalFaces += itr->second.size();

	GLfloat* vertDataBufferArray = new GLfloat[totalFaces * 36];	// 3 vertices + 3 normals + 2 texcoords + 4 colors * 3 faces
	unsigned int bufferIndex = 0;

	for(
			std::map<std::string, std::vector<Mesh::Face> >::iterator itr =
					towerMesh.faceGroups.begin();
			itr != towerMesh.faceGroups.end();
			++itr
		) {
		const unsigned int firstIndex = bufferIndex;

		// insert the vertex attribute data
		for(size_t i = 0; i < itr->second.size(); ++i) {
			for(size_t p = 0; p < 3; ++p) {
				vertDataBufferArray[bufferIndex * 36 + p * 12 + 0] = towerMesh.vertices[itr->second[i].vertices[p]].x;
				vertDataBufferArray[bufferIndex * 36 + p * 12 + 1] = towerMesh.vertices[itr->second[i].vertices[p]].y;
				vertDataBufferArray[bufferIndex * 36 + p * 12 + 2] = towerMesh.vertices[itr->second[i].vertices[p]].z;

				vertDataBufferArray[bufferIndex * 36 + p * 12 + 3] = towerMesh.normals[itr->second[i].normals[p]].x;
				vertDataBufferArray[bufferIndex * 36 + p * 12 + 4] = towerMesh.normals[itr->second[i].normals[p]].y;
				vertDataBufferArray[bufferIndex * 36 + p * 12 + 5] = towerMesh.normals[itr->second[i].normals[p]].z;

				vertDataBufferArray[bufferIndex * 36 + p * 12 + 6] = towerMesh.texCoords[itr->second[i].texCoords[p]].x;
				vertDataBufferArray[bufferIndex * 36 + p * 12 + 7] = towerMesh.texCoords[itr->second[i].texCoords[p]].y;

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

	// send the buffer data
	glBufferData(GL_ARRAY_BUFFER, totalFaces * 36 * sizeof(GLfloat), vertDataBufferArray, GL_STATIC_DRAW);

	delete[] vertDataBufferArray;
}

TowerRenderer::~TowerRenderer() {
// FIXME destroy variables
}

void TowerRenderer::execute(std::map<std::string, void*> arguments) {
	Matrix4 mvMatrix; mvMatrix.identity();
	translateMatrix(gameState->fortress.position.x, gameState->fortress.position.y, gameState->fortress.position.z, mvMatrix);
	mvMatrix = mvMatrix * gameGraphics->currentCamera->mvMatrix;
	float mvMatrixArray[] = {
			mvMatrix.m11, mvMatrix.m12, mvMatrix.m13, mvMatrix.m14,
			mvMatrix.m21, mvMatrix.m22, mvMatrix.m23, mvMatrix.m24,
			mvMatrix.m31, mvMatrix.m32, mvMatrix.m33, mvMatrix.m34,
			mvMatrix.m41, mvMatrix.m42, mvMatrix.m43, mvMatrix.m44
		};

	Matrix4 spinnerMvMatrix; spinnerMvMatrix.identity();
	rotateMatrix(Vector3(0.0f, 1.0f, 0.0f), radians((float) (gameState->getGameMills() % 8000) / 8000.0f * 360.0f), spinnerMvMatrix);
	spinnerMvMatrix *= mvMatrix;
	float spinnerMvMatrixArray[] = {
			spinnerMvMatrix.m11, spinnerMvMatrix.m12, spinnerMvMatrix.m13, spinnerMvMatrix.m14,
			spinnerMvMatrix.m21, spinnerMvMatrix.m22, spinnerMvMatrix.m23, spinnerMvMatrix.m24,
			spinnerMvMatrix.m31, spinnerMvMatrix.m32, spinnerMvMatrix.m33, spinnerMvMatrix.m34,
			spinnerMvMatrix.m41, spinnerMvMatrix.m42, spinnerMvMatrix.m43, spinnerMvMatrix.m44
		};

	Matrix4 turretMvMatrix; turretMvMatrix.identity();
	rotateMatrix(Vector3(0.0f, 0.0f, 1.0f), radians((sin(-PI + (float) (gameState->getGameMills() % 4000) / 4000.0f * 2.0f * PI) * 0.5f + 0.5f ) * 45.0f), turretMvMatrix);
	translateMatrix(turretOrigin.x, turretOrigin.y, turretOrigin.z, turretMvMatrix);
	rotateMatrix(Vector3(0.0f, 1.0f, 0.0f), radians((float) (gameState->getGameMills() % 8000) / 8000.0f * 360.0f), turretMvMatrix);
	turretMvMatrix *= mvMatrix;
	float turretMvMatrixArray[] = {
			turretMvMatrix.m11, turretMvMatrix.m12, turretMvMatrix.m13, turretMvMatrix.m14,
			turretMvMatrix.m21, turretMvMatrix.m22, turretMvMatrix.m23, turretMvMatrix.m24,
			turretMvMatrix.m31, turretMvMatrix.m32, turretMvMatrix.m33, turretMvMatrix.m34,
			turretMvMatrix.m41, turretMvMatrix.m42, turretMvMatrix.m43, turretMvMatrix.m44
		};

	Vector4 lightPosition(1.0f, 1.0f, -1.0f, 0.0f);
	lightPosition = lightPosition * gameGraphics->currentCamera->lightMatrix;

	// state
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glEnable(GL_MULTISAMPLE);

	// enable shader
	glUseProgram(shaderProgram);

	// set uniforms
	glUniform1i(uniforms["texture"], 0);
	glUniform3f(uniforms["ambientColor"], 0.15f, 0.15f, 0.15f);
	glUniform3f(uniforms["diffuseColor"], 0.5f, 0.5f, 0.5f);
	glUniform3f(uniforms["specularColor"], 0.8f, 0.8f, 0.8f);
	glUniform3f(uniforms["lightPosition"], lightPosition.x, lightPosition.y, lightPosition.z);
	glUniform1f(uniforms["shininess"], 10.0f);

	// set the overall drawing state
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers["vertices"]);

	glVertexAttribPointer(attributes["position"], 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GL_FLOAT), (GLvoid*) 0);
	glVertexAttribPointer(attributes["normal"], 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GL_FLOAT),
			(GLvoid*) (3 * sizeof(GLfloat)));
	glVertexAttribPointer(attributes["texCoord"], 2, GL_FLOAT, GL_FALSE, 12 * sizeof(GL_FLOAT),
			(GLvoid*) (6 * sizeof(GLfloat)));
	glVertexAttribPointer(attributes["color"], 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GL_FLOAT),
			(GLvoid*) (8 * sizeof(GLfloat)));

	glEnableVertexAttribArray(attributes["position"]);
	glEnableVertexAttribArray(attributes["normal"]);
	glEnableVertexAttribArray(attributes["texCoord"]);
	glEnableVertexAttribArray(attributes["color"]);

	for(
			std::map<std::string, std::vector<Mesh::Face> >::iterator itr =
					towerMesh.faceGroups.begin();
			itr != towerMesh.faceGroups.end();
			++itr
		) {
		// don't draw the missile origin
		if(itr->first == "turretorigin" || itr->first == "cameraorigin" || itr->first == "shellorigin")
			continue;

		// set the texture
		glActiveTexture(GL_TEXTURE0);
		if(itr->first == "spinner")
			glBindTexture(GL_TEXTURE_2D, gameGraphics->getTextureID(std::string("structure/lightgrain")));
		else if(itr->first == "turret")
			glBindTexture(GL_TEXTURE_2D, gameGraphics->getTextureID(std::string("structure/mediumgrain")));
		else
			glBindTexture(GL_TEXTURE_2D, gameGraphics->getTextureID(std::string("structure/" + itr->first).c_str()));

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		if(itr->first == "turret")
			glUniformMatrix4fv(uniforms["mvMatrix"], 1, GL_FALSE, turretMvMatrixArray);
		else if(itr->first == "spinner")
			glUniformMatrix4fv(uniforms["mvMatrix"], 1, GL_FALSE, spinnerMvMatrixArray);
		else
			glUniformMatrix4fv(uniforms["mvMatrix"], 1, GL_FALSE, mvMatrixArray);

		glUniformMatrix4fv(uniforms["pMatrix"], 1, GL_FALSE, gameGraphics->ppMatrixArray);

		// draw the geometry
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffers[std::string("elements_" + itr->first).c_str()]);

		glDrawElements(GL_TRIANGLES, itr->second.size() * 3, GL_UNSIGNED_INT, NULL);
	}

	glDisableVertexAttribArray(attributes["position"]);
	glDisableVertexAttribArray(attributes["normal"]);
	glDisableVertexAttribArray(attributes["texCoord"]);
	glDisableVertexAttribArray(attributes["color"]);

	// undo state
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_MULTISAMPLE);
}
