#!/bin/bash
set -e

EMSDK=$HOME/mono/sdks/builds/toolchains/emsdk

POSITIONAL=()
while [[ $# -gt 0 ]]
do
key="$1"

case $key in
    -d|--debug)
    EM_DEBUG="true"
    shift # past argument
    ;;
    -emsdk|--emsdk)
    EMSDK="$2"
    shift # past argument
    shift # past value
    ;;
    *)    # unknown option
    POSITIONAL+=("$1") # save it in an array for later
    shift # past argument
    ;;
esac
done
set -- "${POSITIONAL[@]}" # restore positional parameters


CPP_FLAGS="-std=c++11 -Wall -Wextra -Werror -Wno-nonportable-include-path -fno-exceptions -fno-rtti"
EMS_FLAGS="-s ENVIRONMENT=web -s WASM=1 -s USE_WEBGPU=1 -s NO_EXIT_RUNTIME=1 -s STRICT=1 --shell-file shell.html"
OPT_FLAGS=""

if [ -n "$EM_DEBUG" ]; then
  echo "debug mode active..."
  CPP_FLAGS="$CPP_FLAGS -g3 -D_DEBUG=1 -Wno-unused"
  EMS_FLAGS="$EMS_FLAGS -s ASSERTIONS=2 -s DEMANGLE_SUPPORT=1 -s SAFE_HEAP=1 -s STACK_OVERFLOW_CHECK=2"
  OPT_FLAGS="$OPT_FLAGS -O0"
else
  CPP_FLAGS="$CPP_FLAGS -g0 -DNDEBUG=1 -flto"
  EMS_FLAGS="$EMS_FLAGS -s ASSERTIONS=0 -s DISABLE_EXCEPTION_CATCHING=1 -s EVAL_CTORS=1 --closure 1"
  OPT_FLAGS="$OPT_FLAGS -O3"
fi

for file in src/*.cpp; do
  SRCs=$SRCs" $file"
done

INC=-Iinclude

OUT=www/index
mkdir -p www

source $EMSDK/emsdk_env.sh

emcc $CPP_FLAGS $OPT_FLAGS $EMS_FLAGS $INC $SRCs -o $OUT.html
if [ -z ${EM_DEBUG} ]; then
  $EMSDK/upstream/bin/wasm-opt $OPT_FLAGS --converge $OUT.wasm -o $OUT.wasm
fi

echo "Success!"
