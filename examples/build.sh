#!/bin/bash

# Build all ASL examples
rm -rf build
mkdir build
for example in basic_cbuf basic_fifo basic_lifo; do
    echo "Building $example..."
    cd $example 
    sh build.sh || exit 1
    mv ./build/$example* ../build/
    rm -rf build
    cd ..
done

echo "All examples built successfully!"
