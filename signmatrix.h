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

#ifndef __SIGNMATRIX_H
#define __SIGNMATRIX_H

#define PIN_DATA 13
#define PIN_CLOCK 14
#define PIN_LATCH 12
#define PIN_A0 4
#define PIN_A1 5
#define PIN_A2 15
#define PIN_E1 2
#define PIN_IR 16

#define ROW_LEN 80*2
#define NUM_ROW 7

#endif // __SIGNMATRIX_H
