export OPS="nop end snd print roll rollCash spawn post"
tools/make_ops_h.sh > gen/core/ops.h
tools/make_ops_c.sh > gen/core/ops.c

