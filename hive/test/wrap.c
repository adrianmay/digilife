
bool wrap() {
  uint8_t z;
  z = wrapSubtract8(3,1);     assertInt(z,2);
  z = wrapSubtract8(10,250);  assertInt(z,16);
  z = wrapSubtract8(250,10);  assertInt(z,240);
  z = wrapSubtract8(250,255); assertInt(z,251);
  z = wrapAdd8(250,250);      assertInt(z,244);
  z = wrapAdd8(250,10);       assertInt(z,4);
  return true;
}
