#!/bin/bash
mkdir -p data/test data/hive
tools/build.sh 2> >(sed -f tools/doctor.sed >&2) && stdbuf -oL -eL ./Test data/test && ./Hive data/hive
