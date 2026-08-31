#include "types.h"
#include "api.h"

typedef struct { 
  uint8_t prog[300]; 
  int IP;
  char out[300];
  int outcur;
} Core;

typedef struct Mode Mode;

typedef void InstDoer(Core *, Mode *);
typedef InstDoer * InstDoerLookerUpper(uint8_t);
typedef float FloatDoer(Core *, Mode *);
typedef FloatDoer * FloatDoerLookerUpper(uint8_t);

struct Mode { 
  void (*doGenericInstOp)(Core * pC, uint8_t);
  void (*doGenericFloatOp)(Core * pC, uint8_t);
  void (*emitFloat)(Core * pC, float);
}; 

void  parseInst (Core * pC, Mode * mode);
float parseFloat(Core * pC, Mode * mode);

InstDoer * lookupInstParser(uint8_t x) {
  void parseNop(Core * pC, Mode * mode) { mode->doGenericInstOp(pC, x); parseInst(pC, mode); }
  void parseEnd(Core * pC, Mode * mode) { mode->doGenericFloatOp(pC, x); }
  void parsePrf(Core * pC, Mode * mode) { float f = parseFloat(pC, mode); mode->emitFloat(pC, f); }
  InstDoer * fs[] = { parseNop, parseEnd, parsePrf };
  return fs[x];
}

float parseZero(Core * pC, Mode * mode) { float v=0; mode->doGenericFloatOp(pC, 0); return v; }
float parseOne (Core * pC, Mode * mode) { float v=2; mode->doGenericFloatOp(pC, 1); return v; }
FloatDoer * lookupFloatParser(uint8_t x) {
  // float parseImm(Core * pC, Mode * mode) { mode->doGeneric(pC, x); }
  FloatDoer * fs[] = { parseZero, parseOne };
  return fs[x];
}
  
void  parseInst (Core * pC, Mode * mode) { lookupInstParser(pC->prog[pC->IP++])(pC, mode); }
float parseFloat(Core * pC, Mode * mode) { FloatDoer * f = lookupFloatParser(pC->prog[pC->IP++]); return f(pC, mode); }

void ignore8(Core * pC, uint8_t x) {}
void emitFloat(Core * pC, float f){ int l = sprintf(pC->out+pC->outcur, "%f ", f); pC->outcur += l; }
void runProg(Core * pC) { 
  Mode mode = { ignore8, ignore8, emitFloat };
  parseInst(pC, &mode);
}

void disasInstOp(Core * pC, uint8_t x) {}
void disasFloatOp(Core * pC, uint8_t x) {}
void ignoreF(Core * pC, float x) {}
void disasProg(Core * pC) { 
  Mode mode = { disasInstOp, disasFloatOp, ignoreF };
  parseInst(pC, &mode);
}

bool bo() { 
  Core core = {{2,1,1}, 0, {0}, 0};
  runProg(&core);
  printf("FOO: %s\n", core.out);
  return true;
}
