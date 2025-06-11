#!/bin/sh

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Change to the script directory
cd "$SCRIPT_DIR"

# Clean up build artifacts
echo "Cleaning up build artifacts..."
rm -rf build
rm -rf bin

# Create build and bin directories
echo "Creating build directories..."
mkdir -p build bin

# Build the project using CMake
echo "Building project..."
cd build
cmake ..
make
cd ..

# Run the game
echo "Running game..."
./bin/GameEngine 