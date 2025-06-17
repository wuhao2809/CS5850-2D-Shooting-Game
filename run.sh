#!/bin/sh

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Check if we're already in the CPP directory
if [ "$(basename "$PWD")" != "CPP" ]; then
    cd "$SCRIPT_DIR"
fi

# Create build and bin directories if they don't exist
mkdir -p build bin

# Build the project using CMake
cd build
echo "Running CMake..."
cmake -DBUILD_TESTS=OFF ..
if [ $? -ne 0 ]; then
    echo "CMake failed with exit code $?"
    cd ..
    exit 1
fi

echo "Running make..."
make
if [ $? -ne 0 ]; then
    echo "Make failed with exit code $?"
    cd ..
    exit 1
fi

cd ..

# Run the game
echo "Running game..."
./bin/GameEngine
if [ $? -ne 0 ]; then
    echo "Game failed to run with exit code $?"
    exit 1
fi 