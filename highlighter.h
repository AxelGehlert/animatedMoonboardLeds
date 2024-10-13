#ifndef __HIGHLIGHTER_H__
#define __HIGHLIGHTER_H__

#include <stdint.h>

#define MATRIX_PIXELS_X      11
#define MATRIX_PIXELS_Y      12

#define MAIN_CYCLE_MS 100

typedef void (*AnimationFunc)(uint32_t ledColors[][MATRIX_PIXELS_X]);

extern uint32_t currentStaticPicture  [MATRIX_PIXELS_Y][MATRIX_PIXELS_X];

extern int ledCnt;

extern int8_t sinTable[255];

long random(long);

void ledSetAnimationFunc(void (*newFunc)(uint32_t[][MATRIX_PIXELS_X]) );

#endif 