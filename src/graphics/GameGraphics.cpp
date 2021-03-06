// GameGraphics.cpp
// Dominicus

#include "graphics/GameGraphics.h"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <SDL/SDL.h>
#include <sstream>
#include <stdint.h>
#include <string.h>

#include "core/GameSystem.h"
#include "core/MainLoopMember.h"
#include "geometry/DiamondSquare.h"
#include "graphics/DrawingMaster.h"
#include "math/ScalarMath.h"
#include "math/VectorMath.h"
#include "platform/Platform.h"

extern DrawingMaster* drawingMaster;
extern std::map<MainLoopMember*,unsigned int> mainLoopModules;
extern Platform* platform;
extern GameSystem* gameSystem;

GLuint GameGraphics::getShaderID(GLenum shaderType, std::string shaderName) {
	// return the compiled shader ID
	std::map<std::string, GLuint>::iterator itr;
	std::string extension;
	if(shaderType == GL_VERTEX_SHADER) {
		itr = vertexShaderIDs.find(shaderName);

		if(itr != vertexShaderIDs.end())
			return itr->second;

		extension = ".vertex.glsl";
	} else if(shaderType == GL_FRAGMENT_SHADER) {
		itr = fragmentShaderIDs.find(shaderName);

		if(itr != fragmentShaderIDs.end())
			return itr->second;

		extension = ".fragment.glsl";
	}

	std::stringstream filename;
	filename <<
			platform->dataPath <<
			"/shaders/" <<
			shaderName <<
			extension;

	std::ifstream fileStream;
	std::vector<std::string> fileLines;

	fileStream.open(filename.str().c_str());
	while(fileStream.good()) {
		std::string thisChar = "";
		thisChar += fileStream.get();

		if(fileLines.size() == 0)
			fileLines.push_back("");

		fileLines.back() += thisChar;

		if(thisChar == "\n") {
			fileLines.push_back("");
		}
	}
	fileStream.close();

	if(fileLines.size() > 0)
		fileLines.pop_back();
	else
		gameSystem->log(GameSystem::LOG_FATAL,
				std::string("The GLSL shader " +
						filename.str() +
						" could not be opened for reading.").c_str()
			);

	GLsizei count = fileLines.size();
	uint8_t bytes = 0;
	GLchar** shaderSource;

	for(size_t i = 0; i < fileLines.size(); ++i)
		bytes += fileLines[i].size() + 1;

	shaderSource = new GLchar*[fileLines.size()];

	for(size_t i = 0; i < fileLines.size(); ++i) {
		shaderSource[i] = new GLchar[fileLines[i].size() + 1];
		memcpy(
				shaderSource[i],
				fileLines[i].c_str(),
				fileLines[i].size() + 1
			);
		shaderSource[i][fileLines[i].size()] = '\0';
	}

	GLuint shader;
	shader = glCreateShader(shaderType);
	glShaderSource(shader, count, (const GLchar**) shaderSource, NULL);

	for(int i = 0; i < count; ++i)
		delete[] shaderSource[i];
	delete[] shaderSource;

	glCompileShader(shader);

	GLint result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if(result == GL_FALSE) {
		std::stringstream err;
		err << "The GLSL shader "
				<< filename
				<< " did not compile successfully."
				<< std::endl << std::endl;

		GLint sourceLength;
		glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &sourceLength);
		GLchar* sourceLines = new GLchar[sourceLength];
		glGetShaderSource(shader, sourceLength, NULL, sourceLines);

		err << "SHADER SOURCE ON GPU" << std::endl
				<< "--------------------" << std::endl
				<< sourceLines
				<< "--------------------" << std::endl << std::endl;
		delete[] sourceLines;

		GLint logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		GLchar* logLines = new GLchar[logLength];
		glGetShaderInfoLog(shader, logLength, NULL, logLines);

		err << "ERROR LOG" << std::endl
				<< "---------" << std::endl
				<< logLines
				<< "---------";

		delete[] logLines;

		gameSystem->log(GameSystem::LOG_FATAL, err.str().c_str());
	}

	return shader;
}

