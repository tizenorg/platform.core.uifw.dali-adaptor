mkdir build
cd build
rm CMakeCache.txt
/usr/bin/cmake .. -DCMAKE_BUILD_TYPE=Debug -DEMSCRIPTEN=1
make -j9

mv dali-emscripten dali-emscripten.bc

#
emcc dali-emscripten.bc -o dali-emscripten.html -s FULL_ES2=1 -s STB_IMAGE=1 -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=0 -s DISABLE_EXCEPTION_CATCHING=2  -s EXPORT_NAME=\\\"dali\\\" -O2 --bind
