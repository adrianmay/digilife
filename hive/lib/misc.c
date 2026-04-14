#include <stdbool.h>
bool nearly(double a, double b) { double d = a-b; if (d<0) d=-d; return d<(a/10000); }