GLuint GameGraphics::makeProgram(std::vector<GLuint> shaders) {
	GLuint program;

	program = glCreateProgram();

	for(size_t i = 0; i < shaders.size(); ++i)
		glAttachShader(program, shaders[i]);

	glLinkProgram(program);

	GLint result;
	glGetProgramiv(program, GL_LINK_STATUS, &result);

	if(result == GL_FALSE) {
		std::stringstream err;
		err << "The GLSL shader program did not link successfully." << std::endl << std::endl;

		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
		GLchar* logLines = new GLchar[logLength];
		glGetProgramInfoLog(program, logLength, NULL, logLines);

		err << "ERROR LOG" << std::endl
				<< "---------" << std::endl
				<< logLines
				<< "---------";

		delete[] logLines;

		gameSystem->log(GameSystem::LOG_FATAL, err.str().c_str());
	}

	return program;
}

GameGraphics::GameGraphics(bool fullScreen, bool testSystem) :
		fullScreen(fullScreen),
		supportsMultisampling(false),
		currentCamera(NULL) {
	// initialize an SDL window
	resolutionX = (fullScreen ? gameSystem->displayResolutionX :
			atoi(gameSystem->getString("displayWindowedResolution").substr(0, gameSystem->getString("displayWindowedResolution").find('x')).c_str()));
	resolutionY = (fullScreen ? gameSystem->displayResolutionY :
			atoi(gameSystem->getString("displayWindowedResolution").substr(gameSystem->getString("displayWindowedResolution").find('x') + 1, std::string::npos).c_str()));
	aspectRatio = (float) resolutionX / (float) resolutionY;

	uint32_t flags = SDL_OPENGL | (fullScreen ? SDL_FULLSCREEN : 0);

	if(! SDL_VideoModeOK(
			resolutionX,
			resolutionY,
			(int) gameSystem->getFloat("displayColorDepth"),
			flags
		))
		gameSystem->log(GameSystem::LOG_FATAL,
				"SDL cannot initialize a window with the specified settings.");

	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, (int) gameSystem->getFloat("displayFramerateLimiting") == GameSystem::LIMIT_VSYNC ? 1 : 0);
	if(gameSystem->getFloat("displayMultisamplingLevel") == 0.0f) {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
	} else {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, gameSystem->getFloat("displayMultisamplingLevel") == 2.0f ? 2 : 4);
	}
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, (int) gameSystem->getFloat("displayDepthSize"));
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_Surface* surface = SDL_SetVideoMode(
			resolutionX,
			resolutionY,
			(int) gameSystem->getFloat("displayColorDepth"),
			flags
		);

	if(surface == NULL)
		gameSystem->log(GameSystem::LOG_FATAL,
				"SDL could not initialize an OpenGL-ready window.");

	if(! fullScreen)
		SDL_WM_SetCaption(PROGRAM_IDENTIFIER, NULL);

	// apply window element scaling
	std::stringstream resolutionText;
	resolutionText << resolutionX << "x" << resolutionY;
	gameSystem->applyScreenResolution(resolutionText.str().c_str());

	// always check for multisampling support since we set a flag for it
	if(strstr((const char*) glGetString(GL_EXTENSIONS), "GL_ARB_multisample") != NULL)
		supportsMultisampling = true;

	// if specified, do system test
	if(testSystem) {
		// test and log the OpenGL version for compatibility
		std::string openGLVersionStr = (char*) glGetString(GL_VERSION);
		float openGLVersion = (float) atof(openGLVersionStr.substr(0,
				openGLVersionStr.find(' ')).c_str());

		if(openGLVersion < 2.0f) {
			std::stringstream err;
			err << "OpenGL version 2.0 or greater required, and only version " <<
					openGLVersionStr.substr(0, openGLVersionStr.find(' ')) <<
					" detected.";
			gameSystem->log(GameSystem::LOG_FATAL, err.str().c_str());
		} else {
			std::stringstream err;
			err << "OpenGL Version: " <<
					openGLVersion;
			gameSystem->log(
					GameSystem::LOG_VERBOSE,
					err.str().c_str()
				);
		}

		// test and log the GLSL version for compatibility
		float GLSLVersion = (float) atof((char*) glGetString(GL_SHADING_LANGUAGE_VERSION));

		if(GLSLVersion < 1.1f) {
			std::stringstream err;
			err << "GLSL version 1.10 or greater required, and only version " <<
					(char*) glGetString(GL_SHADING_LANGUAGE_VERSION) <<
					" detected.";
			gameSystem->log(GameSystem::LOG_FATAL, err.str().c_str());
		} else {
			gameSystem->log(
					GameSystem::LOG_VERBOSE,
					std::string("GLSL Version: " +
							std::string((char*) glGetString(GL_SHADING_LANGUAGE_VERSION))).c_str()
				);
		}

		// log the OpenGL Renderer
		gameSystem->log(
				GameSystem::LOG_VERBOSE,
				std::string("OpenGL Renderer: " + std::string((char*) glGetString(GL_RENDERER))).c_str()
			);

		// test and log the number of texture image units
		int maxTexUnits = 0;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTexUnits);

		if(maxTexUnits < 8) {
			std::stringstream err;
			err << "OpenGL implementation supports only " <<
					maxTexUnits <<
					" texture units, whereas 8 are required.";
			gameSystem->log(GameSystem::LOG_FATAL, err.str().c_str());
		} else {
			std::stringstream err;
			err << "OpenGL Texture Units Supported: " <<
					maxTexUnits;
			gameSystem->log(
					GameSystem::LOG_VERBOSE,
					err.str().c_str()
				);
		}

		// test and log the presence of the anisotropic filtering extension
