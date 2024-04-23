#!/bin/bash
rm main.exe
cmake -G "Unix Makefiles" .
make
./main.exe 10000000