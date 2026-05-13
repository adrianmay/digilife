
#define assertCond(VAR, COND) \
  if (!(VAR COND)) { \
    fprintf(stdout, "%s:%d: Expected: " #COND "; Got: %'d\n", __FILE__, __LINE__, VAR); \
    return false; \
  }

#define assertLongCond(VAR, COND) \
  if (!(VAR COND)) { \
    printf("%s:%d: Expected: " #COND "; Got: %'ld\n", __FILE__, __LINE__, VAR); \
    return false; \
  }

#define assertLongApprox(VAR, VAL) \
  assertLongCond(VAR, >(VAL*19/20)) \
  assertLongCond(VAR, <(VAL*21/20))


#define assertStringAtLine(S1, E1, MX, LINE) \
  if (0!=strncmp(S1, E1, MX)) { \
    fprintf(stdout, "%s:%s: Expected: '%s'; Got: '%s'\n", __FILE__, LINE, S1, E1); \
    return false; \
  }

#define assertInt(VAR, VAL) \
  if (VAR != VAL) { \
    fprintf(stdout, "%s:%d: Expected: " #VAL "=%d ; Got: %d\n", __FILE__, __LINE__, VAL, VAR); \
    return false; \
  }

#define assertLong(VAR, VAL) \
  if (VAR != VAL) { \
    fprintf(stdout, "%s:%d: Expected: " #VAL "=%'ld ; Got: %'ld\n", __FILE__, __LINE__, VAL, VAR); \
    return false; \
  }

bool nearly(double a, double b);

#define assertDoubleApprox(VAR, VAL) \
  if (!nearly(VAR, VAL)) { \
    fprintf(stdout, "%s:%d: Expected: " #VAL "=%f ; Got: %f\n", __FILE__, __LINE__, VAL, VAR); \
    return false; \
  }

#define assertIntSuf(VAR, VAL, SUF) \
  if (VAR != VAL) { \
    fprintf(stdout, "%s:%d: Expected: " #VAL "=%d ; Got: %d %s\n", __FILE__, __LINE__, VAL, VAR, SUF); \
    return false; \
  }

#define assertIntHex(VAR, VAL) \
  if (VAR != VAL) { \
    fprintf(stdout, "%s:%d: Expected: " #VAL "=0x%x ; Got: 0x%x\n", __FILE__, __LINE__, VAL, VAR); \
    return false; \
  }

#define assertIntSufHex(VAR, VAL, SUF) \
  if (VAR != VAL) { \
    fprintf(stdout, "%s:%d: Expected: " #VAL "=%x ; Got: %x %s\n", __FILE__, __LINE__, VAL, VAR, SUF); \
    return false; \
  }

////////////////////////////////////////////////////////////////

#define TIME_VOID(CLOCK, WORK) \
{ Cycles start = CLOCK(); \
  WORK; \
  Cycles stop  = CLOCK(); \
  cycles = stop - start; \
}

#define TIME_RET(CLOCK, RET, WORK) \
{ Cycles start = CLOCK(); \
  RET = WORK; \
  Cycles stop  = CLOCK(); \
  cycles = stop - start; \
}

#define TIME_VOID_PROC(WORK)     TIME_VOID(readProcessCycles, WORK)
#define TIME_RET_PROC(RET, WORK) TIME_RET(readProcessCycles, RET, WORK)
  
