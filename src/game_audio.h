#pragma once

#include <stdbool.h>

void initGameAudio(void);
void endGameAudio(void);
void stopGameAudio(void);
void updateGameAudio(bool frightened, bool eyesReturning);
void playGhostEatenSound(void);
