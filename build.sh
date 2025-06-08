#!/bin/bash
gcc -o nari nari.c  -lharfbuzz -lfreetype -lfontconfig -lGL -lpodvig -lleif -lrunara -lX11 -lXrender -lXinerama -lm -lXi
