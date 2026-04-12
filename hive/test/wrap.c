#include "test.h"

#include "sleep.h"

bool wrap() {
  uint8_t z;
  z = wrapSub8(3,1);     assertInt(z,2);
  z = wrapSub8(10,250);  assertInt(z,16);
  z = wrapSub8(250,10);  assertInt(z,240);
  z = wrapSub8(250,255); assertInt(z,251);
  z = wrapAdd8(250,250);      assertInt(z,244);
  z = wrapAdd8(250,10);       assertInt(z,4);

  int8_t y;
  y = wrapSub8S(3,1);     assertInt(y,2);
  y = wrapSub8S(1,3);     assertInt(y,-2);
  y = wrapSub8S(250,3);   assertInt(y,-9);
  y = wrapSub8S(3,250);   assertInt(y,9);

  return true;
}
