#include <stdlib.h>
#include <stdio.h>
#include "myLib.h"
#include "game.h"
#include "spritesheet.h"
#include "bg1.h"
#include "bg2.h"
#include "bg3.h"
#include "bg4.h"
#include "bg5.h"
#include "arrowRight1.h"
#include "arrowRight2.h"
#include "arrowRight3.h"
#include "arrowRight4.h"
#include "arrowRight5.h"
#include "turnRight1.h"
#include "turnRight2.h"
#include "turnRight3.h"
#include "turnRight4.h"
#include "turnRight5.h"
#include "arrowLeft1.h"
#include "arrowLeft2.h"
#include "arrowLeft3.h"
#include "arrowLeft4.h"
#include "arrowLeft5.h"
#include "turnLeft1.h"
#include "turnLeft2.h"
#include "turnLeft3.h"
#include "turnLeft4.h"
#include "turnLeft5.h"

// Prototypes
void initGame();
void updateGame();
void drawGame();
void updateCave();
void initPenguin();
void updatePenguin();
void leftWarning();
void rightWarning();

// State prototypes
void goToLose();
void initPenguin();
void rightPenguin();
void leftPenguin();
void straightCave();
void leftCave();
void rightCave();

// Game Structures
PLAYER penguin;
LIFE lives[LIFECOUNT];

// ShadowOAM Structure
OBJ_ATTR shadowOAM[128];

// Screen Variables
int frameCounter;
int straightCounter;
int leftCounter;
int rightCounter;
int penguinFrameCounter;
int crashCounter;
int warningCounter;
int livesRemaining;

// States
enum {LEFTCAVE, RIGHTCAVE, STRAIGHTCAVE, LEFTPENGUIN, RIGHTPENGUIN, IDLEPENGUIN, JUMP, CRASH, LEFTWARNING, RIGHTWARNING};
int penguinState;
int backgroundState;
int curPath;
int nextPath;
int choosePath;

// We initialize the cave, sprites, and the lives method here
void initGame() {
	livesRemaining = 3;
    DMANow(3, spritesheetPal, SPRITEPALETTE, spritesheetPalLen / 2);
   	DMANow(3, spritesheetTiles, &CHARBLOCK[4], spritesheetTilesLen / 2);
   	
   	hideSprites();
	initPenguin();

	curPath = STRAIGHTCAVE;
	nextPath = STRAIGHTCAVE;
	backgroundState = STRAIGHTCAVE;

	frameCounter = 1;
	straightCounter = 1;
	warningCounter = 1;

	penguinFrameCounter = 1;
	crashCounter = 1;
	buttons = BUTTONS;
}

void updateGame() {
	frameCounter++;
	// We update everything here
	updatePenguin();
	checkCollision();

	if (backgroundState == STRAIGHTCAVE) {
		straightCave();
	}
	else if (backgroundState == LEFTCAVE) {
		leftCave();
	}
	else if (backgroundState == RIGHTCAVE) {
		rightCave();
	}
	else if (backgroundState == RIGHTWARNING) {
		rightWarning();
	}
	else if (backgroundState == LEFTWARNING) {
		leftWarning();
	}

	if (frameCounter % 50 == 0) {
		if (backgroundState != STRAIGHTCAVE) {
			if (backgroundState == RIGHTCAVE) {
				rightCounter = 1;
				straightCave();
			}
			else if (backgroundState == LEFTCAVE) {
				leftCounter = 1;
				straightCave();
			}
			else if (backgroundState == LEFTWARNING) {
				warningCounter = 1;
				leftCave();
			}
			else if (backgroundState == RIGHTWARNING) {
				warningCounter = 1;
				rightCave();
			}
			//nextPath = STRAIGHTCAVE;
			
		} else {
			straightCounter = 1;
			choosePath = rand() % 2;	
			updateCave();
		}
	}
}

// We draw all the components of the game here and transfer contents to the OAM.
void drawGame() { 
	DMANow(3, shadowOAM, OAM, 128 * 4);
}

