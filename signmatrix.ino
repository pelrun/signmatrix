/*
/mr: pulled high

to 74hc138
a0: gpio4
a1: gpio5
a2: gpio0
/e1: gpio2 anything

to 74hc595 chain
latch: gpio12 anything
clock: gpio14 sck
data out: gpio13 mosi

IR receiver: gpio16

Row format: 8 bits red, followed by 8 bits green, shifted in from PSU side
 */

#include <SPI.h>

#define PIN_DATA 13
#define PIN_CLOCK 14
#define PIN_LATCH 12
#define PIN_A0 4
#define PIN_A1 5
#define PIN_A2 15
#define PIN_E1 2
#define PIN_IR 16

#define ROW_LEN 20
#define COL_LEN 8

void loadRow(uint8_t *row)
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

void setColumn(uint8_t col)
{
  digitalWrite(PIN_A0, col&1?HIGH:LOW);
  digitalWrite(PIN_A1, col&2?HIGH:LOW);
  digitalWrite(PIN_A2, col&4?HIGH:LOW);
}

void showDisplay(bool enable)
{
  // active low
  digitalWrite(PIN_E1, enable?LOW:HIGH);
}

uint8_t row[ROW_LEN];

void setup() {
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

void testShift()
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

void loop() {
  /*
  for (uint8_t col=0; col<COL_LEN; col++)
  {
    showDisplay(false);
    loadRow(row);
    setColumn(col);
    showDisplay(true);
    //delay(5);
  }*/

  testShift();
}
