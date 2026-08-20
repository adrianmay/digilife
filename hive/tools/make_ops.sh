export OPS="nop end iff elsif print" # roll spawn post"
export TESTS="no yes like gt but"
tools/make_ops_h.sh > gen/core/ops.h
tools/make_ops_c.sh > gen/core/ops.c

