#!/bin/bash

pushd SDL

wget https://github.com/libsdl-org/SDL/releases/download/release-2.26.5/SDL2-2.26.5.zip
unzip -q SDL2-2.26.5.zip
mv SDL2-2.26.5/include include
ln -s . include/SDL2
rm SDL2-2.26.5.zip
rm -r SDL2-2.26.5

popd
