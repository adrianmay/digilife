#!/bin/bash
tools/build.sh 2> >(sed -f tools/doctor.sed >&2) && ./Test && ./Hive
