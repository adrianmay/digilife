#!/bin/bash
tools/build.sh 2> >(sed -f tools/doctor.sed >&2) && stdbuf -oL -eL ./Test && ./Hive