/*
		if(strstr((const char*) glGetString(GL_EXTENSIONS),	"GL_EXT_texture_filter_anisotropic") == NULL)
			gameSystem->log(GameSystem::LOG_VERBOSE, "OpenGL extension not supported: GL_EXT_texture_filter_anisotropic");
		else
			gameSystem->log(GameSystem::LOG_VERBOSE, "OpenGL Extension Found: GL_EXT_texture_filter_anisotropic");
*/

		// test and log the presence of the framebuffer object extension (needed for glBuildMipmaps)
		if(strstr((const char*) glGetString(GL_EXTENSIONS), "GL_EXT_framebuffer_object") == NULL)
			gameSystem->log(GameSystem::LOG_FATAL, "OpenGL extension not supported: GL_EXT_framebuffer_object");
		else
			gameSystem->log(GameSystem::LOG_VERBOSE, "OpenGL Extension Found: GL_EXT_framebuffer_object");

		// test and log the presence of the multisample extension, and set option flag
		if(strstr((const char*) glGetString(GL_EXTENSIONS), "GL_ARB_multisample") == NULL)
			gameSystem->log(GameSystem::LOG_VERBOSE, "OpenGL extension not supported: GL_ARB_multisample");
		else
			gameSystem->log(GameSystem::LOG_VERBOSE, "OpenGL Extension Found: GL_ARB_multisample");
	}

	// set up matrices
	idMatrix = Matrix4(
			1.0, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	float idMatrixArrayVals[] = {
			idMatrix.m11, idMatrix.m12, idMatrix.m13, idMatrix.m14,
			idMatrix.m21, idMatrix.m22, idMatrix.m23, idMatrix.m24,
			idMatrix.m31, idMatrix.m32, idMatrix.m33, idMatrix.m34,
			idMatrix.m41, idMatrix.m42, idMatrix.m43, idMatrix.m44
		};
	memcpy((void*) idMatrixArray, (void*) idMatrixArrayVals, 16 * sizeof(float));

	opMatrix = Matrix4(
			(float) resolutionY / (float) resolutionX, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	float opMatrixArrayVals[] = {
			opMatrix.m11, opMatrix.m12, opMatrix.m13, opMatrix.m14,
			opMatrix.m21, opMatrix.m22, opMatrix.m23, opMatrix.m24,
			opMatrix.m31, opMatrix.m32, opMatrix.m33, opMatrix.m34,
			opMatrix.m41, opMatrix.m42, opMatrix.m43, opMatrix.m44
		};
	memcpy((void*) opMatrixArray, (void*) opMatrixArrayVals, 16 * sizeof(float));

	const float fov = gameSystem->getFloat("renderingPerspectiveFOV");
	const float binoFOV = gameSystem->getFloat("renderingPerspectiveBinocularsFOV");
	const float nClip = gameSystem->getFloat("renderingPerspectiveNearClip");
	const float fClip = gameSystem->getFloat("renderingPerspectiveFarClip");

	ppMatrix = Matrix4(
			1.0f / tan(radians(fov)), 0.0f, 0.0f, 0.0f,
			0.0f, aspectRatio / tan(radians(fov)), 0.0f, 0.0f,
			0.0f, 0.0f, (fClip + nClip) / (fClip - nClip), 1.0f,
			0.0f, 0.0f, -2.0f * fClip * nClip / (fClip - nClip), 0.0f
		);

	float ppMatrixArrayVals[] = {
			ppMatrix.m11, ppMatrix.m12, ppMatrix.m13, ppMatrix.m14,
			ppMatrix.m21, ppMatrix.m22, ppMatrix.m23, ppMatrix.m24,
			ppMatrix.m31, ppMatrix.m32, ppMatrix.m33, ppMatrix.m34,
			ppMatrix.m41, ppMatrix.m42, ppMatrix.m43, ppMatrix.m44
		};
	memcpy((void*) ppMatrixArray, (void*) ppMatrixArrayVals, 16 * sizeof(float));

	ppBinoMatrix = Matrix4(
			1.0f / tan(radians(binoFOV)), 0.0f, 0.0f, 0.0f,
			0.0f, aspectRatio / tan(radians(binoFOV)), 0.0f, 0.0f,
			0.0f, 0.0f, (fClip + nClip) / (fClip - nClip), 1.0f,
			0.0f, 0.0f, -2.0f * fClip * nClip / (fClip - nClip), 0.0f
		);

	float ppBinoMatrixArrayVals[] = {
			ppBinoMatrix.m11, ppBinoMatrix.m12, ppBinoMatrix.m13, ppBinoMatrix.m14,
			ppBinoMatrix.m21, ppBinoMatrix.m22, ppBinoMatrix.m23, ppBinoMatrix.m24,
			ppBinoMatrix.m31, ppBinoMatrix.m32, ppBinoMatrix.m33, ppBinoMatrix.m34,
			ppBinoMatrix.m41, ppBinoMatrix.m42, ppBinoMatrix.m43, ppBinoMatrix.m44
		};
	memcpy((void*) ppBinoMatrixArray, (void*) ppBinoMatrixArrayVals, 16 * sizeof(float));
/*
	ppMatrixInverse = Matrix4(
			tan(radians(fov)) / 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, tan(radians(fov)) / aspectRatio, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, -(fClip - nClip) / (2.0f * fClip * nClip),
			0.0f, 0.0f, 1.0f, (fClip + nClip) / (2.0f * fClip * nClip)
		);

	float ppMatrixInverseArrayVals[] = {
			ppMatrixInverse.m11, ppMatrixInverse.m12, ppMatrixInverse.m13, ppMatrixInverse.m14,
			ppMatrixInverse.m21, ppMatrixInverse.m22, ppMatrixInverse.m23, ppMatrixInverse.m24,
			ppMatrixInverse.m31, ppMatrixInverse.m32, ppMatrixInverse.m33, ppMatrixInverse.m34,
			ppMatrixInverse.m41, ppMatrixInverse.m42, ppMatrixInverse.m43, ppMatrixInverse.m44
		};
	memcpy((void*) ppMatrixInverseArray, (void*) ppMatrixInverseArrayVals, 16 * sizeof(float));
*/

	// set up fonts
	fontManager = new FontManager();
	fontManager->populateCommonChars((unsigned int) gameSystem->getFloat("fontSizeSmall"));
	fontManager->populateCommonChars((unsigned int) gameSystem->getFloat("fontSizeMedium"));
	fontManager->populateCommonChars((unsigned int) gameSystem->getFloat("fontSizeLarge"));
	fontManager->populateCommonChars((unsigned int) gameSystem->getFloat("fontSizeSuper"));

	// create persistent noise textures
	unsigned int noiseDensity = (unsigned int) gameSystem->getFloat("terrainNoiseTextureDensity");
	DiamondSquare noise1(
			noiseDensity,
			gameSystem->getFloat("terrainNoiseTextureRoughness")
		);
	noiseTexture = new Texture(
			noiseDensity,
			noiseDensity,
			Texture::FORMAT_RGB
		);
		for(size_t i = 0; i < (size_t) noiseDensity; ++i)
			for(size_t p = 0; p < (size_t) noiseDensity; ++p)
				noiseTexture->setColorAt(
						i,
						p,
						(uint8_t) (noise1.data[i][p] * 128.0f + 127.0f),
						(uint8_t) (noise1.data[i][p] * 128.0f + 127.0f),
						(uint8_t) (noise1.data[i][p] * 128.0f + 127.0f),
						0xFF
					);

		DiamondSquare noise2(
				noiseDensity,
				gameSystem->getFloat("terrainNoiseTextureRoughness")
			);
		fourDepthNoiseTexture = new Texture(
				noiseDensity,
				noiseDensity,
				Texture::FORMAT_RGB
			);
		for(size_t i = 0; i < (size_t) noiseDensity; ++i)
			for(size_t p = 0; p < (size_t) noiseDensity; ++p)
				fourDepthNoiseTexture->setColorAt(
						i,
						p,
						(uint8_t) (noise2.data[i][p] * 128.0f + 127.0f),
						(uint8_t) (noise2.data[i][p] * 128.0f + 127.0f),
						(uint8_t) (noise2.data[i][p] * 128.0f + 127.0f),
						0xFF
					);
	fourDepthNoiseTexture->setDepth(16);
}

GameGraphics::~GameGraphics() {
	// destroy fonts
	delete fontManager;

	// destroy textures
	delete noiseTexture;
	delete fourDepthNoiseTexture;

	// delete shaders and programs
	std::map<std::string, GLuint>::iterator programItr = programIDs.begin();
	while(programItr != programIDs.end()) {
		glDetachShader(programItr->second, vertexShaderIDs[programItr->first]);
		glDeleteShader(vertexShaderIDs[programItr->first]);
		vertexShaderIDs.erase(vertexShaderIDs.find(programItr->first));

		glDetachShader(programItr->second, fragmentShaderIDs[programItr->first]);
		glDeleteShader(fragmentShaderIDs[programItr->first]);
		fragmentShaderIDs.erase(fragmentShaderIDs.find(programItr->first));

		glDeleteProgram(programItr->second);
		programIDs.erase(programItr);

		programItr = programIDs.begin();
	}

	// delete textures
	std::map<std::string, Texture*>::iterator textureItr;

	for(textureItr = textures.begin(); textureItr != textures.end(); ++textureItr)
		delete textureItr->second;

	std::map<std::string, GLuint>::iterator textureIDItr;

	for(textureIDItr = textureIDs.begin(); textureIDItr != textureIDs.end(); ++textureIDItr)
		if(glIsTexture(textureIDItr->second))
			glDeleteTextures(1, &(textureIDItr->second));
}

GLuint GameGraphics::getProgramID(std::string name) {
	// return the program ID
	std::map<std::string, GLuint>::iterator itr = programIDs.find(name);

	if(itr != programIDs.end())
		return itr->second;

	std::vector<GLuint> shaders;
	shaders.push_back(getShaderID(GL_VERTEX_SHADER, name));
	shaders.push_back(getShaderID(GL_FRAGMENT_SHADER, name));
	GLuint programID = makeProgram(shaders);

	programIDs[name] = programID;

	return programID;
}

Texture* GameGraphics::getTexture(std::string filename) {
	if(textures.find(filename) != textures.end())
		return textures.find(filename)->second;

	std::stringstream filenameStream;
	filenameStream <<
			platform->dataPath <<
			"/data/textures/" <<
			filename <<
			".png";
	textures[filename] = new Texture(filenameStream.str().c_str());

	return textures[filename];
}

GLuint GameGraphics::getTextureID(std::string filename) {
	// return the stored texture ID if it exists
	if(textureIDs.find(filename) != textureIDs.end())
		return textureIDs.find(filename)->second;

	std::stringstream filenameStream;
	filenameStream <<
			platform->dataPath <<
			"/data/textures/" <<
			filename <<
			".png";
	Texture texture(filenameStream.str().c_str());

	// load the texture into OpenGL
	glEnable(GL_TEXTURE_2D);

	GLuint textureID = 0;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(
			GL_TEXTURE_2D,
			0,
			(texture.format == Texture::FORMAT_RGBA ? GL_RGBA : GL_RGB),
			texture.width,
			texture.height,
			0,
			(texture.format == Texture::FORMAT_RGBA ? GL_RGBA : GL_RGB),
			GL_UNSIGNED_BYTE,
			texture.getDataPointer()
	);

	glGenerateMipmap(GL_TEXTURE_2D);

	textureIDs[filename] = textureID;

	return textureID;
}

void GameGraphics::startFrame() {
	// update camera
	if(currentCamera != NULL)
		currentCamera->execute();

	// prepare OpenGL for rendering
	glViewport(0, 0, resolutionX, resolutionY);

	// clear the screen
	Vector4 clearColor = gameSystem->getColor("colorClear");
	glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GameGraphics::finishFrame() {
	// swap buffers
	if(mainLoopModules.find(drawingMaster) == mainLoopModules.end())
		glFinish();
	SDL_GL_SwapBuffers();

	// check for OpenGL errors
	GLenum renderingError = glGetError();
	while(renderingError != GL_NO_ERROR) {
		// log the error and check for another one
		std::stringstream err;
		err << "OpenGL returned an error: "
				<< renderingError;
		gameSystem->log(GameSystem::LOG_VERBOSE, err.str().c_str());

		renderingError = glGetError();
	}
}
