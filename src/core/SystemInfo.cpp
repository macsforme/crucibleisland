/*
 *  SystemInfo.cpp
 *  dominicus
 *
 *  Created by Joshua Bodine on 8/15/10.
 *  Copyright 2010 Joshua Bodine. All rights reserved.
 *
 */

#include "core/SystemInfo.h"

SystemInfo::SystemInfo() {
	// report the build version
	std::stringstream buildInfo;
	buildInfo <<
			"Dominicus " <<
			buildVersion.version << " " <<
			buildVersion.classification << " " <<
			buildVersion.architecture << " " <<
			buildVersion.buildDate;
	programLog->report(ProgramLog::LOG_INFO, buildInfo.str().c_str());

	// the first call to SDL_getVideoInfo gives us the width and height of the desktop
	SDL_VideoInfo* vidInfo = (SDL_VideoInfo*) SDL_GetVideoInfo();
	if(vidInfo == NULL)
		programLog->report(ProgramLog::LOG_FATAL, "Could not obtain screen resolution from SDL.");

	screenWidth = (unsigned short int) vidInfo->current_w;
	screenHeight = (unsigned short int) vidInfo->current_h;
}

void SystemInfo::check() {
	// This function checks the system for compatibility to ensure all
	// needed elements are present, and reports on general system features.

	// ===== OpenGL =====

	// OpenGL version
	std::string openGLVersionStr = (char*) glGetString(GL_VERSION);
	float openGLVersion = (float)atof(openGLVersionStr.substr(0, openGLVersionStr.find(' ')).c_str());

	if(openGLVersion < 2.0f) {
		std::stringstream err;
		err << "OpenGL version 2.0 or greater required, and only version " <<
				openGLVersionStr.substr(0, openGLVersionStr.find(' ')) <<
				" detected.";
		programLog->report(ProgramLog::LOG_FATAL, err.str().c_str());
	} else {
		std::stringstream err;
		err << "OpenGL Version: " <<
				openGLVersion;
		programLog->report(
				ProgramLog::LOG_INFO,
				err.str().c_str()
			);
	}

	// GLSL version
	float GLSLVersion = (float)atof((char*) glGetString(GL_SHADING_LANGUAGE_VERSION));

	if(GLSLVersion < 1.1f) {
		std::stringstream err;
		err << "GLSL version 1.10 or greater required, and only version " <<
				(char*) glGetString(GL_SHADING_LANGUAGE_VERSION) <<
				" detected.";
		programLog->report(ProgramLog::LOG_FATAL, err.str().c_str());
	} else {
		programLog->report(
				ProgramLog::LOG_INFO,
				std::string("GLSL Version: " + std::string((char*) glGetString(GL_SHADING_LANGUAGE_VERSION))).c_str()
			);
	}

	// OpenGL Renderer (no check, report only)
	programLog->report(
			ProgramLog::LOG_INFO,
			std::string("OpenGL Renderer: " + std::string((char*) glGetString(GL_RENDERER))).c_str()
		);

	// texture image units
	int maxTexUnits = 0;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTexUnits);

	if(maxTexUnits < 8) {
		std::stringstream err;
		err << "OpenGL implementation supports only " <<
				maxTexUnits <<
				" texture units, whereas 8 are required.";
		programLog->report(ProgramLog::LOG_FATAL, err.str().c_str());
	} else {
		std::stringstream err;
		err << "OpenGL Texture Units Supported: " <<
				maxTexUnits;
		programLog->report(
				ProgramLog::LOG_INFO,
				err.str().c_str()
			);
	}

	// anisotropic filtering extension
	if(strstr((const char*)glGetString(GL_EXTENSIONS),
			"GL_EXT_texture_filter_anisotropic") == NULL)
		programLog->report(ProgramLog::LOG_FATAL,
				"OpenGL extension not supported: GL_EXT_texture_filter_anisotropic");
	else
		programLog->report(ProgramLog::LOG_INFO, "OpenGL Extension Found: GL_EXT_texture_filter_anisotropic");
}
