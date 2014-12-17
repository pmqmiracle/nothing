#!/bin/sh

./a5 -input scene06_bunny_1k.txt  -size 300 300 -output 6.bmp\
   -shadows -bounces 4 -jitter -filter
./a5 -input scene07_shine.txt  -size 300 300 -output 7.bmp\
   -shadows -bounces 4 -jitter -filter
./a5 -input scene08_c.txt  -size 300 300 -output 8.bmp\
   -shadows -bounces 4 -jitter -filter
./a5 -input scene09_s.txt  -size 300 300 -output 9.bmp\
   -shadows -bounces 4 -jitter -filter
./a5 -input scene10_sphere.txt  -size 300 300 -output 10.bmp\
   -shadows -bounces 4 -jitter -filter
./a5 -input scene11_cube.txt  -size 300 300 -output 11.bmp\
 -shadows -bounces 4 -jitter -filter
./a5 -input scene12_vase.txt  -size 300 300 -output 12.bmp\
 -shadows -bounces 4 -jitter -filter

