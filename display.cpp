/*
Row format: 8 bits red, followed by 8 bits green, shifted in from PSU side
 */

//#include <SPI.h>

#include "signmatrix.h"
#include "display.h"

static void loadRow(uint8_t *row)
{
  digitalWrite(PIN_LATCH,LOW);
  // load shift register chain
  for (int i=0; i<ROW_LEN; i++)
  {
    //SPI.write(row[i]);
    shiftOut(PIN_DATA, PIN_CLOCK, LSBFIRST, row[i]);
  }
  // latch data
  digitalWrite(PIN_LATCH,HIGH);
}

static void setColumn(uint8_t col)
{
  digitalWrite(PIN_A0, col&1?HIGH:LOW);
  digitalWrite(PIN_A1, col&2?HIGH:LOW);
  digitalWrite(PIN_A2, col&4?HIGH:LOW);
}

void Display_enable(bool enable)
{
  // active low
  digitalWrite(PIN_E1, enable?LOW:HIGH);
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
  showDisplay(false);
}

void Display_test()
{
  int col = 0, i = 0;

  setColumn((col++)%7);
  digitalWrite(PIN_CLOCK, LOW);
  // set data
  digitalWrite(PIN_DATA, (i++%20)&1?LOW:HIGH); // shift in 1 high bit every 20 clocks
  // clock in bit
  digitalWrite(PIN_CLOCK, HIGH);
  // latch data
  digitalWrite(PIN_LATCH, HIGH);
  showDisplay(true);
  digitalWrite(PIN_LATCH, LOW);
  delay(500);
  showDisplay(false);
}

