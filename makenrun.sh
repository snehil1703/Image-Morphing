#!/bin/bash

make clean
make

./lab1 Marvin.png Sergey.png
./lab1_gray Marvin.png Sergey.png

./lab1 Sergey.png Sundar.png
./lab1_gray Sergey.png Sundar.png

./lab1 Sundar.png Marvin.png
./lab1_gray Sundar.png Marvin.png

./lab1 gauss.png lena.png
./lab1_gray gauss.png lena.png
