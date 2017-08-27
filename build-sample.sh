#!/usr/bin/bash
gcc -o usage-sample usage-sample.c semver.c -g -lm --std=c11 `pkg-config --libs --cflags glib-2.0`
