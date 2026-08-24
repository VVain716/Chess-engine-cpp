#!/usr/bin/env bash
set -e

echo "=== [1/2] Compiling chess-uci binary for Linux ==="
mkdir -p lichess/engines

STATIC_FLAGS=""
if [[ "$OSTYPE" != "darwin"* ]]; then
    STATIC_FLAGS="-static-libstdc++ -static-libgcc"
fi

# Compile pure C++ core and UCI interface with -O3
g++ -std=c++20 -O3 -Wall -Wextra $STATIC_FLAGS -Iinclude \
    src/core/Types.cpp \
    src/core/Board.cpp \
    src/core/MoveGen.cpp \
    src/core/Notation.cpp \
    src/core/Eval.cpp \
    src/core/Search.cpp \
    src/uci.cpp \
    -o lichess/engines/chess-uci

chmod +x lichess/engines/chess-uci

echo "=== [2/2] Installing Python Dependencies ==="
pip install -r lichess/requirements.txt

echo "=== Build Completed Successfully ==="
