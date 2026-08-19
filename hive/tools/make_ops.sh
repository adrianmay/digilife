export OPS="nop end snd print0 roll0 rollCash spawn0 post0"
tools/make_ops_h.sh > gen/core/ops.h
tools/make_ops_c.sh > gen/core/ops.c

