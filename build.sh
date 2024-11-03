#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

# Get number of CPU cores
NUM_CORES=$(nproc)

# Check if cmake is installed
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}CMake is not installed${NC}"
    echo "Please install cmake using:"
    echo "sudo apt-get install cmake"
    exit 1
fi

# Detect OS and set executable path
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then
    # Configure project with CMake
    echo "Configuring project..."
    if ! cmake -S . -B build; then
        echo -e "${RED}CMake configuration failed${NC}"
        exit 1
    fi
else
    # Configure project with CMake using maximum optimizations
    echo "Configuring project..."
    if ! cmake -S . -B build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_FLAGS="-O3 -march=native -flto=${NUM_CORES} -ffast-math" \
        -DCMAKE_EXE_LINKER_FLAGS="-flto=${NUM_CORES} -fuse-linker-plugin"; then
        echo -e "${RED}CMake configuration failed${NC}"
        exit 1
    fi
fi

# Build project
echo "Building project..."
if ! cmake --build build -j $(nproc); then
    echo -e "${RED}Build failed${NC}"
    exit 1
fi

echo -e "${GREEN}Build completed successfully${NC}"
exit 0