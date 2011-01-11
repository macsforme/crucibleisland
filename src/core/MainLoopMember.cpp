/*
 *  MainLoopMember.cpp
 *  dominicus
 *
 *  Created by Joshua Bodine on 1/11/11.
 *  Copyright 2011 Joshua Bodine. All rights reserved.
 *
 */

#include "MainLoopMember.h"

MainLoopMember::MainLoopMember() {
	// variable initialization
	now = platform.getExecutionTimeMicros();
	last = now;
	sleepMicros = 1.0f;
}

unsigned long int MainLoopMember::getSleepTime(unsigned long int idealSleepTime) {
	now = platform.getExecutionTimeMicros();
	unsigned long int timeDiff = (now - last);

	// adjust the target sleep micros by the factor we are off by
	if(timeDiff == 0) {
		sleepMicros = idealSleepTime;
	} else {
		if(timeDiff < idealSleepTime / 2) {
			// avoid over-compensating if we hit way earlier than expected
			last = now;
			
			return sleepMicros;
		} else {
			// if we hit late, throw it into gear
			sleepMicros *= ((float) idealSleepTime / (float) (now - last));
		}
	}

	last = now;

	return (unsigned long int) sleepMicros;
}