// Initialize penguin
void initPenguin() {
	penguin.width = 64;
    penguin.height = 64;
    penguin.cdel = 1;
	penguin.row = SCREENHEIGHT - penguin.width - 2;
	penguin.col = SCREENWIDTH/2 - penguin.height/2;
	penguinState = IDLEPENGUIN;
}


void checkCollision() {
	if (backgroundState == LEFTCAVE && penguinState != LEFTPENGUIN && penguinState != CRASH) {
		livesRemaining--;
		penguinState = CRASH;
	}
	if (backgroundState == RIGHTCAVE && penguinState != RIGHTPENGUIN && penguinState != CRASH) {
		livesRemaining--;
		penguinState = CRASH;
	}
}

// Contains player's every movement
void updatePenguin() {
	if (penguinState == CRASH) {
		penguinState = 1;
		drawCrash();
	} 
	if (penguinState != CRASH) {
		penguinState = IDLEPENGUIN;
	}
	if (BUTTON_HELD(BUTTON_LEFT)) {
		penguinState = LEFTPENGUIN;
	}
	if (BUTTON_HELD(BUTTON_RIGHT)) {
		penguinState = RIGHTPENGUIN;
	}
	switch(penguinState) {
        case LEFTPENGUIN:
          leftPenguin();
          break;
        case RIGHTPENGUIN:
          rightPenguin();
          break;
        case IDLEPENGUIN:
          idlePenguin();
          break;
        // case CRASH:
        //   drawCrash();
        //   //crash();
        //   break;
        default:
           break;
    }
}

void updateCave() {
	//if (choosePath == 0) {
	//	straightCave();
	//}
	if (choosePath == 0) {
		leftWarning();
	}
	if (choosePath == 1) {
		rightWarning();
	}
}



// We draw the player here
void idlePenguin() {
	shadowOAM[0].attr0 = (penguin.row) | ATTR0_4BPP | ATTR0_SQUARE;
    shadowOAM[0].attr1 = (penguin.col) | ATTR1_LARGE;
    shadowOAM[0].attr2 = ATTR2_TILEID(0, 0) | ATTR2_PALROW(0);	
    // penguinState = IDLEPENGUIN;
}

void leftPenguin() {
	penguinFrameCounter++;
	if (penguinFrameCounter <= 80) {
		shadowOAM[0].attr0 = (penguin.row) | ATTR0_4BPP | ATTR0_SQUARE;
    	shadowOAM[0].attr1 = (penguin.col) | ATTR1_LARGE;
    	shadowOAM[0].attr2 = ATTR2_TILEID(0, 8) | ATTR2_PALROW(0);	
	}
    if (penguinFrameCounter > 80 && penguinFrameCounter <= 160) {
    	shadowOAM[0].attr0 = (penguin.row) | ATTR0_4BPP | ATTR0_SQUARE;
    	shadowOAM[0].attr1 = (penguin.col) | ATTR1_LARGE;
    	shadowOAM[0].attr2 = ATTR2_TILEID(0, 16) | ATTR2_PALROW(0);	
    }
    if (penguinFrameCounter > 160 && penguinFrameCounter <= 180) {
    	shadowOAM[0].attr0 = (penguin.row) | ATTR0_4BPP | ATTR0_SQUARE;
    	shadowOAM[0].attr1 = (penguin.col) | ATTR1_LARGE;
    	shadowOAM[0].attr2 = ATTR2_TILEID(8, 0) | ATTR2_PALROW(0);	
    }
    if (penguinFrameCounter > 180 && penguinFrameCounter <= 200) {
    	shadowOAM[0].attr0 = (penguin.row) | ATTR0_4BPP | ATTR0_SQUARE;
    	shadowOAM[0].attr1 = (penguin.col) | ATTR1_LARGE;
    	shadowOAM[0].attr2 = ATTR2_TILEID(8, 16) | ATTR2_PALROW(0);	
    }
    if (penguinFrameCounter > 200 && penguinFrameCounter <= 220) {
    	shadowOAM[0].attr0 = (penguin.row) | ATTR0_4BPP | ATTR0_SQUARE;
    	shadowOAM[0].attr1 = (penguin.col) | ATTR1_LARGE;
    	shadowOAM[0].attr2 = ATTR2_TILEID(8, 24) | ATTR2_PALROW(0);	
    }
    if (penguinFrameCounter > 220 && penguinFrameCounter <= 240) {
    	shadowOAM[0].attr0 = (penguin.row) | ATTR0_4BPP | ATTR0_SQUARE;
    	shadowOAM[0].attr1 = (penguin.col) | ATTR1_LARGE;
    	shadowOAM[0].attr2 = ATTR2_TILEID(8, 32) | ATTR2_PALROW(0);	
    }
    if (penguinFrameCounter > 240) {
     	penguinState = CRASH;
    } 
    // penguinState = LEFTPENGUIN;
}

