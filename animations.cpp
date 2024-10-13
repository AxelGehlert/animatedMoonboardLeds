/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#include "highlighter.h"
#include "animations.h"

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#define LED_POST_START_ANIMATION_FUNC afAllLedsFullOn  // Animation to start after afStartupScreen is done
#define PROBLEM_VIEW_FLASHINESS       40               // Range 0..51 (Which is 255/5)

/*--------------------------------------------------------------------------*/

uint32_t currentStaticPicture[MATRIX_PIXELS_Y][MATRIX_PIXELS_X] = 
{
  // A   B   C   D   E   F   G   H   I   J   K
  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},  // 12
  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},  // 11
  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},  // 10
  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},  // 9
  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},  // 8
  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},  // 7
  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},  // 6
  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},  // 5
  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},  // 4
  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},  // 3
  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},  // 2
  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}   // 1
};

/*--------------------------------------------------------------------------*/

void afStaticPicture(uint32_t ledColors[][MATRIX_PIXELS_X])
{
  for (int y=0;y<MATRIX_PIXELS_Y;y++)
    for (int x=0;x<MATRIX_PIXELS_X;x++)
      ledColors[y][x] = currentStaticPicture[y][x];
}

/*--------------------------------------------------------------------------*/

void afProblemView(uint32_t ledColors[][MATRIX_PIXELS_X])
{
  for (int y=0;y<MATRIX_PIXELS_Y;y++)
  {
    for (int x=0;x<MATRIX_PIXELS_X;x++)
    {
      uint16_t brightness = (ledCnt + x + y) % 10;
      if (brightness<5)
        brightness = 255-brightness*PROBLEM_VIEW_FLASHINESS;
      else
        brightness = 255-(10-brightness)*PROBLEM_VIEW_FLASHINESS;

      uint32_t pixelCol = currentStaticPicture[y][x];
      uint16_t r = (pixelCol >> 16) & 0xff;
      uint16_t g = (pixelCol >>  8) & 0xff;
      uint16_t b = (pixelCol      ) & 0xff;

      r *= brightness;
      g *= brightness;
      b *= brightness;

      pixelCol = (((uint32_t)r & 0xff00) << 8) | ((uint32_t)g & 0xff00) | (((uint32_t)b & 0xff00) >> 8);

      ledColors[y][x] = pixelCol;
    }
  }
  ledCnt += 1;
}

/*--------------------------------------------------------------------------*/

void afLedTest(uint32_t ledColors[][MATRIX_PIXELS_X])
{
  int xPos = (ledCnt / 3) % 11;
  for (int y=0;y<MATRIX_PIXELS_Y;y++)
  {
    for (int x=0;x<MATRIX_PIXELS_X;x++)
    {
      if (x==xPos)
      {
        ledColors[y][x] = 0xffffff;
      }
      else
      {
        ledColors[y][x] = 0;
      }
    }
  }
  ledCnt = (ledCnt+1)%(3*11);
}

/*--------------------------------------------------------------------------*/

void afLedTest2(uint32_t ledColors[][MATRIX_PIXELS_X])
{
  for (int y=0;y<MATRIX_PIXELS_Y;y++)
  {
    for (int x=0;x<MATRIX_PIXELS_X;x++)
    {
      int sinPos1 = ((int)ledCnt)*256/20 + (x*256/MATRIX_PIXELS_X);
      int sinPos2 = ((int)ledCnt)*256/20 - (x*256/MATRIX_PIXELS_X) + (y*256/MATRIX_PIXELS_Y);
      int sinPos3 = ((int)ledCnt)*256/20 + (y*256/MATRIX_PIXELS_Y);
      
      uint32_t col = (128 + sinTable[(uint8_t)(sinPos1 & 0xff)]) << 16;
      col += (128 + sinTable[(uint8_t)(sinPos2 & 0xff)]) << 8;
      col += (128 + sinTable[(uint8_t)(sinPos3 & 0xff)]);

      ledColors[y][x] = col;
    }
  }
  ledCnt = (ledCnt+1) % 20;
}

/*--------------------------------------------------------------------------*/

void afAllLedsFullOn(uint32_t ledColors[][MATRIX_PIXELS_X])
{
  uint32_t ledCol;
  if (ledCnt<20)
  {
    ledCol = 13*ledCnt;
    ledCol = ledCol | (ledCol << 8) | (ledCol << 16);

    ledCnt += 1;
  }
  else
  {
    ledCol = 0xffffff;
  }

  for (int y=0;y<MATRIX_PIXELS_Y;y++)
    for (int x=0;x<MATRIX_PIXELS_X;x++)
      ledColors[y][x] = ledCol;
}

/*--------------------------------------------------------------------------*/

