#!/bin/bash

# Change with your names, separated with a '_'. Do not use whitespaces!
NAMES="CORREGE-SAINT_CIRGUE"

# Clean all generated files
cd work
make clean
cd ..

# Create archive
tar -czvf $NAMES.tar.gz work report