void rightPenguin() {
	penguinFrameCounter++;
	if (penguinFrameCounter <= 80) {
		shadowOAM[0].attr0 = (penguin.row) | ATTR0_4BPP | ATTR0_SQUARE;
	    shadowOAM[0].attr1 = (penguin.col) | ATTR1_LARGE | ATTR1_HFLIP;
	    shadowOAM[0].attr2 = ATTR2_TILEID(0, 8) | ATTR2_PALROW(0);	
	}
    if (penguinFrameCounter > 80 && penguinFrameCounter <= 160) {
    	shadowOAM[0].attr0 = (penguin.row) | ATTR0_4BPP | ATTR0_SQUARE;
    	shadowOAM[0].attr1 = (penguin.col) | ATTR1_LARGE | ATTR1_HFLIP;
    	shadowOAM[0].attr2 = ATTR2_TILEID(0, 16) | ATTR2_PALROW(0);	
    }
    if (penguinFrameCounter > 160 && penguinFrameCounter <= 180) {
    	shadowOAM[0].attr0 = (penguin.row) | ATTR0_4BPP | ATTR0_SQUARE;
    	shadowOAM[0].attr1 = (penguin.col) | ATTR1_LARGE | ATTR1_HFLIP;
    	shadowOAM[0].attr2 = ATTR2_TILEID(8, 0) | ATTR2_PALROW(0);	
    }
    if (penguinFrameCounter > 180 && penguinFrameCounter <= 200) {
    	shadowOAM[0].attr0 = (penguin.row) | ATTR0_4BPP | ATTR0_SQUARE;
    	shadowOAM[0].attr1 = (penguin.col) | ATTR1_LARGE | ATTR1_HFLIP;
    	shadowOAM[0].attr2 = ATTR2_TILEID(8, 16) | ATTR2_PALROW(0);	
    }
    if (penguinFrameCounter > 200 && penguinFrameCounter <= 220) {
    	shadowOAM[0].attr0 = (penguin.row) | ATTR0_4BPP | ATTR0_SQUARE;
    	shadowOAM[0].attr1 = (penguin.col) | ATTR1_LARGE | ATTR1_HFLIP;
    	shadowOAM[0].attr2 = ATTR2_TILEID(8, 24) | ATTR2_PALROW(0);	
    }
    if (penguinFrameCounter > 220 && penguinFrameCounter <= 240) {
    	shadowOAM[0].attr0 = (penguin.row) | ATTR0_4BPP | ATTR0_SQUARE;
    	shadowOAM[0].attr1 = (penguin.col) | ATTR1_LARGE | ATTR1_HFLIP;
    	shadowOAM[0].attr2 = ATTR2_TILEID(8, 32) | ATTR2_PALROW(0);	
    }
    if (penguinFrameCounter > 240) {
     	penguinState = CRASH;
    } 
    // penguinState = RIGHTPENGUIN;
}

void crash() {
	penguinFrameCounter = 1;
	if (backgroundState == STRAIGHTCAVE) {
		penguinState = IDLEPENGUIN;
	}
}

