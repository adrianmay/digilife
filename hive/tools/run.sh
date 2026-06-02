#!/bin/bash
mkdir -p data/test data/hive
rm -rf data/test/*
tools/build.sh 2> >(sed -f tools/doctor.sed >&2) && stdbuf -o0 -e0 ./Test data/test && ./Hive data/hive
