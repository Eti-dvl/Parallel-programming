#!/bin/bash

# Change by your names
NAMES="CORREGE-SAINT_CIRGUE"

# Clean all generated files
cd parallel
make clean
cd ..

# Create archive
tar -czvf $NAMES.tar.gz parallel
