#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

# Check if cmake is installed
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}CMake is not installed${NC}"
    echo "Please install cmake using:"
    echo "sudo apt-get install cmake"
    exit 1
fi

# Configure project with CMake
echo "Configuring project..."
if ! cmake -S . -B build; then
    echo -e "${RED}CMake configuration failed${NC}"
    exit 1
fi

# Build project
echo "Building project..."
if ! cmake --build build -j $(nproc); then
    echo -e "${RED}Build failed${NC}"
    exit 1
fi

echo -e "${GREEN}Build completed successfully${NC}"
exit 0