void drawCrash() {
	crashCounter++;
	if (crashCounter <= 20) {
		shadowOAM[0].attr0 = (penguin.row) | ATTR0_4BPP | ATTR0_SQUARE;
    	shadowOAM[0].attr1 = (penguin.col) | ATTR1_LARGE;
    	shadowOAM[0].attr2 = ATTR2_TILEID(16, 0) | ATTR2_PALROW(0);	
	}
    if (crashCounter > 20 && crashCounter <= 30) {
    	shadowOAM[0].attr0 = (penguin.row) | ATTR0_4BPP | ATTR0_SQUARE;
    	shadowOAM[0].attr1 = (penguin.col) | ATTR1_LARGE;
    	shadowOAM[0].attr2 = ATTR2_TILEID(16, 8) | ATTR2_PALROW(0);	
    }
    if (crashCounter > 30 && crashCounter <= 40) {
    	shadowOAM[0].attr0 = (penguin.row) | ATTR0_4BPP | ATTR0_SQUARE;
    	shadowOAM[0].attr1 = (penguin.col) | ATTR1_LARGE;
    	shadowOAM[0].attr2 = ATTR2_TILEID(16, 16) | ATTR2_PALROW(0);	
    }
    if (crashCounter > 40) {
    	shadowOAM[0].attr0 = ATTR0_HIDE;
    }
}

void straightCave() {
	straightCounter++;
	if (straightCounter <= 10) {
		DMANow(3, bg1Pal, PALETTE, bg1PalLen / 2);
    	DMANow(3, bg1Tiles, &CHARBLOCK[0], bg1TilesLen / 2);
    	DMANow(3, bg1Map, &SCREENBLOCK[28], bg1MapLen / 2);
    	backgroundState = STRAIGHTCAVE;
	}
	if (straightCounter > 10 && straightCounter <= 20) {
		DMANow(3, bg2Pal, PALETTE, bg2PalLen / 2);
    	DMANow(3, bg2Tiles, &CHARBLOCK[0], bg2TilesLen / 2);
    	DMANow(3, bg2Map, &SCREENBLOCK[28], bg2MapLen / 2);
    	backgroundState = STRAIGHTCAVE;
	}
	if (straightCounter > 20 && straightCounter <= 30) {
		DMANow(3, bg3Pal, PALETTE, bg3PalLen / 2);
    	DMANow(3, bg3Tiles, &CHARBLOCK[0], bg3TilesLen / 2);
    	DMANow(3, bg3Map, &SCREENBLOCK[28], bg3MapLen / 2);
    	backgroundState = STRAIGHTCAVE;
	}
	if (straightCounter > 30 && straightCounter <= 40) {
		DMANow(3, bg4Pal, PALETTE, bg4PalLen / 2);
    	DMANow(3, bg4Tiles, &CHARBLOCK[0], bg4TilesLen / 2);
    	DMANow(3, bg4Map, &SCREENBLOCK[28], bg4MapLen / 2);
    	backgroundState = STRAIGHTCAVE;
	}
	if (straightCounter > 40 && straightCounter <= 50) {
		DMANow(3, bg4Pal, PALETTE, bg4PalLen / 2);
    	DMANow(3, bg4Tiles, &CHARBLOCK[0], bg4TilesLen / 2);
    	DMANow(3, bg4Map, &SCREENBLOCK[28], bg4MapLen / 2);
    	backgroundState = STRAIGHTCAVE;
	}
}

