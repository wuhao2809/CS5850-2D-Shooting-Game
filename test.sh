#!/bin/bash

# Exit on error
set -e

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

# Enter build directory
cd build

# Configure CMake
cmake ..

# Build the tests
cmake --build .

# Return to original directory
cd ..

# Find and run all test executables
echo "Running all tests..."
echo "===================="
for test in bin/*Test bin/test_* ; do
    if [ -x "$test" ]; then  # Check if file is executable
        echo -e "\nRunning ${test}..."
        echo "--------------------"
        ./"$test"
        echo "--------------------"
    fi
done 