void afStartupScreen(uint32_t ledColors[][MATRIX_PIXELS_X])
{
  if (ledCnt<20)
  {
    for (int y=0;y<MATRIX_PIXELS_Y;y++)
      for (int x=0;x<MATRIX_PIXELS_X;x++)
        ledColors[y][x] = 0x0000ff;

    ledCnt += 1;
  }
  else if (ledCnt<40)
  {
    int yPixelPos = (ledCnt-20);
    yPixelPos *= yPixelPos;
    yPixelPos *= 3;
    int ySubPixelPos = yPixelPos % 100;
    yPixelPos    = yPixelPos / 100;

    for (int y=0;y<MATRIX_PIXELS_Y;y++)
    {
      uint32_t ledCol;
      if (y<yPixelPos)
      {
        ledCol = 0;
      }
      else if (y==yPixelPos)
      {
        ledCol = (0x0000ff*(100-ySubPixelPos))/100;
      }
      else
      {
        ledCol = 0x0000ff;
      }
      for (int x=0;x<MATRIX_PIXELS_X;x++)
        ledColors[y][x] = ledCol;
    }

    ledCnt += 1;
  }
  else if (ledCnt<60)
  {
    for (int y=0;y<MATRIX_PIXELS_Y;y++)
      for (int x=0;x<MATRIX_PIXELS_X;x++)
        ledColors[y][x] = 0;

    ledCnt += 1;
  }
  else
  {
    ledSetAnimationFunc(LED_POST_START_ANIMATION_FUNC);
  }

}

/*--------------------------------------------------------------------------*/

void afTwinklingStars(uint32_t ledColors[][MATRIX_PIXELS_X])
{
  int x;
  int y;
  uint32_t col;

  for (y=0;y<MATRIX_PIXELS_Y;y++)
    for (x=0;x<MATRIX_PIXELS_X;x++)
      ledColors[y][x] = 0;

  if (ledCnt<20)
  {
    x = random(MATRIX_PIXELS_X);
    y = random(MATRIX_PIXELS_Y);
    currentStaticPicture[ledCnt % MATRIX_PIXELS_Y][ledCnt / MATRIX_PIXELS_Y] = ((uint32_t)x << 16) | ((uint32_t)y << 8) | 1;

    ledCnt += 1;
  }

  for (int ledIdx=0;ledIdx<ledCnt;ledIdx++)
  {
    uint32_t* currentLedInfoPtr = &currentStaticPicture[ledIdx % MATRIX_PIXELS_Y][ledIdx / MATRIX_PIXELS_Y];

    col =  (*currentLedInfoPtr) & 0xff;
    if (col<4)
      col = col * 85;
    else
      col = 510 - col*85;

    col = (col << 16) | (col << 8) | col;

    x = ((*currentLedInfoPtr) >> 16) & 0xff;
    y = ((*currentLedInfoPtr) >>  8) & 0xff;

    ledColors[y][x] = col;

    (*currentLedInfoPtr) = (*currentLedInfoPtr) + 1;
    if (((*currentLedInfoPtr) & 0xff) >= 6)
    {
      x = random(MATRIX_PIXELS_X);
      y = random(MATRIX_PIXELS_Y);
      (*currentLedInfoPtr) = (x << 16) | (y << 8) | 1;
    }
  }
}

/*--------------------------------------------------------------------------*/

void afRain(uint32_t ledColors[][MATRIX_PIXELS_X])
{
  int x;
  int y;
  int suby;
  int speed;
  uint32_t col;

  for (y=0;y<MATRIX_PIXELS_Y;y++)
    for (x=0;x<MATRIX_PIXELS_X;x++)
      ledColors[y][x] = 0;

  if (ledCnt<32)
  {
    x = random(MATRIX_PIXELS_X);
    y = 0;
    suby = 0;
    speed = 64+random(191);
    currentStaticPicture[ledCnt % MATRIX_PIXELS_Y][ledCnt / MATRIX_PIXELS_Y] = ((uint32_t)speed << 24) | ((uint32_t)x << 16) | ((uint32_t)y << 8) | suby;

    ledCnt += 1;
  }

  for (int ledIdx=0;ledIdx<ledCnt;ledIdx++)
  {
    uint32_t* currentLedInfoPtr = &currentStaticPicture[ledIdx % MATRIX_PIXELS_Y][ledIdx / MATRIX_PIXELS_Y];

    x = ((*currentLedInfoPtr) >> 16) & 0xff;
    y = ((*currentLedInfoPtr) >>  8) & 0xff;
    suby = (*currentLedInfoPtr) & 0xff;
    speed = ((*currentLedInfoPtr) >> 24) & 0xff;

    if (speed>=192)
    {
      col = (256-speed)*2;
      col = (col << 16) | (col << 8) | 0xff;
    }
    else
    {
      col = 64+speed;
    }
    ledColors[y][x] = col;

    suby += speed;
    while (suby>=256)
    {
        y += 1;
        suby -= 256;
    }

    if (y >= MATRIX_PIXELS_Y)
    {
      x = random(MATRIX_PIXELS_X);
      y = 0;
      suby = 0;
      speed = 64+random(191);
    }
    (*currentLedInfoPtr) = ((uint32_t)speed << 24) | ((uint32_t)x << 16) | ((uint32_t)y << 8) | suby;
  }
}

