/*
 */

#include "signmatrix.h"
#include "display.h"

void setup() {
  Display_setup();
  Display_enable(false);
}

void loop() {
  Display_test();
}
