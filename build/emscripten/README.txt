
1) Build dali with emscripten 's own llvm to llvm bitcode

  # setup env
  export PATH=/home/likewise-open/SERILOCAL/$USER/installs/emscripten/emscripten-fastcomp/build/bin:/home/likewise-open/SERILOCAL/$USER/installs/emscripten/emscripten:$PATH
  export EM_PKG_CONFIG_PATH=$PKG_CONFIG_PATH
  cd dali/build/slp
  # use llvm's drop in replacements for configure/make
  emconfigure
  emmake

2) Build dali-adaptor

  cd ./Build
  rm CMakeCache.txt
  /usr/bin/cmake .. -DCMAKE_BUILD_TYPE=Debug -DEMSCRIPTEN=1
  make -j9

  mv dali-emscripten dali-emscripten.bc

  #
  emcc dali-emscripten.bc -o dali-emscripten.html -s FULL_ES2=1 -s STB_IMAGE=1 -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=0 -s DISABLE_EXCEPTION_CATCHING=2  -s EXPORT_NAME=\\\"dali\\\" -O2 --bind



# debug options

      "exec emcc dali-emscripten.bc -o dali-emscripten.html -s FULL_ES2=1 -s STB_IMAGE=1 -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=2 -s DISABLE_EXCEPTION_CATCHING=0 -s EXPORT_NAME=\\\"dali\\\" -O1 --js-opts 0 -g4 --bind",
