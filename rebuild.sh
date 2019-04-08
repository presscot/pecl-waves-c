#!/bin/bash -
phpize --clean && phpize
aclocal && libtoolize --force && autoreconf
./configure --with-waves --enable-waves-debug
make clean
make -j3