/*--------------------------------------------------------------------------*/

void afPenniesFromHeaven(uint32_t ledColors[][MATRIX_PIXELS_X])
{
  int x;
  int y;
  uint32_t col;

  for (y=0;y<MATRIX_PIXELS_Y;y++)
    for (x=0;x<MATRIX_PIXELS_X;x++)
      ledColors[y][x] = 0;

  if (ledCnt<10)
  {
    x = random(MATRIX_PIXELS_X);
    y = random(MATRIX_PIXELS_Y);
    currentStaticPicture[ledCnt % MATRIX_PIXELS_Y][ledCnt / MATRIX_PIXELS_Y] = ((uint32_t)x << 16) | ((uint32_t)y << 8) | 1;

    ledCnt += 1;
  }

  for (int ledIdx=0;ledIdx<ledCnt;ledIdx++)
  {
    uint32_t* currentLedInfoPtr = &currentStaticPicture[ledIdx % MATRIX_PIXELS_Y][ledIdx / MATRIX_PIXELS_Y];

    col =  (*currentLedInfoPtr) & 0xff;
    if (col<4)
      col = col * 85;
    else
      col = 510 - col*85;

    col = (col << 16) | (col << 8);

    x = ((*currentLedInfoPtr) >> 16) & 0xff;
    y = ((*currentLedInfoPtr) >>  8) & 0xff;

    ledColors[y][x] = col;

    y += 1;
    col = ((*currentLedInfoPtr) & 0xff) + 1;

    if ((col >= 6) || (y>=MATRIX_PIXELS_Y))
    {
      x = random(MATRIX_PIXELS_X);
      y = random(MATRIX_PIXELS_Y);
      col = 1;
    }
    (*currentLedInfoPtr) = (x << 16) | (y << 8) | col;
  }
}

/*--------------------------------------------------------------------------*/

void afThreeDemoBars(uint32_t ledColors[][MATRIX_PIXELS_X])
{
  int x;
  int y;
  uint32_t col;
  int cnt;

  int barPos[3];
  int order[3];

  for (cnt=0;cnt<3;cnt++)
    barPos[cnt] = (int)sinTable[(ledCnt+85*cnt)%256] * (MATRIX_PIXELS_Y/2) + (MATRIX_PIXELS_Y*128);

  if (ledCnt<=42)  // 0°-60°
  {
    order[0] = 0;
    order[1] = 2;
    order[2] = 1;
  }
  else if (ledCnt<85) //60°-120°
  {
    order[0] = 2;
    order[1] = 0;
    order[2] = 1;
  }
  else if (ledCnt<128) //120°-180°
  {
    order[0] = 2;
    order[1] = 1;
    order[2] = 0;
  }
  else if (ledCnt<170) //180°-240°
  {
    order[0] = 1;
    order[1] = 2;
    order[2] = 0;
  }
  else if (ledCnt<213) //240°-300°
  {
    order[0] = 1;
    order[1] = 0;
    order[2] = 2;
  }
  else
  {
    order[0] = 0;
    order[1] = 1;
    order[2] = 2;
  }

  for (y=0;y<MATRIX_PIXELS_Y;y++)
  {
    col = 0;

    for (cnt=0;cnt<3;cnt++)
    {
      int barDist = barPos[order[cnt]]-(y*256);
      if (barDist<0)
        barDist = -barDist;
      if (barDist<512)
      {
        col = ((512-barDist)/2) << (order[cnt] * 8);
        col |= ((512-barDist)/2) << (((order[cnt]+2)%3) * 8);
      }
    }

    for (x=0;x<MATRIX_PIXELS_X;x++)
      ledColors[y][x] = col;
  }

  ledCnt = (ledCnt + 6) % 256;
}

/*--------------------------------------------------------------------------*/

void afDiscoInferno(uint32_t ledColors[][MATRIX_PIXELS_X])
{
  int x;
  int y;
  uint32_t col;

  col = 0;
  if ((ledCnt % 2)==0)
    col = 0xffffff;

  for (y=0;y<(MATRIX_PIXELS_Y/2);y++)
    for (x=0;x<MATRIX_PIXELS_X;x++)
      ledColors[y][x] = col;

  if (ledCnt<=5)
    col = 51*ledCnt;
  else
    col = 51*(10-ledCnt);

  col = col << 16;

  for (y=(MATRIX_PIXELS_Y/2);y<MATRIX_PIXELS_Y;y++)
    for (x=0;x<(MATRIX_PIXELS_X/2);x++)
      ledColors[y][x] = col;

  if (ledCnt<=5)
    col = 51*(5-ledCnt);
  else
    col = 51*(ledCnt-5);
    
  col = col << 8;

  for (y=(MATRIX_PIXELS_Y/2);y<MATRIX_PIXELS_Y;y++)
    for (x=(MATRIX_PIXELS_X/2);x<MATRIX_PIXELS_X;x++)
      ledColors[y][x] = col;

  ledCnt = (ledCnt+1)%10;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
