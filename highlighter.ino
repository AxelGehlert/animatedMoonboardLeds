/****************************************************************************/

#include <FastLED.h>
#include <HardwareBLESerial.h>
#include "animations.h"
#include "highlighter.h"

/****************************************************************************/

#define NEOPIXEL_DOUT_PIN     4

/****************************************************************************/

#define MYLOG_INIT()
#define MYLOG(x) 
//#define MYLOG(x) Serial.println(x)

/****************************************************************************/

#define MAX_SERIAL_CMD_SIZE 256

#define BLE_MAX_RX_BUF_SIZE 512

#define LED_KIND_START  0
#define LED_KIND_NORMAL 1
#define LED_KIND_END    2
#define LED_KIND_LEFT   3
#define LED_KIND_RIGHT  4
#define LED_KIND_MATCH  5
#define LED_KIND_FOOT   6

#define HOLD_START_COLOR   0x00FF00l   // GREEN
#define HOLD_NORMAL_COLOR  0x0000FFl   // BLUE
#define HOLD_END_COLOR     0xFF0000l   // RED
#define HOLD_LEFT_COLOR    0xFF00FFl   // VIOLET
#define HOLD_RIGHT_COLOR   0x0000FFl   // BLUE
#define HOLD_MATCH_COLOR   0xFFFF00l   // ORANGE// YELLOW
#define HOLD_FOOT_COLOR    0x00FFFFl   // CYAN

/****************************************************************************/

// Maps matrix positions to Neopixel indices
// -1 means no LED present at the location

#define NUM_NEOPIXELS (MATRIX_PIXELS_Y*MATRIX_PIXELS_X)

static int ledIndices[MATRIX_PIXELS_Y][MATRIX_PIXELS_X] = 
{
  // A   B   C   D   E   F   G   H   I   J   K
  {121,122,123,124,125,126,127,128,129,130,131},  // 12
  {120,119,118,117,116,115,114,113,112,111,110},  // 11
  { 99,100,101,102,103,104,105,106,107,108,109},  // 10
  { 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88},  // 9
  { 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87},  // 8
  { 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66},  // 7
  { 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65},  // 6
  { 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44},  // 5
  { 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43},  // 4
  { 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22},  // 3
  { 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21},  // 2
  { 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0}   // 1
};

extern uint32_t fullLedColorMatrix[MATRIX_PIXELS_Y][MATRIX_PIXELS_X] = 
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

// "Movie" animation data

#include "animation_arcanoid.h"

/****************************************************************************/

int8_t sinTable[255] = 
  {0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 34, 37, 39, 42, 45, 
  48, 51, 54, 57, 60, 62, 65, 68, 70, 73, 75, 78, 80, 83, 85, 87, 
  90, 92, 94, 96, 98, 100, 102, 104, 105, 107, 109, 110, 112, 113, 115, 116, 
  117, 118, 119, 120, 121, 122, 123, 124, 124, 125, 125, 126, 126, 126, 126, 126, 
  126, 126, 126, 126, 126, 125, 125, 124, 124, 123, 122, 122, 121, 120, 119, 118, 
  116, 115, 114, 112, 111, 109, 108, 106, 104, 103, 101, 99, 97, 95, 93, 91, 
  88, 86, 84, 82, 79, 77, 74, 72, 69, 66, 64, 61, 58, 55, 53, 50, 
  47, 44, 41, 38, 35, 32, 29, 26, 23, 20, 17, 14, 10, 7, 4, 1, 
  -1, -4, -7, -10, -14, -17, -20, -23, -26, -29, -32, -35, -38, -41, -44, -47, 
  -50, -53, -55, -58, -61, -64, -66, -69, -72, -74, -77, -79, -82, -84, -86, -88, 
  -91, -93, -95, -97, -99, -101, -103, -104, -106, -108, -109, -111, -112, -114, -115, -116, 
  -118, -119, -120, -121, -122, -122, -123, -124, -124, -125, -125, -126, -126, -126, -126, -126, 
  -126, -126, -126, -126, -126, -125, -125, -124, -124, -123, -122, -121, -120, -119, -118, -117, 
  -116, -115, -113, -112, -110, -109, -107, -105, -104, -102, -100, -98, -96, -94, -92, -90, 
  -87, -85, -83, -80, -78, -75, -73, -70, -68, -65, -62, -60, -57, -54, -51, -48, 
  -45, -42, -39, -37, -34, -30, -27, -24, -21, -18, -15, -12, -9, -6, -3};

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

