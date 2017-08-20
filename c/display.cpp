/*
Row format: 8 bits red, followed by 8 bits green, shifted in from PSU side
 */

//#include <SPI.h>
#include <Ticker.h>

#include "signmatrix.h"
#include "display.h"

static Ticker frameUpdater;

#define NUM_BUF 2

static uint8_t frameBuf[NUM_BUF][NUM_ROW][ROW_LEN];
static uint8_t bufNum;
static uint8_t tick;

static void loadRow(uint8_t *row, uint8_t timeslice)
{
  digitalWrite(PIN_LATCH, LOW);
  // load shift register chain
  for (int i=0; i<ROW_LEN; i++)
  {
    digitalWrite(PIN_CLOCK, LOW);
    digitalWrite(PIN_DATA, row[i]>timeslice?HIGH:LOW);
    digitalWrite(PIN_DATA, HIGH);
  }
  // latch data
  digitalWrite(PIN_LATCH, HIGH);
}

static void setRow(uint8_t row)
{
  digitalWrite(PIN_A0, row&1?HIGH:LOW);
  digitalWrite(PIN_A1, row&2?HIGH:LOW);
  digitalWrite(PIN_A2, row&4?HIGH:LOW);
}

static void updateFrame(uint8_t *frame);
{
  // FIXME: properly determine the framerate/pwm resolution
  tick = (++tick)%16;

  for (row=0; row<NUM_ROW; row++)
  {
    digitalWrite(PIN_E1, HIGH);
    setRow(row);
    loadRow(frame[bufNum][row], tick*16);
    digitalWrite(PIN_E1, LOW);
  }
}

void Display_newFrame(uint8_t *frame)
{
  uint8_t newBufNum = (bufNum+1)%NUM_BUF;
  memcpy(frame[newBufNum],frame,ROW_LEN*NUM_ROW);
  // flip buffer
  bufNum = newBufNum;
}

void Display_enable(bool enable)
{
  if (enable)
  {
    // enable update thread
    frameUpdater.attach_ms(1, updateFrame);
  }
  else
  {
    // disable update thread
    frameUpdater.detach();
    // disable display
    digitalWrite(PIN_E1, HIGH);
  }
}

void Display_setup()
{
  //SPI.begin();

  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  digitalWrite(PIN_CLOCK,LOW);
  pinMode(PIN_LATCH, OUTPUT);
  digitalWrite(PIN_LATCH,LOW);
  pinMode(PIN_A0, OUTPUT);
  pinMode(PIN_A1, OUTPUT);
  pinMode(PIN_A2, OUTPUT);
  pinMode(PIN_E1, OUTPUT);
//  pinMode(PIN_IR, INPUT_PULLUP);
  Display_enable(false);
}

void Display_test()
{
  int row = 0, i = 0;

  setrow((row++)%NUM_ROW);
  digitalWrite(PIN_CLOCK, LOW);
  // set data
  digitalWrite(PIN_DATA, (i++%20)&1?LOW:HIGH); // shift in 1 high bit every 20 clocks
  // clock in bit
  digitalWrite(PIN_CLOCK, HIGH);
  // latch data
  digitalWrite(PIN_LATCH, HIGH);
  digitalWrite(PIN_E1, LOW);
  digitalWrite(PIN_LATCH, LOW);
  delay(500);
  digitalWrite(PIN_E1, HIGH);
}