void leftWarning() {
	warningCounter++;
	if (warningCounter < 10) {
		DMANow(3, arrowLeft1Pal, PALETTE, arrowLeft1PalLen / 2);
    	DMANow(3, arrowLeft1Tiles, &CHARBLOCK[0], arrowLeft1TilesLen / 2);
    	DMANow(3, arrowLeft1Map, &SCREENBLOCK[28], arrowLeft1MapLen / 2);
    	backgroundState = LEFTWARNING;
	}
	if (warningCounter >= 10 && warningCounter < 20) {
		DMANow(3, arrowLeft2Pal, PALETTE, arrowLeft2PalLen / 2);
		DMANow(3, arrowLeft2Tiles, &CHARBLOCK[0], arrowLeft2TilesLen / 2);
		DMANow(3, arrowLeft2Map, &SCREENBLOCK[28], arrowLeft2MapLen / 2);
		backgroundState = LEFTWARNING;
	}
	if (warningCounter >= 20 && warningCounter < 30) {
		DMANow(3, arrowLeft3Pal, PALETTE, arrowLeft3PalLen / 2);
		DMANow(3, arrowLeft3Tiles, &CHARBLOCK[0], arrowLeft3TilesLen / 2);
		DMANow(3, arrowLeft3Map, &SCREENBLOCK[28], arrowLeft3MapLen / 2);
		backgroundState = LEFTWARNING;
	}
	if (warningCounter >= 30 && warningCounter < 40) {
		DMANow(3, arrowLeft4Pal, PALETTE, arrowLeft4PalLen / 2);
		DMANow(3, arrowLeft4Tiles, &CHARBLOCK[0], arrowLeft4TilesLen / 2);
		DMANow(3, arrowLeft4Map, &SCREENBLOCK[28], arrowLeft4MapLen / 2);
		backgroundState = LEFTWARNING;
	}
	if (warningCounter >= 40 && warningCounter < 50) {
		DMANow(3, arrowLeft5Pal, PALETTE, arrowLeft5PalLen / 2);
		DMANow(3, arrowLeft5Tiles, &CHARBLOCK[0], arrowLeft5TilesLen / 2);
		DMANow(3, arrowLeft5Map, &SCREENBLOCK[28], arrowLeft5MapLen / 2);
		backgroundState = LEFTWARNING;
	}
}

void rightWarning() {
	warningCounter++;
	if (warningCounter < 10) {
		DMANow(3, arrowRight1Pal, PALETTE, arrowRight1PalLen / 2);
    	DMANow(3, arrowRight1Tiles, &CHARBLOCK[0], arrowRight1TilesLen / 2);
    	DMANow(3, arrowRight1Map, &SCREENBLOCK[28], arrowRight1MapLen / 2);
    	backgroundState = RIGHTWARNING;
	}
	if (warningCounter >= 10 && warningCounter < 20) {
		DMANow(3, arrowRight2Pal, PALETTE, arrowRight2PalLen / 2);
		DMANow(3, arrowRight2Tiles, &CHARBLOCK[0], arrowRight2TilesLen / 2);
		DMANow(3, arrowRight2Map, &SCREENBLOCK[28], arrowRight2MapLen / 2);
		backgroundState = RIGHTWARNING;
	}
	if (warningCounter >= 20 && warningCounter < 30) {
		DMANow(3, arrowRight3Pal, PALETTE, arrowRight3PalLen / 2);
		DMANow(3, arrowRight3Tiles, &CHARBLOCK[0], arrowRight3TilesLen / 2);
		DMANow(3, arrowRight3Map, &SCREENBLOCK[28], arrowRight3MapLen / 2);
		backgroundState = RIGHTWARNING;
	}
	if (warningCounter >= 30 && warningCounter < 40) {
		DMANow(3, arrowRight4Pal, PALETTE, arrowRight4PalLen / 2);
		DMANow(3, arrowRight4Tiles, &CHARBLOCK[0], arrowRight4TilesLen / 2);
		DMANow(3, arrowRight4Map, &SCREENBLOCK[28], arrowRight4MapLen / 2);
		backgroundState = RIGHTWARNING;
	}
	if (warningCounter >= 40 && warningCounter < 50) {
		DMANow(3, arrowRight5Pal, PALETTE, arrowRight5PalLen / 2);
		DMANow(3, arrowRight5Tiles, &CHARBLOCK[0], arrowRight5TilesLen / 2);
		DMANow(3, arrowRight5Map, &SCREENBLOCK[28], arrowRight5MapLen / 2);
		backgroundState = RIGHTWARNING;
	}
}