// This command needs to be done before the server starts...

// $stty -F /dev/ttyACM0 cs8 9600 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts -hupcl

// Maybe interesting for logging:
// $cat /dev/ttyACM0

// Attention: The first cat cmd will reset the arduino

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

unsigned long cycleStartTime;
byte          cycleCnt;

/*--------------------------------------------------------------------------*/

void ledSetup();
void bleSetup();
void serialSetup();

/*--------------------------------------------------------------------------*/

void setup() 
{
  serialSetup();
  ledSetup();
  bleSetup();

  cycleStartTime = millis();
  cycleCnt = 0;

  MYLOG("START");
}

/*--------------------------------------------------------------------------*/

void mainLoopFunc()
{
  serialMainFunc();
  ledMainFunc();
  bleMainFunc();
}

/*--------------------------------------------------------------------------*/

void loop() 
{
  mainLoopFunc();

  unsigned long nextCycleStartTime = cycleStartTime + MAIN_CYCLE_MS;
  unsigned long sleepTime          = nextCycleStartTime - millis();

  cycleStartTime = nextCycleStartTime;

  if (sleepTime<=(MAIN_CYCLE_MS*2))
  {
    delay(sleepTime);
  }
  else
  {
    MYLOG("MAIN CYCLE TIME exceeded");
  }

  cycleCnt = (cycleCnt+1) % 10;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

CRGB leds[NUM_NEOPIXELS];

typedef void (*AnimationFunc)(uint32_t ledColors[][MATRIX_PIXELS_X]);

AnimationFunc ledCurrentAnimationFunc;
int           ledCnt;

void ledClearLeds();

/*--------------------------------------------------------------------------*/

void ledSetup()
{
  FastLED.addLeds<PL9823,NEOPIXEL_DOUT_PIN>(leds, NUM_NEOPIXELS);
  ledClearLeds();

  ledCurrentAnimationFunc = afStartupScreen;
  ledCnt = 0;
}

/*--------------------------------------------------------------------------*/

void ledSetAnimationFunc(void (*newFunc)(uint32_t[][MATRIX_PIXELS_X]) )
{
  ledCurrentAnimationFunc = newFunc;
  ledCnt = 0;
}

/*--------------------------------------------------------------------------*/

void ledMainFunc()
{
  uint32_t col;
  int r,g,b;
  ledCurrentAnimationFunc(fullLedColorMatrix);
  for (int y=0;y<MATRIX_PIXELS_Y;y++)
  {
    for (int x=0;x<MATRIX_PIXELS_X;x++)
    {
      int ledIndex = ledIndices[y][x];
      if (ledIndex>=0)
      {
        col = fullLedColorMatrix[y][x];

        r = (col >> 16) & 0xff;
        g = (col >> 8) & 0xff;
        b = (col) & 0xff;
        if (r>=170) r = 2*r - 256; else r = r/2;
        if (g>=170) g = 2*g - 256; else g = g/2;
        if (b>=170) b = 2*b - 256; else b = b/2;
        col = (r << 16) | (g << 8) | b;

        leds[ledIndex] = col;
      }
    }
  }
  FastLED.show();
}

/*--------------------------------------------------------------------------*/

void ledClearLeds()
{
  ledSetAnimationFunc(afStaticPicture);
  for (int y=0;y<MATRIX_PIXELS_Y;y++)
  {
    for (int x=0;x<MATRIX_PIXELS_X;x++)
    {
      fullLedColorMatrix  [y][x] = 0;
      currentStaticPicture[y][x] = 0;
    }
  }
}

/*--------------------------------------------------------------------------*/

void ledSetMoonboardLedValue(byte ledKind,byte colIdx,int rowIdx)
{
  uint32_t col;
  switch (ledKind)
  {
    case LED_KIND_START:
      col = HOLD_START_COLOR;
      break;
    case LED_KIND_END:
      col = HOLD_END_COLOR;
      break;
    case LED_KIND_NORMAL:
      col = HOLD_NORMAL_COLOR;
      break;
    case LED_KIND_LEFT:
      col = HOLD_LEFT_COLOR;
      break;
    case LED_KIND_RIGHT:
      col = HOLD_RIGHT_COLOR;
      break;
    case LED_KIND_MATCH:
      col = HOLD_MATCH_COLOR;
      break;
    case LED_KIND_FOOT:
      col = HOLD_FOOT_COLOR;
      break;
    default:
      col = HOLD_NORMAL_COLOR;
      break;
  }

  currentStaticPicture[MATRIX_PIXELS_Y-rowIdx-1][colIdx] = col;
}

/*--------------------------------------------------------------------------*/

void afMovie(uint32_t ledColors[][MATRIX_PIXELS_X])
{
  long int offs = ((long int)ledCnt) * ANIMATION_FRAME_SIZE;
  
  for (int y=0;y<MATRIX_PIXELS_Y;y++)
  {
    for (int x=0;x<MATRIX_PIXELS_X;x++)
    {
      uint32_t gCol = pgm_read_word_near(ANIMATION_DATA+offs);

      uint32_t comp;
      uint32_t col = comp;

      comp = (gCol & 0x1f  ) * 0x8;
      col = comp;

      comp = (gCol & 0x3e0 ) * 0x40;
      col  = col | comp;
      
      comp = (gCol & 0x7c00) * 0x200;
      col  = col | comp;
      
      ledColors[y][x] = col;
      
      offs++;
    }
  }
  ledCnt += 1;
  if (ledCnt>=ANIMATION_NUM_FRAMES)
    ledCnt = ANIMATION_LOOP_START_FRAME;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

HardwareBLESerial &bleNordicUartService = HardwareBLESerial::getInstance();

// pin to use for the "BLE connected" LED
const int bleErrorLedPin = LED_BUILTIN; 

char bleRxBuf[BLE_MAX_RX_BUF_SIZE];
int  bleRxBufPos = 0;

bool bleError     = false;

int  gBleLedCnt;
int  gBleLastLedIdx;
bool gBleDoubleLeds = false;

/*--------------------------------------------------------------------------*/

void bleSetup()
{
  if (!bleNordicUartService.beginAndSetupBLE("MoonBoard A"))
  {
    bleError = true;
  }
}

/*--------------------------------------------------------------------------*/

void bleSetLed(byte ledKind,int ledPos,bool doubledLed)
{
  MYLOG(ledKind);
  MYLOG(ledPos);

  int col = (ledPos / MATRIX_PIXELS_Y);
  int row = (ledPos % MATRIX_PIXELS_Y);
  if ((col % 2)==1)
    row = MATRIX_PIXELS_Y - 1 - row;

  if (doubledLed)
  {
    if (row<(MATRIX_PIXELS_Y-1))
      ledSetMoonboardLedValue(ledKind,col,row+1);
  }
  else
  {
    ledSetMoonboardLedValue(ledKind,col,row);
  }
}

/*--------------------------------------------------------------------------*/

void  bleProcessHiddenLedCmd()
{
  switch (gBleLastLedIdx)
  {
    case 11:
      ledSetAnimationFunc(afLedTest);
      break;

    case 12:
      ledSetAnimationFunc(afLedTest2);
      break;

    case 35:
      ledSetAnimationFunc(afMovie);
      break;

    case 36:
      ledSetAnimationFunc(afAllLedsFullOn);
      break;

    case 59:
      ledSetAnimationFunc(afTwinklingStars);
      break;

    case 60:
      ledSetAnimationFunc(afRain);
      break;

    case 83:
      ledSetAnimationFunc(afPenniesFromHeaven);
      break;

    case 84:
      ledSetAnimationFunc(afThreeDemoBars);
      break;

    case 107:
      ledSetAnimationFunc(afDiscoInferno);
      break;

    //case 108:
    //  ledSetAnimationFunc(afBlah1);
    //  break;

    //case 131:
    //  ledSetAnimationFunc(afBlah2);
    //  break;

    default:
      ledClearLeds();
      break;
  }
}

/*--------------------------------------------------------------------------*/

int bleProcessSingleLedInput(int pos,bool doubledLed)
{
  if (pos>=bleRxBufPos)
    return BLE_MAX_RX_BUF_SIZE;

  byte ledKind;

  char ch = bleRxBuf[pos];
  switch (ch)
  {
    case 'S':
      ledKind = LED_KIND_START;
      break;
    case 'P':
      ledKind = LED_KIND_NORMAL;
      break;
    case 'L':
      ledKind = LED_KIND_LEFT;
      break;
    case 'R':
      ledKind = LED_KIND_RIGHT;
      break;
    case 'E':
      ledKind = LED_KIND_END;
      break;
    case 'M':
      ledKind = LED_KIND_MATCH;
      break;
    case 'F':
      ledKind = LED_KIND_FOOT;
      break;
    default:
      return BLE_MAX_RX_BUF_SIZE;
  }

  int numPos = pos+1;
  int holdPos = 0;
  while ( (numPos<bleRxBufPos) && (bleRxBuf[numPos]>='0') && (bleRxBuf[numPos]<='9') )
  {
    int digit = bleRxBuf[numPos] - '0';
    holdPos = holdPos*10 + digit;
    numPos++;
  }
  
  if (numPos>=bleRxBufPos)
    return BLE_MAX_RX_BUF_SIZE;

  bleSetLed(ledKind,holdPos,doubledLed);
  gBleLedCnt     = gBleLedCnt + 1;
  gBleLastLedIdx = holdPos;

  if (bleRxBuf[numPos]==',')
    return numPos+1;

  return BLE_MAX_RX_BUF_SIZE;
}

/*--------------------------------------------------------------------------*/

void bleProcessLedCmd()
{
  ledClearLeds();
  
  int pos;
  if (gBleDoubleLeds)
  {
    pos = 2;  
    while (pos<(bleRxBufPos-1))
    {
      pos = bleProcessSingleLedInput(pos,true);
    }

    gBleDoubleLeds = false;
  }

  gBleLedCnt     = 0;
  gBleLastLedIdx = -1;

  pos = 2;  
  while (pos<(bleRxBufPos-1))
  {
    pos = bleProcessSingleLedInput(pos,false);
  }

  ledSetAnimationFunc(afProblemView);

  if (gBleLedCnt==1)
  {
    bleProcessHiddenLedCmd();
  }
}

/*--------------------------------------------------------------------------*/

void bleHandleRxBufV1()
{
  if (bleRxBufPos<BLE_MAX_RX_BUF_SIZE)
  {
    bleRxBuf[bleRxBufPos] = '\0';
    MYLOG(bleRxBuf);

    if (bleRxBufPos>3)
    {
      if ( (bleRxBuf[0]=='l') && (bleRxBuf[1]=='#') && (bleRxBuf[bleRxBufPos-1]=='#') )
      {
        bleProcessLedCmd();
      }
    }

    bleRxBufPos = 0;
    return;
  }  
}

/*--------------------------------------------------------------------------*/

int bleProcessSingleConfigInput(int pos)
{
  if (pos>=bleRxBufPos)
    return BLE_MAX_RX_BUF_SIZE;

  byte ledKind;

  char ch = bleRxBuf[pos];
  switch (ch)
  {
    case 'D':
      gBleDoubleLeds = true;
      MYLOG("v2Dbl");
      break;
    default:
      break;
  }

  int myPos = pos+1;
  if (myPos>=bleRxBufPos)
    return BLE_MAX_RX_BUF_SIZE;

  if (bleRxBuf[myPos]==',')
    return myPos+1;

  return BLE_MAX_RX_BUF_SIZE;
}

/*--------------------------------------------------------------------------*/

void bleProcessConfigCmd()
{
  int pos = 1;
  while (pos<(bleRxBufPos-1))
  {
    pos = bleProcessSingleConfigInput(pos);
  }
}

/*--------------------------------------------------------------------------*/

void bleHandleRxBufV2()
{
  if (bleRxBufPos<BLE_MAX_RX_BUF_SIZE)
  {
    bleRxBuf[bleRxBufPos] = '\0';
    MYLOG(bleRxBuf);

    if (bleRxBufPos>=3)
    {
      if ( (bleRxBuf[0]=='~') && (bleRxBuf[bleRxBufPos-1]=='*') )
      {
        bleProcessConfigCmd();
      }
    }

    bleRxBufPos = 0;
    return;
  }  
}

/*--------------------------------------------------------------------------*/

void bleHandleRxChar(char ch)
{
  if (bleRxBufPos<BLE_MAX_RX_BUF_SIZE)
  {
    bleRxBuf[bleRxBufPos++] = ch;
    if (ch=='#')
    {
      if (bleRxBufPos>2)
      {
        bleHandleRxBufV1();
      }
    }
    else if (ch=='*')
    {
      if (bleRxBufPos>2)
      {
        bleHandleRxBufV2();
      }
    }
  }
}

/*--------------------------------------------------------------------------*/

void bleMainFunc()
{
  if (bleError)
  {
    digitalWrite(bleErrorLedPin, (cycleCnt<5) ? LOW : HIGH);
    return;
  }
    
  bleNordicUartService.poll();
  while (bleNordicUartService.available()>0)
  {
    bleHandleRxChar(bleNordicUartService.read());
  }
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

char serialCmd[MAX_SERIAL_CMD_SIZE];
int  serialCmdSize = 0;

/*--------------------------------------------------------------------------*/

void serialSetup()
{
  Serial.begin(9600); 
}

/*--------------------------------------------------------------------------*/

void serialProcessCmdHighlight()
{
  ledClearLeds();

  MYLOG("processSerialCmdHighlight");
  
  int pos=1;
  while (pos<serialCmdSize)
  {
    // ----- Get LED kind
    
    byte ledKind = serialCmd[pos]-'0';
    if ((pos++)>=serialCmdSize) return;

    // ----- Get column index

    byte colIdx = serialCmd[pos]-'A';
    if ((pos++)>=serialCmdSize) return;

    // ----- Get row index

    int rowIdx = -1;

    byte firstRowDigit = serialCmd[pos]-'0';
    pos++;

    if (pos<serialCmdSize)
    {
      char ch = serialCmd[pos];
      if ((ch>='0')&&(ch<='9')) 
      {
        byte secondRowDigit = ch - '0';
  
        rowIdx = firstRowDigit*10 + secondRowDigit - 1;
  
        pos++;
      }
      else
      {
        rowIdx = firstRowDigit-1;
      }
    }
    else
    {
      return;
    }
    if (rowIdx<0) return;

    // ----- Show led

    MYLOG(colIdx);
    MYLOG(rowIdx);
    MYLOG(ledKind);
    ledSetMoonboardLedValue(ledKind,colIdx,rowIdx);

    // ----- Check for "," or EOL

    if (pos==serialCmdSize)
    {
      // EOL: Okay, end
      return;
    }
    else if (pos<serialCmdSize)
    {
      char ch = serialCmd[pos];
      if (ch==',')
      {
        // Comma: Okay, continue
        pos++;
      }
      else if (ch==';')
      {
        // Semicolon: Okay, stop
        return;
      }
      else
      {
        return;
      }
    }
  }
}

/*--------------------------------------------------------------------------*/

void serialProcessCmd()
{
  MYLOG("AH");
  MYLOG(serialCmdSize);

  if (serialCmdSize<1)
    return;

  char cmd = serialCmd[0];
  switch (cmd)
  {
    case 'h':
    {
      serialProcessCmdHighlight();
      break;
    }
    case 'c':
    {
      ledClearLeds();
      break;
    }
    case 't':
    {
      ledSetAnimationFunc(afLedTest);
      break;
    }
    case 'T':
    {
      ledSetAnimationFunc(afLedTest2);
      break;
    }
    case 'm':
    {
      ledSetAnimationFunc(afMovie);
      break;
    }
    default:
    {
      break;
    }
  }
  
  serialCmdSize = 0;
}

/****************************************************************************/

void serialMainFunc()
{
  while (Serial.available() > 0) 
  {
    byte bt = Serial.read();

    if (bt=='\n')
    {
      serialProcessCmd();
    }
    else if (serialCmdSize<MAX_SERIAL_CMD_SIZE)
    {
      MYLOG(bt);
      serialCmd[serialCmdSize++] = bt;
    }
  }
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
