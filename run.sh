#! /bin/bash

clear

cmake --build ./cmake-build-default --target ds2test -j 6 || exit

cd "./cmake-build-default/build/" || exit

pcsx2 -nogui -batch -elf "$(realpath "./ds2test.elf")"