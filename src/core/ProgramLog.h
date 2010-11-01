/*
 *  ProgramLog.h
 *  dominicus
 *
 *  Created by Joshua Bodine on 5/28/10.
 *  Copyright 2010 Joshua Bodine. All rights reserved.
 *
 */

#ifndef PROGRAMLOG_H
#define PROGRAMLOG_H

// library headers
#include <cstdlib>
#include <iostream>

enum LogDetail {
		LOG_INTERNALERROR,	// safety mechanisms in case of internal bugs (no external libs or files)
		LOG_FATAL,	// errors created by outside means that we can't survive
		LOG_DEBUG,	// problems that shouldn't normally happen
		LOG_INFO	// verbose information that users generally don't need to see
	};

class ProgramLog {
public:
	static void report(LogDetail detail, const char* report);
};

#endif