void leftCave() {
	leftCounter++;
	if (leftCounter <= 10) {
		DMANow(3, turnLeft1Pal, PALETTE, turnLeft1PalLen / 2);
    	DMANow(3, turnLeft1Tiles, &CHARBLOCK[0], turnLeft1TilesLen / 2);
    	DMANow(3, turnLeft1Map, &SCREENBLOCK[28], turnLeft1MapLen / 2);
    	backgroundState = LEFTCAVE;
	}
	if (leftCounter > 10 && leftCounter <= 20) {
		DMANow(3, turnLeft2Pal, PALETTE, turnLeft2PalLen / 2);
    	DMANow(3, turnLeft2Tiles, &CHARBLOCK[0], turnLeft2TilesLen / 2);
    	DMANow(3, turnLeft2Map, &SCREENBLOCK[28], turnLeft2MapLen / 2);
    	backgroundState = LEFTCAVE;
	}
	if (leftCounter > 20 && leftCounter <= 30) {
		DMANow(3, turnLeft3Pal, PALETTE, turnLeft3PalLen / 2);
    	DMANow(3, turnLeft3Tiles, &CHARBLOCK[0], turnLeft3TilesLen / 2);
    	DMANow(3, turnLeft3Map, &SCREENBLOCK[28], turnLeft3MapLen / 2);
    	backgroundState = LEFTCAVE;
	}
	if (leftCounter > 30 && leftCounter <= 40) {
		DMANow(3, turnLeft4Pal, PALETTE, turnLeft4PalLen / 2);
    	DMANow(3, turnLeft4Tiles, &CHARBLOCK[0], turnLeft4TilesLen / 2);
    	DMANow(3, turnLeft4Map, &SCREENBLOCK[28], turnLeft4MapLen / 2);
    	backgroundState = LEFTCAVE;
	}
	if (leftCounter > 40 && leftCounter <= 50) {
		DMANow(3, turnLeft5Pal, PALETTE, turnLeft5PalLen / 2);
    	DMANow(3, turnLeft5Tiles, &CHARBLOCK[0], turnLeft5TilesLen / 2);
    	DMANow(3, turnLeft5Map, &SCREENBLOCK[28], turnLeft5MapLen / 2);
    	backgroundState = LEFTCAVE;
	}
}

void rightCave() {
	rightCounter++;
	if (rightCounter <= 10) {
		DMANow(3, turnRight1Pal, PALETTE, turnRight1PalLen / 2);
    	DMANow(3, turnRight1Tiles, &CHARBLOCK[0], turnRight1TilesLen / 2);
    	DMANow(3, turnRight1Map, &SCREENBLOCK[28], turnRight1MapLen / 2);
		backgroundState = RIGHTCAVE;
    	curPath = RIGHTCAVE;
	}
	if (rightCounter > 10 && rightCounter <= 20) {
		DMANow(3, turnRight2Pal, PALETTE, turnRight2PalLen / 2);
    	DMANow(3, turnRight2Tiles, &CHARBLOCK[0], turnRight2TilesLen / 2);
    	DMANow(3, turnRight2Map, &SCREENBLOCK[28], turnRight2MapLen / 2);
  		backgroundState = RIGHTCAVE;
    	curPath = RIGHTCAVE;
	}
	if (rightCounter > 20 && rightCounter <= 30) {
		DMANow(3, turnRight3Pal, PALETTE, turnRight3PalLen / 2);
    	DMANow(3, turnRight3Tiles, &CHARBLOCK[0], turnRight3TilesLen / 2);
    	DMANow(3, turnRight3Map, &SCREENBLOCK[28], turnRight3MapLen / 2);
		backgroundState = RIGHTCAVE;
    	curPath = RIGHTCAVE;
	}
	if (rightCounter > 30 && rightCounter <= 40) {
		DMANow(3, turnRight4Pal, PALETTE, turnRight4PalLen / 2);
    	DMANow(3, turnRight4Tiles, &CHARBLOCK[0], turnRight4TilesLen / 2);
    	DMANow(3, turnRight4Map, &SCREENBLOCK[28], turnRight4MapLen / 2);
		backgroundState = RIGHTCAVE;
    	curPath = RIGHTCAVE;
	}
	if (rightCounter > 40 && rightCounter <= 50) {
		DMANow(3, turnRight5Pal, PALETTE, turnRight5PalLen / 2);
    	DMANow(3, turnRight5Tiles, &CHARBLOCK[0], turnRight5TilesLen / 2);
    	DMANow(3, turnRight5Map, &SCREENBLOCK[28], turnRight5MapLen / 2);
		backgroundState = RIGHTCAVE;
    	curPath = RIGHTCAVE;
	}
}

