/*
 *  RenderingMaster.cpp
 *  dominicus
 *
 *  Created by Joshua Bodine on 10/28/10.
 *  Copyright 2010 Joshua Bodine. All rights reserved.
 *
 */

#include "RenderingMaster.h"

RenderingMaster::RenderingMaster(GameWindow* gameWindow) {
	bCamera = new BackCamera(gameWindow);
	fpCamera = new FirstPersonCamera(gameWindow);
	wvCamera = new WorldViewCamera(gameWindow);

	cameraToggleKeyTrap = new KeyTrap("toggleCamera");

	activeMatrix = &(fpCamera->vpMatrix);
}

void RenderingMaster::loop() {
	bCamera->loop();
	fpCamera->loop();
	wvCamera->loop();

	cameraToggleKeyTrap->loop();

	// toggle cameras if necessary
	if(cameraToggleKeyTrap->newPress()) {
		if(activeMatrix == &(bCamera->vpMatrix))
			activeMatrix = &(fpCamera->vpMatrix);
		else if(activeMatrix == &(fpCamera->vpMatrix))
			activeMatrix = &(wvCamera->vpMatrix);
		else if(activeMatrix == &(wvCamera->vpMatrix))
			activeMatrix = &(bCamera->vpMatrix);
	}

	if(activeMatrix == &(bCamera->vpMatrix))
		shipRenderer.render(*activeMatrix);
	else
		terrainRenderer.render(*activeMatrix);
}
